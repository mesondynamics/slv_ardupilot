#pragma once

#include "AP_YuTong_config.h"

#if HAL_MSP_YUTONG_ENABLED

#include "AP_YuTong.h"
#include "AP_YuTong_Backend.h"

// Data timeout
#define AP_YUTONG_MSP_TIMEOUT_MS 500

class AP_YuTong_MSP : public AP_YuTong_Backend
{

public:
    // constructor
    AP_YuTong_MSP(YuTong::YuTong_State &_state, AP_YuTong_Params &_params);

    // static detection function
    static bool detect();

    // update state
    void update(void) override;

    // Get update from msp
    void handle_msp(const MSP::msp_yutong_data_message_t &pkt) override;

protected:

    MAV_DISTANCE_SENSOR _get_mav_distance_sensor_type() const override {
        return MAV_DISTANCE_SENSOR_UNKNOWN;
    }

private:
    uint16_t distance_cm;

    // start a reading
    static bool start_reading(void);
    static bool get_reading(uint16_t &reading_cm);
};

#endif  //HAL_MSP_YUTONG_ENABLED

