#include <zephyr/logging/log.h>
#include <string.h>
#include <stdio.h>
#include <zephyr/drivers/i2c.h>

#include "si70xx.h"

LOG_MODULE_REGISTER(si70xx);

#define I2C_INST DT_NODELABEL(i2c1)

const struct device *const i2c_dev = DEVICE_DT_GET(I2C_INST);


/*  Return - 0 means success, negative value is failure.

    u_int8_t*  sensor_id  - sensor_id to return to the user.
    u_int64_t* serial_num - serial_num to return to the user.
*/
int32_t si70xx_get_sensor_id(u_int8_t* sensor_id, u_int64_t* serial_num)
{
    int32_t                     ret     = -1;
    u_int8_t                    read_buf[6];
    u_int8_t                    write_buf[2];

    if( sensor_id == NULL ){
        LOG_ERR("sensor_id == NULL.\n");
        return ret;
    }
    if( serial_num == NULL ){
        LOG_ERR("serial_num == NULL.\n");
        return ret;
    }

    memset(read_buf, 0, 6);
    memset(write_buf, 0, 2);

    write_buf[0] = 0xFA;
    write_buf[1] = 0x0F;
    ret = i2c_write(i2c_dev, (u_int8_t*)write_buf, 2, SI70XX_I2C_ADDR);
    if( ret != 0 ){
        LOG_ERR("i2c_write() failed w/ err: %d", ret);
        return ret;
    }
    ret = i2c_read(i2c_dev, (u_int8_t*)read_buf, 6, SI70XX_I2C_ADDR);
    if( ret != 0 ) {
        return ret;
    }

    *serial_num = ((((u_int64_t)read_buf[0] << 56)&0xFF00000000000000) + *serial_num);
    *serial_num = ((((u_int64_t)read_buf[1] << 48)&0xFF000000000000) + *serial_num);
    *serial_num = ((((u_int64_t)read_buf[2] << 40)&0xFF0000000000) + *serial_num);
    *serial_num = ((((u_int64_t)read_buf[3] << 32)&0xFF00000000) + *serial_num);
    memset(read_buf, 0, 6);
    memset(write_buf, 0, 2);

    write_buf[0] = 0xFC;
    write_buf[1] = 0xC9;
    ret = i2c_write(i2c_dev, (u_int8_t*)write_buf, 2, SI70XX_I2C_ADDR);
    if( ret != 0 ){
        LOG_ERR("i2c_write() failed w/ err: %d", ret);
        return ret;
    }
    ret = i2c_read(i2c_dev, (u_int8_t*)read_buf, 6, SI70XX_I2C_ADDR);
    if( ret == 0 ){
        *sensor_id = read_buf[0];
    } else {
        return ret;
    }
    *serial_num = ((((u_int64_t)read_buf[0] << 24)&0xFF000000) + *serial_num);
    *serial_num = ((((u_int64_t)read_buf[1] << 16)&0xFF0000) + *serial_num);
    *serial_num = ((((u_int64_t)read_buf[2] <<  8)&0xFF00) + *serial_num);
    *serial_num = ((((u_int64_t)read_buf[3] <<  0)&0xFF) + *serial_num);

    return 0;
}

/*  Return - 0 means success, negative value is failure.

    int8_t* fw_ver  - fw_ver to return to the user.
*/
int32_t si70xx_get_fw(u_int8_t* fw_ver)
{
    int32_t                     ret     = -1;
    u_int8_t                    write_buf[2];
    u_int8_t                    read_buf[1];
    memset(read_buf, 0, 1);
    memset(write_buf, 0, 2);

    if( fw_ver == NULL ){
        LOG_ERR("fw_ver == NULL.\n");
        return ret;
    }

    write_buf[0] = 0x84;
    write_buf[1] = 0xB8;
    ret = i2c_write(i2c_dev, (u_int8_t*)write_buf, 2, SI70XX_I2C_ADDR);
    if( ret != 0 ){
        LOG_ERR("i2c_write() failed w/ err: %d", ret);
        return ret;
    }
    ret = i2c_read(i2c_dev, (u_int8_t*)read_buf, 1, SI70XX_I2C_ADDR);
    if( ret == 0 ){
        *fw_ver = read_buf[0];
    }

    return 0;
}

