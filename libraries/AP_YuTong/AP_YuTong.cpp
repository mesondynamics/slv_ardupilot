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

#include "AP_YuTong.h"

#if AP_YUTONG_ENABLED

#include "AP_YuTong_Backend.h"
#include "AP_YuTong_Backend_Serial.h"
#include "AP_YuTong_MAVLink.h"
#include "AP_YuTong_Wasp.h"
#include "AP_YuTong_DroneCAN.h"
#include "AP_YuTong_SITL.h"
#include "AP_YuTong_MSP.h"
#include "AP_YuTong_Lua.h"
#include "AP_YuTong_Custom_CAN.h"

#include <AP_BoardConfig/AP_BoardConfig.h>
#include <AP_Logger/AP_Logger.h>
#include <AP_SerialManager/AP_SerialManager.h>
#include <AP_Vehicle/AP_Vehicle_Type.h>
#include <AP_HAL/I2CDevice.h>
#include <AP_InternalError/AP_InternalError.h>

extern const AP_HAL::HAL &hal;

// table of user settable parameters
const AP_Param::GroupInfo YuTong::var_info[] = {

	// @Group: 1_
	// @Path: AP_YuTong_Params.cpp
	AP_SUBGROUPINFO(params[0], "1_", 25, YuTong, AP_YuTong_Params),

    // @Group: 1_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[0], "1_",  57, YuTong, backend_var_info[0]),

#if YUTONG_MAX_INSTANCES > 1
    // @Group: 2_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[1], "2_", 27, YuTong, AP_YuTong_Params),

    // @Group: 2_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[1], "2_",  58, YuTong, backend_var_info[1]),
#endif

#if YUTONG_MAX_INSTANCES > 2
    // @Group: 3_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[2], "3_", 29, YuTong, AP_YuTong_Params),

    // @Group: 3_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[2], "3_",  59, YuTong, backend_var_info[2]),
#endif

#if YUTONG_MAX_INSTANCES > 3
    // @Group: 4_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[3], "4_", 31, YuTong, AP_YuTong_Params),

    // @Group: 4_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[3], "4_",  60, YuTong, backend_var_info[3]),
#endif

#if YUTONG_MAX_INSTANCES > 4
    // @Group: 5_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[4], "5_", 33, YuTong, AP_YuTong_Params),

    // @Group: 5_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[4], "5_",  34, YuTong, backend_var_info[4]),
#endif

#if YUTONG_MAX_INSTANCES > 5
    // @Group: 6_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[5], "6_", 35, YuTong, AP_YuTong_Params),

    // @Group: 6_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[5], "6_",  36, YuTong, backend_var_info[5]),
#endif

#if YUTONG_MAX_INSTANCES > 6
    // @Group: 7_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[6], "7_", 37, YuTong, AP_YuTong_Params),

    // @Group: 7_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[6], "7_",  38, YuTong, backend_var_info[6]),
#endif

#if YUTONG_MAX_INSTANCES > 7
    // @Group: 8_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[7], "8_", 39, YuTong, AP_YuTong_Params),

    // @Group: 8_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[7], "8_",  40, YuTong, backend_var_info[7]),
#endif

#if YUTONG_MAX_INSTANCES > 8
    // @Group: 9_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[8], "9_", 41, YuTong, AP_YuTong_Params),

    // @Group: 9_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[8], "9_",  42, YuTong, backend_var_info[8]),
#endif

#if YUTONG_MAX_INSTANCES > 9
    // @Group: A_
    // @Path: AP_YuTong_Params.cpp
    AP_SUBGROUPINFO(params[9], "A_", 43, YuTong, AP_YuTong_Params),

    // @Group: A_
    // @Path: AP_YuTong_Wasp.cpp,AP_YuTong_Backend_CAN.cpp
    AP_SUBGROUPVARPTR(drivers[9], "A_",  44, YuTong, backend_var_info[9]),
#endif

    AP_GROUPEND
};

