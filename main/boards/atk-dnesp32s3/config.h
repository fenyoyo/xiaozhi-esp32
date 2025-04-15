
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE 24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_3
#define AUDIO_I2S_GPIO_WS GPIO_NUM_9
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_46
#define AUDIO_I2S_GPIO_DIN GPIO_NUM_14
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_10

#define AUDIO_CODEC_I2C_SDA_PIN GPIO_NUM_41
#define AUDIO_CODEC_I2C_SCL_PIN GPIO_NUM_42
#define AUDIO_CODEC_ES8388_ADDR ES8388_CODEC_DEFAULT_ADDR

#define BOOT_BUTTON_GPIO GPIO_NUM_0

#define BUILTIN_LED_GPIO GPIO_NUM_1

#define LCD_SCLK_PIN GPIO_NUM_12
#define LCD_MOSI_PIN GPIO_NUM_11
#define LCD_MISO_PIN GPIO_NUM_13
#define LCD_DC_PIN GPIO_NUM_40
#define LCD_CS_PIN GPIO_NUM_21

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY true

#define DISPLAY_OFFSET_X 0
#define DISPLAY_OFFSET_Y 0

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_NC
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT true

#endif // _BOARD_CONFIG_H_
