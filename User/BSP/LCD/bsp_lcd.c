#include "main.h"
#include "bsp_lcd.h"

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

    lcd_display_dir(1); //横屏
    
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
