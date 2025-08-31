#ifndef ST7262_LCD_H
#define ST7262_LCD_H

/***************************************************************************************************************************
 * INCLUDE SECTION
 ***************************************************************************************************************************/
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_types.h"
#include "driver/gpio.h"

/***************************************************************************************************************************
 * PUBLIC DEFINITIONS
 ***************************************************************************************************************************/
#define LCD_PIXEL_CLOCK_HZ     (16 * 1000 * 1000)
#define LCD_H_RES              800
#define LCD_V_RES              480
#define LCD_HSYNC              1
#define LCD_HBP                40
#define LCD_HFP                20
#define LCD_VSYNC              1
#define LCD_VBP                10
#define LCD_VFP                5

#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define PIN_NUM_BK_LIGHT       -1
#define PIN_NUM_DISP_EN        -1

#define LCD_IO_RGB_VSYNC        (GPIO_NUM_3)
#define LCD_IO_RGB_HSYNC        (GPIO_NUM_46)
#define LCD_IO_RGB_DE           (GPIO_NUM_5)
#define LCD_IO_RGB_PCLK         (GPIO_NUM_7)

#define PIN_NUM_HSYNC          LCD_IO_RGB_HSYNC
#define PIN_NUM_VSYNC          LCD_IO_RGB_VSYNC
#define PIN_NUM_DE             LCD_IO_RGB_DE
#define PIN_NUM_PCLK           LCD_IO_RGB_PCLK

#define LCD_DATA0_GPIO        (GPIO_NUM_14)
#define LCD_DATA1_GPIO        (GPIO_NUM_38)
#define LCD_DATA2_GPIO        (GPIO_NUM_18)
#define LCD_DATA3_GPIO        (GPIO_NUM_17)
#define LCD_DATA4_GPIO        (GPIO_NUM_10)
#define LCD_DATA5_GPIO        (GPIO_NUM_39)
#define LCD_DATA6_GPIO        (GPIO_NUM_0)
#define LCD_DATA7_GPIO        (GPIO_NUM_45)
#define LCD_DATA8_GPIO        (GPIO_NUM_48)
#define LCD_DATA9_GPIO        (GPIO_NUM_47)
#define LCD_DATA10_GPIO       (GPIO_NUM_21)
#define LCD_DATA11_GPIO       (GPIO_NUM_1)
#define LCD_DATA12_GPIO       (GPIO_NUM_2)
#define LCD_DATA13_GPIO       (GPIO_NUM_42)
#define LCD_DATA14_GPIO       (GPIO_NUM_41)
#define LCD_DATA15_GPIO       (GPIO_NUM_40)

#define PIN_NUM_DATA0          LCD_DATA0_GPIO
#define PIN_NUM_DATA1          LCD_DATA1_GPIO
#define PIN_NUM_DATA2          LCD_DATA2_GPIO
#define PIN_NUM_DATA3          LCD_DATA3_GPIO
#define PIN_NUM_DATA4          LCD_DATA4_GPIO
#define PIN_NUM_DATA5          LCD_DATA5_GPIO
#define PIN_NUM_DATA6          LCD_DATA6_GPIO
#define PIN_NUM_DATA7          LCD_DATA7_GPIO
#define PIN_NUM_DATA8          LCD_DATA8_GPIO
#define PIN_NUM_DATA9          LCD_DATA9_GPIO
#define PIN_NUM_DATA10         LCD_DATA10_GPIO
#define PIN_NUM_DATA11         LCD_DATA11_GPIO
#define PIN_NUM_DATA12         LCD_DATA12_GPIO
#define PIN_NUM_DATA13         LCD_DATA13_GPIO
#define PIN_NUM_DATA14         LCD_DATA14_GPIO
#define PIN_NUM_DATA15         LCD_DATA15_GPIO
#if CONFIG_EXAMPLE_LCD_DATA_LINES > 16
#define PIN_NUM_DATA16         LCD_DATA16_GPIO
#define PIN_NUM_DATA17         LCD_DATA17_GPIO
#define PIN_NUM_DATA18         LCD_DATA18_GPIO
#define PIN_NUM_DATA19         LCD_DATA19_GPIO
#define PIN_NUM_DATA20         LCD_DATA20_GPIO
#define PIN_NUM_DATA21         LCD_DATA21_GPIO
#define PIN_NUM_DATA22         LCD_DATA22_GPIO
#define PIN_NUM_DATA23         LCD_DATA23_GPIO
#endif

#if CONFIG_EXAMPLE_LCD_DATA_LINES_16
#define DATA_BUS_WIDTH         16
#define PIXEL_SIZE             2
#define LV_COLOR_FORMAT        LV_COLOR_FORMAT_RGB565
#elif CONFIG_EXAMPLE_LCD_DATA_LINES_24
#define DATA_BUS_WIDTH         24
#define PIXEL_SIZE             3
#define LV_COLOR_FORMAT        LV_COLOR_FORMAT_RGB888
#endif

#if CONFIG_EXAMPLE_USE_DOUBLE_FB
#define EXAMPLE_LCD_NUM_FB             2
#else
#define EXAMPLE_LCD_NUM_FB             1
#endif // CONFIG_EXAMPLE_USE_DOUBLE_FB

/***************************************************************************************************************************
 * PUBLIC DATA & TYPES
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * PUBLIC FUNCTION DECLARATIONS
 ***************************************************************************************************************************/

extern esp_lcd_panel_handle_t* st7262_lcd_init(void);

#endif