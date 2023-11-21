
// setup
#include <Arduino.h>
#if !defined(ESP32) && !defined(SAMD_SERIES) && !defined(ARDUINO_RASPBERRY_PI_PICO)
#define SOFTWARE_SERIAL_IMPLEMENTED true
#else
#define SOFTWARE_SERIAL_IMPLEMENTED false
#endif
#if SOFTWARE_SERIAL_IMPLEMENTED
#include <SoftwareSerial.h>
#endif

class TMC2209base
{
public:
    //********************* Settings *********************//
    // Serial Settings
    const static uint8_t BITS_PER_BYTE = 8;
    const static uint32_t ECHO_DELAY_INC_MICROSECONDS = 1;
    const static uint32_t REPLY_DELAY_INC_MICROSECONDS = 1;

    //********************* Limits *********************//

    // Serial
    const static uint8_t BYTE_MAX_VALUE = 0xFF;
    const static uint32_t ECHO_DELAY_MAX_MICROSECONDS = 4000;
    const static uint32_t REPLY_DELAY_MAX_MICROSECONDS = 10000;

    const static uint8_t CURRENT_SCALING_MAX = 31;
    const static uint8_t PERCENT_MIN = 0;
    const static uint8_t PERCENT_MAX = 100;
    const static uint8_t CURRENT_SETTING_MIN = 10;
    const static uint8_t CURRENT_SETTING_MAX = 31;
    const static uint8_t HOLD_DELAY_MIN = 0;
    const static uint8_t HOLD_DELAY_MAX = 15;

    const static uint8_t SEIMIN_UPPER_CURRENT_LIMIT = 20;

    const static uint8_t SEMIN_MIN = 1;
    const static uint8_t SEMIN_MAX = 15;
    const static uint8_t SEMAX_MIN = 0;
    const static uint8_t SEMAX_MAX = 15;

    const static size_t MICROSTEPS_PER_STEP_MIN = 1;
    const static size_t MICROSTEPS_PER_STEP_MAX = 256;

    const static uint8_t PWM_OFFSET_MIN = 0;
    const static uint8_t PWM_OFFSET_MAX = 255;

    const static uint8_t PWM_GRAD_MIN = 0;
    const static uint8_t PWM_GRAD_MAX = 255;

    //********************* Defines *********************//

    // addresses
    const static uint8_t ADDRESS_GCONF = 0x00;
    const static uint8_t ADDRESS_GSTAT = 0x01;
    const static uint8_t ADDRESS_IFCNT = 0x02;
    const static uint8_t ADDRESS_REPLYDELAY = 0x03;
    const static uint8_t ADDRESS_IOIN = 0x06;
    const static uint8_t ADDRESS_IHOLD_IRUN = 0x10;
    const static uint8_t ADDRESS_TPOWERDOWN = 0x11;
    const static uint8_t ADDRESS_TSTEP = 0x12;
    const static uint8_t ADDRESS_TPWMTHRS = 0x13;
    const static uint8_t ADDRESS_VACTUAL = 0x22;
    const static uint8_t ADDRESS_TCOOLTHRS = 0x14;
    const static uint8_t ADDRESS_SGTHRS = 0x40;
    const static uint8_t ADDRESS_SG_RESULT = 0x41;
    const static uint8_t ADDRESS_COOLCONF = 0x42;
    const static uint8_t ADDRESS_MSCNT = 0x6A;
    const static uint8_t ADDRESS_MSCURACT = 0x6B;
    const static uint8_t ADDRESS_CHOPCONF = 0x6C;
    const static uint8_t ADDRESS_DRV_STATUS = 0x6F;
    const static uint8_t ADDRESS_PWMCONF = 0x70;
    const static uint8_t ADDRESS_PWM_SCALE = 0x71;
    const static uint8_t ADDRESS_PWM_AUTO = 0x72;

    const static uint8_t STEPPER_DRIVER_FEATURE_OFF = 0;
    const static uint8_t STEPPER_DRIVER_FEATURE_ON = 1;

    // Datagrams
    const static uint8_t WRITE_READ_REPLY_DATAGRAM_SIZE = 8;
    const static uint8_t DATA_SIZE = 4;
    const static uint8_t READ_REQUEST_DATAGRAM_SIZE = 4;

    const static uint8_t SYNC = 0b101;
    const static uint8_t RW_READ = 0;
    const static uint8_t RW_WRITE = 1;
    const static uint8_t READ_REPLY_SERIAL_ADDRESS = 0b11111111;

    const static int32_t VACTUAL_DEFAULT = 0;
    const static int32_t VACTUAL_STEP_DIR_INTERFACE = 0;

