#pragma once

#include "AP_YuTong_config.h"

#if AP_YUTONG_BACKEND_CAN_ENABLED

#include "AP_YuTong_Backend.h"
#include <AP_CANManager/AP_CANSensor.h>
#include <AP_BoardConfig/AP_BoardConfig.h>

class YuTong_MultiCAN;

class AP_YuTong_Backend_CAN : public AP_YuTong_Backend
{
public:
    // constructor
    AP_YuTong_Backend_CAN(YuTong::YuTong_State &_state,
                                AP_YuTong_Params &_params, AP_CAN::Protocol can_type,
                                const char *driver_name);

    friend class YuTong_MultiCAN;

    static const struct AP_Param::GroupInfo var_info[];

protected:

    // update state
    virtual void update(void) override;

    // get distance measurement
    bool get_reading(float &reading_m);

    // it is essential that anyone relying on the base-class update to implement this
    virtual bool handle_frame(AP_HAL::CANFrame &frame) = 0;

    // maximum time between readings before we change state to NoData:
    virtual uint32_t read_timeout_ms() const { return 200; }

    virtual MAV_DISTANCE_SENSOR _get_mav_distance_sensor_type() const override {
        return MAV_DISTANCE_SENSOR_RADAR;
    }

    // return true if the CAN ID is correct
    bool is_correct_id(uint32_t can_id) const;

    // set distance and count
    void accumulate_distance_m(float distance_m) {
        _distance_sum += distance_m;
        _distance_count++;
    };

    // linked list
    AP_YuTong_Backend_CAN *next;

    AP_Int32 receive_id; // CAN ID to receive for this backend
    AP_Int32 snr_min; // minimum signal strength to accept packet

    MultiCAN* multican_yutong; // Allows for multiple CAN yutongs on a single bus
private:

    float _distance_sum; // meters
    uint32_t _distance_count;
};

#endif  // AP_YUTONG_BACKEND_CAN_ENABLED
