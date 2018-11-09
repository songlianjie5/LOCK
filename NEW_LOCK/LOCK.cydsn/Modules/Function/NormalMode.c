#include "Function.h"

static uint8_t LockStatus_Proc();
typedef enum{
    E_PMPSW=1,
    E_USERPSW=2,
    E_400SN=3,
    E_MUTE_CHANG=4,
    E_ARM=5,                    //布防
    E_ERRO_INPUT=6
}TPout_E;
static bool TuchPadProcess(uint8_t *pswLen,uint8_t psw[],bool lock,TPout_E *out);
static void OnKeyDoor_Proc();



//报防伪码
extern bool ReadSnNuber(void);
//电量显示
static void DisplayBat(uint8_t batPer)
{
	uint8_t i,lev;                                                                  //lev 0-14;
	lev = (uint8_t)(((float)batPer/100.0)*14.0);
	Display_picture(100, 1, 100 + 20, 1 + 2,(char *)Bterry20x10);					//电池框
	for(i=0;i<lev;i++)
	{
		Display_picture(105+13-i, 1, 105+13-i+2, 1 + 2,(char *)Bterry1x10);         //电量
	}
}
//显示主页面
static void DisplayNormalMainPage(Rtc_t *Rtc,uint8_t bat,uint8_t pswlen)
{
    static bool flash = false;                                               
    char str[30];

    sprintf((char*)str, "%.4d-%.2d-%.2d", Rtc->year, Rtc->month, Rtc->day);
    Display_english_str(0, 0, str, false);
    sprintf((char*)str, "%.2d:%.2d:%.2d", Rtc->hour, Rtc->minute, Rtc->second);
    Display_english_str(0, 1, str, false);
    
    DisplayBat(bat);                                                            //显示电量                                                 

    if(Function_status()->ManageMode.Language==CHN)
        Display_chinese_str(0, 2, TIPS_QSRYHMMYJHJJS.TipsChinese, false);
    else if(Function_status()->ManageMode.Language==EN)
        Display_english_str(0, 2, TIPS_QSRYHMMYJHJJS.TipsEnglish, false);

    if(flash)
        Display_english_str(pswlen * 8, 3, "_", false);                         //图标闪烁
    else
        Display_english_str(pswlen * 8, 3, " ", false);
    flash = !flash; 
}
//=====================================================================================
//正常模式
//=====================================================================================
uint8_t NormalMode(void)
{
    static Rtc_t Rtc;
    static uint8_t bat;
    static uint8_t step = 0;
    static uint8_t Tcount = 0;
    
    uint8_t id;                                                                     //临时Id
    TPout_E TuchPadOut;
    static uint8_t pswnum = 0;                                                      //密码长度
    static uint8_t psw[USER_PSW_LEN];                                               //输入的密码
    static uint8_t ErroTime = 0;                                                    //统计错误次数
    static bool tipflag = false;                                                    //状态分步 
    static uint8_t msg=0;
    
    static bool pswCheckOk = false;                                               //用于安全模式下计数
    static bool cardCheckOk = false;
    static bool figCheckOk = false;
    
    //30位虚伪密码

    switch(step)
    {
    case 0:
        Display_clear_screen();                                                     //清屏
        memset(psw, 0, sizeof(psw));                                                //清空密码
        pswnum = 0;                                                                 //清空密码个数
        Tcount = (500 / TIME_BASIC_MS);
        step = 1;
        break;
    case 1:
        //=======================================================================
        if(++Tcount > (500 / TIME_BASIC_MS))
        {
            Tcount = 0;
            Rtc_Get(&Rtc);                                                      //或取时间
            bat = Bat_Numeric();                                                //电池电量检测
            DisplayNormalMainPage(&Rtc,bat,pswnum);                             //显示主页
        }
        //------------------------------------------------------------------------
        //键盘输入处理
        if(TuchPadProcess(&pswnum,psw,false,&TuchPadOut))
        {
            
            if(TuchPadOut==E_USERPSW)                                           //用户密码对比
            {
                if(ReadAndcheckUserPswByPSW(&id,psw))                           //查找用户密码 时按upsw查找ID
                {                                                                                
                    if(id==0x89)                                           //０９为胁迫密码
                    {
                       Voice_play(BAO_JING_YIN);                           //通过模块发送报警信号
                    }
                    else if(id>0x84)                                        //０５－０８为临时密码
                    {
                        DelUserPswRecord(id,false);                             //开一次就失效  删除
                    }
                    //tipflag = true;
                    //step = 2;                                                   //验证成功
                    //-----------------------------------------------------------
                    if(Function_status()->ManageMode.OpenDoorMode==DOOR_NORMAL) //是否在安全模式下的工作
                    {
                        WriteOpenDoorRecordById(C_PSW,id|0x80,(RtcRecord_t *)&Rtc,0);   //写开门记录
                        tipflag = true;
                        step = 2;                                               //直跳成功
                    }
                    else                                                        //在安全模式下工作
                    {
                        if(cardCheckOk || figCheckOk)                           //密码和卡片或指纹同时正确
                        {
                            WriteOpenDoorRecordById(C_PSW,id|0x80,(RtcRecord_t *)&Rtc,0);   //写开门记录
                            tipflag = true;
                            step = 2;                                           //跳成功
                        }
                        else 
                        {
                            pswCheckOk = true;//第一次已验证通过
                            Voice_play(QING_SHUA_KA_HUO_AN_ZHI_WEN);          //请刷卡或按指纹//请刷卡，或指纹
                            DisplayTips(&TIPS_QSKHAZW);
                            step = 6;                                           //跳到等待状态
                        }
                    }
                    //-----------------------------------------------------------
                }
                else
                {
                    pswCheckOk = false;                                       //清第一次成功标记 
                    tipflag = true;
                    step = 3;                                                   //验证失败
                }
            }
            else if(TuchPadOut==E_PMPSW)                                        //管理密码对比
            {
                if(ReadAndcheckUserPmpswByPSW(psw))
                {
                    memcpy(&(Function_status()->NormalMode.ManagePSW),psw,12);  //复制输入的管理密码
                    Function_status()->workmode = MANAGE_MODE;                  //进入管理模式
                    step = 0;                                                   //下次重初始化
                    Function_status()->NormalMode.FANG_SUO = false;             //解除反锁
                }
                else                                                            
                {
                    tipflag = true;
                    step = 3;                                                   //验证失败
                }
            }
            else if(TuchPadOut==E_MUTE_CHANG)                                   //长按键
            {
                if(Function_status()->ManageMode.Voice==MUTE_ON)                //静音模式
                {
                    Function_status()->ManageMode.Voice = MUTE_OFF;
                    msg = 13;
                    step = 5;
                }   
                else                                                            //语音模式
                {
                    Function_status()->ManageMode.Voice = MUTE_ON;
                    msg = 12;
                    step = 5;
                } 
            }
            else if(TuchPadOut==E_400SN)                                        //400+报号
            {
                step =4;                                                        //跳到报码状态
            }
            else if(TuchPadOut ==E_ARM)                                         //在上锁后4秒
            {
                msg = 7;                                                        //已启动布防
                step = 5;
                Function_status()->NormalMode.ALM = true;                       //布防后由室外解除  室内开锁就报警
            }
            else                                                                 //输入错误 
            {
                Voice_play(SHU_RU_CUO_WU);                                       //输入错误
                step = 0;                                                        //下次重初始化      
            }                

            memset(psw, 0, sizeof(psw));                                         //清空密码
            pswnum = 0;                                                          //清空密码个数
        }
        //------------------------------------------------------------------------ 
        //管理密码
        //------------------------------------------------------------------------ 
        //密码开门
        //------------------------------------------------------------------------
        //指纹开门
        if(Fingerprint_status()->NormalEven==GET_ING)
        { 
            Fingerprint_status()->NormalEven = F_NULL;                      //清标记  
            Voice_play(ZHI_WEN_YIN_DO);                                     //指纹音Do
        }
        if(Fingerprint_status()->NormalEven==SEARCH_OK)
        {
            Fingerprint_status()->NormalEven = F_NULL;                      //清标记  
            WriteOpenDoorRecordById(C_FIG,(Fingerprint_status()->PID)|0x80,(RtcRecord_t *)&Rtc,0); //写开门记录
            tipflag = true;
            step = 2;                                                       //验证成功
        }
        else if((Fingerprint_status()->NormalEven==SEARCH_FAIL) || (Fingerprint_status()->NormalEven==GET_FAIL))
        {
            Fingerprint_status()->NormalEven = F_NULL;                      //清标记  
            tipflag = true;
            step = 3;                                                       //验证失败
        }
        //------------------------------------------------------------------------
        //卡片开门
        uint8_t CID[11];
        if(Card_ReadId((uint8_t *)CID)==0)
        {
            Voice_play(ZHI_WEN_YIN_DO);                                     //指纹音Do                    
            Function_status()->TimeOutCount = 0;                            //重置休眠计数                      

            if(ReadAndcheckCardByUID(&id,CID)==true)                         //4字节ID
            {
               if(Function_status()->ManageMode.OpenDoorMode==DOOR_NORMAL) //是否在安全模式下的工作
                {
                    WriteOpenDoorRecordById(C_CARD,id|0x80,(RtcRecord_t *)&Rtc,CID); //写开门记录 
                    tipflag = true;
                    step = 2;                                                   //验证成功
                }
                else                                                        //安全模式下
                {
                    if(pswCheckOk || figCheckOk)                           //卡片与密码同时成功 或卡片指纹同时成功
                    {
                        WriteOpenDoorRecordById(C_CARD,id|0x80,(RtcRecord_t *)&Rtc,CID); //写开门记录 
                        tipflag = true;
                        step = 2;                                                   //验证成功
                    }
                    else
                    {
                        cardCheckOk = true;                                 //第一次标记成功
                        Voice_play(QING_SHU_RU_MI_MA_HUO_ZHI_WEN);//请输入密码或指纹
                        DisplayTips(&TIPS_QSRMMHZW);//请输入密码或指纹
                        step = 6;                                           //跳到等待状态
                    }
                }
            }
            else 
            {
                tipflag = true;
                step = 3;                                                   //验证失败
            }   
        }
        //=======================================================================
        break;
    case 2:                                                                 //验证成功
        if(tipflag)
        {
            Function_status()->TimeOutCount = 0;                            //重置休眠计数
            if(Voice_Status()==VOICE_ST_IDLE) 
            {
                if(Function_status()->NormalMode.FANG_SUO)                          //在反锁模式下提示反锁
                {
                    msg = 3;//提示反锁
                    step = 5;
                }
                else 
                {
                    tipflag = false;
                    if(Function_status()->ManageMode.Voice==MUTE_OFF)               //静音模式下不放声音
                        Voice_play(YAN_ZHENG_CHENG_GONG_QING_KAI_MEN);                  //请开门
                    DisplayTips(&TIPS_QKM);
                  //  Lock_ProcessVerifySuceed();                                     //开锁
                    ErroTime = 0;                                                   //清错误次数
                    Function_status()->NormalMode.BAT_ALM = true;                   //开门成功只报一次电量
                    
                    pswCheckOk = cardCheckOk = figCheckOk = false;                  //清安全模式的统计
                    //-----------------------------
                    if(1)
                        Function_status()->NormalMode.ALM = false;                  //解除布防模式
                    else
                    {
                        msg = 8;                                                    //室内开门 报警10秒
                        step = 5;
                    }
                    //-----------------------------
                }
            }
        }
        else 
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(Function_status()->ManageMode.Voice==MUTE_OFF)               //静音模式下不放声音
                    Voice_play(HUAN_YING_HUI_JIA);                                  //欢迎回家
                step = 0;                                                        //下次重初始化
            }    
        }
        if(Fingerprint_status()->NormalEven==SEARCH_OK)
        {
            Voice_play(ZHI_WEN_YIN_DO);                                     //指纹音Do
            Fingerprint_status()->NormalEven = F_NULL;
        }
        break;
    case 3:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            if(Function_status()->NormalMode.FANG_SUO)                          //在反锁模式下提示反锁
            {
                msg = 3;//提示反锁
                step = 5;
            }
            else 
            {
                if(tipflag)
                {
                    Function_status()->TimeOutCount = 0;                            //重置休眠计数
                    tipflag = false;
                    if(Function_status()->ManageMode.Voice==MUTE_OFF)               //静音模式下不放声音
                        Voice_play(YAN_ZHENG_SHI_BAI);
                    DisplayTips(&TIPS_YZSB);                                            //验证失败
                    Led_Play(XXX);
                    ErroTime++;                                                     //错误次数累计
                    
                    pswCheckOk = cardCheckOk = figCheckOk = false;                  //清安全模式的统计
                }
                else 
                {
                    if(Function_status()->ManageMode.Voice==MUTE_OFF)               //静音模式下不放声音
                        Voice_play(QING_CHONG_XIN_SHU_RU);                              //请重新输入
                    step = 0;                                                        //下次重初始化
                }  
            }
        }
        if(Fingerprint_status()->NormalEven==GET_ING)
        {
            Voice_play(ZHI_WEN_YIN_DO);                                     //指纹音Do
            Fingerprint_status()->NormalEven = F_NULL;
        }
        break;
    case 4:                                                                     //报400状态
        if(ReadSnNuber())
           step = 0;
        else 
           Function_status()->TimeOutCount = 0;                            //重置休眠计数
        break;
    case 5:                                                                     //提示状态
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            switch(msg)
            {
                case 1: Voice_play(YI_GUAN_MEN);                                               //已关门
                        DisplayTips(&TIPS_YGM);
                        break;
                case 2: Voice_play(YI_KAI_MEN);                                                //已开门
                        break;
                case 3:Voice_play(YI_FAN_SUO);                                                //已反锁  长按Close键
                        DisplayTips(&TIPS_YFS);  
                        break;
                case 4:Voice_play(MEN_WEI_SHANG_SUO);                                              
                        DisplayTips(&TIPS_MWSS);                                                //门未上锁    
                        break;
                case 5:DisplayTips(&TIPS_MSYC);                                                //门锁异常
                        //报警声
                        break;
                case 6: DisplayTips(&TIPS_FQBJ);                                                //防撬报警 20秒
                        break; 
                case 7:Voice_play(YI_QI_DONG_BU_FANG);                                         //已启动布防
                        DisplayTips(&TIPS_YQDBF); 
                        break;
                case 8:DisplayTips(&TIPS_BFBJ);                                                //布防报警 
                        Voice_play(BAO_JING_YIN);                                               //报警音      
                        break;
                case 9: DisplayTips(&TIPS_SDMS);                                                //手动模式
                        Voice_play(SHOU_DONG_MO_SHI);    
                        break;
                case 10:DisplayTips(&TIPS_ZDMS);                                                //自动模式
                        Voice_play(ZI_DONG_MO_SHI);    
                        break;
                case 11:
                        DisplayTips(&TIPS_XTYSD);
                        DisplayTipsEx(&TIPS_QSHZS,2,false);
                        Voice_play(XI_TONG_YI_SUO_DING_QING_SHAO_HOU_ZAI_SHI);                //系统已锁定，请稍候再试 
                        break; 
                case 12:
                        DisplayTips(&TIPS_JYMS);
                        Voice_play(JING_YIN_MO_SHI);                                            //静音模式
                        break;
                case 13:
                        DisplayTips(&TIPS_YYMS);                                                //语音模式
                        Voice_play(YU_YIN_MO_SHI);
                        break;
                case 14:
                        Voice_play(DIAN_LIANG_DI_QING_GENG_HUAN_DIAN_CHI);                  //电量低请更换电池 1次
                        DisplayTipsEx(&TIPS_DLDQGHDC,2,false); 
                        break;
                default:step = 0;break;
            }
            msg = 0;
        }
        break;
    case 6:                                                                                 //等待状态
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            step = 0;
        }
        break;
    default:
        break;
    }
    //------------------------------------------------------------------------------
    //if(手动模式)
    //if(布防模式) 室外任一种方式成功开锁后解除　室内开门报警
    //if(已反锁) 室外除管理密码和钥匙外其它都不能开并提示已反锁　室内开门解除
    //if(安全模式) 常用模式下每单项连续１０次，其它项开门或断电解除，安全模式下所有都不能解除只能断电
    
    //在手动模式下 门报一次 门状态
    
    //------------------------------------------------------------------------------
