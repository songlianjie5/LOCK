#include "nxp520.h"
#include "project.h"

/*
一、MF1卡(S50/S70卡)的技术参数
　　1、工作频率：13.56MHz
　　2、存储容量：
　　　　S50卡：1024字节，16个扇区，每个扇区4个块,每块16个字节
　　　　S70卡：4096字节，40个扇区，前32个扇区每扇区4个块，后8个扇区每扇区16个块
　　3、协议标准：ISO 14443 A
　　4、擦写寿命：大于100,000次
　　5、数据保存时间：10年
　　6、应用范围：在一卡通领域等广泛应用


            0块数据  card1   card2   card3
卡号[0]     byte[0]  0x12    0xf5    0x42
卡号[1]     byte[1]  0x01    0xd2    0xdc
卡号[2]     byte[2]  0xf0    0x5d    0xc5
卡号[3]     byte[3]  0x43    0xbb    0xb6
卡号CRC     byte[4]  0xa0    0xc1    0xed
卡SIZE      byte[5]  0x08    0x08    0x08
卡片类型    byte[6]  0x04    0x04    0x04
            byte[7]  0x00    0x00    0x00
            byte[8]  0x69    0x23    0x62
            byte[9]  0x73    0x56    0x63
            byte[10] 0x73    0x83    0x64
            byte[11] 0x69    0x49    0x65
            byte[12] 0x35    0x04    0x66
            byte[13] 0x31    0x11    0x67
            byte[14] 0x34    0xff    0x68
            byte[15] 0x30    0xff    0x69
   */

/*****************SPI接口定义******************/
#define MF_CS_H      SPI_1_CARD_ss0_m_Write(1);//CS控制引脚
#define MF_CS_L      SPI_1_CARD_ss0_m_Write(0);
#define MF_NRST_H    CARD_RESET_Write(1);//复位引脚
#define MF_NRST_L    CARD_RESET_Write(0);

#define delay_us(us)  CyDelayUs(us)
#define delay_ms(ms)  CyDelay(ms)

/*******************内部函数***********************/
static uint8_t Spi_ReadWrite_Byte(uint8_t byte);//读写一个字节

static void Nxp520_Portinit(void);//读卡SPI接口初始化

static void ClearBitMask(unsigned char  reg, unsigned char  mask);

static void SetBitMask(unsigned char  reg, unsigned char  mask);

static void spi_write_string(uint8_t cmd, uint8_t *pointer, uint16_t len);

static void spi_read_string(uint8_t cmd, uint8_t *pointer, uint16_t len);

static void Nxp520_Write_Reg(uint8_t addr, uint8_t date);

static uint8_t Nxp520_Read_Reg(uint8_t addr);

static void Set_Reg_Mask(unsigned char reg, unsigned char mask);

static void cls_reg_mask(unsigned char reg, unsigned char mask);

static int8_t  nxp520_communicate_card( unsigned char Command,
                                        unsigned char *pInData,
                                        unsigned char InLenByte,
                                        unsigned char *pOutData,
                                        unsigned int  *pOutLenBit);

static uint8_t Spi_ReadWrite_Byte(uint8_t byte)//读写一个字节
{
    uint32_t i = 0;
    SPI_1_CARD_SpiUartWriteTxData((uint32_t)byte);
    while(SPI_1_CARD_SpiUartGetRxBufferSize()==0&&(++i)<10000);
    return (uint8_t)SPI_1_CARD_SpiUartReadRxData();
}

