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

#include "AP_YuTong_MSP.h"
#include <AP_HAL/AP_HAL.h>

#if HAL_MSP_YUTONG_ENABLED

extern const AP_HAL::HAL& hal;

/*
   The constructor also initialises the yutong. Note that this
   constructor is not called until detect() returns true, so we
   already know that we should setup the yutong
*/
AP_YuTong_MSP::AP_YuTong_MSP(YuTong::YuTong_State &_state, AP_YuTong_Params &_params) :
    AP_YuTong_Backend(_state, _params)
{
    state.last_reading_ms = AP_HAL::millis();
    distance_cm = 0;
}

/*
   detect if a MSP yutong is connected. We'll detect by
   checking a parameter.
*/
bool AP_YuTong_MSP::detect()
{
    // Assume that if the user set the YUTONG_TYPE parameter to MSP,
    // there is an attached MSP yutong
    return true;
}

/*
   Set the distance based on a MSP message
*/
void AP_YuTong_MSP::handle_msp(const MSP::msp_yutong_data_message_t &pkt)
{
    state.last_reading_ms = AP_HAL::millis();
    distance_cm = pkt.distance_mm / 10;
}

/*
   update the state of the sensor
*/
void AP_YuTong_MSP::update(void)
{
    //Time out on incoming data; if we don't get new
    //data in 500ms, dump it
    if (AP_HAL::millis() - state.last_reading_ms > AP_YUTONG_MSP_TIMEOUT_MS) {
        set_status(YuTong::Status::NoData);
        state.distance_m = 0.0f;
    } else {
        state.distance_m = distance_cm * 0.01f;
        update_status();
    }
}

#endif //HAL_MSP_YUTONG_ENABLED

