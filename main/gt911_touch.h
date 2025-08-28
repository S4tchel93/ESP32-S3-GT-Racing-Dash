#ifndef GT911_TOUCH_H
#define GT911_TOUCH_H

#include "driver/gpio.h"

#define I2C_MASTER_SCL_IO           9       /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           8       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0       /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define EXAMPLE_PIN_NUM_TOUCH_RST       (-1)            // -1 if not used
#define EXAMPLE_PIN_NUM_TOUCH_INT       (-1)            // -1 if not used
#define GPIO_INPUT_IO_4    4
#define GPIO_INPUT_PIN_SEL  1ULL<<GPIO_INPUT_IO_4

extern void gt911_touch_init(void);

#endif