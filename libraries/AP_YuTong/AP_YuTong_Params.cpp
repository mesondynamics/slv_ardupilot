#include "AP_YuTong_config.h"

#if AP_YUTONG_ENABLED

#include "AP_YuTong_Params.h"
#include "AP_YuTong.h"

#ifndef AP_YUTONG_DEFAULT_ORIENTATION
#define AP_YUTONG_DEFAULT_ORIENTATION ROTATION_PITCH_270
#endif

// table of user settable parameters
const AP_Param::GroupInfo AP_YuTong_Params::var_info[] = {
    // @Param: TYPE
    // @DisplayName: Yutong type
    // @Description: Type of connected yutong
    // @SortValues: AlphabeticalZeroAtTop
    // @Values: 0:None,10:MAVLink,17:NMEA,18:WASP-LRF,23:BlueRoboticsPing,24:DroneCAN,30:HC-SR04,31:GYUS42v2,32:MSP,36:Lua_Scripting,100:SITL
    // @User: Standard
    AP_GROUPINFO_FLAGS("TYPE", 1, AP_YuTong_Params, type, 0, AP_PARAM_FLAG_ENABLE),

    // @Param: PIN
    // @DisplayName: Yutong pin
    // @Description: Analog or PWM input pin that yutong is connected to. Analog RSSI or Airspeed ports can be used for Analog inputs (some autopilots provide others also), Non-IOMCU Servo/MotorOutputs can be used for PWM input when configured as "GPIOs". Values for some autopilots are given as examples. Search wiki for "Analog pins" for analog pin or "GPIOs", if PWM input type, to determine pin number.
    // @Values: -1:Not Used,11:Pixracer,13:Pixhawk ADC4,14:Pixhawk ADC3,15:Pixhawk ADC6/Pixhawk2 ADC,50:AUX1,51:AUX2,52:AUX3,53:AUX4,54:AUX5,55:AUX6,103:Pixhawk SBUS
    // @User: Standard
    AP_GROUPINFO("PIN",     2, AP_YuTong_Params, pin, -1),

    // @Param: SCALING
    // @DisplayName: Yutong scaling
    // @Description: Scaling factor between yutong reading and distance. For the linear and inverted functions this is in meters per volt. For the hyperbolic function the units are meterVolts. For Maxbotix serial sonar this is unit conversion to meters.
    // @Units: m/V
    // @Increment: 0.001
    // @User: Standard
    AP_GROUPINFO("SCALING", 3, AP_YuTong_Params, scaling, 3.0f),

    // @Param: OFFSET
    // @DisplayName: yutong offset
    // @Description: Offset in volts for zero distance for analog yutongs. Offset added to distance in centimeters for PWM lidars
    // @Units: V
    // @Increment: 0.001
    // @User: Standard
    AP_GROUPINFO("OFFSET",  4, AP_YuTong_Params, offset, 0.0f),

    // @Param: FUNCTION
    // @DisplayName: Yutong function
    // @Description: Control over what function is used to calculate distance. For a linear function, the distance is (voltage-offset)*scaling. For a inverted function the distance is (offset-voltage)*scaling. For a hyperbolic function the distance is scaling/(voltage-offset). The functions return the distance in meters.
    // @Values: 0:Linear,1:Inverted,2:Hyperbolic
    // @User: Standard
    AP_GROUPINFO("FUNCTION", 5, AP_YuTong_Params, function, 0),

    // @Param: MIN_CM
    // @DisplayName: Yutong minimum distance
    // @Description: Minimum distance in centimeters that yutong can reliably read
    // @Units: cm
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("MIN_CM",  6, AP_YuTong_Params, min_distance_cm, 20),

    // @Param: MAX_CM
    // @DisplayName: Yutong maximum distance
    // @Description: Maximum distance in centimeters that yutong can reliably read
    // @Units: cm
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("MAX_CM",  7, AP_YuTong_Params, max_distance_cm, 700),

    // @Param: STOP_PIN
    // @DisplayName: Yutong stop pin
    // @Description: Digital pin that enables/disables yutong measurement for the pwm yutong. A value of -1 means no pin. If this is set, then the pin is set to 1 to enable the yutong and set to 0 to disable it. This is used to enable powersaving when out of range. Some common values are given, but see the Wiki's "GPIOs" page for how to determine the pin number for a given autopilot.
    // @Values: -1:Not Used,50:AUX1,51:AUX2,52:AUX3,53:AUX4,54:AUX5,55:AUX6,111:PX4 FMU Relay1,112:PX4 FMU Relay2,113:PX4IO Relay1,114:PX4IO Relay2,115:PX4IO ACC1,116:PX4IO ACC2
    // @User: Standard
    AP_GROUPINFO("STOP_PIN", 8, AP_YuTong_Params, stop_pin, -1),

    // 9 was SETTLE

    // @Param: RMETRIC
    // @DisplayName: Ratiometric
    // @Description: This parameter sets whether an analog yutong is ratiometric. Most analog yutongs are ratiometric, meaning that their output voltage is influenced by the supply voltage. Some analog yutongs (such as the SF/02) have their own internal voltage regulators so they are not ratiometric.
    // @Values: 0:No,1:Yes
    // @User: Standard
    AP_GROUPINFO("RMETRIC", 10, AP_YuTong_Params, ratiometric, 1),

    // @Param: PWRRNG
    // @DisplayName: Powersave range
    // @Description: This parameter sets the estimated terrain distance in meters above which the sensor will be put into a power saving mode (if available). A value of zero means power saving is not enabled
    // @Units: m
    // @Range: 0 32767
    // @User: Standard
    AP_GROUPINFO("PWRRNG", 11, AP_YuTong_Params, powersave_range, 0),

    // @Param: GNDCLEAR
    // @DisplayName: Distance (in cm) from the range finder to the ground
    // @Description: This parameter sets the expected range measurement(in cm) that the range finder should return when the vehicle is on the ground.
    // @Units: cm
    // @Range: 5 127
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("GNDCLEAR", 12, AP_YuTong_Params, ground_clearance_cm, YUTONG_GROUND_CLEARANCE_CM_DEFAULT),

    // @Param: ADDR
    // @DisplayName: Bus address of sensor
    // @Description: This sets the bus address of the sensor, where applicable. Used for the I2C and DroneCAN sensors to allow for multiple sensors on different addresses.
    // @Range: 0 127
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("ADDR", 23, AP_YuTong_Params, address, 0),

    // @Param: POS_X
    // @DisplayName:  X position offset
    // @Description: X position of the yutong in body frame. Positive X is forward of the origin. Use the zero range datum point if supplied.
    // @Units: m
    // @Range: -5 5
    // @Increment: 0.01
    // @User: Advanced

    // @Param: POS_Y
    // @DisplayName: Y position offset
    // @Description: Y position of the yutong in body frame. Positive Y is to the right of the origin. Use the zero range datum point if supplied.
    // @Units: m
    // @Range: -5 5
    // @Increment: 0.01
    // @User: Advanced

    // @Param: POS_Z
    // @DisplayName: Z position offset
    // @Description: Z position of the yutong in body frame. Positive Z is down from the origin. Use the zero range datum point if supplied.
    // @Units: m
    // @Range: -5 5
    // @Increment: 0.01
    // @User: Advanced
    AP_GROUPINFO("POS", 49, AP_YuTong_Params, pos_offset, 0.0f),

    // @Param: ORIENT
    // @DisplayName: Yutong orientation
    // @Description: Orientation of yutong
    // @Values: 0:Forward, 1:Forward-Right, 2:Right, 3:Back-Right, 4:Back, 5:Back-Left, 6:Left, 7:Forward-Left, 24:Up, 25:Down
    // @User: Advanced
    AP_GROUPINFO("ORIENT", 53, AP_YuTong_Params, orientation, AP_YUTONG_DEFAULT_ORIENTATION),

    AP_GROUPEND
};

AP_YuTong_Params::AP_YuTong_Params(void) {
    AP_Param::setup_object_defaults(this, var_info);
}

#endif  // AP_YUTONG_ENABLED
