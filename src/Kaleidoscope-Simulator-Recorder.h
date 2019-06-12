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

#include <Kaleidoscope.h>

namespace kaleidoscope {
namespace plugin {
   
class SimulatorRecorder : public Plugin {
   public:

      EventHandlerResult onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state);
      EventHandlerResult beforeEachCycle();
      
      void setFirmwareId(const char *id_string) { id_string_ = id_string; }
      
   private:
      
      static void setLEDsWait(uint8_t red, uint8_t green, uint8_t blue);
      void sendProtocolHeader() const;
      static void displayIntro();
      
      static void sendReportHook(uint8_t id, const void* data, int len, int result);
      
      bool first_use_ = true;
      uint16_t cycle_id_ = 0;
      const char *id_string_ = nullptr;
};
   
} // namespace plugin
} // namespace kaleidoscope

extern kaleidoscope::plugin::SimulatorRecorder SimulatorRecorder;