/*      0x0D = Si7013
        0x14 = Si7020
        0x15 = Si7021       */
void print_elec_serl_num(int8_t fw_id)
{
    if( fw_id == SI70XX_I2C_SENSOR_VER_13 )
        LOG_DBG("Electric serial number for the Si7013 sensor.\n");
    else if( fw_id == SI70XX_I2C_SENSOR_VER_20 )
        LOG_DBG("Electric serial number for the Si7020 sensor.\n");
    else if( fw_id == SI70XX_I2C_SENSOR_VER_21 )
        LOG_DBG("Electric serial number for the Si7021 sensor.\n");
    else
        LOG_DBG("%s: error could not detect electric serial number. fw_id:0x%x\n", __func__, fw_id);
}

/*      0xFF = Firmware version 1.0
        0x20 = Firmware version 2.0     */
void print_fw_ver(u_int8_t fw_rev)
{
    if( fw_rev == SI70XX_I2C_FW_REV_1_0 )
        LOG_DBG("Firware version is 1.0.\n");
    else if( fw_rev == SI70XX_I2C_FW_REV_2_0 )
        LOG_DBG("Firware version is 2.0.\n");
    else
        LOG_ERR("%s: error could not detect FW version. fw_rev:0x%x\n", __func__, fw_rev);
}

int32_t init_i2c_device()
{
    int32_t     ret = -1;

	if (!device_is_ready(i2c_dev)) {
		LOG_ERR("i2c is not ready!\n");
        return ret;
	} else {
		LOG_DBG("i2c is ready\n");
        ret = 0;
	}

    return ret;
}

/*
    Verify Si70xx Version Info.
*/
int32_t verify_si70xx_version(u_int64_t* serial_num)
{
    int32_t     ret = -1;
    u_int8_t    fw_ver = 0x0, sensor_id_01 = 0x0;
    u_int8_t    exp_fw_ver_01 = 0xFF, exp_fw_ver_02 = 0x20,  exp_sensor_id_01 = 0x14, exp_sensor_id_02 = 0x15;

    if( serial_num == NULL ){
        return ret;
    }

    ret = init_i2c_device();
    if ( ret < 0 ){
        LOG_ERR("Failed to open the i2c device.");
        return -2;
    }

    ret = si70xx_get_fw(&fw_ver);
    if ( ret < 0 ){
        LOG_ERR("Failed to get the FW id from the tempature/humidity device.");
        return -3;
    }

    ret = si70xx_get_sensor_id(&sensor_id_01, serial_num);
    if ( ret < 0 ){
        LOG_ERR("Failed to get the FW id from the tempature/humidity device.");
        return -4;
    }
    LOG_DBG("serial_num: 0x%llx\n", *serial_num);

    if( (exp_fw_ver_01    == fw_ver || exp_fw_ver_02 == fw_ver ) &&
        (exp_sensor_id_01 == sensor_id_01 ||
         exp_sensor_id_02 == sensor_id_01 ) ){
        return 0;
    }

    LOG_ERR("Error one of FW version, sensor ID 01, or sensor ID 02 did not match!\n");
    return -5;
}

double convert_and_print_temp(u_int16_t raw_temp)
{
    double       double_temp      = 0.0;
    double_temp = ((((double)raw_temp * 175.72)/65536.0)-46.85);
    LOG_DBG("Temperature: %lf C \t%lf F\n", double_temp, ((1.8*double_temp)+32.0));
    return double_temp;
}

double convert_and_print_humid(u_int16_t raw_humid)
{
    double       double_rel_humid = 0.0;
    double_rel_humid = ((((double)raw_humid * 125.0)/65536.0) - 6.0);
    printf("Relative Humidity: %lf %%\n", double_rel_humid );
    return double_rel_humid;
}

