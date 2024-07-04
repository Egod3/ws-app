#include <zephyr/logging/log.h>
#include <string.h>
#include <stdio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/shell/shell.h>

#include "si70xx.h"

static int temp(const struct shell* sh)
{
    int		status;
    double      temp = 0.0;
    u_int16_t	raw_temp = 0;

    status = read_temperature(&raw_temp);
    if( status < 0 ){
        shell_print(sh, "failed to get the temperature, status: %d", status);
        return status;
    }
    temp = ((((double)raw_temp * 175.72)/65536.0)-46.85);
    shell_print(sh, "temperature: %lf C \t%lf F", temp, ((1.8*temp)+32.0));

    return status;
}

static int humid(const struct shell* sh)
{
    int         status;
    double      rel_humid = 0.0;
    u_int16_t	raw_humid = 0;

    status = read_humidity_temperature(&raw_humid, NULL);
    if( status < 0 ){
        shell_print(sh, "failed to get the relative humidity or temperature, status: %d", status);
        return status;
    }
    rel_humid = ((((double)raw_humid * 125.0)/65536.0) - 6.0);
    shell_print(sh, "relative humidity: %lf %%", rel_humid );

    return status;
}

static int fw_version(const struct shell* sh)
{
    int32_t     status = -1;
    u_int8_t    fw_ver = 0x0;

    status = si70xx_get_fw(&fw_ver);
    if ( status < 0 ){
        shell_print(sh, "failed to get the Firmware version, status: %d", status);
        return status;
    }
    shell_print(sh, "firmware version: 0x%x ", fw_ver );

    return status;
}

static int sensor_id(const struct shell* sh)
{
    int32_t     status = -1;
    u_int8_t    sensor_id_01 = 0x0;
    u_int64_t   serial_num = 0;

    status = si70xx_get_sensor_id(&sensor_id_01, &serial_num);
    if ( status < 0 ){
        shell_print(sh, "failed to get the sensor id, status: %d", status);
        return status;
    }
    shell_print(sh, "sensor_id: 0x%x", sensor_id_01);

    return status;
}

static int serial_num(const struct shell* sh)
{
    int32_t     status = -1;
    u_int8_t    sensor_id_01 = 0x0;
    u_int64_t   serial_num = 0;

    status = si70xx_get_sensor_id(&sensor_id_01, &serial_num);
    if ( status < 0 ){
        shell_print(sh, "failed to get the serial number, status: %d", status);
        return status;
    }
    shell_print(sh, "serial_num: 0x%llx", serial_num);

    return status;
}

/* Creating subcommands (level 1 command) array for command "si70xx". */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_si20xx,
        SHELL_CMD(temp, NULL, "get temperature", temp),
        SHELL_CMD(humid,   NULL, "get humidity", humid),
        SHELL_CMD(sensor_id,   NULL, "get sensor ID", sensor_id),
        SHELL_CMD(serial_num,   NULL, "get serial number", serial_num),
        SHELL_CMD(fw_version,   NULL, "get firmware version", fw_version),
        SHELL_SUBCMD_SET_END
);
/* Creating root (level 0) command "si70xx" */
SHELL_CMD_REGISTER(si70xx, &sub_si20xx, "si70xx relative humidity & temperature sensor", NULL);