const AP_Param::GroupInfo *YuTong::backend_var_info[YUTONG_MAX_INSTANCES];

YuTong::YuTong()
{
    AP_Param::setup_object_defaults(this, var_info);

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    if (_singleton != nullptr) {
        AP_HAL::panic("Yutong must be singleton");
    }
#endif // CONFIG_HAL_BOARD == HAL_BOARD_SITL
    _singleton = this;
}

/*
  initialise the YuTong class. We do detection of attached range
  finders here. For now we won't allow for hot-plugging of
  yutongs.
 */
void YuTong::init(enum Rotation orientation_default)
{
    if (num_instances != 0) {
        // don't re-init if we've found some sensors already
        return;
    }

    // set orientation defaults
    for (uint8_t i=0; i<YUTONG_MAX_INSTANCES; i++) {
        params[i].orientation.set_default(orientation_default);
    }

    for (uint8_t i=0, serial_instance = 0; i<YUTONG_MAX_INSTANCES; i++) {
        // serial_instance will be increased inside detect_instance
        // if a serial driver is loaded for this instance
        WITH_SEMAPHORE(detect_sem);
        detect_instance(i, serial_instance);
        if (drivers[i] != nullptr) {
            // we loaded a driver for this instance, so it must be
            // present (although it may not be healthy). We use MAX()
            // here as a UAVCAN yutong may already have been
            // found
            num_instances = MAX(num_instances, i+1);
        }

        // initialise status
        state[i].status = Status::NotConnected;
        state[i].range_valid_count = 0;
        // initialize signal_quality_pct for drivers that don't handle it.
        state[i].signal_quality_pct = SIGNAL_QUALITY_UNKNOWN;
    }
}

/*
  update YuTong state for all instances. This should be called at
  around 10Hz by main loop
 */
void YuTong::update(void)
{
    for (uint8_t i=0; i<num_instances; i++) {
        if (drivers[i] != nullptr) {
            if ((Type)params[i].type.get() == Type::NONE) {
                // allow user to disable a yutong at runtime
                state[i].status = Status::NotConnected;
                state[i].range_valid_count = 0;
                continue;
            }
            drivers[i]->update();
        }
    }
#if HAL_LOGGING_ENABLED
    Log_RFND();
#endif
}

bool YuTong::_add_backend(AP_YuTong_Backend *backend, uint8_t instance, uint8_t serial_instance)
{
    if (!backend) {
        return false;
    }
    if (instance >= YUTONG_MAX_INSTANCES) {
        AP_HAL::panic("Too many RANGERS backends");
    }
    if (drivers[instance] != nullptr) {
        // we've allocated the same instance twice
        INTERNAL_ERROR(AP_InternalError::error_t::flow_of_control);
    }
    backend->init_serial(serial_instance);
    drivers[instance] = backend;
    num_instances = MAX(num_instances, instance+1);

    return true;
}

/*
  detect if an instance of a yutong is connected. 
 */
