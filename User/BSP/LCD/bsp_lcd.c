#include "main.h"
#include "bsp_lcd.h"
#include "bsp_lcdfont.h"

typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE        ((uint32_t)(0x6C000000 | 0x0000007E))
#define LCD             ((LCD_TypeDef *) LCD_BASE)


void lcd_wr_data(volatile uint16_t data);
void lcd_wr_regno(volatile uint16_t regno);
void lcd_write_reg(uint16_t regno, uint16_t data);
#define delay_ms HAL_Delay

/* 将寄存器初始化长序列包含进来 (必须放在声明之后) */
#include "bsp_lcd_ex.c"

/* LCD的画笔颜色和背景色 */
uint32_t g_point_color = 0xF800;    /* 画笔颜色 */
uint32_t g_back_color  = 0xFFFF;    /* 背景色 */

/* 管理LCD重要参数 */
_lcd_dev lcddev;

void lcd_wr_data(volatile uint16_t data)
{
    data = data;            
    LCD->LCD_RAM = data;
}

void lcd_wr_regno(volatile uint16_t regno)
{
    regno = regno;          
    LCD->LCD_REG = regno;   
}

void lcd_write_reg(uint16_t regno, uint16_t data)
{
    LCD->LCD_REG = regno;   
    LCD->LCD_RAM = data;    
}

static void lcd_opt_delay(uint32_t i)
{
    while (i--); 
}

static uint16_t lcd_rd_data(void)
{
    volatile uint16_t ram;  
    lcd_opt_delay(2);
    ram = LCD->LCD_RAM;
    return ram;
}

void lcd_write_ram_prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint16_t r = 0, g = 0, b = 0;
    if (x >= lcddev.width || y >= lcddev.height) return 0;
    lcd_set_cursor(x, y);       

    if (lcddev.id == 0x5510) lcd_wr_regno(0x2E00);  
    else lcd_wr_regno(0x2E);      

    r = lcd_rd_data();          
    if (lcddev.id == 0x1963) return r;   

    r = lcd_rd_data();          
    if (lcddev.id == 0x7796) return r;
    
    b = lcd_rd_data();
    g = r & 0xFF;               
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  
}

void lcd_display_on(void)
{
    if (lcddev.id == 0x5510) lcd_wr_regno(0x2900);  
    else lcd_wr_regno(0x29);      
}

void lcd_display_off(void)
{
    if (lcddev.id == 0x5510) lcd_wr_regno(0x2800);  
    else lcd_wr_regno(0x28);      
}

void lcd_set_cursor(uint16_t x, uint16_t y)
{
    if (lcddev.id == 0x1963)
    {
        if (lcddev.dir == 0)    
        {
            x = lcddev.width - 1 - x;
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(0);
            lcd_wr_data(0);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
        }
        else                    
        {
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
            lcd_wr_data((lcddev.width - 1) >> 8);
            lcd_wr_data((lcddev.width - 1) & 0xFF);
        }
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0xFF);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
    else if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(x & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(y & 0xFF);
    }
    else    
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_data(x & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0xFF);
    }
}

void lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;

    if ((lcddev.dir == 1 && lcddev.id != 0x1963) || (lcddev.dir == 0 && lcddev.id == 0x1963))
    {
        switch (dir)   
        {
            case 0: dir = 6; break;
            case 1: dir = 7; break;
            case 2: dir = 4; break;
            case 3: dir = 5; break;
            case 4: dir = 1; break;
            case 5: dir = 0; break;
            case 6: dir = 3; break;
            case 7: dir = 2; break;
        }
    }

    switch (dir)
    {
        case L2R_U2D: regval |= (0 << 7) | (0 << 6) | (0 << 5); break;
        case L2R_D2U: regval |= (1 << 7) | (0 << 6) | (0 << 5); break;
        case R2L_U2D: regval |= (0 << 7) | (1 << 6) | (0 << 5); break;
        case R2L_D2U: regval |= (1 << 7) | (1 << 6) | (0 << 5); break;
        case U2D_L2R: regval |= (0 << 7) | (0 << 6) | (1 << 5); break;
        case U2D_R2L: regval |= (0 << 7) | (1 << 6) | (1 << 5); break;
        case D2U_L2R: regval |= (1 << 7) | (0 << 6) | (1 << 5); break;
        case D2U_R2L: regval |= (1 << 7) | (1 << 6) | (1 << 5); break;
    }

    dirreg = 0x36;  
    if (lcddev.id == 0x5510) dirreg = 0x3600;   
    if (lcddev.id == 0x9341 || lcddev.id == 0x7789 || lcddev.id == 0x7796) regval |= 0x08;
    lcd_write_reg(dirreg, regval);

    if (lcddev.id != 0x1963)                    
    {
        if (regval & 0x20)
        {
            if (lcddev.width < lcddev.height)   
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }

    if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data((lcddev.width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
    else
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_data((lcddev.width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
}

void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    lcd_set_cursor(x, y);       
    lcd_write_ram_prepare();    
    LCD->LCD_RAM = color;
}

void lcd_ssd_backlight_set(uint8_t pwm)
{
    lcd_wr_regno(0xBE);         
    lcd_wr_data(0x05);          
    lcd_wr_data(pwm * 2.55);    
    lcd_wr_data(0x01);          
    lcd_wr_data(0xFF);          
    lcd_wr_data(0x00);          
    lcd_wr_data(0x00);          
}

void lcd_display_dir(uint8_t dir)
{
    lcddev.dir = dir;   

    if (dir == 0)       
    {
        lcddev.width = 240;
        lcddev.height = 320;

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0x2C00;
            lcddev.setxcmd = 0x2A00;
            lcddev.setycmd = 0x2B00;
            lcddev.width = 480;
            lcddev.height = 800;
        }
        else if (lcddev.id == 0x1963)
        {
            lcddev.wramcmd = 0x2C;  
            lcddev.setxcmd = 0x2B;  
            lcddev.setycmd = 0x2A;  
            lcddev.width = 480;     
            lcddev.height = 800;    
        }
        else   
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
        }

        if (lcddev.id == 0x5310 || lcddev.id == 0x7796)     
        {
            lcddev.width = 320;
            lcddev.height = 480;
        }
        if (lcddev.id == 0X9806)    
        {
            lcddev.width = 480;
            lcddev.height = 800;
        }  
    }
    else        
    {
        lcddev.width = 320;         
        lcddev.height = 240;        

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0x2C00;
            lcddev.setxcmd = 0x2A00;
            lcddev.setycmd = 0x2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else if (lcddev.id == 0x1963 || lcddev.id == 0x9806)
        {
            lcddev.wramcmd = 0x2C;  
            lcddev.setxcmd = 0x2A;  
            lcddev.setycmd = 0x2B;  
            lcddev.width = 800;     
            lcddev.height = 480;    
        }
        else   
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
        }

        if (lcddev.id == 0x5310 || lcddev.id == 0x7796)     
        {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }
    lcd_scan_dir(DFT_SCAN_DIR);     
}

void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

   if (lcddev.id == 0x1963 && lcddev.dir != 1)     
    {
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_data((sx + width - 1) >> 8);
        lcd_wr_data((sx + width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_data(height >> 8);
        lcd_wr_data(height & 0xFF);
    }
    else if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data(twidth >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data(twidth & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data(theight >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data(theight & 0xFF);
    }
    else    
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_data(twidth >> 8);
        lcd_wr_data(twidth & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_data(theight >> 8);
        lcd_wr_data(theight & 0xFF);
    }
}


void lcd_init(void)
{
    HAL_Delay(50); /* 上电后等待稳定 */

    /* 尝试9341 ID的读取 */
    lcd_wr_regno(0xD3);
    lcddev.id = lcd_rd_data();  
    lcddev.id = lcd_rd_data();  
    lcddev.id = lcd_rd_data();  
    lcddev.id <<= 8;
    lcddev.id |= lcd_rd_data(); 

    if (lcddev.id != 0x9341)    
    {
        lcd_wr_regno(0x04);
        lcddev.id = lcd_rd_data();      
        lcddev.id = lcd_rd_data();      
        lcddev.id = lcd_rd_data();      
        lcddev.id <<= 8;
        lcddev.id |= lcd_rd_data();     
        
        if (lcddev.id == 0x8552) lcddev.id = 0x7789;

        if (lcddev.id != 0x7789)        
        {
            lcd_wr_regno(0xD4);
            lcddev.id = lcd_rd_data();  
            lcddev.id = lcd_rd_data();  
            lcddev.id = lcd_rd_data();  
            lcddev.id <<= 8;
            lcddev.id |= lcd_rd_data(); 

            if (lcddev.id != 0x5310)    
            {
                lcd_wr_regno(0XD3);
                lcddev.id = lcd_rd_data();  
                lcddev.id = lcd_rd_data();  
                lcddev.id = lcd_rd_data();  
                lcddev.id <<= 8;
                lcddev.id |= lcd_rd_data(); 
                
                if (lcddev.id != 0x7796)    
                {
                    lcd_write_reg(0xF000, 0x0055);
                    lcd_write_reg(0xF001, 0x00AA);
                    lcd_write_reg(0xF002, 0x0052);
                    lcd_write_reg(0xF003, 0x0008);
                    lcd_write_reg(0xF004, 0x0001);
                    
                    lcd_wr_regno(0xC500);       
                    lcddev.id = lcd_rd_data();  
                    lcddev.id <<= 8;

                    lcd_wr_regno(0xC501);       
                    lcddev.id |= lcd_rd_data(); 
                    
                    HAL_Delay(5);                

                    if (lcddev.id != 0x5510)    
                    {
                        lcd_wr_regno(0XD3);
                        lcddev.id = lcd_rd_data();  
                        lcddev.id = lcd_rd_data();  
                        lcddev.id = lcd_rd_data();  
                        lcddev.id <<= 8;
                        lcddev.id |= lcd_rd_data(); 
                        
                        if (lcddev.id != 0x9806)    
                        {
                            lcd_wr_regno(0xA1);
                            lcddev.id = lcd_rd_data();
                            lcddev.id = lcd_rd_data();  
                            lcddev.id <<= 8;
                            lcddev.id |= lcd_rd_data(); 

                            if (lcddev.id == 0x5761) lcddev.id = 0x1963; 
                        }
                    }
                }
            }
        }
    }


    if (lcddev.id == 0x7789)
    {
        lcd_ex_st7789_reginit();    
    }
    else if (lcddev.id == 0x9341)
    {
        lcd_ex_ili9341_reginit();   
    }
    else if (lcddev.id == 0x5310)
    {
        lcd_ex_nt35310_reginit();   
    }
    else if (lcddev.id == 0x7796)
    {
        lcd_ex_st7796_reginit();    
    }
    else if (lcddev.id == 0x5510)
    {
        lcd_ex_nt35510_reginit();   
    }
    else if (lcddev.id == 0x9806)
    {
        lcd_ex_ili9806_reginit();   
    }
    else if (lcddev.id == 0x1963)
    {
        lcd_ex_ssd1963_reginit();   
        lcd_ssd_backlight_set(100); 
    }

    lcd_display_dir(0); /* 默认为竖屏 */
    
    /* HAL库控制引脚点亮背光 */
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET); 
    
    lcd_clear(WHITE);
}

void lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcddev.width;

    totalpoint *= lcddev.height;    
    lcd_set_cursor(0x00, 0x0000);   
    lcd_write_ram_prepare();        

    for (index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        lcd_set_cursor(sx, i);      
        lcd_write_ram_prepare();    

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = color;   
        }
    }
}

void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width;
    uint16_t i, j;

    width = ex - sx + 1;            
    height = ey - sy + 1;           

    for (i = 0; i < height; i++)
    {
        lcd_set_cursor(sx, sy + i); 
        lcd_write_ram_prepare();    

        for (j = 0; j < width; j++)
        {
            LCD->LCD_RAM = color[i * width + j]; 
        }
    }
}