/*
    Read both the raw humidity and the temperature raw data and return it.

    Based on the Si7021 data sheet there is a 4.5 ms (max) delay when
    reading 10-bit Relative Humidity and there is a 6.2 ms (max) delay
    when reading 13-bit temperature.
*/
int32_t read_humidity_temperature(u_int16_t* raw_humid, u_int16_t* raw_temp)
{
    u_int8_t    read_buf[2];
    u_int8_t    write_buf[1];
    int32_t     ret             = -1;

    if( raw_humid == NULL ){
        LOG_ERR("raw_humid == NULL.\n");
        return ret;
    }

    ret = init_i2c_device();
    if ( ret < 0 ){
        LOG_ERR("Failed to open the i2c device.");
        return -2;
    }

    memset(read_buf, 0, 2);
    memset(write_buf, 0, 1);
    write_buf[0] = 0xE5;
    ret = i2c_write(i2c_dev, (u_int8_t*)write_buf, 1, SI70XX_I2C_ADDR);
    if( ret != 0 ){
        LOG_ERR("i2c_write() failed w/ err: %d", ret);
        return ret;
    }
    ret = i2c_read(i2c_dev, (u_int8_t*)read_buf, 2, SI70XX_I2C_ADDR);
    if( ret == 0 ){
        *raw_humid = ((((u_int16_t)read_buf[0] <<  8)&0xFF00) + *raw_humid);
        *raw_humid = ((((u_int16_t)read_buf[1] <<  0)&0xFF)   + *raw_humid);
    } else {
        return ret;
    }

    /* We do this check here, rather than up top, so we can support
      raw_humid being non-null and raw_temp being null so this API
      will support just reading the raw_humid or reading both raw_humid
      and raw_temp
    */
    if( raw_temp == NULL ){
        // return 0 because the above raw_humid call worked.
        return 0;
    }

    memset(read_buf, 0, 2);
    memset(write_buf, 0, 1);
    write_buf[0] = 0xE0; /* read temp from previous RH measurement. */
    ret = i2c_write(i2c_dev, (u_int8_t*)write_buf, 1, SI70XX_I2C_ADDR);
    if( ret != 0 ){
        LOG_ERR("i2c_write() failed w/ err: %d", ret);
        return ret;
    }
    ret = i2c_read(i2c_dev, (u_int8_t*)read_buf, 2, SI70XX_I2C_ADDR);
    if( ret == 0 ){
        *raw_temp = ((((u_int16_t)read_buf[0] <<  8)&0xFF00) + *raw_temp);
        *raw_temp = ((((u_int16_t)read_buf[1] <<  0)&0xFF)   + *raw_temp);
    } else {
        return ret;
    }

    return 0;
}

/*
    Read the raw temperature data and return it.

    Based on the Si7021 data sheet there is a 4.5 ms (max) delay when
    reading 10-bit Relative Humidity and there is a 6.2 ms (max) delay
    when reading 13-bit temperature.
*/
int32_t read_temperature(u_int16_t* raw_temp)
{
    u_int8_t    read_buf[2];
    u_int8_t    write_buf[1];
    int32_t     ret         = -1;

    if( raw_temp == NULL ){
        LOG_ERR("raw_temp == NULL.\n");
        return ret;
    }

    ret = init_i2c_device();
    if ( ret < 0 ){
        LOG_ERR("Failed to open the i2c device.");
        return -2;
    }

    memset(read_buf, 0, 2);
    memset(write_buf, 0, 1);
    write_buf[0] = 0xE3;
    ret = i2c_write(i2c_dev, (u_int8_t*)write_buf, 1, SI70XX_I2C_ADDR);
    if( ret != 0 ){
        LOG_ERR("i2c_write() failed w/ err: %d", ret);
        return ret;
    }
    ret = i2c_read(i2c_dev, (u_int8_t*)read_buf, 2, SI70XX_I2C_ADDR);
    if( ret == 0 ){
        *raw_temp = ((((u_int16_t)read_buf[0] <<  8)&0xFF00) + *raw_temp);
        *raw_temp = ((((u_int16_t)read_buf[1] <<  0)&0xFF)   + *raw_temp);
    } else {
        return ret;
    }

    return 0;
}

