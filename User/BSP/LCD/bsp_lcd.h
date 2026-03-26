#ifndef __BSP_LCD_H
#define __BSP_LCD_H

#include "main.h"

extern void delay_us(uint32_t us);

/* LCD重要参数集 */
typedef struct
{
    uint16_t width;     /* LCD 宽度 */
    uint16_t height;    /* LCD 高度 */
    uint16_t id;        /* LCD ID */
    uint8_t  dir;       /* 横屏还是竖屏控制：0，竖屏；1，横屏 */
    uint16_t wramcmd;   /* 开始写gram指令 */
    uint16_t setxcmd;   /* 设置x坐标指令 */
    uint16_t setycmd;   /* 设置y坐标指令 */
} _lcd_dev;

extern _lcd_dev lcddev;

/* 画笔颜色和背景色 */
extern uint32_t g_point_color;
extern uint32_t g_back_color;

/* LCD扫描方向宏定义 */
#define L2R_U2D  0
#define L2R_D2U  1
#define R2L_U2D  2
#define R2L_D2U  3
#define U2D_L2R  4
#define U2D_R2L  5
#define D2U_L2R  6
#define D2U_R2L  7
#define DFT_SCAN_DIR  L2R_U2D  /* 默认扫描方向 */

#define SSD_HOR_RESOLUTION      800     /* LCD水平分辨率 */
#define SSD_VER_RESOLUTION      480     /* LCD垂直分辨率 */
#define SSD_HT                  1056    /* 水平总周期 */
#define SSD_HPS                 46      /* 水平同步脉冲开始位置 */
#define SSD_HOR_PULSE_WIDTH     210     /* 水平同步脉冲宽度 */
#define SSD_VT                  525     /* 垂直总周期 */
#define SSD_VPS                 23      /* 垂直同步脉冲开始位置 */
#define SSD_VER_FRONT_PORCH     22      /* 垂直前端前沿 */

/* 常用颜色值(RGB565格式) */
#define WHITE   0xFFFF
#define BLACK   0x0000
#define BLUE    0x001F
#define BRED    0XF81F
#define GRED    0XFFE0
#define GBLUE   0X07FF
#define RED     0xF800
#define MAGENTA 0xF81F
#define GREEN   0x07E0
#define CYAN    0x7FFF
#define YELLOW  0xFFE0
#define BROWN   0XBC40
#define BRRED   0XFC07
#define GRAY    0X8430

/* 核心函数声明 */
void lcd_init(void);
void lcd_clear(uint16_t color);
void lcd_display_on(void);
void lcd_display_off(void);
void lcd_set_cursor(uint16_t x, uint16_t y);
void lcd_write_ram_prepare(void);
uint32_t lcd_read_point(uint16_t x, uint16_t y);
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);
void lcd_ssd_backlight_set(uint8_t pwm);
void lcd_display_dir(uint8_t dir);
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);

#endif
