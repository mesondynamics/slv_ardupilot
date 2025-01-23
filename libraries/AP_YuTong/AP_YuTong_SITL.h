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
#pragma once

#include "AP_YuTong_config.h"

#if AP_YUTONG_SIM_ENABLED

#include "AP_YuTong_Backend.h"

class AP_YuTong_SITL : public AP_YuTong_Backend {
public:
    // constructor. This incorporates initialisation as well.
    AP_YuTong_SITL(YuTong::YuTong_State &_state, AP_YuTong_Params &_params, uint8_t instance);

    // update the state structure
    void update() override;

protected:

    MAV_DISTANCE_SENSOR _get_mav_distance_sensor_type() const override {
        return MAV_DISTANCE_SENSOR_UNKNOWN;
    }

    uint8_t _instance;

};

#endif  // AP_YUTONG_SIM_ENABLED