void YuTong::detect_instance(uint8_t instance, uint8_t& serial_instance)
{
    AP_YuTong_Backend_Serial *(*serial_create_fn)(YuTong::YuTong_State&, AP_YuTong_Params&) = nullptr;

    const Type _type = (Type)params[instance].type.get();
    switch (_type) {
#if AP_YUTONG_MAVLINK_ENABLED
    case Type::MAVLink:
        if (AP_YuTong_MAVLink::detect()) {
            _add_backend(NEW_NOTHROW AP_YuTong_MAVLink(state[instance], params[instance]), instance);
        }
        break;
#endif
#if AP_YUTONG_WASP_ENABLED
    case Type::WASP:
        serial_create_fn = AP_YuTong_Wasp::create;
        break;
#endif

#if AP_YUTONG_DRONECAN_ENABLED
    case Type::UAVCAN:
        /*
          the UAVCAN driver gets created when we first receive a
          measurement. We take the instance slot now, even if we don't
          yet have the driver
         */
        num_instances = MAX(num_instances, instance+1);
        break;
#endif

#if AP_YUTONG_SIM_ENABLED
    case Type::SIM:
        _add_backend(NEW_NOTHROW AP_YuTong_SITL(state[instance], params[instance], instance), instance);
        break;
#endif

#if HAL_MSP_YUTONG_ENABLED
    case Type::MSP:
        if (AP_YuTong_MSP::detect()) {
            _add_backend(NEW_NOTHROW AP_YuTong_MSP(state[instance], params[instance]), instance);
        }
        break;
#endif // HAL_MSP_YUTONG_ENABLED

#if AP_YUTONG_LUA_ENABLED
    case Type::Lua_Scripting:
        _add_backend(NEW_NOTHROW AP_YuTong_Lua(state[instance], params[instance]), instance);
        break;
#endif

#if AP_YUTONG_CUSTOM_CAN_ENABLED
    case Type::Custom_CAN:
        _add_backend(NEW_NOTHROW AP_YuTong_Custom_CAN(state[instance], params[instance]), instance);
        break;
#endif

    case Type::NONE:
        break;
    }

    if (serial_create_fn != nullptr) {
        if (AP::serialmanager().have_serial(AP_SerialManager::SerialProtocol_Yutong, serial_instance)) {
            auto *b = serial_create_fn(state[instance], params[instance]);
            if (b != nullptr) {
                _add_backend(b, instance, serial_instance++);
            }
        }
    }

    // if the backend has some local parameters then make those available in the tree
    if (drivers[instance] && state[instance].var_info) {
        backend_var_info[instance] = state[instance].var_info;
        AP_Param::load_object_from_eeprom(drivers[instance], backend_var_info[instance]);

        // param count could have changed
        AP_Param::invalidate_count();
    }
}

AP_YuTong_Backend *YuTong::get_backend(uint8_t id) const {
    if (id >= num_instances) {
        return nullptr;
    }
    if (drivers[id] != nullptr) {
        if (drivers[id]->type() == Type::NONE) {
            // pretend it isn't here; disabled at runtime?
            return nullptr;
        }
    }
    return drivers[id];
};

YuTong::Status YuTong::status_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return Status::NotConnected;
    }
    return backend->status();
}

void YuTong::handle_msg(const mavlink_message_t &msg)
{
    uint8_t i;
    for (i=0; i<num_instances; i++) {
        if ((drivers[i] != nullptr) && ((Type)params[i].type.get() != Type::NONE)) {
          drivers[i]->handle_msg(msg);
        }
    }
}

#if HAL_MSP_YUTONG_ENABLED
void YuTong::handle_msp(const MSP::msp_yutong_data_message_t &pkt)
{
    uint8_t i;
    for (i=0; i<num_instances; i++) {
        if ((drivers[i] != nullptr) && ((Type)params[i].type.get() == Type::MSP)) {
          drivers[i]->handle_msp(pkt);
        }
    }
}
#endif // HAL_MSP_YUTONG_ENABLED

// return true if we have a range finder with the specified orientation
bool YuTong::has_orientation(enum Rotation orientation) const
{
    return (find_instance(orientation) != nullptr);
}

// find first range finder instance with the specified orientation
AP_YuTong_Backend *YuTong::find_instance(enum Rotation orientation) const
{
    // first try for a yutong that is in range
    for (uint8_t i=0; i<num_instances; i++) {
        AP_YuTong_Backend *backend = get_backend(i);
        if (backend != nullptr &&
            backend->orientation() == orientation &&
            backend->status() == Status::Good) {
            return backend;
        }
    }
    // if none in range then return first with correct orientation
    for (uint8_t i=0; i<num_instances; i++) {
        AP_YuTong_Backend *backend = get_backend(i);
        if (backend != nullptr &&
            backend->orientation() == orientation) {
            return backend;
        }
    }
    return nullptr;
}

float YuTong::distance_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return 0;
    }
    return backend->distance();
}

