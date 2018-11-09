#ifndef __LED_H#define __LED_H    #include "main.h"typedef enum{    IDEL=0,    ALL_ON,                         //全部LED打开    ALL_OFF,                        //全部LED关闭    TUCH,                           //触摸    XXX,                            //播放XXX图案    XXX_F,                          //带呼吸播放XXX图案    UPDOWN,                         //播放上下图案    UPDOWN_F,                       //带呼吸播放上下图案                    }Led_e;typedef struct {    uint8_t Ia;                     //LED的全局最大电流控制    uint8_t Luma;                   //LED的全局亮度控制    Led_e Status;     uint8_t Num;                    //键对应LED}Led_t;extern uint8_t Led_Init(void);      //初始化extern Led_t *Led_Status(void);     //状态接口extern void Led_Play(Led_e m);      //可中断调用extern void Led_Process(void);      //TIME_BASIC_MS 调用extern void LED_switch(uint8_t num,uint8_t sta);  #define LED_FRONT_LOCK(x)           LED_switch(12,x)//前面板 左下角，锁上#define LED_FRONT_BAT(x)            LED_switch(13,x)//前面板 右下角，电量低符号#define LED_FRONT_OPEN(x)           LED_switch(14,x)//前面板 右上角 OPEN#define LED_FRONT_CLOSE(x)          LED_switch(15,x)//前面板 左上角 CLOSE#define LED_BACK_OPEN(x)            LED_switch(16,x)//后面板绿OPEN    #define LED_BACK_CLOSE(x)           LED_switch(17,x)//后面板红CLOSE#endif