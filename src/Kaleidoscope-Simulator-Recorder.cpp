/* -*- mode: c++ -*-
 * Kaleidoscope-Simulator-Recorder
 *     -- Use the physical keyboard to record I/O to generate simulator input
 * Copyright (C) 2019  noseglasses (shinynoseglasses@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Kaleidoscope-Simulator-Recorder.h"

#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-FocusSerial.h"
#include "HID.h"
#include "aglais/src/Aglais.h"
#include "aglais/src/v1/Grammar.h"

#define FSH (const __FlashStringHelper*)  //A helper to allow printing the PROGMEM strings.

using namespace aglais::v1;

namespace kaleidoscope {
namespace plugin {
   
struct HandlerFlags {
   static constexpr uint8_t on_setup           = 1;
   static constexpr uint8_t on_keyswitch_event = 2;
   static constexpr uint8_t before_each_cycle  = 3;
};

struct CallId {
   static constexpr uint8_t first  = 0;
   static constexpr uint8_t second = 1;
};
   
void SimulatorRecorder::setLEDsWait(uint8_t red, uint8_t green, uint8_t blue) {
   LEDControl::set_all_leds_to(red, green, blue);
   LEDControl::syncLeds();
   delay(500);
}

void SimulatorRecorder::sendReportHook(uint8_t id, const void* data, 
                                       int len, int result)
{
   Focus.send(Command::reaction, SubCommand::hid_report);
   Focus.send(id, len);
   auto byte_data = static_cast<const uint8_t *>(data);
   for(int i = 0; i < len; ++i) {
      Focus.send(byte_data[i]);
   }
   Focus.sendRaw('\n');
}

void SimulatorRecorder::sendProtocolHeader() const
{
   Focus.send(protocol_version, aglais::DocumentType::compressed);
   Focus.sendRaw('\n');
   Focus.send(Command::firmware_id);
   Focus.sendRaw('\"');
   if(id_string_) {
      Focus.sendRaw(FSH(id_string_));
   }
   Focus.sendRaw("\"\n");
}

void SimulatorRecorder::displayIntro()
{
   // Have an intro-sequence that prepares for recording
   //
   delay(500);
   setLEDsWait(255, 0, 0);
   setLEDsWait(0, 0, 0);
   setLEDsWait(255, 0, 0);
   setLEDsWait(0, 0, 0);
   setLEDsWait(255, 0, 0);
   setLEDsWait(0, 0, 0);
   setLEDsWait(0, 255, 0);
   setLEDsWait(0, 255, 0);
   setLEDsWait(0, 0, 0);
}

EventHandlerResult SimulatorRecorder::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state)
{   
   if(   (row >= KeyboardHardware.matrix_rows)
      || (col >= KeyboardHardware.matrix_columns)) {
      return EventHandlerResult::OK;
   }
   
   if(keyToggledOn(key_state)) {
      Focus.send(Command::action, SubCommand::key_pressed);
      Focus.send(row, col);
      Focus.sendRaw('\n');
   }
   if(keyToggledOff(key_state)) {
      Focus.send(Command::action, SubCommand::key_released);
      Focus.send(row, col);
      Focus.sendRaw('\n');
   }
   
   return EventHandlerResult::OK;
}

EventHandlerResult SimulatorRecorder::beforeEachCycle()
{
   auto cur_time = Kaleidoscope.millisAtCycleStart();
      
   if(first_use_) {
      
      // Catch any HID reports send to the host
      //
      HID().setSendReportHook(&SimulatorRecorder::sendReportHook);
      this->displayIntro();
      this->sendProtocolHeader();
      first_use_ = false;
   }
   else {
      Focus.send(Command::end_cycle, cycle_id_, cur_time);
      Focus.sendRaw("\n");
      ++cycle_id_;
   }
   
   Focus.send(Command::start_cycle, cycle_id_, cur_time);
   Focus.sendRaw("\n");
   
   return EventHandlerResult::OK;
}

} // namespace plugin
} // namespace kaleidoscope

kaleidoscope::plugin::SimulatorRecorder SimulatorRecorder;
