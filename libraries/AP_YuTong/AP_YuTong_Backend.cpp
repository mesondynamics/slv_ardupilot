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

#if AP_YUTONG_ENABLED

#include <AP_Common/AP_Common.h>
#include <AP_HAL/AP_HAL.h>
#include "AP_YuTong.h"
#include "AP_YuTong_Backend.h"

extern const AP_HAL::HAL& hal;

/*
  base class constructor. 
  This incorporates initialisation as well.
*/
AP_YuTong_Backend::AP_YuTong_Backend(YuTong::YuTong_State &_state, AP_YuTong_Params &_params) :
        state(_state),
		params(_params)
{
    _backend_type = type();
}

MAV_DISTANCE_SENSOR AP_YuTong_Backend::get_mav_distance_sensor_type() const {
    if (type() == YuTong::Type::NONE) {
        return MAV_DISTANCE_SENSOR_UNKNOWN;
    }
    return _get_mav_distance_sensor_type();
}

YuTong::Status AP_YuTong_Backend::status() const {
    if (type() == YuTong::Type::NONE) {
        // turned off at runtime?
        return YuTong::Status::NotConnected;
    }
    return state.status;
}

// true if sensor is returning data
bool AP_YuTong_Backend::has_data() const {
    return ((state.status != YuTong::Status::NotConnected) &&
            (state.status != YuTong::Status::NoData));
}

// update status based on distance measurement
void AP_YuTong_Backend::update_status(YuTong::YuTong_State &state_arg) const
{
    // check distance
    if (state_arg.distance_m > max_distance_cm() * 0.01f) {
        set_status(state_arg, YuTong::Status::OutOfRangeHigh);
    } else if (state_arg.distance_m < min_distance_cm() * 0.01f) {
        set_status(state_arg, YuTong::Status::OutOfRangeLow);
    } else {
        set_status(state_arg, YuTong::Status::Good);
    }
}

// set status and update valid count
void AP_YuTong_Backend::set_status(YuTong::YuTong_State &state_arg, YuTong::Status _status)
{
    state_arg.status = _status;

    // update valid count
    if (_status == YuTong::Status::Good) {
        if (state_arg.range_valid_count < 10) {
            state_arg.range_valid_count++;
        }
    } else {
        state_arg.range_valid_count = 0;
    }
}

#if AP_SCRIPTING_ENABLED
// get a copy of state structure
void AP_YuTong_Backend::get_state(YuTong::YuTong_State &state_arg)
{
    WITH_SEMAPHORE(_sem);
    state_arg = state;
}
#endif

#endif  // AP_YUTONG_ENABLED
