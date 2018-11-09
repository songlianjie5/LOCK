#ifndef __NXP520_H__
#define __NXP520_H__
//等待PICC返回的循环和低功耗波形相关
#define TOLERANT_WAIT_LOOP       35

//天线设置 单天线 1，2 ，双天线
#define ANT_CONFIG             ANT_TXD

#define ANT_TX1                  1
#define ANT_TX2                  2
#define ANT_TXD                  3 //双天线

/***************MF520命令字****************/
#define PCD_IDLE              0x00//取消当前命令
#define PCD_MEM               0x01//储存25字节数据到内部BUFF 
#define PCD_CALCCRC           0x03//CRC计算
#define PCD_TRANSMIT          0x04//发送数据
#define PCD_NOCMDCHANGE       0x07//设置CommandReg 不改变里面的Command 

#define PCD_RECEIVE           0x08//接收数据
#define PCD_TRANSCEIVE        0x0C//发送并接收数据
#define PCD_AUTHENT           0x0E//验证密钥
#define PCD_RESETPHASE        0x0F//复位
/**********Mifare_One卡片命令字************/
#define PICC_REQIDL           0x26//寻天线区内未进入休眠状态
#define PICC_REQALL           0x52//寻天线区内全部卡
#define PICC_ANTICOLL1        0x93//防冲撞
#define PICC_ANTICOLL2        0x95//防冲撞
#define PICC_ANTICOLL3        0x97//防冲撞3
#define PICC_AUTHENT1A        0x60//验证A密钥
#define PICC_AUTHENT1B        0x61//验证B密钥
#define PICC_READ             0x30//读块
#define PICC_WRITE            0xA0//写块
#define PICC_DECREMENT        0xC0//扣款
#define PICC_INCREMENT        0xC1//充值
#define PICC_RESTORE          0xC2//调块数据到缓冲区
#define PICC_TRANSFER         0xB0//保存缓冲区中数据
#define PICC_HALT             0x50//休眠