    const static uint8_t SEIMIN_LOWER_SETTING = 0;
    const static uint8_t SEIMIN_UPPER_SETTING = 1;
    const static uint8_t SEMIN_OFF = 0;

    const static uint8_t TOFF_DISABLE = 0;

    const static uint8_t MRES_256 = 0b0000;
    const static uint8_t MRES_128 = 0b0001;
    const static uint8_t MRES_064 = 0b0010;
    const static uint8_t MRES_032 = 0b0011;
    const static uint8_t MRES_016 = 0b0100;
    const static uint8_t MRES_008 = 0b0101;
    const static uint8_t MRES_004 = 0b0110;
    const static uint8_t MRES_002 = 0b0111;
    const static uint8_t MRES_001 = 0b1000;

    const static uint8_t VERSION = 0x21;

    //********************* Defaults *********************//

    const static uint8_t IHOLD_DEFAULT = 16;
    const static uint8_t IRUN_DEFAULT = 31;
    const static uint8_t IHOLDDELAY_DEFAULT = 1;
    const static uint8_t TPOWERDOWN_DEFAULT = 20;
    const static uint32_t TPWMTHRS_DEFAULT = 0;
    const static uint8_t TCOOLTHRS_DEFAULT = 0;
    const static uint8_t SGTHRS_DEFAULT = 0;
    const static uint8_t COOLCONF_DEFAULT = 0;
    const static uint32_t CHOPPER_CONFIG_DEFAULT = 0x10000053;
    const static uint8_t TBL_DEFAULT = 0b10;
    const static uint8_t HEND_DEFAULT = 0;
    const static uint8_t HSTART_DEFAULT = 5;
    const static uint8_t TOFF_DEFAULT = 3;
    const static uint32_t PWM_CONFIG_DEFAULT = 0xC10D0024;
    const static uint8_t PWM_OFFSET_DEFAULT = 0x24;
    const static uint8_t PWM_GRAD_DEFAULT = 0x14;

    //********************* Enums and structs *********************//

    struct Settings
    {
        bool is_communicating;
        bool is_setup;
        bool software_enabled;
        uint16_t microsteps_per_step;
        bool inverse_motor_direction_enabled;
        bool stealth_chop_enabled;
        uint8_t standstill_mode;
        uint8_t irun_percent;
        uint8_t irun_register_value;
        uint8_t ihold_percent;
        uint8_t ihold_register_value;
        uint8_t iholddelay_percent;
        uint8_t iholddelay_register_value;
        bool automatic_current_scaling_enabled;
        bool automatic_gradient_adaptation_enabled;
        uint8_t pwm_offset;
        uint8_t pwm_gradient;
        bool cool_step_enabled;
        bool analog_current_scaling_enabled;
        bool internal_sense_resistors_enabled;
    };

    struct Status
    {
        uint32_t over_temperature_warning : 1;
        uint32_t over_temperature_shutdown : 1;
        uint32_t short_to_ground_a : 1;
        uint32_t short_to_ground_b : 1;
        uint32_t low_side_short_a : 1;
        uint32_t low_side_short_b : 1;
        uint32_t open_load_a : 1;
        uint32_t open_load_b : 1;
        uint32_t over_temperature_120c : 1;
        uint32_t over_temperature_143c : 1;
        uint32_t over_temperature_150c : 1;
        uint32_t over_temperature_157c : 1;
        uint32_t reserved0 : 4;
        uint32_t current_scaling : 5;
        uint32_t reserved1 : 9;
        uint32_t stealth_chop_mode : 1;
        uint32_t standstill : 1;
    };

    union WriteReadReplyDatagram
    {
        struct
        {
            uint64_t sync : 4;
            uint64_t reserved : 4;
            uint64_t serial_address : 8;
            uint64_t register_address : 7;
            uint64_t rw : 1;
            uint64_t data : 32;
            uint64_t crc : 8;
        };
        uint64_t bytes;
    };

    union ReadRequestDatagram
    {
        struct
        {
            uint32_t sync : 4;
            uint32_t reserved : 4;
            uint32_t serial_address : 8;
            uint32_t register_address : 7;
            uint32_t rw : 1;
            uint32_t crc : 8;
        };
        uint32_t bytes;
    };

    union GlobalConfig
    {
        struct
        {
            uint32_t i_scale_analog : 1;
            uint32_t internal_rsense : 1;
            uint32_t enable_spread_cycle : 1;
            uint32_t shaft : 1;
            uint32_t index_otpw : 1;
            uint32_t index_step : 1;
            uint32_t pdn_disable : 1;
            uint32_t mstep_reg_select : 1;
            uint32_t multistep_filt : 1;
            uint32_t test_mode : 1;
            uint32_t reserved : 22;
        };
        uint32_t bytes;
    };