void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;      
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0) incx = 1;      
    else if (delta_x == 0) incx = 0;      
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0) incy = 1;
    else if (delta_y == 0) incy = 0;      
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y) distance = delta_x;  
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++)     
    {
        lcd_draw_point(row, col, color);    
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > lcddev.width) || (y > lcddev.height)) return;
    lcd_fill(x, y, x + len - 1, y, color);
}

void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;

    a = 0;
    b = r;
    di = 3 - (r << 1);       

    while (a <= b)
    {
        lcd_draw_point(x0 + a, y0 - b, color);  
        lcd_draw_point(x0 + b, y0 - a, color);  
        lcd_draw_point(x0 + b, y0 + a, color);  
        lcd_draw_point(x0 + a, y0 + b, color);  
        lcd_draw_point(x0 - a, y0 + b, color);  
        lcd_draw_point(x0 - b, y0 + a, color);
        lcd_draw_point(x0 - a, y0 - b, color);  
        lcd_draw_point(x0 - b, y0 - a, color);  
        a++;

        if (di < 0) di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            if (xr > imax)
            {
                lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }
            xr--;
        }

        lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); 
    chr = chr - ' ';    

    switch (size)
    {
        case 12: pfont = (uint8_t *)asc2_1206[chr]; break;
        case 16: pfont = (uint8_t *)asc2_1608[chr]; break;
        case 24: pfont = (uint8_t *)asc2_2412[chr]; break;
        case 32: pfont = (uint8_t *)asc2_3216[chr]; break;
        default: return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];                            

        for (t1 = 0; t1 < 8; t1++)                  
        {
            if (temp & 0x80) lcd_draw_point(x, y, color);        
            else if (mode == 0) lcd_draw_point(x, y, g_back_color); 
            temp <<= 1;                             
            y++;
            if (y >= lcddev.height)return;          
            if ((y - y0) == size)                   
            {
                y = y0; 
                x++;    
                if (x >= lcddev.width) return;        
                break;
            }
        }
    }
}

static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--) result *= m;
    return result;
}

void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;   

        if (enshow == 0 && t < (len - 1))               
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2) * t, y, ' ', size, 0, color);  
                continue;       
            }
            else enshow = 1;      
        }
        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);   
    }
}

void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)       
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    

        if (enshow == 0 && t < (len - 1))   
        {
            if (temp == 0)
            {
                if (mode & 0x80) lcd_show_char(x + (size / 2) * t, y, '0', size, mode & 0x01, color);    
                else lcd_show_char(x + (size / 2) * t, y, ' ', size, mode & 0x01, color);    
                continue;
            }
            else enshow = 1;         
        }
        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, mode & 0x01, color);
    }
}

void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height) break;      
        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}