#if 1    
    if(Function_status()->NormalMode.BAT_ALM)
    {
        Function_status()->NormalMode.BAT_ALM = false;                           //只报一次电量
        if(bat<10)                                                                   //电量小于n%报警
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                Voice_play(DIAN_LIANG_DI_QING_GENG_HUAN_DIAN_CHI);                  //电量低请更换电池
                DisplayTipsEx(&TIPS_DLDQGHDC,2,false);
            }
        }
    }
#endif    
    //------------------------------------------------------------------------------
    //统计连续验证失败的10次数 用于改变模式 锁定5分钟 在常用模式下只锁键盘 安全模式下锁键盘指纹与卡片
#if 1
    if(ErroTime >= MAX_ERRO_TIME)                                              //失败次数统计    
    {
        ErroTime = 0;
        Function_status()->NormalMode.SYSTEM_LOCK  = true;                     //系统已锁定
        msg = 11;//系统已锁定，请稍候再试
        step = 5;
    }
#endif    
    //------------------------------------------------------------------------------
    Fingerprint_NormalProcess();//必须放在 TIME_BASIC_MS里
    
//    LockStatus_Proc();          //锁体
//    if (Lock_Status()->locked.status == LOCK_MSG_LOCKED_OK)
//    {
//        Lock_Status()->locked.status = 0;
//        LED_BACK_CLOSE(ON);
//        LED_BACK_OPEN(OFF);
//        LED_FRONT_CLOSE(ON);
//        LED_FRONT_OPEN(OFF);
//        msg = 1;
//        step = 5;                   //报已关门
//        //Voice_play(YI_GUAN_MEN);
//        Function_status()->TimeOutCount = 0;                            //重置休眠计数
//    }                                                                                    
 
    
    OnKeyDoor_Proc();            //一键开关门键处理
    
    if(SwitchKey_Status()->DoorOneKey== (LOCK_BNT_KEY|LONG_CLICK)) //长按反锁
    {
        SwitchKey_Status()->DoorOneKey = 0;
        Function_status()->TimeOutCount = 0;
        msg = 3;        //长按反锁
        step = 5;
        Function_status()->NormalMode.FANG_SUO = true;                          //已反锁
        
        Function_status()->TimeOutCount = 0;                            //重置休眠计数
    }  
    
    if(SwitchKey_Status()->ModeSwAct)//自动手动开关
    {
        SwitchKey_Status()->ModeSwAct = false;
        Function_status()->TimeOutCount = 0;                            //重置休眠计数
        if(SwitchKey_Status()->ModeSwStatus == AUTO_MODE)                       //手自动按键
        {
            //Lock_SetMode(LOCK_MODE_AUTO);
            Function_status()->NormalMode.AUTOMODE = true;              
            msg = 10;
            step = 5;
        }
        else if (SwitchKey_Status()->ModeSwStatus == MANUAL_MODE)
        {
            //Lock_SetMode(LOCK_MODE_MANUAL);]
            Function_status()->NormalMode.AUTOMODE = false;
            msg = 9;
            step = 5;
        }                                                            
    } 
    //------------------------------------------------------------------------------
    //超时处理