    union GlobalStatus
    {
        struct
        {
            uint32_t reset : 1;
            uint32_t drv_err : 1;
            uint32_t uv_cp : 1;
            uint32_t reserved : 29;
        };
        uint32_t bytes;
    };

    union ReplyDelay
    {
        struct
        {
            uint32_t reserved_0 : 8;
            uint32_t replydelay : 4;
            uint32_t reserved_1 : 20;
        };
        uint32_t bytes;
    };

    union Input
    {
        struct
        {
            uint32_t enn : 1;
            uint32_t reserved_0 : 1;
            uint32_t ms1 : 1;
            uint32_t ms2 : 1;
            uint32_t diag : 1;
            uint32_t reserved_1 : 1;
            uint32_t pdn_serial : 1;
            uint32_t step : 1;
            uint32_t spread_en : 1;
            uint32_t dir : 1;
            uint32_t reserved_2 : 14;
            uint32_t version : 8;
        };
        uint32_t bytes;
    };

    union DriverCurrent
    {
        struct
        {
            uint32_t ihold : 5;
            uint32_t reserved_0 : 3;
            uint32_t irun : 5;
            uint32_t reserved_1 : 3;
            uint32_t iholddelay : 4;
            uint32_t reserved_2 : 12;
        };
        uint32_t bytes;
    };

    union CoolConfig
    {
        struct
        {
            uint32_t semin : 4;
            uint32_t reserved_0 : 1;
            uint32_t seup : 2;
            uint32_t reserved_1 : 1;
            uint32_t semax : 4;
            uint32_t reserved_2 : 1;
            uint32_t sedn : 2;
            uint32_t seimin : 1;
            uint32_t reserved_3 : 16;
        };
        uint32_t bytes;
    };

    union ChopperConfig
    {
        struct
        {
            uint32_t toff : 4;
            uint32_t hstart : 3;
            uint32_t hend : 4;
            uint32_t reserved_0 : 4;
            uint32_t tbl : 2;
            uint32_t vsense : 1;
            uint32_t reserved_1 : 6;
            uint32_t mres : 4;
            uint32_t interpolation : 1;
            uint32_t double_edge : 1;
            uint32_t diss2g : 1;
            uint32_t diss2vs : 1;
        };
        uint32_t bytes;
    };

    union DriveStatus
    {
        struct
        {
            Status status;
        };
        uint32_t bytes;
    };

    union PwmConfig
    {
        struct
        {
            uint32_t pwm_offset : 8;
            uint32_t pwm_grad : 8;
            uint32_t pwm_freq : 2;
            uint32_t pwm_autoscale : 1;
            uint32_t pwm_autograd : 1;
            uint32_t freewheel : 2;
            uint32_t reserved : 2;
            uint32_t pwm_reg : 4;
            uint32_t pwm_lim : 4;
        };
        uint32_t bytes;
    };

    union PwmScale
    {
        struct
        {
            uint32_t pwm_scale_sum : 8;
            uint32_t reserved_0 : 8;
            uint32_t pwm_scale_auto : 9;
            uint32_t reserved_1 : 7;
        };
        uint32_t bytes;
    };

    union PwmAuto
    {
        struct
        {
            uint32_t pwm_offset_auto : 8;
            uint32_t reserved_0 : 8;
            uint32_t pwm_gradient_auto : 8;
            uint32_t reserved_1 : 8;
        };
        uint32_t bytes;
    };

    enum MeasurementCount
    {
        MEASUREMENT_COUNT_32 = 0,
        MEASUREMENT_COUNT_8 = 1,
        MEASUREMENT_COUNT_2 = 2,
        MEASUREMENT_COUNT_1 = 3,
    };

    enum CurrentIncrement
    {
        CURRENT_INCREMENT_1 = 0,
        CURRENT_INCREMENT_2 = 1,
        CURRENT_INCREMENT_4 = 2,
        CURRENT_INCREMENT_8 = 3,
    };

    enum SerialAddress
    {
        SERIAL_ADDRESS_0 = 0,
        SERIAL_ADDRESS_1 = 1,
        SERIAL_ADDRESS_2 = 2,
        SERIAL_ADDRESS_3 = 3,
    };

    enum StandstillMode
    {
        NORMAL = 0,
        FREEWHEELING = 1,
        STRONG_BRAKING = 2,
        BRAKING = 3,
    };
};