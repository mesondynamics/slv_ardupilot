#pragma once

#include "AP_YuTong_config.h"

#if AP_YUTONG_LUA_ENABLED

#include "AP_YuTong_Backend.h"

// Data timeout
#define AP_YUTONG_LUA_TIMEOUT_MS 500

class AP_YuTong_Lua : public AP_YuTong_Backend
{
public:

    // constructor
    AP_YuTong_Lua(YuTong::YuTong_State &_state, AP_YuTong_Params &_params);

    // update state
    void update(void) override;

    // Get update from Lua script
    bool handle_script_msg(float dist_m) override;
    bool handle_script_msg(const YuTong::YuTong_State &state_arg) override;

    MAV_DISTANCE_SENSOR _get_mav_distance_sensor_type() const override {
        return MAV_DISTANCE_SENSOR_UNKNOWN;
    }

private:

    YuTong::YuTong_State _state_pending = {};
};

#endif  // AP_YUTONG_LUA_ENABLED