static void Nxp520_Portinit(void)//读卡SPI接口初始化
{
    SPI_1_CARD_Start();
    CY_SET_REG32((void *)CYREG_HSIOM_PORT_SEL4,CY_GET_REG32(CYREG_HSIOM_PORT_SEL4)&(~0xF0000000));//设置P4_7 CS脚为固件控制
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
static void ClearBitMask(unsigned char  reg, unsigned char  mask)
{
    char  tmp = 0x0;
    tmp = Nxp520_Read_Reg(reg);
    Nxp520_Write_Reg(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
static void SetBitMask(unsigned char  reg, unsigned char  mask)
{
    char  tmp = 0x0;
    tmp = Nxp520_Read_Reg(reg);
    Nxp520_Write_Reg(reg, tmp | mask);  // set bit mask
}

//SPI写数据
static void spi_write_string(uint8_t cmd, uint8_t *pointer, uint16_t len)
{
    uint8_t i;
    MF_CS_L;
    Spi_ReadWrite_Byte(cmd);
    for ( i = 0; i < len; i++ )
    {
        Spi_ReadWrite_Byte(pointer[i]);
    }
    MF_CS_H;
}

//SPI读数据
static void spi_read_string(uint8_t cmd, uint8_t *pointer, uint16_t len)
{
    uint8_t i;
    MF_CS_L;
    Spi_ReadWrite_Byte(cmd);
    for ( i = 0; i < len; i++ )
    {
        pointer[i] = Spi_ReadWrite_Byte(0x00);
    }
    MF_CS_H;
}

//NXP520 写寄存器
static void Nxp520_Write_Reg(uint8_t addr, uint8_t date)
{
    addr = (addr << 1) & 0x7e; //写寄存器的时候，地址最高位为 0，最低位为0，1-6位取决于地址
    MF_CS_L;
    Spi_ReadWrite_Byte(addr);
    Spi_ReadWrite_Byte(date);
    MF_CS_H;
}

//NXP520 读寄存器
static uint8_t Nxp520_Read_Reg(uint8_t addr)
{
    uint8_t ret;
    addr = ((addr << 1) & 0x7e) | 0x80; //读寄存器的时候，地址最高位为 1，最低位为0，1-6位取决于地址
    MF_CS_L;
    Spi_ReadWrite_Byte(addr);
    ret = Spi_ReadWrite_Byte(0);
    MF_CS_H;
    return  ret;
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
static void Set_Reg_Mask(unsigned char reg, unsigned char mask)
{
    char tmp = 0x0;
    tmp = Nxp520_Read_Reg(reg);
    Nxp520_Write_Reg(reg, tmp | mask);
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
static void cls_reg_mask(unsigned char reg, unsigned char mask)
{
    char tmp = 0x0;
    tmp = Nxp520_Read_Reg(reg);
    Nxp520_Write_Reg(reg, tmp & ~mask);
}

signed char Nxp520_Reset(void)
{
	uint16_t i = 10000;
	MF_NRST_L;
	MF_NRST_H;

    while((--i)!=0&&Nxp520_Read_Reg(CommandReg) != 0x20);//等待芯片就绪
    
    Nxp520_Write_Reg(CommandReg, PCD_RESETPHASE);//MF CV520 的复位
    Nxp520_Write_Reg(ModeReg, 0x3D);//如果 RF 场产生，则发送器启动,MFIN 引脚高电平有效,定义 CRC 处理器执行 CalcCRC 指令的预设值6363h
    Nxp520_Write_Reg(TReloadRegL,30);//16 位定时器的重载值低位
    Nxp520_Write_Reg(TReloadRegH,0);//16 位定时器的重载值高位
    Nxp520_Write_Reg(TModeReg, 0x8D);//内部定时器的设置高4位
    Nxp520_Write_Reg(TPrescalerReg, 0x3E);//内部定时器的设置低8位
    Nxp520_Write_Reg(GsNReg, 0xf8);//定义天线驱动器 TX1 和 TX2 为 n-driver 时的电导率
    Nxp520_Write_Reg(RxThresholdReg, 0x55);//位解码器阀值的选择
//    Nxp520_Write_Reg(RFCfgReg,0x68);//接收增益的配置
    Nxp520_Write_Reg(TxASKReg, 0x40);//强制进行 100 % ASK 的调制
    
    return MI_OK;
}

//与卡片通信
/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
//////////////////////////////////////////////////////////////////
static int8_t  nxp520_communicate_card( unsigned char Command,
                             unsigned char *pInData,
                             unsigned char InLenByte,
                             unsigned char *pOutData,
                             unsigned int  *pOutLenBit)
{
    signed   char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    volatile unsigned int i;

    switch (Command)
    {
        case PCD_AUTHENT:
        {
            irqEn   = 0x12;
            waitFor = 0x10;
        }
        break;

        case PCD_TRANSCEIVE:
        {
            irqEn   = 0x77;
            waitFor = 0x30;
        }
        break;
        default:
            break;
    }

    Nxp520_Write_Reg(ComIEnReg, irqEn | 0x80);//使能接受和发送中断请求 
    cls_reg_mask(ComIrqReg, 0x80); //置ComIrqReg为0xff
    Nxp520_Write_Reg(CommandReg, PCD_IDLE); //取消当前命令
    Set_Reg_Mask(FIFOLevelReg, 0x80);
    for (i = 0; i < InLenByte; i++)
    {
        Nxp520_Write_Reg(FIFODataReg, pInData[i]);
    }
    Nxp520_Write_Reg(CommandReg, Command); //开始发送
    if (Command == PCD_TRANSCEIVE)
    {
        Set_Reg_Mask(BitFramingReg, 0x80);
    }

    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
        n = Nxp520_Read_Reg(ComIrqReg);
        i--;
    }
    while ((i != 0) && !(n & 0x01) && !(n & waitFor));
    cls_reg_mask(BitFramingReg, 0x80);//发送结束

    if (i != 0) //发送完成
    {
        if(!(Nxp520_Read_Reg(ErrorReg) & 0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }

            if (Command == PCD_TRANSCEIVE)
            {
                n = Nxp520_Read_Reg(FIFOLevelReg);
                lastBits = Nxp520_Read_Reg(ControlReg) & 0x07;

                if (lastBits) *pOutLenBit = (n - 1) * 8 + lastBits;
                else
                {
                    *pOutLenBit = n * 8;
                }

                if (n == 0) n = 1;

                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++)
                {
                    pOutData[i] = Nxp520_Read_Reg(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

    Set_Reg_Mask(ControlReg, 0x80);          
    Nxp520_Write_Reg(CommandReg, PCD_IDLE);

    return status;
}

/*************************外部接口函数**************************/
//NXP520初始化
void Nxp520_Init(void)
{
    Nxp520_Portinit();
    Nxp520_Reset();
}

void Nxp520_Reset_Wait(void)
{
    extern void Wdt_Feed_Dog(void);
    uint16_t i = 10000;
    do
    {
        Wdt_Feed_Dog();
    }while((--i)!=0&&Nxp520_Read_Reg(CommandReg) != 0x20);//等待芯片就绪
}

//开启天线
void Nxp520_Antenna_On( void )
{
    unsigned char i;
    i = Nxp520_Read_Reg(TxControlReg);
    if (!(i & ANT_CONFIG))
    {
        Set_Reg_Mask(TxControlReg, ANT_CONFIG);
//        Set_Reg_Mask(TxControlReg, ANT_TX1); 
    }
}

//关闭天线
void Nxp520_Antenna_Off( void )
{
    cls_reg_mask(TxControlReg, ANT_CONFIG);
//    cls_reg_mask(TxControlReg, ANT_TX1);
}

void Ad_Nxp520_Antenna_Off( void )
{
    cls_reg_mask(TxControlReg, ANT_TX1);
}

//////////////////////////////////////////////////////////////////////
//设置RC522的工作方式
//////////////////////////////////////////////////////////////////////
signed char nxp520ConfigISOType(uint8_t type)
{
    if (type == 'A')
    {
        ClearBitMask(Status2Reg, 0x08);
        Nxp520_Write_Reg(ModeReg, 0x3D);
        Nxp520_Write_Reg(RxSelReg, 0x86);
        Nxp520_Write_Reg(RFCfgReg, 0x7F);
        Nxp520_Write_Reg(TReloadRegL, 30);
        Nxp520_Write_Reg(TReloadRegH, 0);
        Nxp520_Write_Reg(TModeReg, 0x8D);
        Nxp520_Write_Reg(TPrescalerReg, 0x3E);
        delay_ms(10);
        Nxp520_Antenna_On();
    }
    else
    {
        return MI_NOTAGERR;
    }

    return MI_OK;
}

/**
  * @brief  读取卡片ID
  *           
  * @param  pId：卡片ID数据指针
  * @param  type：卡片类型代码
	*         0x4400 = Mifare_UltraLight
	*         0x0400 = Mifare_One(S50)
	*         0x0200 = Mifare_One(S70)
	*         0x0800 = Mifare_Pro(X)
	*         0x4403 = Mifare_DESFire
  *
  * @return 返回ID号长度，读取失败返回0
  */
uint8_t Nxp520_ReadCardId(uint16_t *pType)
{
//	uint8_t len = 0;

	if (Lowpower_Request(PICC_REQIDL, (uint8_t *)pType) == MI_OK)
	{
		return  MI_OK;
	}
	return 0;
}

//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//                pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
signed char Nxp520_Request(unsigned char req_code, unsigned char *pTagType)
{
    volatile signed char status;  //不加 volatile 可能会被优化 2012.007.05
    volatile unsigned int  unLen;
    volatile unsigned char ucComMF522Buf[MAXRLEN];
    volatile unsigned char *p = pTagType;
    
    Nxp520_Write_Reg(TxModeReg,0);
    Nxp520_Write_Reg(RxModeReg,0);
    cls_reg_mask(Status2Reg, 0x08);  //清除密文发送      
    Nxp520_Write_Reg(BitFramingReg, 0x07); //最后字节只传7位，请求命令是短帧
    Set_Reg_Mask(TxControlReg, ANT_CONFIG); //打开天线脚驱动 

    ucComMF522Buf[0] = req_code;//寻天线为进入休眠状态的卡

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x10))
    {
        *p     = ucComMF522Buf[0];
        *(p + 1) = ucComMF522Buf[1];
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
signed char Nxp520_Anticoll(unsigned char *pSnr)
{
    signed status;
    unsigned char i, snr_check = 0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];
    cls_reg_mask(Status2Reg, 0x08); //清0，当验证密码成功时此位置1
    Nxp520_Write_Reg(BitFramingReg, 0x00);
    cls_reg_mask(CollReg, 0x80);
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;
    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen);
    if (status == MI_OK)
    {
        for (i = 0; i < 4; i++)
        {
            *(pSnr + i)  = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i])
        {
            status = MI_ERR;
        }
    }
    Set_Reg_Mask(CollReg, 0x80);
    Nxp520_Antenna_Off();
    return status;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void nxp520_calulate_crc(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
    unsigned char i, n;

    cls_reg_mask(DivIrqReg, 0x04);
    Nxp520_Write_Reg(CommandReg, PCD_IDLE);
    Set_Reg_Mask(FIFOLevelReg, 0x80);
    for (i = 0; i < len; i++)
    {
        Nxp520_Write_Reg(FIFODataReg, *(pIndata + i));
    }
    Nxp520_Write_Reg(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = Nxp520_Read_Reg(DivIrqReg);
        i--;
    }
    while ((i != 0) && !(n & 0x04));
    pOutData[0] = Nxp520_Read_Reg(CRCResultRegL);
    pOutData[1] = Nxp520_Read_Reg(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
signed char Nxp520_Select(unsigned char *pSnr)
{
    signed char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i = 0; i < 4; i++)
    {
        ucComMF522Buf[i + 2] = *(pSnr + i);
        ucComMF522Buf[6]  ^= *(pSnr + i);
    }
    nxp520_calulate_crc(ucComMF522Buf, 7, &ucComMF522Buf[7]);

    cls_reg_mask(Status2Reg, 0x08);

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {
        status = MI_OK;
        pSnr[5] =  ucComMF522Buf[0]; //卡容量2012.07.03
    }
    else
    {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
signed char Nxp520_Auth_State(unsigned char auth_mode, unsigned char addr, unsigned char *pKey, unsigned char *pSnr)
{
    signed char   status;
    unsigned int  unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i = 0; i < 6; i++)
    {
        ucComMF522Buf[i + 2] = *(pKey + i);
    }
    for (i = 0; i < 6; i++)
    {
        ucComMF522Buf[i + 8] = *(pSnr + i);
    }

    status = nxp520_communicate_card(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);
    if ((status != MI_OK) || (!(Nxp520_Read_Reg(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
signed char Nxp520_Read(unsigned char addr, unsigned char *pData)
{
    signed char status;
    unsigned int  unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    nxp520_calulate_crc(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i = 0; i < 16; i++)
        {
            *(pData + i) = ucComMF522Buf[i];
        }
    }
    else
    {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
signed char Nxp520_write(unsigned char addr, unsigned char *pData)
{
    signed char status;
    unsigned int  unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    nxp520_calulate_crc(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
       
        for (i = 0; i < 16; i++)
        {
            ucComMF522Buf[i] = *(pData + i);
        }
        nxp520_calulate_crc(ucComMF522Buf, 16, &ucComMF522Buf[16]);

        status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
signed char nxp520_value(unsigned char dd_mode, unsigned char addr, unsigned char *pValue)
{
    signed char status;
    unsigned int  unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    nxp520_calulate_crc(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
       
        for (i = 0; i < 16; i++)
        {
            ucComMF522Buf[i] = *(pValue + i);
        }
        nxp520_calulate_crc(ucComMF522Buf, 4, &ucComMF522Buf[4]);
        unLen = 0;
        status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 6, ucComMF522Buf, &unLen);
        if (status != MI_ERR)
        {
            status = MI_OK;
        }
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        nxp520_calulate_crc(ucComMF522Buf, 2, &ucComMF522Buf[2]);

        status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
signed char nxp520_bak_value(unsigned char sourceaddr, unsigned char goaladdr)
{
    signed char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    nxp520_calulate_crc(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        nxp520_calulate_crc(ucComMF522Buf, 4, &ucComMF522Buf[4]);

        status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 6, ucComMF522Buf, &unLen);
        if (status != MI_ERR)
        {
            status = MI_OK;
        }
    }

    if (status != MI_OK)
    {
        return MI_ERR;
    }

    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    nxp520_calulate_crc(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
signed char Nxp520_Halt(void)
{
    signed char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    nxp520_calulate_crc(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    return status;
}

//以上为标准接口
//***********************************************************************************
//低功耗寻卡   周期：520ms cv520工作时间：2.150ms 天线寻卡时间：920us

/////////////////////////////////////////////////////////////////////
//功    能：自定义寻卡    开天线时间没那么块
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
//***********************************************************************************
signed char lowpower_communicate_card( unsigned char Command,
                                       unsigned char *pInData,
                                       unsigned char InLenByte,
                                       unsigned char *pOutData,
                                       unsigned int  *pOutLenBit)
{
    signed   char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;

    switch (Command)
    {
        case PCD_AUTHENT:
        {
            irqEn   = 0x12;
            waitFor = 0x10;
        }
        break;

        case PCD_TRANSCEIVE:
        {
            irqEn   = 0x77;
            waitFor = 0x30;
        }
        break;

        default:
            break;
    }

    Nxp520_Write_Reg(ComIEnReg, irqEn | 0x80); //配置中断输出电平，允许一些中断
    cls_reg_mask(ComIrqReg, 0x80);
    Nxp520_Write_Reg(CommandReg, PCD_IDLE);
    Set_Reg_Mask(FIFOLevelReg, 0x80);

    Set_Reg_Mask(TxControlReg, ANT_CONFIG); //打开天线脚驱动     低功耗设计 
    delay_ms(1);
    for (i = 0; i < InLenByte; i++)
    {
        Nxp520_Write_Reg(FIFODataReg, pInData[i]);
    }
    Nxp520_Write_Reg(CommandReg, Command);

    if (Command == PCD_TRANSCEIVE) Set_Reg_Mask(BitFramingReg, 0x80);

    i = 600;//Get_Waitcmd_Loop();//根据时钟频率调整，n = 600操作M1卡最大等待时间25ms
    do
    {
        n = Nxp520_Read_Reg(ComIrqReg);
        i--;
    }
    while ((i != 0) && !(n & 0x01) && !(n & waitFor));

    cls_reg_mask(BitFramingReg, 0x80);

    if (i != 0) //发送完成
    {
        status = Nxp520_Read_Reg(ErrorReg);
        Set_Reg_Mask(ErrorReg, 0x00);
        if(!(Nxp520_Read_Reg(ErrorReg) & 0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01) status = MI_NOTAGERR;

            if (Command == PCD_TRANSCEIVE)
            {
                n = Nxp520_Read_Reg(FIFOLevelReg);
                lastBits = Nxp520_Read_Reg(ControlReg) & 0x07;
                if (lastBits) *pOutLenBit = (n - 1) * 8 + lastBits;
                else  *pOutLenBit = n * 8;

                if (n == 0) n = 1;
                if (n > MAXRLEN) n = MAXRLEN;
                for (i = 0; i < n; i++)
                {
                    pOutData[i] = Nxp520_Read_Reg(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

    Set_Reg_Mask(ControlReg, 0x80);          // stop timer now
    Nxp520_Write_Reg(CommandReg, PCD_IDLE);

    return status;
}

signed char Lowpower_Request(unsigned char req_code, unsigned char *pTagType)
{
    volatile signed char   status;  //不加 volatile 可能会被优化 2012.007.05
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];
    unsigned char *p = pTagType;
    Set_Reg_Mask(TxControlReg, ANT_CONFIG); //打开天线脚驱动
    delay_us(200);		//有些卡片震荡起来慢，延时以适配不同卡片
    cls_reg_mask(Status2Reg, 0x08);        //清除密文发送
    cls_reg_mask(TxModeReg, 0x80); //ZG
    cls_reg_mask(RxModeReg, 0x80); //ZG
    Nxp520_Write_Reg(BitFramingReg, 0x07); //最后字节只传7位，请求命令是短帧！！！！
    //Set_Reg_Mask(TxControlReg,ANT_CONFIG);   //打开天线脚驱动      这里先不开，在后面 nxp520_communicate_card中凯

    ucComMF522Buf[0] = req_code;

    status = lowpower_communicate_card(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen); //寻卡

    if ((status == MI_OK) && (unLen == 0x10))
    {
        *p     = ucComMF522Buf[0];
        *(p + 1) = ucComMF522Buf[1];
    }
    else
    {
        status = MI_ERR;
    }
    return status;
}

//***********************************************************************************
//功    能：天线在这里面开
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
//***********************************************************************************
signed char lowpower_cate_card( unsigned char Command,
                                unsigned char *pInData,
                                unsigned char InLenByte,
                                unsigned char *pOutData,
                                unsigned int  *pOutLenBit
                              )
{
//    signed   char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
//    unsigned char lastBits;
    unsigned char n;
    unsigned int i;

    switch (Command)
    {
        case PCD_AUTHENT:
        {
            irqEn   = 0x12;
            waitFor = 0x10;
        }
        break;

        case PCD_TRANSCEIVE:
        {
            irqEn   = 0x77;
            waitFor = 0x30;
        }
        break;

        default:
            break;
    }
    Nxp520_Write_Reg(ComIEnReg, irqEn | 0x80); //配置中断输出电平，允许一些中断
    cls_reg_mask(ComIrqReg, 0x80);
    Nxp520_Write_Reg(CommandReg, PCD_IDLE);
    Set_Reg_Mask(FIFOLevelReg, 0x80);
    //Set_Reg_Mask(TxControlReg,ANT_CONFIG);   //打开天线脚驱动     低功耗设计
    delay_ms( 2 );
    for (i = 0; i < InLenByte; i++)
    {
        Nxp520_Write_Reg(FIFODataReg, pInData[i]);
    }
    Nxp520_Write_Reg(CommandReg, Command);

    if (Command == PCD_TRANSCEIVE) Set_Reg_Mask(BitFramingReg, 0x80);

    i = 1000;// Get_Waitcmd_Loop();//根据时钟频率调整，n = 600操作M1卡最大等待时间25ms
    do
    { 
        n = Nxp520_Read_Reg(ComIrqReg);
        if(n == 0X64)break;
        i--;
    }
    while ((i != 0) && !(n & 0x01) && !(n & waitFor));
    Nxp520_Antenna_Off();
    return n;
}

signed char Lowpower_ReadCardrequest(unsigned char req_code, unsigned char *pTagType)
{
    volatile signed char   status;  //不加 volatile 可能会被优化 2012.007.05
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];
    // unsigned char *p = pTagType;
    Set_Reg_Mask(TxControlReg, ANT_TX1); 	//打开天线脚驱动
    cls_reg_mask(Status2Reg, 0x08);        	//清除密文发送
    Nxp520_Write_Reg(BitFramingReg, 0x07); 	//最后字节只传7位，请求命令是短帧！！！！
    ucComMF522Buf[0] = req_code;
    status = lowpower_cate_card(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen); //寻卡
    return status;
}

/*************************************************************************************************
 CV520_PcdAnticoll() : 通过防冲撞得到卡号.
参数:   nLevel 冲撞等级 0 --2
        pID    ID return
返回:   MI_OK / MI_ERR
注意: 如果卡号[0]为0x88, 表示还有后续卡号
**************************************************************************************************/
static char CV520_PcdAnticoll( char nLevel, unsigned char *pID )
{
    char status = MI_ERR;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComBuf[MAXRLEN]={0};
    unsigned char ucID[MAXRLEN]={0};
    unsigned char	ucAntiColl[] = { PICC_ANTICOLL1, PICC_ANTICOLL2, PICC_ANTICOLL3 };
//    memset(ucComBuf,0,MAXRLEN);
//    memset(ucID,0,MAXRLEN);
    // 第n级防冲撞. 得到ID的4个字节
    ClearBitMask(Status2Reg, 0x08);
    Nxp520_Write_Reg(BitFramingReg, 0x00);
    ClearBitMask(CollReg, 0x80);

    if( nLevel > 2 )
        nLevel = 2;

    ucComBuf[0] = ucAntiColl[nLevel];
    ucComBuf[1] = 0x20;

    status = nxp520_communicate_card( PCD_TRANSCEIVE, ucComBuf, 2, ucID, &unLen );

    if (status != MI_OK)
    {
        return MI_ERR;
    }

    // 根据ISO14333规定, 如果ID[0]为0x88, 表示Cascade tag, 后续3字节为有效ID
    if( ucID[0] == 0x88 )
        memcpy( pID, &ucID[1], 3 );
    else
        memcpy( pID, &ucID[0], 4 );

    // 防冲撞OK, 这里select卡
    SetBitMask( CollReg, 0x80 );
    ClearBitMask(Status2Reg, 0x08);
    Nxp520_Write_Reg(BitFramingReg, 0x00);
    ClearBitMask(CollReg, 0x80);

    ucComBuf[0] = ucAntiColl[nLevel];
    ucComBuf[1] = 0x70;

    for (i = 0; i < 5; i++)
    {
        ucComBuf[i + 2] = ucID[i];
    }
    nxp520_calulate_crc( ucComBuf, 7, &ucComBuf[7] );

    status = nxp520_communicate_card(PCD_TRANSCEIVE, ucComBuf, 9, ucComBuf, &unLen);

    if( status == MI_OK && ucID[0] == 0x88 )
        status = MI_CT;

    return status;
}

/////////////////////////////////////////////////////////////////////
//  UL_PcdAnticoll() : 得到MIFIRE的卡号
//    4字节卡号一次冲撞, 7字节/10字节需要2/3级冲撞才能得到卡号
/////////////////////////////////////////////////////////////////////
char UL_PcdAnticoll( unsigned char *pSnr, unsigned char *cardlen)
{
    char status ;

    status = CV520_PcdAnticoll( 0, pSnr );
	*cardlen = 4;

    if(status == MI_CT)
	{
        status = CV520_PcdAnticoll( 1, pSnr + 3 );
		*cardlen = 7;
	}
    if( status == MI_CT )
	{
        status = CV520_PcdAnticoll( 2, pSnr + 6 );
		*cardlen = 10;
	}
    return status;
}