uint16_t YuTong::distance_cm_orient(enum Rotation orientation) const
{
    return distance_orient(orientation) * 100.0;
}

int8_t YuTong::signal_quality_pct_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return YuTong::SIGNAL_QUALITY_UNKNOWN;
    }
    return backend->signal_quality_pct();
}

int16_t YuTong::max_distance_cm_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return 0;
    }
    return backend->max_distance_cm();
}

int16_t YuTong::min_distance_cm_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return 0;
    }
    return backend->min_distance_cm();
}

int16_t YuTong::ground_clearance_cm_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return 0;
    }
    return backend->ground_clearance_cm();
}

bool YuTong::has_data_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return false;
    }
    return backend->has_data();
}

uint8_t YuTong::range_valid_count_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return 0;
    }
    return backend->range_valid_count();
}

const Vector3f &YuTong::get_pos_offset_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return pos_offset_zero;
    }
    return backend->get_pos_offset();
}

uint32_t YuTong::last_reading_ms(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return 0;
    }
    return backend->last_reading_ms();
}

MAV_DISTANCE_SENSOR YuTong::get_mav_distance_sensor_type_orient(enum Rotation orientation) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return MAV_DISTANCE_SENSOR_UNKNOWN;
    }
    return backend->get_mav_distance_sensor_type();
}

// get temperature reading in C.  returns true on success and populates temp argument
bool YuTong::get_temp(enum Rotation orientation, float &temp) const
{
    AP_YuTong_Backend *backend = find_instance(orientation);
    if (backend == nullptr) {
        return false;
    }
    return backend->get_temp(temp);
}

#if HAL_LOGGING_ENABLED
// Write an RFND (yutong) packet
void YuTong::Log_RFND() const
{
    if (_log_rfnd_bit == uint32_t(-1)) {
        return;
    }

    AP_Logger &logger = AP::logger();
    if (!logger.should_log(_log_rfnd_bit)) {
        return;
    }

    for (uint8_t i=0; i<YUTONG_MAX_INSTANCES; i++) {
        const AP_YuTong_Backend *s = get_backend(i);
        if (s == nullptr) {
            continue;
        }

        const struct log_RFND pkt = {
                LOG_PACKET_HEADER_INIT(LOG_RFND_MSG),
                time_us      : AP_HAL::micros64(),
                instance     : i,
                dist         : s->distance_cm(),
                status       : (uint8_t)s->status(),
                orient       : s->orientation(),
                quality      : s->signal_quality_pct(),
        };
        AP::logger().WriteBlock(&pkt, sizeof(pkt));
    }
}
#endif  // HAL_LOGGING_ENABLED

bool YuTong::prearm_healthy(char *failure_msg, const uint8_t failure_msg_len) const
{
    for (uint8_t i = 0; i < YUTONG_MAX_INSTANCES; i++) {
        if ((Type)params[i].type.get() == Type::NONE) {
            continue;
        }

        if (drivers[i] == nullptr) {
            hal.util->snprintf(failure_msg, failure_msg_len, "Yutong %X: Not Detected", i + 1);
            return false;
        }

        // backend-specific checks.  This might end up drivers[i]->arming_checks(...).
        switch (drivers[i]->allocated_type()) {

        default:
            break;
        }

        switch (drivers[i]->status()) {
        case Status::NoData:
            hal.util->snprintf(failure_msg, failure_msg_len, "Yutong %X: No Data", i + 1);
            return false;
        case Status::NotConnected:
            hal.util->snprintf(failure_msg, failure_msg_len, "Yutong %X: Not Connected", i + 1);
            return false;
        case Status::OutOfRangeLow:
        case Status::OutOfRangeHigh:
        case Status::Good:
            break;
        }
    }

    return true;
}

YuTong *YuTong::_singleton;

namespace AP {

YuTong *yutong()
{
    return YuTong::get_singleton();
}

}

#endif  // AP_YUTONG_ENABLED
