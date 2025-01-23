#pragma once
#include "AP_YuTong_config.h"

#if AP_YUTONG_CUSTOM_CAN_ENABLED

#include "AP_YuTong_Backend_CAN.h"

class AP_YuTong_Custom_CAN : public AP_YuTong_Backend_CAN {
public:
    AP_YuTong_Custom_CAN(YuTong::YuTong_State &_state, AP_YuTong_Params &_params) :
        AP_YuTong_Backend_CAN(_state, _params, AP_CAN::Protocol::YuTong, "yutong")
    {
    }

    // handler for incoming frames
    bool handle_frame(AP_HAL::CANFrame &frame) override;

    static const struct AP_Param::GroupInfo var_info[];
};

#endif  // AP_YUTONG_CUSTOM_CAN_ENABLED
