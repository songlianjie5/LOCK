#include "Display.h"

#ifdef PLATFORM
#define __Display_CS_SET()          SPI_0_OLED_FLASH_ss1_m_Write(1)
#define __Display_CS_CLR()          SPI_0_OLED_FLASH_ss1_m_Write(0)
#define __Display_DC_SET()          OLED_DC_Write(1)
#define __Display_DC_CLR()          OLED_DC_Write(0)
#define __Display_WRITE_BYTE(DATA)  SPI_ReadWriteByte(DATA)
#endif

static void Display_write_array(const uint8_t *data,uint32_t len);

static void Display_write_byte(uint8_t chData, uint8_t chCmd);

uint8_t SPI_ReadWriteByte(uint8_t TxData)
{
#ifdef PLATFORM
  	SPI_0_OLED_FLASH_SpiUartWriteTxData(TxData);    
//    while(SPI_0_OLED_FLASH_SpiUartGetTxBufferSize()!=0);
    while(SPI_0_OLED_FLASH_SpiUartGetRxBufferSize()==0);
    return (uint8_t)SPI_0_OLED_FLASH_SpiUartReadRxData();
#endif
    return 0;
}

static void Display_write_byte(uint8_t chData, uint8_t chCmd)
{
#ifdef PLATFORM
    __Display_CS_CLR();
    if (chCmd) 
    {
        __Display_DC_SET();
    } else 
    {
        __Display_DC_CLR();
    }
    __Display_WRITE_BYTE(chData);
    __Display_DC_SET();
    __Display_CS_SET();
#endif
}

static void Display_write_array(const uint8_t *data,uint32_t len)
{
#ifdef PLATFORM
    __Display_CS_CLR();
    __Display_DC_SET();
    SPI_0_OLED_FLASH_SpiUartPutArray(data,len);
    while(SPI_0_OLED_FLASH_SpiUartGetTxBufferSize()!=0);
    CyDelayUs(1);
    __Display_DC_SET();
    __Display_CS_SET();    
#endif    
}

int8_t Display_init(void)
{
    OLED_FLASH_POWER(ON);//打开电源
#ifdef PLATFORM
    CyDelayUs(1000);
    SPI_0_OLED_FLASH_Start();
    CY_SET_REG32((void *)CYREG_HSIOM_PORT_SEL2,CY_GET_REG32(CYREG_HSIOM_PORT_SEL2)&(~0x000FF000));//P2_3 P2_4 CS脚
//    CyDelayUs(100);
    __Display_CS_SET();   								//CS set
    __Display_DC_CLR();   								//D/C reset
#endif

    Display_write_byte(0xAE, _CMD);//关闭显示
    Display_write_byte(0x20, _CMD);//设置存储器寻址模式
    Display_write_byte(0x10, _CMD);//00,水平寻址;01,垂直寻址;10,寻址模式复位;11,无效
    Display_write_byte(0xb0, _CMD);//设置页(行)起始地址，0-7行(页)
    Display_write_byte(0xc8, _CMD);//设置COM输出扫描方向
    Display_write_byte(0x00, _CMD);//设置列地址低位
    Display_write_byte(0x10, _CMD);//设置列地址高位
    Display_write_byte(0x40, _CMD);//设置列起始地址
    Display_write_byte(0x81, _CMD);//设置对比度控制寄存器
    Display_write_byte(0x7f, _CMD);
    Display_write_byte(0xa1, _CMD);//将片段重新映射为0到127
    Display_write_byte(0xa1, _CMD);//设置正常显示(正常/反向设置) 0xA1为正
    Display_write_byte(0xa8, _CMD);//设置复用率(1到64)
    Display_write_byte(0x3F, _CMD);//
    Display_write_byte(0xa4, _CMD);//0xa4,输出跟随RAM内容;0xa5,输出忽略RAM内容
    Display_write_byte(0xd3, _CMD);//设置显示偏移
    Display_write_byte(0x00, _CMD);//没有抵消
    Display_write_byte(0xd5, _CMD);//设置显示时钟分频比/振荡器频率
    Display_write_byte(0xf0, _CMD);//设置分频比
    Display_write_byte(0xd9, _CMD);//设置预充电期
    Display_write_byte(0x22, _CMD);//
    Display_write_byte(0xda, _CMD);//设置com引脚硬件配置
    Display_write_byte(0x12, _CMD);
    Display_write_byte(0xdb, _CMD);//设置 vcomh
    Display_write_byte(0x20, _CMD);//0x20,0.77xVcc
    Display_write_byte(0x8d, _CMD);
    Display_write_byte(0x14, _CMD);//
    Display_write_byte(0xaf, _CMD);//开启OLED面板显示
    
    Display_clear_screen();
    return 0;
}
static void Display_set_pos(uint8_t x, uint8_t y)
{
    Display_write_byte(0xB0 + y, _CMD);
    Display_write_byte(((x & 0xf0) >> 4) | 0x10, _CMD);
    Display_write_byte((x & 0x0f) | 0x01, _CMD);
}