#if 0   
    if(++Function_status()->TimeOutCount>(DEEPSLEEP_TIME/TIME_BASIC_MS))
    {
        Function_status()->TimeOutCount = 0;                                //重置休眠计数
        Function_status()->McuState = DEEP_SLEEP;                           //跳到DEEP睡眠
        step = 0;                                                           //下次重初始化
    }
#endif
    //------------------------------------------------------------------------------   
    return 0;
}
//输入：Tuch_status()->even Tuch_status()->val 门锁状态
//输出: 1、用户密码与长度 2、管理密码与长度 3、报号400+ 4、点按布防 5、长按静音 
bool TuchPadProcess(uint8_t *pswLen,uint8_t psw[],bool lock,TPout_E *out)
{
    bool rev = false;
    static uint8_t esc=0;                                                   //统一*键的次数
    if(Tuch_status()->even)
    {
        Function_status()->TimeOutCount = 0;                                //重置休眠计数
        
        if(Tuch_status()->val <TUCH_KEY_ENTER_UP)                           //屏蔽长按，上升键升
        {
            if(Function_status()->ManageMode.Voice==MUTE_OFF)               //静音模式下不放声音
                Voice_play(AN_JIAN_YIN_DI);                                 //播放按键声
            if(Led_Status()->Status==IDEL)
            {
                Led_Status()->Num = Tuch_status()->val;                     //播放键盘LED
                Led_Play(TUCH);                                             //播放LED
            }
        }

        if((Tuch_status()->val <= TUCH_KEY9) && ((*pswLen) < 15))            //数字输入
        {
            psw[*pswLen] = Tuch_status()->val +0x80;                         //录入输入密码+0x80前缀
            Display_english_str((*pswLen) * 8, 3, "*", false);               //显示*号
            (*pswLen)++;
            if(esc<2)esc  = 0;                                               //清*键计数
        }
        else if(Tuch_status()->val == TUCH_KEY_ESC)                          //*取消键
        {
            esc += 1;                                                        //*键计数
            memset(psw, 0, USER_PSW_LEN);                                    //清空密码
            *pswLen = 0;                                                     //清空密码个数
            Display_english_str(0, 3, "                ", false);            //清空密码显示
        }
        else if(Tuch_status()->val == TUCH_KEY_ENTER_UP)                     //#确认键 这里是上升才确认
        {
            uint8_t SNkey[]={0x84,0x80,0x80};                                //400 
            if(esc >= 2)                                                     //是否为管理密码
                *out = E_PMPSW;                                               //输出管理密码
            else if((*pswLen)==3)                                            //对比长度 是否为400+#  
            {
                if(0 == strncmp((const char *)psw,(const char *)SNkey, sizeof(SNkey)) ) 
                    *out = E_400SN;                                           //输出400+ 
                else
                    *out = E_ERRO_INPUT;                                      //输出错误
            }
            else
            {
                *out = E_USERPSW;                                            //输出用户密码
            }
            rev = true;
            esc  = 0;                                                       //清*键计数
            Display_english_str(0, 3, "                ", false);           //清空密码显示
        }
        else if(Tuch_status()->val == (TUCH_KEY_ENTER|LONG_CLICK))          //#长按
        {
            rev  = true;
            *out  = E_MUTE_CHANG;                                           //输出长按
        }
        if(lock && (Tuch_status()->val==TUCH_KEY0))                         //在上锁状态下按0启动布防
        {
            rev = true;
            *out = E_ARM;                                                   //输出布防
        }
        Tuch_status()->even = 0;                                            //清按键事件
    }
    return rev;
}
static void OnKeyDoor_Proc()
{
    if (SwitchKey_Status()->DoorOneKey==LOCK_BNT_KEY)//一键开关门单击
    {
        SwitchKey_Status()->DoorOneKey = 0; 
        Function_status()->TimeOutCount = 0;
      //  Lock_ProcessCloseKey(1);
         LockClose(); //关门
    }
    else if (SwitchKey_Status()->DoorOneKey == (LOCK_BNT_KEY|DOUBLE_CLICK))//双击
    {  
        SwitchKey_Status()->DoorOneKey = 0;
        Function_status()->TimeOutCount = 0;
       // Lock_ProcessCloseKey(2);
         LockOpen(); //开门
    }
//    else if(SwitchKey_Status()->DoorOneKey== (LOCK_BNT_KEY|LONG_CLICK)) //长按反锁
//    {
//        SwitchKey_Status()->DoorOneKey = 0;
//        Function_status()->TimeOutCount = 0;
//        Voice_play(YI_FAN_SUO);                                                //已反锁  长按Close键
//        DisplayTips(&TIPS_YFS);   
//    }
}
//=====================================================================================
//自动模式
//=====================================================================================
//static uint8_t LockStatus_Proc()
//{
//	static uint8_t stat=0;
////    if (Lock_Status()->locked.status == LOCK_MSG_LOCKED_OK)
////    {
////        Lock_Status()->locked.status = 0;
////        LED_BACK_CLOSE(ON);
////        LED_BACK_OPEN(OFF);
////        LED_FRONT_CLOSE(ON);
////        LED_FRONT_OPEN(OFF);
////        //Voice_play(YI_GUAN_MEN);
////    }                                                                                    
//    
////    if (stat == 0)
////    {
//        if (Lock_Status()->unlock.status == LOCK_MSG_UNLOCK_OK)   
//        {
//            Lock_Status()->unlock.status = 0;
//            LED_BACK_CLOSE(OFF);
//            LED_BACK_OPEN(ON);
//            LED_FRONT_CLOSE(OFF);
//            LED_FRONT_OPEN(ON); 
//            stat = 1;
//            Lock_Status()->locking.status = 0;
//            
//            Function_status()->NormalMode.FANG_SUO = false;//解除反锁
//        }
////    }
////    else if (stat == 1)
////    {
////        if (Lock_Status()->unlock.param == LOCK_PARAM_KEYS)
////        {
////            if (Voice_Status()==VOICE_ST_IDLE)
////            {
////                //Voice_play(HUAN_YING_HUI_JIA); //欢迎回家
////                stat = 0;
////            }
////        } 
////        else
////        {
////            stat = 0;
////        }
////    }
//    
//    if (Lock_Status()->locking.status == LOCK_MSG_NOT_LOCKED)
//    {
//        Lock_Status()->locking.status = 0;
//        Voice_play(MEN_WEI_SHANG_SUO);  //门没上锁
//    }
//    
//    if (Lock_Status()->locking.status == LOCK_MSG_TAMPER)
//    {
//        if (Voice_Status()==VOICE_ST_IDLE)
//        {
//            Voice_play(BAO_JING_YIN); //报警音
//        } 
//        Function_status()->TimeOutCount = 0;                            //重置休眠计数
//    }
//    
//    return 0;
//}



