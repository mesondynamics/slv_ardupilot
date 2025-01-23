#include "AP_YuTong_config.h"

#if AP_YUTONG_CUSTOM_CAN_ENABLED

#include "AP_YuTong_Custom_CAN.h"
#include <AP_BoardConfig/AP_BoardConfig.h>
#include <AP_HAL/utility/sparse-endian.h>
#include <AP_HAL/AP_HAL.h>

extern const AP_HAL::HAL& hal;

// handler for incoming frames. These come in at 10-30Hz
bool AP_YuTong_Custom_CAN::handle_frame(AP_HAL::CANFrame &frame)
{
    // hal.console->printf("YuTong Custom CAN Raw: [%lx] %02x %02x %02x %02x %02x %02x %02x %02x\n", frame.id & ~0x80000000, frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7]);

    WITH_SEMAPHORE(_sem);
    // const uint32_t id = frame.id - 0x200U;

    // if (!is_correct_id(id)) {
    //     return false;
    // }

    const int32_t dist_mm = (int32_t)(frame.data[0] << 8U | frame.data[1] << 16U | frame.data[2] << 24U) >> 8;
    // const uint8_t status = frame.data[3];
    // const uint16_t snr = le16toh_ptr(&frame.data[4]);

    // hal.console->printf("ID: %lx, dist: %ld, status: %d, snr: %d\n", id, dist_mm, status, snr);

    // if ((snr_min != 0 && snr < uint16_t(snr_min.get())) || status > 0) {
    //     // too low signal strength or bad status
    //     return false;
    // }

    state.soc = (int32_t)(frame.data[0])*0.4;
    state.speed_kmh = (int32_t)(frame.data[2]);
    state.motor_temperature = (int32_t)(frame.data[3])-40.0;
    state.gear = (uint8_t)((frame.data[4] >> 5) & 0x07);

    accumulate_distance_m(dist_mm * 0.001);
    return true;
}

#endif  // AP_YUTONG_CUSTOM_CAN_ENABLED
