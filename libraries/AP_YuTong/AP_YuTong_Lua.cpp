/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AP_YuTong_config.h"

#if AP_YUTONG_LUA_ENABLED

#include "AP_YuTong_Lua.h"
#include <AP_HAL/AP_HAL.h>

// constructor
AP_YuTong_Lua::AP_YuTong_Lua(YuTong::YuTong_State &_state, AP_YuTong_Params &_params) :
    AP_YuTong_Backend(_state, _params)
{
}


// Process range finder data from a lua driver. The state structure needs to be completely
// filled in by the lua script. The data passed to this method is copied to a pending_state
// structure. The update() method periodically copies data from pending_state to state. The get_state()
// method returns data from state.
bool AP_YuTong_Lua::handle_script_msg(const YuTong::YuTong_State &state_arg)
{
    WITH_SEMAPHORE(_sem);
    _state_pending = state_arg;
    return true;
}

// Process range finder data from a lua driver - legacy interface. This method takes
// a distance measurement and fills in the pending state structure. In this legacy mode
// the lua script only passes in a distance measurement and does not manage the rest
// of the fields in the state structure.
bool AP_YuTong_Lua::handle_script_msg(float dist_m) {

    const uint32_t now = AP_HAL::millis();

    WITH_SEMAPHORE(_sem);

    _state_pending.last_reading_ms = now;
    _state_pending.distance_m = dist_m;
    _state_pending.signal_quality_pct = YuTong::SIGNAL_QUALITY_UNKNOWN;
    _state_pending.voltage_mv = 0;
    update_status(_state_pending);

    return true;
}

// Update the state of the sensor
void AP_YuTong_Lua::update(void)
{
    WITH_SEMAPHORE(_sem);

    // Time out on incoming data
    if (_state_pending.status != YuTong::Status::NotConnected &&
            AP_HAL::millis() - _state_pending.last_reading_ms > AP_YUTONG_LUA_TIMEOUT_MS) {
        set_status(_state_pending, YuTong::Status::NoData);
    }
    state = _state_pending;
}

#endif  // AP_YUTONG_LUA_ENABLED
