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

#include "Kaleidoscope.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-FocusSerial.h"
#include "HID.h"
#include "Aglais.h"
#include "aglais/v1/Grammar.h"

#define FSH (const __FlashStringHelper*)  //A helper to allow printing the PROGMEM strings.

using namespace aglais::v1;

namespace kaleidoscope {
namespace plugin {
   
void SimulatorRecorder::setLEDs(uint8_t red, uint8_t green, uint8_t blue) {
   LEDControl::set_all_leds_to(red, green, blue);
   LEDControl::syncLeds();
}

HIDReportObserver::SendReportHook SimulatorRecorder::previous_hook_ = nullptr;

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
   
   if(previous_hook_) {
      (*previous_hook_)(id, data, len, result);
   }
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

bool SimulatorRecorder::stageFinished(uint16_t duration) const {
   return Kaleidoscope.hasTimeExpired(t_start_stage_, duration);
}

void SimulatorRecorder::nextStage() {
   t_start_stage_ = Kaleidoscope.millisAtCycleStart();
   ++intro_stage_;
}

void SimulatorRecorder::displayIntro()
{
   switch(intro_stage_) {
      case 0:
         this->nextStage();
         break;
      case 1:
         this->stage(0, 0, 0);
         break;
      case 2:
         this->stage(255, 0, 0);
         break;
      case 3:
         this->stage(0, 0, 0);
         break;
      case 4:
         this->stage(255, 0, 0);
         break;
      case 5:
         this->stage(0, 0, 0);
         break;
      case 6:
         this->stage(255, 0, 0);
         break;
      case 7:
         this->stage(0, 0, 0);
         break;
      case 8:
         this->stage(0, 255, 0);
         break;
      case 9:
         this->stage(0, 255, 0);
         break;
      case 10:
         this->stage(0, 0, 0);
         break;
      case 11:
         previous_hook_ = HIDReportObserver::resetHook(&SimulatorRecorder::sendReportHook);
         this->sendProtocolHeader();
         recording_enabled_ = true; 
         
         // Start cycle command of the first step
         //
         auto cur_time = Kaleidoscope.millisAtCycleStart();
         Focus.send(Command::start_cycle, cycle_id_, cur_time);
         Focus.sendRaw("\n");
         break;
   }
}

EventHandlerResult SimulatorRecorder::onKeyswitchEvent(Key &mapped_key, KeyAddr key_addr, uint8_t key_state)
{   
   if(!key_addr.isValid()) {
      return EventHandlerResult::OK;
   }
   
   if(!recording_enabled_) {
      return EventHandlerResult::OK;
   }
   
   if(keyToggledOn(key_state)) {
      Focus.send(Command::action, SubCommand::key_pressed);
      Focus.send(key_addr.row(), key_addr.col());
      Focus.sendRaw('\n');
   }
   if(keyToggledOff(key_state)) {
      Focus.send(Command::action, SubCommand::key_released);
      Focus.send(key_addr.row(), key_addr.col());
      Focus.sendRaw('\n');
   }
   
   return EventHandlerResult::OK;
}

EventHandlerResult SimulatorRecorder::beforeEachCycle()
{
   auto cur_time = Kaleidoscope.millisAtCycleStart();
      
   if(!recording_enabled_) {
      this->displayIntro();
      return EventHandlerResult::OK;
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
