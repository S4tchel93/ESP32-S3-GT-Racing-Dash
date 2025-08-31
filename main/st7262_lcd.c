/***************************************************************************************************************************
 * INCLUDE SECTION
 ***************************************************************************************************************************/
/*project includes*/
#include "st7262_lcd.h"
/*System/ESP IDF Includes*/
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"

/***************************************************************************************************************************
 * PRIVATE DEFINITIONS
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * PRIVATE DATA & TYPES
 ***************************************************************************************************************************/
static const char *TAG = "ST7262_LCD";

/**
 * @brief LCD Panel Handle
 */
static esp_lcd_panel_handle_t panel_handle = NULL;

/***************************************************************************************************************************
 * PRIVATE FUNCTION DECLARATIONS
 ***************************************************************************************************************************/

/***************************************************************************************************************************
* PRIVATE FUNCTION DEFINITIONS
***************************************************************************************************************************/

/***************************************************************************************************************************
 * PUBLIC FUNCTION DEFINITIONS
 ***************************************************************************************************************************/

/**
 * @brief Initializes the ST7262 LCD RGB Panel
 * 
 * @return esp_lcd_panel_handle_t pointer with LCD Panel configurations
 */
esp_lcd_panel_handle_t* st7262_lcd_init(void)
{

    static esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = DATA_BUS_WIDTH,
        .dma_burst_size = 64,
        .num_fbs = 2,
#if CONFIG_EXAMPLE_USE_BOUNCE_BUFFER
        .bounce_buffer_size_px = 20 * LCD_H_RES,
#endif
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = PIN_NUM_DISP_EN,
        .pclk_gpio_num = PIN_NUM_PCLK,
        .vsync_gpio_num = PIN_NUM_VSYNC,
        .hsync_gpio_num = PIN_NUM_HSYNC,
        .de_gpio_num = PIN_NUM_DE,
        .data_gpio_nums = {
            PIN_NUM_DATA0,
            PIN_NUM_DATA1,
            PIN_NUM_DATA2,
            PIN_NUM_DATA3,
            PIN_NUM_DATA4,
            PIN_NUM_DATA5,
            PIN_NUM_DATA6,
            PIN_NUM_DATA7,
            PIN_NUM_DATA8,
            PIN_NUM_DATA9,
            PIN_NUM_DATA10,
            PIN_NUM_DATA11,
            PIN_NUM_DATA12,
            PIN_NUM_DATA13,
            PIN_NUM_DATA14,
            PIN_NUM_DATA15,
#if CONFIG_EXAMPLE_LCD_DATA_LINES > 16
            PIN_NUM_DATA16,
            PIN_NUM_DATA17,
            PIN_NUM_DATA18,
            PIN_NUM_DATA19,
            PIN_NUM_DATA20,
            PIN_NUM_DATA21,
            PIN_NUM_DATA22,
            PIN_NUM_DATA23
#endif
        },
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ,
            .h_res = LCD_H_RES,
            .v_res = LCD_V_RES,
            .hsync_back_porch = LCD_HBP,
            .hsync_front_porch = LCD_HFP,
            .hsync_pulse_width = LCD_HSYNC,
            .vsync_back_porch = LCD_VBP,
            .vsync_front_porch = LCD_VFP,
            .vsync_pulse_width = LCD_VSYNC,
            .flags = {
                .pclk_active_neg = true,
            },
        },
        .flags.fb_in_psram = true, // allocate frame buffer in PSRAM
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    return &panel_handle;
}