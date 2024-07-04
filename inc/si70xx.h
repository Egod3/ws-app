#ifndef __si70xx__H__
#define __si70xx__H__

#include <unistd.h>
#include <sys/types.h>

#define SI70XX_I2C_ADDR                             0x40
#define SI70XX_I2C_NAME                             "Si70xx"

/* DEBUG FLAGS FOR MORE PRINTF INFO */
//#define IOCTL_DBG

/*      0x0D = Si7013
        0x14 = Si7020
        0x15 = Si7021       */
#define SI70XX_I2C_SENSOR_VER_13                    13
#define SI70XX_I2C_SENSOR_VER_20                    20
#define SI70XX_I2C_SENSOR_VER_21                    21
#define SI70XX_I2C_FW_REV_1_0                       0xFF
#define SI70XX_I2C_FW_REV_2_0                       0x20

// These are the max delay at 12-bit RH value.
#define SI70XX_REL_HUMIDTY_READ_MAX_DELAY_MS        12000
// These are the max delay at 14-bit temperature value.
#define SI70XX_TEMPERATURE_READ_MAX_DELAY_MS        10800

int32_t verify_si70xx_version(u_int64_t* serial_num);
int32_t read_humidity_temperature(u_int16_t* rh_hum, u_int16_t* temper);
int32_t read_temperature(u_int16_t* raw_temp);
int32_t read_relative_humidity(u_int16_t* rh_hum);
void    print_fw_ver(u_int8_t fw_id);
void    print_elec_serl_num(int8_t fw_id);
double   convert_and_print_temp(u_int16_t raw_temp);
double   convert_and_print_humid(u_int16_t raw_humid);
int32_t si70xx_get_fw(u_int8_t* fw_ver);
int32_t si70xx_get_sensor_id(u_int8_t* sensor_id, u_int64_t* serial_num);

#endif /*  __si70xx__H__  */
