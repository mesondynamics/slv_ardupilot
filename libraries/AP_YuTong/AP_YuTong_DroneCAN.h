#pragma once

#include "AP_YuTong_config.h"

#if AP_YUTONG_DRONECAN_ENABLED

#include "AP_YuTong_Backend.h"
#include <AP_DroneCAN/AP_DroneCAN.h>

class MeasurementCb;

class AP_YuTong_DroneCAN : public AP_YuTong_Backend {
public:
    //constructor - registers instance at top YuTong driver
    using AP_YuTong_Backend::AP_YuTong_Backend;

    void update() override;

    static bool subscribe_msgs(AP_DroneCAN* ap_dronecan);
    static AP_YuTong_DroneCAN* get_dronecan_backend(AP_DroneCAN* ap_dronecan, uint8_t node_id, uint8_t address, bool create_new);
    static AP_YuTong_Backend* detect(YuTong::YuTong_State &_state, AP_YuTong_Params &_params);

    static void handle_measurement(AP_DroneCAN *ap_dronecan, const CanardRxTransfer& transfer, const uavcan_equipment_range_sensor_Measurement &msg);

protected:
    virtual MAV_DISTANCE_SENSOR _get_mav_distance_sensor_type() const override {
        return _sensor_type;
    }
private:
    uint8_t _instance;
    YuTong::Status _status;
    uint16_t _distance_cm;
    uint32_t _last_reading_ms;
    AP_DroneCAN* _ap_dronecan;
    uint8_t _node_id;
    bool new_data;
    MAV_DISTANCE_SENSOR _sensor_type;
};
#endif  // AP_YUTONG_DRONECAN_ENABLED