/**************MFRC520寄存器定义**************/
// Page 0 :  控制和状态   
#define     RFU00                 0x00     //保留
#define     CommandReg            0x01     //启动和停止指令
#define     ComIEnReg             0x02     //使能和禁用中断请求
#define     DivlEnReg             0x03     //使能和禁用中断请求
#define     ComIrqReg             0x04     //中断请求
#define     DivIrqReg             0x05     //中断请求
#define     ErrorReg              0x06     //显示上一个指令执行的错误状态
#define     Status1Reg            0x07     //通信状态
#define     Status2Reg            0x08     //接收和发送状态
#define     FIFODataReg           0x09     //64 字节 FIFO 缓冲区的输入和输出
#define     FIFOLevelReg          0x0A     //FIFO 缓冲区中已存储字节的数量
#define     WaterLevelReg         0x0B     //FIFO 缓冲区溢出和空警告
#define     ControlReg            0x0C     //多种控制寄存器
#define     BitFramingReg         0x0D     //面向位的帧的调整
#define     CollReg               0x0E     //检查产生位冲突的第一个位的地址
#define     RFU0F                 0x0F     //保留
//PAGE 1 : 通信
#define     RFU10                 0x10     //保留
#define     ModeReg               0x11     //发送和接收的通用模式的设置
#define     TxModeReg             0x12     //数据发送时的传输速率
#define     RxModeReg             0x13     //数据接收时的传输速率
#define     TxControlReg          0x14     //天线驱动器引脚 TX1 和 TX2 的控制
#define     TxASKReg              0x15     //发送调制的设置
#define     TxSelReg              0x16     //选择天线驱动的内部信号源
#define     RxSelReg              0x17     //内部接收器的设置
#define     RxThresholdReg        0x18     //位解码器阀值的选择
#define     DemodReg              0x19     //解调器的设置
#define     RFU1A                 0x1A     //保留
#define     RFU1B                 0x1B     //保留
#define     MfTxReg               0x1C     //MIFARE 通信时发送参数的设置
#define     MfRxReg               0x1D     //MIFARE 通信时接收参数的设置
#define     RFU1E                 0x1E     //保留
#define     RFU1F                 0x1F     //保留
//Page 2 : 配置
#define     RFU20                 0x20     //保留
#define     CRCResultRegM         0x21     //CRC 计算后 MSB 位的值
#define     CRCResultRegL         0x22     //CRC 计算后 LSB 位的值
#define     RFU23                 0x23     //保留
#define     ModWidthReg           0x24     //调制宽度的设置
#define     RFU25                 0x25     //保留
#define     RFCfgReg              0x26     //接收增益的配置
#define     GsNReg                0x27     //选择天线驱动器引脚 TX1 和 TX2 在调制时的电导值
#define     CWGsCfgReg            0x28     //定义p-driver无调制的输出电导
#define     ModGsCfgReg           0x29     //定义p-driver经过调制的输出电导
#define     TModeReg              0x2A     //定义定时器的设置中的 Tprescaler 设置高 4 位的值
#define     TPrescalerReg         0x2B     //TprescalerReg 寄存器设置低 8 位的值
#define     TReloadRegH           0x2C     //16 位定时器的重载值高8位
#define     TReloadRegL           0x2D     //16 位定时器的重载值低8位
#define     TCounterValueRegH     0x2E     //显示 16 定时器的当前值高8位
#define     TCounterValueRegL     0x2F     //显示 16 定时器的当前值低8位
//Page 3: 测试
#define     RFU30                 0x30     //保留
#define     TestSel1Reg           0x31     //测试信号的配置
#define     TestSel2Reg           0x32     //测试信号的配置和PRBS 控制
#define     TestPinEnReg          0x33     //使能D1到D7 的输出驱动器
#define     TestPinValueReg       0x34     //定义引脚 DI 到 D7 当其用于 I/O 口时的电平值
#define     TestBusReg            0x35     //显示内部测试总线的状态
#define     AutoTestReg           0x36     //数字自检的控制
#define     VersionReg            0x37     //显示软件版本
#define     AnalogTestReg         0x38     //引脚AUX1和AUX2 的控制
#define     TestDAC1Reg           0x39     //定义TestDAC1 的测试值
#define     TestDAC2Reg           0x3A     //定义TestDAC2 的测试值
#define     TestADCReg            0x3B     //显示 ADC I 和 Q 通道的值
#define     RFU3C                 0x3C     //保留
#define     RFU3D                 0x3D     //保留
#define     RFU3E                 0x3E     //保留
#define     RFU3F		          0x3F     //保留

/************和MF522通讯时返回的错误代码*********/
#define MI_OK                     0
#define MI_NOTAGERR              (-1)
#define MI_ERR                   (-2)
#define MI_COLLERR                3
#define MI_CT					  1				     // CASCADE TAG
#define DEF_FIFO_LENGTH           64                 //FIFO size=64byte
#define MAXRLEN                   18

signed char Nxp520_Reset(void);
extern void Nxp520_Init(void);
void Nxp520_Antenna_On( void );
unsigned char Nxp520_ReadCardId(unsigned short *pType);
void Nxp520_Antenna_Off( void );
signed char Nxp520_Request(unsigned char req_code, unsigned char *pTagType);
signed char Nxp520_Anticoll(unsigned char *pSnr);
signed char Nxp520_Select(unsigned char *pSnr);
signed char Nxp520_Auth_State(unsigned char auth_mode, unsigned char addr, unsigned char *pKey, unsigned char *pSnr);
signed char Nxp520_write(unsigned char addr, unsigned char *pData);
signed char Nxp520_Read(unsigned char addr, unsigned char *pData);
signed char Nxp520_Halt(void);
signed char Lowpower_Request(unsigned char req_code, unsigned char *pTagType);
signed char Lowpower_ReadCardrequest(unsigned char req_code, unsigned char *pTagType);
void Ad_Nxp520_Antenna_Off( void );
char UL_PcdAnticoll( unsigned char *pSnr, unsigned char *cardlen);
signed char nxp520ConfigISOType(unsigned char type);
extern void Nxp520_Reset_Wait(void);

#endif
