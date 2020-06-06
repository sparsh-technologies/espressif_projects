


#ifndef __I2C_DRIVER_API_H__
#define __I2C_DRIVER_API_H__

#define CONFIG_I2C_MASTER_SCL               19
#define CONFIG_I2C_MASTER_SDA               18
#define CONFIG_I2C_MASTER_PORT_NUM          1
#define CONFIG_I2C_MASTER_FREQUENCY         100000

#define CONFIG_I2C_SLAVE_SCL                5
#define CONFIG_I2C_SLAVE_SDA                4
#define CONFIG_I2C_SLAVE_PORT_NUM           0
#define CONFIG_I2C_SLAVE_ADDRESS            0x28

#define CONFIG_BH1750_I2C_ADDRESS_LOW       y
#define CONFIG_BH1750_ADDR                  0x23
#define CONFIG_BH1750_ONETIME_L_RESOLUTION  y
#define CONFIG_BH1750_OPMODE                0x23

void i2c_app_main();

#endif
