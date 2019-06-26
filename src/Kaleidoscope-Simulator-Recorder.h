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

#pragma once

#include "Kaleidoscope.h"
#include "HIDReportObserver.h"

namespace kaleidoscope {
namespace plugin {
   
class SimulatorRecorder : public Plugin {
   public:

      EventHandlerResult onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state);
      EventHandlerResult beforeEachCycle();
      
      void setFirmwareId(const char *id_string) { id_string_ = id_string; }
      
   private:
      
      bool stageFinished(uint16_t duration) const;
      void nextStage();
      
      void stage(uint8_t red, uint8_t green, uint8_t blue) {
         if(stageFinished(stage_duration_)) {
            this->setLEDs(red, green, blue);
            this->nextStage();
         }
      }
      
      static void setLEDs(uint8_t red, uint8_t green, uint8_t blue);
      void sendProtocolHeader() const;
      void displayIntro();
      
      static void sendReportHook(uint8_t id, const void* data, int len, int result);

      static constexpr uint16_t stage_duration_ = 500;
      uint8_t intro_stage_ = 0;
      uint16_t t_start_stage_ = 0;
      bool recording_enabled_ = false;
      uint32_t cycle_id_ = 0;
      const char *id_string_ = nullptr;
      static HIDReportObserver::SendReportHook previous_hook_;
};
   
} // namespace plugin
} // namespace kaleidoscope

extern kaleidoscope::plugin::SimulatorRecorder SimulatorRecorder;