void Display_on(void)
{
    Display_write_byte(0x8D, _CMD);
    Display_write_byte(0x14, _CMD);
    Display_write_byte(0xAF, _CMD);
}

void Display_off(void)
{
    Display_write_byte(0x8D, _CMD);
    Display_write_byte(0x10, _CMD);
    Display_write_byte(0xAE, _CMD);
}

void Display_clear_screen(void)
{
    uint8_t i;
    uint8_t data[128]={0x00};
    memset(data,0x00,128);
    for(i = 0; i < 8; i++)
    {
        Display_write_byte (0xb0 + i, _CMD);
        Display_write_byte (0x02, _CMD);
        Display_write_byte (0x10, _CMD);
        Display_write_array(data,128);
    }
    Display_write_byte (0x20, _CMD);
}

static int8_t Display_getGBcodeindex(uint16_t *index, char *gb2312)
{
    uint16_t i;
    uint16_t code;
    uint8_t str[2];

    memcpy(str, gb2312, 2);
    code  = (str[0] << 8) | str[1];
    for(i = 0; i < CN_NUM; i++)
    {
        if(CN16X16[i].GBcode == code)
        {
            *index = i;
            return 0;
        }
    }
    return -1;
}

void Display_picture(uint8_t x, uint8_t y, uint8_t w, uint8_t h, char *pic)
{
    uint16_t j = 0;
    uint8_t xx, yy;

    if(h % 8 == 0) yy = h / 8;
    else yy = h / 8 + 1;
    for(yy = y; yy < h; yy++)
    {
        Display_set_pos(x, yy);
        for(xx = x; xx < w; xx++)
        {
            Display_write_byte(pic[j++], _DAT);
        }
    }
}

void Display_char(uint8_t x, uint8_t y, uint8_t chr,uint8_t N)
{
    uint8_t c = 0, i = 0;
    uint8_t strbuf[8];
    c = chr - ' ';
    if(x > 128 - 1) {
        x = 0;
        y = y + 2;
    }
    if (N)
    {
        for (i=0;i<8;i++)   
            strbuf[i]=~F8X16[c*16+i];
        Display_set_pos(x, y);
        Display_write_array(strbuf,8);
        for (i=0;i<8;i++)   
            strbuf[i]=~F8X16[c*16+8+i];
        Display_set_pos(x, y + 1);
        Display_write_array(strbuf,8);
    }
    else
    {
        Display_set_pos(x, y);
        Display_write_array(&F8X16[c*16],8);
        Display_set_pos(x, y + 1);
        Display_write_array(&F8X16[c*16+8],8);
    }
}

void Display_english_str(uint8_t x, uint8_t y, char *str, uint8_t N)
{
    uint8_t j = 0;
    while (str[j] != '\0')
    {
        Display_char(x, y * 2, str[j],N);
        x += 8;
        if(x > 120) {
            x = 0;
            y += 2;
        }
        j++;
    }
}

void Display_chinese_str(uint8_t x, uint8_t y, char *str, uint8_t N)
{
    uint8_t i,num=0;
    uint16_t GBcodeindex = 0;
    uint8_t strbuf[16];
    while(*str)
    {
        if(Display_getGBcodeindex(&GBcodeindex, str) == 0)
        {
            if(N)
            {
                Display_set_pos(x+16*num, y*2);
                for(i = 0; i < 16; i++)
                    strbuf[i] = ~CN16X16[GBcodeindex].dat[i];
                Display_write_array(strbuf,16);    
                Display_set_pos(x+16*num, y*2 + 1);
                for(i = 0; i < 16; i++)
                    strbuf[i] = ~CN16X16[GBcodeindex].dat[16+i];
                Display_write_array(strbuf,16);
            }
            else
            {
                Display_set_pos(x+16*num, y*2);
                Display_write_array(CN16X16[GBcodeindex].dat,16);
                Display_set_pos(x+16*num, y*2 + 1);
                Display_write_array(&(CN16X16[GBcodeindex].dat[16]),16);
            }
        }
        num++;
        str +=2;
    }
}
