#include "Function.h"

typedef struct
{
    int8_t slect_index;
    bool autoRead;
    uint16_t Tcount;
    uint8_t step;
    uint16_t time_out;
    void (*function)(void);
} MenuNode_t;

//一级主菜单
void MainMenu(void);
//二级菜单
void UserSetMenu(void);
void SystemSetMenu(void);
void RecordMenu(void);
void ExternMenu(void);
//三级菜单 用户设置
void AddUserPswMenu(void);
void AddUserFigMenu(void);
void AddUserCarMenu(void);
void DelUserPswMenu(void);
void DelUserFigMenu(void);
void DelUserCarMenu(void);
void SetOpenModeMenu(void);
void AddManPswMenu(void);
//三级菜单 系统设置
void TimeSetMenu(void);
void DateSetMenu(void);
void VoiceSetMenu(void);
void LanguageSetMenu(void);
//三级菜单 记录查询
void SysRecordMenu(void);
void OrderRecordMenu(void);
void NumRecordMenu(void);
//三级菜单 扩展功能
void JinNetMenu(void);
void QuitNetMenu(void);

volatile MenuNode_t UI = {0, true, 0, 0, 0, MainMenu}; //一级主菜单

const MenuNode_t Menu_tab[] = {
    {0, true, 0, 0, 0, MainMenu},                  //一级主菜单

    {0, true, 0, 0, 0, UserSetMenu},               //二级菜单
    {0, true, 0, 0, 0, SystemSetMenu},             //二级菜单
    {0, true, 0, 0, 0, RecordMenu},                //二级菜单
    {0, true, 0, 0, 0, ExternMenu},                //二级菜单
};
const MenuNode_t UserSetMenu_tab[] =               //三级菜单
{
    {0, true, 0, 0, 0, AddUserPswMenu},
    {0, true, 0, 0, 0, AddUserFigMenu},
    {0, true, 0, 0, 0, AddUserCarMenu},
    {0, true, 0, 0, 0, DelUserPswMenu},
    {0, true, 0, 0, 0, DelUserFigMenu},
    {0, true, 0, 0, 0, DelUserCarMenu},
    {0, true, 0, 0, 0, SetOpenModeMenu},
    {0, true, 0, 0, 0, AddManPswMenu},
};
const MenuNode_t SystemSetMenu_tab[] =            //三级菜单
{
    {0, true, 0, 0, 0, TimeSetMenu},
    {0, true, 0, 0, 0, DateSetMenu},
    {0, true, 0, 0, 0, VoiceSetMenu},
    {0, true, 0, 0, 0, LanguageSetMenu}
};
const MenuNode_t RecordMenu_tab[] =              //三级菜单
{
    {0, true, 0, 0, 0, SysRecordMenu},
    {0, true, 0, 0, 0, OrderRecordMenu},
    {0, true, 0, 0, 0, NumRecordMenu}
};
const MenuNode_t ExternMenu_tab[] =              //三级菜单
{
    {0, true, 0, 0, 0, JinNetMenu},
    {0, true, 0, 0, 0, QuitNetMenu}
};
//=====================================================================================
//管理模式
//=====================================================================================
uint8_t ManageMode(void)
{  
    if(Tuch_status()->even == true)
        if(Tuch_status()->val <TUCH_KEY_ENTER_UP)                                       //屏蔽长按，上升键升
            Voice_play(AN_JIAN_YIN_DI);                                                 //按键声 

    if(UI.function != NULL)UI.function();                                               //菜单页面

    if(Tuch_status()->even == true)                                                     //有按键清超时
    {
        UI.time_out = 0;                                                                //重置管理模式下超时退出
        Function_status()->TimeOutCount = 0;                                            //重置休眠计数
        
        Tuch_status()->even = false;                                                    //清按键事件
        
        if(Led_Status()->Status==IDEL)                                                  //看情况是否要中断LED
        {
            Led_Status()->Num = Tuch_status()->val;
            Led_Play(TUCH);                                                             //播放LED
        }
    }
#if 1    
    if(UI.autoRead == false)                                                            //在自动播状态下计时
    {
        if(++UI.time_out > (10000 / TIME_BASIC_MS))                                     //超时退出
        {
            UI.time_out = 0;
            Function_status()->workmode = NORMAL_MODE;
        }
    }
#endif
    if(SwitchKey_Status()->ConverStatus==COVER_CLOSE)
    {
        Function_status()->workmode = NORMAL_MODE;                                      //先退出管理模式
    }

    if(Function_status()->workmode != MANAGE_MODE)                                      //对比模式是否改变
    {   
        DisplayTips(&TIPS_YTCGLMS);                                                     //显示已退出管理模式
        memcpy((void *)&UI, &Menu_tab[0], sizeof(MenuNode_t));                          //重初始化一次，以便下次进入
        do{
            Wdt_Feed_Dog();                                                             //清看门狗
            Led_Process();                                                              //LED模块状态更新
            do
            {
               CySysPmSleep();		                                                    
            }while(Function_status()->Tick == false);                                   
            Function_status()->Tick = false;  
        }while(Voice_Status()!=VOICE_ST_IDLE);
        Voice_play(YI_TUI_CHU_GUAN_LI_MO_SHI);                                          //已退出管理模
        //这句语音是不能被打断的
        //------------------------------------------------------------------------------
        do{
            Wdt_Feed_Dog();                                                             //清看门狗
            Led_Process();                                                              //LED模块状态更新
            do
            {
               CySysPmSleep();		                                                    
            }while(Function_status()->Tick == false);                                   
            Function_status()->Tick = false;  
        }while(Voice_Status()!=VOICE_ST_IDLE);
        //------------------------------------------------------------------------------
    }
    return 0;
}
//=====================================================================================
//显示条目、选择条目
//=====================================================================================
void DisplayItem(uint8_t index, const item_t *Item, uint8_t ItemNum)                    
{
    uint8_t i;
    Display_clear_screen();
    if(Function_status()->ManageMode.Language==CHN)
    {
        for(i = 0; i < ItemNum; i++)
        {
            Display_english_str(0, i, Item[i].Num, false);
            if(index == i)
                Display_chinese_str(16, i, Item[i].MenuChinese, true);
            else
                Display_chinese_str(16, i, Item[i].MenuChinese, false);
        }
    }
    else if(Function_status()->ManageMode.Language==EN)
    {
        for(i = 0; i < ItemNum; i++)
        {
            Display_english_str(0, i, Item[i].Num, false);
            if(index == i)
                Display_english_str(16, i, Item[i].MenuEnglish, true);
            else
                Display_english_str(16, i, Item[i].MenuEnglish, false);
        }        
    }else {;}
}
void DisplayItemSelect(bool n, uint8_t index, const item_t *Item)
{
    if(Function_status()->ManageMode.Language==CHN)
    {
        if(n == true)
            Display_chinese_str(16, index, Item[index].MenuChinese, true);
        else
            Display_chinese_str(16, index, Item[index].MenuChinese, false);
    }
    else if(Function_status()->ManageMode.Language==EN)
    {
        if(n == true)
            Display_english_str(16, index, Item[index].MenuEnglish, true);
        else
            Display_english_str(16, index, Item[index].MenuEnglish, false);
    }else {;}    
}
//=====================================================================================
//一级菜单
//=====================================================================================
void MainMenu(void)
{
    static uint8_t playStep = 0;
    switch(UI.step)
    {
    case 0:
        if(UI.Tcount == 0)                                                             //密码过于简单
        {
            UI.slect_index = 0;
            if(Pwd_Analyse(Function_status()->NormalMode.ManagePSW))                   //判断正常模式下输入管理密码
             {
                DisplayTips(&TIPS_MMGYJD);Voice_play(MI_MA_GUO_YU_JIAN_DAN);
                UI.Tcount = 1;
             }
        }
        if(Voice_Status()==VOICE_ST_IDLE)                                              //等待语音播完
        {
            UI.Tcount = 0;
            UI.step = 1;
        }
        break;
    case 1: 
        if(UI.Tcount == 0)
        {
            DisplayTips(&TIPS_YJRGLMS);Voice_play(YI_JIN_RU_GUAN_LI_MO_SHI);           //已进入管理模
            UI.Tcount = 1;
        } 
        if(Voice_Status()==VOICE_ST_IDLE)                                              //等待语音播完
        {
            UI.Tcount = 0;
            UI.step = 2;                                                               //跳到下一步
        }
        break;
    case 2:
        DisplayItem(0, main_menu, 4);                                                  //显示菜单
        UI.step = 3;
        Voice_play(main_menu[0].SpeakAddr);                                            //第一次播放 用户设置请按
        playStep = 0;
        UI.slect_index = 0;
        break;
    case 3:
        if(UI.autoRead==1)                                                             //自动播报
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)                                                        //播放数字状态
                {
                    Voice_play(YU_YIN_1+UI.slect_index);                               //播放数字
                    if(UI.slect_index ==3)                                             //0-3菜单条目数
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)                                                   //播放菜单状态
                {
                    Voice_play(main_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, main_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, main_menu);
                }
                else if(playStep==2)                                                   //播放结尾状态
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);        //返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;                                                   //停止自动播放
                }
            }
        }     
        break;
    default:
        break;
    }

    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
        case TUCH_KEY4:
            memcpy((char *)&UI, &Menu_tab[Tuch_status()->val], sizeof(MenuNode_t));     //按键1-4 跳到相应的菜单
            break;
        case TUCH_KEY8:                                                                 //上翻键 
            UI.autoRead = false;                                                        //停止自动播报
            DisplayItemSelect(false, UI.slect_index, main_menu);
            if(--UI.slect_index < 0)UI.slect_index = 3;
            DisplayItemSelect(true, UI.slect_index, main_menu);
            break;
        case TUCH_KEY0:                                                                 //下翻键
            UI.autoRead = false;                                                        //停止自动播报
            DisplayItemSelect(false, UI.slect_index, main_menu);
            if(++UI.slect_index > 3)UI.slect_index = 0;
            DisplayItemSelect(true, UI.slect_index, main_menu);
            break;
        case TUCH_KEY_ESC:                                                              //*键返回上一级菜单
            Function_status()->workmode = NORMAL_MODE;                                  //退出管理模式
            break;
        case TUCH_KEY_ENTER:                                                            //#确认键进入已选菜单 
            memcpy((char *)&UI, &Menu_tab[UI.slect_index + 1], sizeof(MenuNode_t));
            break;
        default:
            break;
        }
    }
}
//=====================================================================================
//二级菜单
//=====================================================================================
void UserSetMenu(void)
{
    const uint8_t indexTab[]={1,2,3,4,5,6,7,9};                                         //为了跳过数字8
    static uint8_t startIndex=0;                                                        //起点
    static uint8_t playStep = 0;
    switch(UI.step)
    {
    case 0:
        startIndex = 0;
        DisplayItem(0, &user_menu[startIndex], 4);
        UI.step = 1;
        Voice_play(user_menu[0].SpeakAddr);                                             //添加用户密码请按
        playStep = 0;
        break;
    case 1:
       if(UI.autoRead==1)                                                               //自动播报
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)                                                         //播放数字状态
                {
                    if(UI.slect_index ==7)                                              //0-7菜单条目数
                        {playStep = 2; Voice_play(YU_YIN_1+UI.slect_index+1);}          //8播放数字9代替
                    else 
                        {playStep = 1; Voice_play(YU_YIN_1+UI.slect_index);}            //播放数字
                }
                else if(playStep==1)                                                    //播放菜单状态
                {
                    Voice_play(user_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
    
                    if(UI.slect_index<3)
                    {
                        DisplayItemSelect(false, UI.slect_index, &user_menu[startIndex]);
                        UI.slect_index++;
                        DisplayItemSelect(true, UI.slect_index, &user_menu[startIndex]);
                    }
                    else if((UI.slect_index>=3) && (UI.slect_index<7))
                    {
                        startIndex = UI.slect_index-2;
                        DisplayItem(3, &user_menu[startIndex], 4);
                        UI.slect_index++;
                    }  
                }
                else if(playStep==2)                                                     //播放结尾状态                                         
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);          //返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;                                                     //停止自动播报
                }
            }
        }   
        break;
    default:
        break;
    }

    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
        case TUCH_KEY4:
        case TUCH_KEY5:
        case TUCH_KEY6:
        case TUCH_KEY7:
            memcpy((char *)&UI, &UserSetMenu_tab[Tuch_status()->val - 1], sizeof(MenuNode_t)); //按键1-7 跳到相应的菜单
            break;
        case TUCH_KEY9:
            memcpy((char *)&UI, &UserSetMenu_tab[Tuch_status()->val - 2], sizeof(MenuNode_t)); //按键9 跳到第8条菜单
            break;
        case TUCH_KEY8:                                                                  //上翻键 
            UI.autoRead = 0;                                                             //停止自动播报     
            //startIndex 4
            if(UI.slect_index > startIndex)
            {
                DisplayItemSelect(false, UI.slect_index-startIndex, &user_menu[startIndex]);
                if(--UI.slect_index < 0)UI.slect_index = 7;
                DisplayItemSelect(true, UI.slect_index-startIndex, &user_menu[startIndex]);    
            }
            else 
            {
                UI.slect_index--;
                if(UI.slect_index >= 0)
                {
                    startIndex = UI.slect_index;
                    DisplayItem(0, &user_menu[startIndex], 4);
                }
                else 
                {
                    UI.slect_index = 7; 
                    startIndex = 4;
                    DisplayItem(3, &user_menu[startIndex], 4);
                }
            }
            break;
        case TUCH_KEY0:                                                                  //下翻键
            UI.autoRead = 0;                                                             //停止自动播报 
            //startIndex 4
            if(UI.slect_index < (startIndex+3))
            {
                DisplayItemSelect(false, UI.slect_index-startIndex, &user_menu[startIndex]);
                if(++UI.slect_index > 7)UI.slect_index = 0;
                DisplayItemSelect(true, UI.slect_index-startIndex, &user_menu[startIndex]);    
            }
            else
            {
                if(++UI.slect_index > 7)
                {
                    UI.slect_index = 0;
                    startIndex = 0;
                    DisplayItem(0, &user_menu[startIndex], 4); 
                }
                else
                {
                    startIndex = UI.slect_index-3;
                    DisplayItem(3, &user_menu[startIndex], 4);                    
                }
            }  
            break;
        case TUCH_KEY_ESC:                                                               //*键返回上一级菜单
            memcpy((void *)&UI, &Menu_tab[0], sizeof(MenuNode_t));
            UI.step = 2;                                                                 //返回主菜单不重播状态
            break;
        case TUCH_KEY_ENTER:
            if(UI.slect_index < 8)
               memcpy((char *)&UI, &UserSetMenu_tab[UI.slect_index], sizeof(MenuNode_t));//#确认键进入已选菜单 
            break;
        default:
            break;
        }
    }
}
//=====================================================================================
//二级菜单
//=====================================================================================
void SystemSetMenu(void)
{
    static uint8_t playStep = 0;
    switch(UI.step)
    {
    case 0:
        DisplayItem(0, sys_menu, 4);
        UI.step = 1;
        Voice_play(sys_menu[UI.slect_index].SpeakAddr);                                 //时间设置请按
        playStep = 0;
        break;
    case 1:
        if(UI.autoRead==1)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)                                                         //播放数字状态         
                {
                    Voice_play(YU_YIN_1+UI.slect_index);
                    if(UI.slect_index ==3)                                              //0-3菜单条目数
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)                                                    //播放菜单状态
                {
                    Voice_play(sys_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, sys_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, sys_menu);
                }
                else if(playStep==2)                                                    //播放结尾状态    
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);         //返回上级菜单请按*号键
                    playStep = 0;   
                    UI.autoRead = 0;                                                    //停止自动播报
                }
            }
        }
        break;
    default:
        break;
    }

    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
        case TUCH_KEY4:
            memcpy((char *)&UI, &SystemSetMenu_tab[Tuch_status()->val - 1], sizeof(MenuNode_t)); //按键1-4 跳到相应的菜单
            break;
        case TUCH_KEY8:                                                                 //上翻
            UI.autoRead = 0;                                                            //停止自动播报
            DisplayItemSelect(false, UI.slect_index, sys_menu);
            if(--UI.slect_index < 0)UI.slect_index = 3;
            DisplayItemSelect(true, UI.slect_index, sys_menu);
            break;
        case TUCH_KEY0:                                                                 //下翻
            UI.autoRead = 0;                                                            //停止自动播报
            DisplayItemSelect(false, UI.slect_index, sys_menu);
            if(++UI.slect_index > 3)UI.slect_index = 0;
            DisplayItemSelect(true, UI.slect_index, sys_menu);
            break;
        case TUCH_KEY_ESC:                                                              //*键返回上一级菜单
            memcpy((void *)&UI, &Menu_tab[0], sizeof(MenuNode_t));                      //返回主菜单
            UI.step = 2;                                                                //返回主菜单不重播状态
            break;
        case TUCH_KEY_ENTER:
            if(UI.slect_index < 4)                                                      //#确认键进入已选菜单    
                memcpy((char *)&UI, &SystemSetMenu_tab[UI.slect_index], sizeof(MenuNode_t)); 
            break;
        default:
            break;
        }
    }
}
//=====================================================================================
//二级菜单
//=====================================================================================
void RecordMenu(void)
{
    static uint8_t playStep = 0;
    switch(UI.step)
    {
    case 0:
        DisplayItem(0, record_menu, 3);
        UI.step = 1;
        Voice_play(record_menu[UI.slect_index].SpeakAddr); 
        playStep = 0;
        break;
    case 1:
        if(UI.autoRead)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)
                {
                    Voice_play(YU_YIN_1+UI.slect_index);//1
                    if(UI.slect_index ==2)
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)
                {
                    Voice_play(record_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, record_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, record_menu);
                }
                else if(playStep==2)
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);//返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;
                }
            }        
        }
        break;
    default:
        break;
    }

    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
            memcpy((char *)&UI, &RecordMenu_tab[Tuch_status()->val - 1], sizeof(MenuNode_t)); //按键1-3
            break;
        case TUCH_KEY8:                                                                //上翻
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, record_menu);
            if(--UI.slect_index < 0)UI.slect_index = 2;
            DisplayItemSelect(true, UI.slect_index, record_menu);
            break;
        case TUCH_KEY0:                                                                 //下翻
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, record_menu);
            if(++UI.slect_index > 2)UI.slect_index = 0;
            DisplayItemSelect(true, UI.slect_index, record_menu);
            break;
        case TUCH_KEY_ESC:                                                              //*键返回上一级菜单
            memcpy((void *)&UI, &Menu_tab[0], sizeof(MenuNode_t));
            UI.step = 2;                                                                //返回主菜单不重播状态
            break;
        case TUCH_KEY_ENTER:
            if(UI.slect_index < 3)                                                      //#确认键进入已选菜单
                memcpy((char *)&UI, &RecordMenu_tab[UI.slect_index], sizeof(MenuNode_t)); //按键1-4
            break;
        default:
            break;
        }
    }
}
//=====================================================================================
//二级菜单
//=====================================================================================
void ExternMenu(void)
{
    static uint8_t playStep = 0;
    switch(UI.step)
    {
    case 0:
        DisplayItem(0, ext_menu, 2);
        UI.step = 1;
        Voice_play(ext_menu[UI.slect_index].SpeakAddr); 
        playStep = 0;
        break;
    case 1:
        if(UI.autoRead)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)
                {
                    Voice_play(YU_YIN_1+UI.slect_index);//1
                    if(UI.slect_index ==1)
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)
                {
                    Voice_play(ext_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, ext_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, ext_menu);
                }
                else if(playStep==2)
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);//返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;
                }
            }        
        }    
        break;
    default:
        break;
    }

    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
        case TUCH_KEY2:
            memcpy((char *)&UI, &ExternMenu_tab[Tuch_status()->val - 1], sizeof(MenuNode_t)); //按键1-2
            break;
        case TUCH_KEY8:
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, ext_menu);
            if(--UI.slect_index < 0)UI.slect_index = 1;
            DisplayItemSelect(true, UI.slect_index, ext_menu);
            break;//上翻
        case TUCH_KEY0:
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, ext_menu);
            if(++UI.slect_index > 1)UI.slect_index = 0;
            DisplayItemSelect(true, UI.slect_index, ext_menu);
            break;//下翻
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[0], sizeof(MenuNode_t));
            UI.step = 2;                                                                //返回主菜单不重播状态
            break;
        case TUCH_KEY_ENTER:
            if(UI.slect_index < 2)
                memcpy((char *)&UI, &ExternMenu_tab[UI.slect_index], sizeof(MenuNode_t)); //按键1-2
            break;
        default:
            break;
        }
    }
}
//=====================================================================================
//三级菜单
//=====================================================================================
void AddUserPswMenu(void)
{
    static uint8_t BianHao[2];                                                          //编号
    static uint8_t Psw[2][16];                                                          //两组密码
    static uint8_t pswnum=0;
    static uint8_t pswindex=0;
    char str[17];
    uint8_t temp[16];
   
    uint8_t i;
    switch(UI.step)
    {
    case 0:
        Voice_play(QING_SHU_RU_LIANG_WEI_BIAN_HAO_YI_JING_HAO_JIAN_JIE_SHU);            //请输入两位编号以#号键结束
        DisplayTips(&TIPS_QSRLWBHYJHJJS);                                               //提示请输入两位号
        UI.step = 1;                                                                    //跳到等待编号输入态
        memset(BianHao,0,2);                                                            //清空编号
        memset(Psw,0,16*2);                                                             //清空密码
        pswnum = 0;                                                                     //输入密码长度
        pswindex = 0;                                                                   //第一组密码
        break;
    case 1:                                                                             //等待编号输入状态
        
        break;
    case 2:                                                                             //等待
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.Tcount = 0;
            UI.step = 0;
        }
        break;
    case 3:                                                                             //等待密码输入状态
        
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY0:    
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
        case TUCH_KEY4:
        case TUCH_KEY5:
        case TUCH_KEY6:
        case TUCH_KEY7:
        case TUCH_KEY8:
        case TUCH_KEY9:
            if(UI.step==1)                                                              //在等待编号输入状态下                                                           
            {
                sprintf(str,"%d",Tuch_status()->val);                                   //按键０－９数字输入
                if((BianHao[0]&0x80)== 0x00) 
                {
                    BianHao[0] = 0x80+Tuch_status()->val;
                    Display_english_str(56, 2,str, false);
                }
                else if((BianHao[1]&0x80)== 0x00)
                {
                    BianHao[1] = 0x80+Tuch_status()->val;
                    Display_english_str(64, 2,str, false);
                }
            }
            else if(UI.step==3)                                                         //在等待密码输入状态下                                                       
            {
                Display_english_str(pswnum * 8, 2, "*", false);
                Psw[pswindex][pswnum] = Tuch_status()->val+0x80;                        //第pswindex组录入密码+0x80前缀
                if(++pswnum>12)                                                         //密码超过１２显示错误　
                {
                    Voice_play(SHU_RU_CUO_WU);                                          
                    DisplayTips(&TIPS_SRCW);                                            //输入错误　
                    UI.step = 2;                                                        //跳等待状态后回到输入两位编号状态
                }
            }
            break;
        case TUCH_KEY_ESC:
            memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                      //按*返回用户设置菜单
            break;
        case TUCH_KEY_ENTER:                                                            //按#键确认   
            if(UI.step==1)                                                              //在等待编号输入状态下
            {
                if((BianHao[0]&0x80)==0 || (BianHao[1]&0x80)==0  || ((BianHao[0]&~0x80)>0))//检查输入是否错误
                {
                    Voice_play(SHU_RU_CUO_WU);                                          
                    DisplayTips(&TIPS_SRCW);                                            //输入错误　
                    UI.step = 2;                                                        //跳等待状态后回到输入两位编号状态
                }
                else if(ReadAndcheckUserPswByID(temp,(((BianHao[0]-0x80)*10)+(BianHao[1]-0x80))|0x80)==true)//读取EEPROM 查找已存在的编号
                {
                    Voice_play(BIAN_HAO_YI_CUN_ZAI);                                    //编号已存在
                    DisplayTips(&TIPS_BHYCZ);
                    UI.step = 2;                                                        //跳等待状态后回到输入两位编号状态                                           
                }
                else                                                                      
                {
                    Voice_play(QING_SHU_RU_YONG_HU_MI_MA_YI_JING_HAO_JIAN_JIE_SHU);     //请输入用户密码以#号键结束
                    DisplayTips(&TIPS_QSRYHMMYJHJJS);                                   //请输入用户密码以#号键结束
                    UI.step = 3;                                                        //等待密码输入状态
                }
            }
            else if(UI.step == 3)                                                       //等待密码输入状态      
            {
                if(pswindex == 1)                                                       //已录好第一组                      
                {
                    if(0)                                                               //判断是否过于简单或其它
                    {
                        ;
                    }
                    else if(strncmp((const char *)&Psw[0],(const char *)&Psw[1],16))    //两次密码不一样
                    {
                        DisplayTips(&TIPS_SRCW);                                        //输入错误
                        Voice_play(LIANG_CI_SHU_RU_DE_MA_MA_BU_YI_ZHI);                 //两次输入的密码不一致
                        UI.step = 2;                                                    //跳等待状态后回到输入两位编号状态 						                                        
                    }
                    else                                                                //输入成功 
                    {   
                        if(WriteUserPSW( (((BianHao[0]-0x80)*10)+(BianHao[1]-0x80))|0x80 ,Psw[0]))//因为Flash特殊擦除后为0x00 所以|0x80
                        {
                            Voice_play(TIAN_JIA_CHENG_GONG);                            //添加成功
                            DisplayTips(&TIPS_TJCG);                            
                        }
                        else
                        {
                            Voice_play(BIAN_HAO_YI_CUN_ZAI);                            //编号已存在
                            DisplayTips(&TIPS_TJSB);                                    
                        }
                        UI.step = 2;                                                    //跳等待状态后回到输入两位编号状态
                    }
                }
                else                                                                    //正在第一组状态下 
                {
                    Voice_play(QING_ZAI_SHU_RU_YI_CI);                                  //请再输入一次
                    DisplayTips(&TIPS_QZSRYC);                                          //再输入一次密码
                    UI.step = 3;
                    pswindex =  1;                                                      //开始录入第二组秘密
                }
                pswnum = 0;                                                             //重新计数
            }
            break;
        default:
            break;
        }
    }
}
//添加指纹
void AddUserFigMenu(void)
{
    uint8_t Pidtemp;
    static uint8_t BianHao[2];
    char str[17];
    switch(UI.step)
    {
    case 0:
        Voice_play(QING_SHU_RU_LIANG_WEI_BIAN_HAO_YI_JING_HAO_JIAN_JIE_SHU);            //请输入两位编号以#号键结束
        DisplayTips(&TIPS_QSRLWBHYJHJJS);                                               //请输入两位编号以#号键结束
        UI.step = 1;
        memset(BianHao,0,2);
        Fingerprint_status()->PID = 0;                                                  //编号
        Fingerprint_status()->BID = 0;
        Fingerprint_status()->step =0;
        break;
    case 1:                                                                             
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);                 //返回上级菜单请按*号键
            UI.step = 4;
        }
        break;
    case 4:                                                                             //输入等待
        break;
    case 2:
        if(Voice_Status() ==VOICE_ST_IDLE)
        {
            UI.Tcount = 0;
            UI.step = 0;
        }
        break;
    case 3:
        //------------------------------------------------------------------------ 
        Fingerprint_RegProcess();
        if(Fingerprint_status()->RegEven!=F_NULL)
        {
            switch(Fingerprint_status()->RegEven)
            {
                case GET_OK:
                     Voice_play(ZHI_WEN_YIN_DO);                                        //指纹音Do
                     DisplayTips(&TIPS_QNKSZZFYC);                                      //请拿开手指，再放一次  
                case GET_FAIL:  
                     //Voice_play(ZHI_WEN_YIN_DO);                                        //指纹音Do
                      break;
                case GENCHAR_OK:  
                      Voice_play(QING_NA_KAI_SHOU_ZHI_ZAI_FANG_YI_CI);                  //请拿开手指，再放一次
                      sprintf(str,"%d",Fingerprint_status()->BID);Display_english_str(64-8,2,str,false);//显示次数
                      break;
                case GENCHAR_SAME:
                      //DisplayTips(&TIPS_QXZWWYDSZ); //请向左微微移动手指
                      break;
                case STORE_OK:                                                                    
                      Voice_play(TIAN_JIA_CHENG_GONG);                                   //添加成功
                      DisplayTips(&TIPS_TJCG);                                           //添加成功
                      WriteFigRecord((BianHao[0]&0x7F)*10+(BianHao[1]&0x7F)|0x80,(BianHao[0]&0x7F)*10+(BianHao[1]&0x7F));//覆盖式写id|0x80 FID不要|0x80
                      break;
                case STORE_FAIL:
                      Voice_play(TIAN_JIA_SHI_BAI);                                      //添加失败
                      DisplayTips(&TIPS_TJSB);                                           //添加失败
                      break;
                case SLEEP_OK:
                      UI.step = 2;UI.Tcount = 0;                                         //跳出这个程序 这里有个BUG 第二次出现
                      break;
                default:break;
            }
            Fingerprint_status()->RegEven =F_NULL;                                       //先清空事件
        }
        //------------------------------------------------------------------------
    case 5:
        
        break;
    default:
        break;
    }
    
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY0:    
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
        case TUCH_KEY4:
        case TUCH_KEY5:
        case TUCH_KEY6:
        case TUCH_KEY7:
        case TUCH_KEY8:
        case TUCH_KEY9:
            if(UI.step==1 || UI.step==4)
            {
                sprintf(str,"%d",Tuch_status()->val);
                if((BianHao[0]&0x80)== 0x00) 
                {
                    BianHao[0] = 0x80+Tuch_status()->val;
                    Display_english_str(56, 2,str, false);
                }
                else if((BianHao[1]&0x80)== 0x00)
                {
                    BianHao[1] = 0x80+Tuch_status()->val;
                    Display_english_str(64, 2,str, false);
                }
            }    
            break;  
        case TUCH_KEY_ESC:                                                               //*返回                                                              
            Fingerprint_Sleep();                                                         //返回要睡眠和断电 
            CyDelay(20);                                                                 //这个要改掉，否则。。。。
            Fingerprint_Power(false);
            memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                       //返回用户设置菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            if(UI.step==1 || UI.step==4)
            { 
                if((BianHao[0]&0x80)==0 || (BianHao[1]&0x80)==0)                        //输入是否错误
                {
                    Voice_play(SHU_RU_CUO_WU);                                          //输入错误
                    DisplayTips(&TIPS_SRCW);
                    UI.step = 2;
                }
                else 
                {
                   Fingerprint_status()->PID = (BianHao[0]&0x7F)*10+(BianHao[1]&0x7F);//输入的编号
                   if(ReadAndcheckFigByid((BianHao[0]&0x7F)*10+(BianHao[1]&0x7F)|0x80,&Pidtemp))//查找已存在的指纹
                    {
                        Voice_play(ZHI_WEN_YI_CUN_ZAI);                                 //指纹已存在
                        DisplayTips(&TIPS_ZWYCZ);
                        UI.step = 2;              
                    }
                  else                                                                  //请刷卡或按指纹   
                    {
                        Voice_play(QING_AN_ZHI_WEN);                                    //请按指纹                  
                        Fingerprint_status()->step =0;
                        Fingerprint_status()->BID = 0;
                        DisplayTips(&TIPS_QSRZWHKP);
                        UI.step = 3;
                        Fingerprint_status()->RegEven = F_NULL;                         //先清空事件
                    } 
                }
            }
            break;
        default:
            break;
        }
    }
}
//添加卡片
void AddUserCarMenu(void)
{
    uint8_t TempID[11];
    static uint8_t BianHao[2];                                                          //编号
    char str[17];
    uint8_t i;
    switch(UI.step)
    {
    case 0:
        Voice_play(QING_SHU_RU_LIANG_WEI_BIAN_HAO_YI_JING_HAO_JIAN_JIE_SHU);            //请输入两位编号以#号键结束
        DisplayTips(&TIPS_QSRLWBHYJHJJS);
        UI.step = 1;
        memset(BianHao,0,2);
        break;
    case 1:                                                                             
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);                 //返回上级菜单请按*号键
            UI.step = 4;
        }
        break;
    case 4:                                                                             //输入等待
        
        break;
    case 2:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.Tcount = 0;
            UI.step = 0;
        }
        break;
    case 3:
        //------------------------------------------------------------------------
        if(Card_ReadId(TempID)==0)
        {
            Voice_play(ZHI_WEN_YIN_DO);                                                 //音Do
            WriteCardRecord((((BianHao[0]-0x80)*10)+(BianHao[1]-0x80))|0x80,TempID);    //保存卡片与ID                         
            UI.step = 5;                                                                //跳到成功
        }
        //------------------------------------------------------------------------   
        break; 
    case 5:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            Voice_play(TIAN_JIA_CHENG_GONG);                                            //添加成功
            DisplayTips(&TIPS_TJCG);                                                    //添加成功 
            UI.step = 2;
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY0:    
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
        case TUCH_KEY4:
        case TUCH_KEY5:
        case TUCH_KEY6:
        case TUCH_KEY7:
        case TUCH_KEY8:
        case TUCH_KEY9:
            if(UI.step==1 || UI.step==4)
            {
                UI.step = 4;
                sprintf(str,"%d",Tuch_status()->val);
                if((BianHao[0]&0x80)== 0x00) 
                {
                    BianHao[0] = 0x80+Tuch_status()->val;
                    Display_english_str(56, 2,str, false);
                }
                else if((BianHao[1]&0x80)== 0x00)
                {
                    BianHao[1] = 0x80+Tuch_status()->val;
                    Display_english_str(64, 2,str, false);
                }
            }    
            break;
        case TUCH_KEY_ESC:
            memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                      //返回用户设置菜单
            break;                                                                      //*返回
        case TUCH_KEY_ENTER:                                                            //#确认
            if(UI.step==1 || UI.step==4)
            {
                if((BianHao[0]&0x80)==0 || (BianHao[1]&0x80)==0)                        //输入是否错误
                {                                                           
                    Voice_play(SHU_RU_CUO_WU);                                          //输入错误
                    DisplayTips(&TIPS_SRCW);
                    UI.step = 2;
                }
                else if(ReadAndcheckCardByid((((BianHao[0]-0x80)*10)+(BianHao[1]-0x80))|0x80,TempID))//查找已存在的卡片
                {
                    //BIAN_HAO_YI_CUN_ZAI                                                //编号已存在
                    Voice_play(KA_PIAN_YI_CUN_ZAI);                                      //卡片已存在
                    DisplayTips(&TIPS_MKYCZ);
                    UI.step = 2;              
                }
                else                                                                     //请刷卡或按指纹   
                {
                    Voice_play(QING_SHUA_KA);                                            //请刷卡
                    DisplayTips(&TIPS_QSRZWHKP);
                    UI.step = 3;
                }
            }
            break;                                                                       
        default:
            break;
        }
    }
}
//删除用户密码
void DelUserPswMenu(void)
{
    char str[30];
    static uint8_t playStep = 0;
    static uint8_t BianHao[2];                                                          //编号
    switch(UI.step)
    {
    case 0:
        DisplayItem(0, Del_menu, 2);
        UI.step = 1;
        UI.slect_index = 0;
        Voice_play(Del_menu[UI.slect_index].SpeakAddr);                                 //单个删除请按1
        memset(BianHao,0,2);
        playStep = 0;
        UI.autoRead = true;
        break;
    case 1:
       if(UI.autoRead)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)
                {
                    Voice_play(YU_YIN_1+UI.slect_index);                                //1
                    if(UI.slect_index == 1)                                             //0-1条目
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)
                {
                    Voice_play(Del_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, Del_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, Del_menu);
                }
                else if(playStep==2)
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);             //返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;
                }
            }
        }       
        break;
    case 2:                                                                                 //输入编号状态    
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.step = 3;  
            Voice_play(TIPS_QSRLWBHYJHJJS.SpeakAddr);                                       //请输入两位编号以#号键结束
            DisplayTips(&TIPS_QSRLWBHYJHJJS);
        }
        break;
    case 3:                                                                                 //输入编号状态  
        if(Tuch_status()->even)
        {
            switch(Tuch_status()->val)
            {
                case TUCH_KEY0:    
                case TUCH_KEY1:
                case TUCH_KEY2:
                case TUCH_KEY3:
                case TUCH_KEY4:
                case TUCH_KEY5:
                case TUCH_KEY6:
                case TUCH_KEY7:
                case TUCH_KEY8:
                case TUCH_KEY9:
                        sprintf(str,"%d",Tuch_status()->val);
                        if((BianHao[0]&0x80)== 0x00) 
                        {
                            BianHao[0] = 0x80+Tuch_status()->val;
                            Display_english_str(56, 2,str, false);
                        }
                        else if((BianHao[1]&0x80)== 0x00)
                        {
                            BianHao[1] = 0x80+Tuch_status()->val;
                            Display_english_str(64, 2,str, false);
                        }    
                        break;
               default:break;
            }
            if(Tuch_status()->val <= TUCH_KEY9)
                Tuch_status()->val = TUCH_KEY_NULL;                                         //在输入状态下数字不作用 其它键还有效
        }
        break;
    case 4:                                                                                 //删除成功与失败
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.step = 0;
            UI.autoRead = true;
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
            UI.autoRead = 0;
            if(UI.step==1)UI.step = 2;                                                      //跳到输入编号状态
            break;
        case TUCH_KEY2:
            UI.autoRead = 0;
            if(UI.step==1)                                                                  //在菜单选择状态下
            {
                if(DelUserPswRecord(0,true))                                                //全部删除
                {
                   DisplayTips(&TIPS_SCCG);                                                 //删除成功   
                   Voice_play(SHAN_CHU_CHENG_GONG);                                         //删除成功                                    
                }
                else 
                {
                    DisplayTips(&TIPS_SCSB);
                    Voice_play(SHAN_CHU_SHI_BAI);                                           //删除失败
                }
                UI.step = 4;                                                                //返回
            }
            break; 
        case TUCH_KEY8:
            if(UI.step==1)                                                                  //在选择菜单下
            {
                UI.autoRead = 0;
                DisplayItemSelect(false, UI.slect_index, Del_menu);
                if(--UI.slect_index < 0)UI.slect_index = 1;
                DisplayItemSelect(true, UI.slect_index, Del_menu);
            }
            break;                                                                          //上翻
        case TUCH_KEY0:
            if(UI.step==1)                                                                  //在选择菜单下
            {
                UI.autoRead = 0;
                DisplayItemSelect(false, UI.slect_index, Del_menu);
                if(++UI.slect_index > 1)UI.slect_index = 0;
                DisplayItemSelect(true, UI.slect_index, Del_menu);
            }
            break;                                                                          //下翻
        case TUCH_KEY_ESC:
            if(UI.step==2||UI.step==3)                                                      //在单个删除状态下返回选择菜单
                UI.step = 0;  
            else 
                memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                      //返回用户设置菜单 *返回
            break;
        case TUCH_KEY_ENTER:                                                                //#确认
            UI.autoRead = false;
            if(UI.step==1)                                                                  //在菜单选择状态下
            {
                if(UI.slect_index==0)                                                       //单个删除
                    UI.step = 2;                                                            //跳到输入编号状态
                else                                                                        //全部删除
                {
                    if(DelUserPswRecord(0,true))                                            //全部删除
                    {
                       DisplayTips(&TIPS_SCCG);                                             //删除成功   
                       Voice_play(SHAN_CHU_CHENG_GONG);                                     //删除成功                                    
                    }
                    else 
                    {
                        DisplayTips(&TIPS_SCSB);                                            //删除失败
                        Voice_play(SHAN_CHU_SHI_BAI);                                       //删除失败
                    }
                    UI.step = 4;                                                            //返回
                }
            }
            else if(UI.step==3)                                                             //在单个删除输入ID号状态下
            {
                if(((BianHao[0]-0x80)==0)&& (BianHao[1]!=0))                                //编号是否正确 10位必须是0
                {
                    if(DelUserPswRecord((((BianHao[0]-0x80)*10)+(BianHao[1]-0x80))|0x80,false))
                    {
                       DisplayTips(&TIPS_SCCG);                                             //删除成功   
                       Voice_play(SHAN_CHU_CHENG_GONG);                                     //删除成功  
                    }
                    else 
                    {
                        DisplayTips(&TIPS_SCSB);                                            //删除失败
                        Voice_play(SHAN_CHU_SHI_BAI);                                       //删除失败
                    }
                }
                else
                {
                    Voice_play(SHU_RU_CUO_WU);                                              //输入错误 
                    DisplayTips(&TIPS_SRCW);                                                //输入错误                
                }   
                UI.step = 4;                                                                //返回
            }
            break;
        default:
            break;
        }
    }
}
//删除指纹
void DelUserFigMenu(void)
{
    static bool alldel = false;                                                         //是否为全删或按PID删
    static uint8_t PidTemp=0;                                                           
    char str[30];
    static uint8_t playStep = 0;
    static uint8_t BianHao[2];    
    switch(UI.step)
    {
    case 0:
        DisplayItem(0, Del_menu, 2);
        UI.step = 1;
        UI.slect_index = 0;
        Voice_play(Del_menu[UI.slect_index].SpeakAddr);                                 //单个删除请按1
        memset(BianHao,0,2);
        playStep = 0;
        UI.autoRead = true;
        break;
    case 1:
       if(UI.autoRead)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)
                {
                    Voice_play(YU_YIN_1+UI.slect_index);                                //1
                    if(UI.slect_index == 1)                                             //0-1条目
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)
                {
                    Voice_play(Del_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, Del_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, Del_menu);
                }
                else if(playStep==2)
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);             //返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;
                }
            }
        } 
        break;
    case 2:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.step = 3;  
            Voice_play(TIPS_QSRLWBHYJHJJS.SpeakAddr);                                       //请输入两位编号以#号键结束
            DisplayTips(&TIPS_QSRLWBHYJHJJS);
        }  
        break;
    case 3:                                                                                 //输入两位编号状态
        if(Tuch_status()->even)
        {
            switch(Tuch_status()->val)
            {
                case TUCH_KEY0:    
                case TUCH_KEY1:
                case TUCH_KEY2:
                case TUCH_KEY3:
                case TUCH_KEY4:
                case TUCH_KEY5:
                case TUCH_KEY6:
                case TUCH_KEY7:
                case TUCH_KEY8:
                case TUCH_KEY9:
                        sprintf(str,"%d",Tuch_status()->val);
                        if((BianHao[0]&0x80)== 0x00) 
                        {
                            BianHao[0] = 0x80+Tuch_status()->val;
                            Display_english_str(56, 2,str, false);
                        }
                        else if((BianHao[1]&0x80)== 0x00)
                        {
                            BianHao[1] = 0x80+Tuch_status()->val;
                            Display_english_str(64, 2,str, false);
                        }    
                        break;
               default:break;
            }
            if(Tuch_status()->val <= TUCH_KEY9)
                Tuch_status()->val = TUCH_KEY_NULL;                                         //在输入状态下数字不作用 其它键还有效
        }
        //-----------------------------------------------------------------------------------
        if(Fingerprint_status()->NormalEven==GET_ING)
        { 
            Voice_play(ZHI_WEN_YIN_DO);                                                     //指纹音Do
            Fingerprint_status()->NormalEven = F_NULL;
        }
        else if(Fingerprint_status()->NormalEven==SEARCH_OK)                                //搜索到了 再在状态下才退出这个状态
        {
            Fingerprint_status()->NormalEven = F_NULL;
            if(DelFigRecord(Fingerprint_status()->PID|0x80,false))                          //按id |0x80来删除                                             
            {
                DisplayTips(&TIPS_SCCG);                                                    //删除成功   
                Voice_play(SHAN_CHU_CHENG_GONG);                                            //删除成功  
            }
            else 
            {
                DisplayTips(&TIPS_SCSB);                                                    //删除失败
                Voice_play(SHAN_CHU_SHI_BAI);                                               //删除失败
            }
            UI.step = 4;  
        }
        else if(Fingerprint_status()->NormalEven==SEARCH_FAIL)                              //没搜索到返回
        {
            Fingerprint_status()->NormalEven = F_NULL;
            DisplayTips(&TIPS_SCSB);                                                        //删除失败
            Voice_play(SHAN_CHU_SHI_BAI);                                                   //删除失败
            UI.step = 4; 
        }
        //-----------------------------------------------------------------------------------
        break;
    case 4:                                                                                 //删除成功与失败
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.step = 0;
            UI.autoRead = true;
        }
        break;
    case 5:                                                                                 //正在删除指纹
        //-------------------------------------------------------------
        Fingerprint_RegDelProcess(PidTemp,alldel);                                          //多次查询全部删除
        if(Fingerprint_status()->RegEven!=F_NULL)
        {
            switch(Fingerprint_status()->RegEven)
            {
                case EMPTY_OK:DisplayTips(&TIPS_SCCG);
                     break;
                case EMPTY_FAIL:
                     UI.step = 4;
                     DisplayTips(&TIPS_SCSB);
                     Voice_play(SHAN_CHU_SHI_BAI);                                          //删除失败
                     break;
                case SLEEP_OK:
                     UI.step = 4;
                     Voice_play(SHAN_CHU_CHENG_GONG);                                       //删除成功
                     DisplayTips(&TIPS_SCCG);
                     break;
                default:break;
            }
            Fingerprint_status()->RegEven =F_NULL;
        }
        //-------------------------------------------------------------
        break;
    default:
        break;
    }

    if(UI.step!=5)Fingerprint_NormalProcess();                                              //读取指纹  必须放在 TIME_BASIC_MS里
    
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
            UI.autoRead = 0;
            if(UI.step==1)UI.step = 2;                                                      //跳到输入编号状态
            break;
        case TUCH_KEY2:
            UI.autoRead = 0;
            if(UI.step==1)                                                                  //在菜单选择状态下
            {
                DisplayTips(&TIPS_ZZSCZW);                                                  //正在删除指纹
                alldel = true;                                                              //全删标记
                UI.step = 5;                                                                //跳到删除状态
            }
            break;    
        case TUCH_KEY8:
            if(UI.step==1)                                                                  //在选择菜单下
            {
                UI.autoRead = 0;
                DisplayItemSelect(false, UI.slect_index, Del_menu);
                if(--UI.slect_index < 0)UI.slect_index = 1;
                DisplayItemSelect(true, UI.slect_index, Del_menu);
            }
            break;                                                                          //上翻
        case TUCH_KEY0:
            if(UI.step==1)                                                                  //在选择菜单下
            {
                UI.autoRead = 0;
                DisplayItemSelect(false, UI.slect_index, Del_menu);
                if(++UI.slect_index > 1)UI.slect_index = 0;
                DisplayItemSelect(true, UI.slect_index, Del_menu);
            }
            break; 
        case TUCH_KEY_ESC:
            if(UI.step==2||UI.step==3)                                                      //在单个删除状态下返回选择菜单
                UI.step = 0;  
            else 
                memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                      //返回用户设置菜单 *返回
            break;
        case TUCH_KEY_ENTER: 
            UI.autoRead = false;
            if(UI.step==1)                                                                  //在菜单选择状态下
            {
                if(UI.slect_index==0)                                                       //单个删除
                    UI.step = 2;                                                            //跳到输入编号状态
                else                                                                        //全部删除
                {
                    alldel = true;                                                          //全删标记
                    UI.step = 5;                                                            //跳到删除状态
                }     
            }
            else if(UI.step==3)                                                             //在单个删除输入ID号状态下
            {
                if(((BianHao[0])!=0)&& (BianHao[1]!=0))                                     //编号是否正确 10位必须是个位必须有数
                {
                    Fingerprint_status()->step = 0;
                    DisplayTips(&TIPS_ZZSCZW);                                              //正在删除指纹 按Pid来删除
                    alldel = false;                                                         //按Pid来删除标记
                    PidTemp = ((BianHao[0]-0x80)*10)+(BianHao[1]-0x80);                     //这个不能带0x80
                    UI.step = 5;                                                            //跳到删除状态   
                    DelFigRecord(PidTemp|0x80,false);                                       //删除EEProm里的id与FID 按Pid来
                }
                else
                {
                    Voice_play(SHU_RU_CUO_WU);                                              //输入错误 
                    DisplayTips(&TIPS_SRCW);                                                //输入错误  
                    UI.step = 4;                                                            //返回
                }      
            }       
            break;//#确认
        default:
            break;
        }
    }
}
//删除卡片
void DelUserCarMenu(void)
{
    char str[30];
    uint8_t TempId[4];
    static uint8_t playStep = 0;
    static uint8_t BianHao[2];                                                          //编号
    switch(UI.step)
    {
    case 0:
        DisplayItem(0, Del_menu, 2);
        UI.step = 1;
        UI.slect_index = 0;
        Voice_play(Del_menu[UI.slect_index].SpeakAddr);                                 //单个删除请按1
        memset(BianHao,0,2);
        playStep = 0;
        UI.autoRead = true;
        break;
    case 1:
       if(UI.autoRead)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)
                {
                    Voice_play(YU_YIN_1+UI.slect_index);                                //1
                    if(UI.slect_index == 1)                                             //0-1条目
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)
                {
                    Voice_play(Del_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, Del_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, Del_menu);
                }
                else if(playStep==2)
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);             //返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;
                }
            }
        }       
        break;
    case 2:                                                                                 //输入编号状态    
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.step = 3;  
            Voice_play(TIPS_QSRLWBHYJHJJS.SpeakAddr);                                       //请输入两位编号以#号键结束
            DisplayTips(&TIPS_QSRLWBHYJHJJS);
        }else if(Card_ReadId(TempId)==0)
        {
            UI.step = 3;                                                                    //当在播音时刷到卡快速跳到3步
        }
        break;
    case 3:                                                                                 //输入编号状态  
        if(Tuch_status()->even)
        {
            switch(Tuch_status()->val)
            {
                case TUCH_KEY0:    
                case TUCH_KEY1:
                case TUCH_KEY2:
                case TUCH_KEY3:
                case TUCH_KEY4:
                case TUCH_KEY5:
                case TUCH_KEY6:
                case TUCH_KEY7:
                case TUCH_KEY8:
                case TUCH_KEY9:
                        sprintf(str,"%d",Tuch_status()->val);
                        if((BianHao[0]&0x80)== 0x00) 
                        {
                            BianHao[0] = 0x80+Tuch_status()->val;
                            Display_english_str(56, 2,str, false);
                        }
                        else if((BianHao[1]&0x80)== 0x00)
                        {
                            BianHao[1] = 0x80+Tuch_status()->val;
                            Display_english_str(64, 2,str, false);
                        }    
                        break;
               default:break;
            }
            if(Tuch_status()->val <= TUCH_KEY9)
                Tuch_status()->val = TUCH_KEY_NULL;                                         //在输入状态下数字不作用 其它键还有效
        }
        //----------------------------------------------------------------------------------
        if(Card_ReadId(TempId)==0)                                                          //读取卡片
        {
            Voice_play(ZHI_WEN_YIN_DO);                                                     //音Do
            if(DelCardRecord(0,TempId,false))                                               //按TempID来删除
            {
                DisplayTips(&TIPS_SCCG);                                                    //删除成功   
                Voice_play(SHAN_CHU_CHENG_GONG);                                            //删除成功  
            }
            else 
            {
                DisplayTips(&TIPS_SCSB);                                                    //删除失败
                Voice_play(SHAN_CHU_SHI_BAI);                                               //删除失败
            }
            UI.step = 4;                                                            
        }
        //----------------------------------------------------------------------------------
        break;
    case 4:                                                                                 //删除成功与失败
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.step = 0;
            UI.autoRead = true;
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
            UI.autoRead = 0;
            if(UI.step==1)UI.step = 2;                                                      //跳到输入编号状态
            break;
        case TUCH_KEY2:
            UI.autoRead = 0;
            if(UI.step==1)                                                                  //在菜单选择状态下
            {                                              
                if(DelCardRecord(0,0,true))                                                 //全部删除卡片
                {
                   DisplayTips(&TIPS_SCCG);                                                 //删除成功   
                   Voice_play(SHAN_CHU_CHENG_GONG);                                         //删除成功                                    
                }
                else 
                {
                    DisplayTips(&TIPS_SCSB);
                    Voice_play(SHAN_CHU_SHI_BAI);                                           //删除失败
                }
                UI.step = 4;                                                                //返回
            }
            break; 
        case TUCH_KEY8:
            if(UI.step==1)                                                                  //在选择菜单下
            {
                UI.autoRead = 0;
                DisplayItemSelect(false, UI.slect_index, Del_menu);
                if(--UI.slect_index < 0)UI.slect_index = 1;
                DisplayItemSelect(true, UI.slect_index, Del_menu);
            }
            break;                                                                          //上翻
        case TUCH_KEY0:
            if(UI.step==1)                                                                  //在选择菜单下
            {
                UI.autoRead = 0;
                DisplayItemSelect(false, UI.slect_index, Del_menu);
                if(++UI.slect_index > 1)UI.slect_index = 0;
                DisplayItemSelect(true, UI.slect_index, Del_menu);
            }
            break;                                                                          //下翻
        case TUCH_KEY_ESC:
            if(UI.step==2||UI.step==3)                                                      //在单个删除状态下返回选择菜单
                UI.step = 0;  
            else 
                memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                      //返回用户设置菜单 *返回
            break;
        case TUCH_KEY_ENTER:                                                                //#确认
            UI.autoRead = false;
            if(UI.step==1)                                                                  //在菜单选择状态下
            {
                if(UI.slect_index==0)                                                       //单个删除
                    UI.step = 2;                                                            //跳到输入编号状态
                else                                                                        //全部删除
                {
                    if(DelCardRecord(0,0,true))                                             //全部删除 卡片
                    {
                       DisplayTips(&TIPS_SCCG);                                             //删除成功   
                       Voice_play(SHAN_CHU_CHENG_GONG);                                     //删除成功                                    
                    }
                    else 
                    {
                        DisplayTips(&TIPS_SCSB);                                            //删除失败
                        Voice_play(SHAN_CHU_SHI_BAI);                                       //删除失败
                    }
                    UI.step = 4;                                                            //返回
                }
            }
            else if(UI.step==3)                                                             //在单个删除输入ID号状态下
            {
                if(((BianHao[0])!=0)&& (BianHao[1]!=0))                                     //编号是否正确 10位必须是个位必须有数
                {
                    if(DelCardRecord((((BianHao[0]-0x80)*10)+(BianHao[1]-0x80))|0x80,TempId,false))//按id来删除
                    {
                        DisplayTips(&TIPS_SCCG);                                            //删除成功   
                        Voice_play(SHAN_CHU_CHENG_GONG);                                    //删除成功  
                    }
                    else 
                    {
                        DisplayTips(&TIPS_SCSB);                                            //删除失败
                        Voice_play(SHAN_CHU_SHI_BAI);                                       //删除失败
                    }
                }
                else
                {
                    Voice_play(SHU_RU_CUO_WU);                                              //输入错误 
                    DisplayTips(&TIPS_SRCW);                                                //输入错误                
                }   
                UI.step = 4;                                                                //返回
            }
            break;
        default:
            break;
        }
    }
}
//设置开门模式
void SetOpenModeMenu(void)
{
    OptionRecord_t optionTemp;
    static uint8_t playStep = 0;
    switch(UI.step)
    {
    case 0:
        DisplayItem(0, Mode_menu, 2);
        UI.step = 1;
        Voice_play(Mode_menu[UI.slect_index].SpeakAddr);                                //常用模式
        playStep = 0;
        break;
    case 1:
      if(UI.autoRead)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)
                {
                    Voice_play(YU_YIN_1+UI.slect_index);                                 //播报数字
                    if(UI.slect_index == 1)                                              //0-1条目
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)                                                     //播报中文
                {
                    Voice_play(Mode_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, Mode_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, Mode_menu);
                }
                else if(playStep==2)
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);           //返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;
                }
            }
        }     
        break;
    case 2:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            UI.step = 0;
            memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                         //返回用户设置菜单
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
            Function_status()->ManageMode.OpenDoorMode = DOOR_NORMAL;
            Voice_play(SHE_ZHI_CHENG_GONG);                                                 //设置成功
            DisplayTips(&TIPS_SZCG);
            ReadOptionRecord(&optionTemp);                                                  //先读设置
            optionTemp.OpenDoorMode = DOOR_NORMAL;                                          //改
            WriteOptionRecord(&optionTemp);                                                 //后写
            UI.step = 2;
            break;
        case TUCH_KEY2:
            Function_status()->ManageMode.OpenDoorMode = DOOR_SAVFT;
            Voice_play(SHE_ZHI_CHENG_GONG);                                                //设置成功
            DisplayTips(&TIPS_SZCG);
            ReadOptionRecord(&optionTemp);                                                  //先读设置
            optionTemp.OpenDoorMode = DOOR_SAVFT;                                           //改
            WriteOptionRecord(&optionTemp);                                                 //后写
            UI.step = 2;
            break;       
        case TUCH_KEY8:                                                                    //上翻
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, Mode_menu);
            if(--UI.slect_index < 0)UI.slect_index = 1;
            DisplayItemSelect(true, UI.slect_index, Mode_menu);
            break;
        case TUCH_KEY0:                                                                    //下翻
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, Mode_menu);
            if(++UI.slect_index > 1)UI.slect_index = 0;
            DisplayItemSelect(true, UI.slect_index, Mode_menu);
            break;
        case TUCH_KEY_ESC:                                                                 //*返回
            memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                         //返回用户设置菜单
            break;
        case TUCH_KEY_ENTER:                                                               //#确认
            //在没添加指纹和密码和卡片时不能 设置为安全模式，要语音提示
            
            //先读是否已录入指纹和密码和卡片
            
            if(UI.slect_index==0)Function_status()->ManageMode.OpenDoorMode = DOOR_NORMAL;
            else Function_status()->ManageMode.OpenDoorMode = DOOR_SAVFT;
            Voice_play(SHE_ZHI_CHENG_GONG);                                                //设置成功
            ReadOptionRecord(&optionTemp);                                                  //先读设置
            optionTemp.OpenDoorMode = Function_status()->ManageMode.OpenDoorMode;           //改
            WriteOptionRecord(&optionTemp);                                                 //后写
            DisplayTips(&TIPS_SZCG);   
            UI.step = 2;
            break;
        default:
            break;
        }
    }
}
//修改管理密码
void AddManPswMenu(void)
{
    static uint8_t Psw[2][16];                                                          //两组密码
    static uint8_t pswnum=0;
    static uint8_t pswindex=0;
        
    switch(UI.step)
    {
    case 0:
        if(Voice_Status()==VOICE_ST_IDLE)
        { 
        Voice_play(QING_TIAN_JIA_GUAN_LI_MI_MA_YI_JING_HAO_JIAN_JIE_SHU);               //请添加管理密码以#号键结束
        DisplayTips(&TIPS_QTJGLMM);
        UI.step = 1;
        memset(Psw,0,16*2);                                                             //清空密码
        pswnum = 0;                                                                     //输入密码长度
        pswindex = 0;                                                                   //第一组密码
        }
        break;
    case 1:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);                 //返回上级菜单请按*号键
            UI.step = 2;
        }
        break;
    case 2:                                                                             //输入等待

        break;
    case 3: 
        if(Voice_Status()==VOICE_ST_IDLE)                                               //设置成功
        {
            memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                      //返回用户设置菜单
        }
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY0:
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
        case TUCH_KEY4:
        case TUCH_KEY5:
        case TUCH_KEY6:
        case TUCH_KEY7:
        case TUCH_KEY8:
        case TUCH_KEY9:
                Display_english_str(pswnum * 8, 2, "*", false);
                Psw[pswindex][pswnum] = Tuch_status()->val+0x80;                        //第pswindex组录入密码+0x80前缀
                if(++pswnum>12)                                                         //密码超过１２显示错误　
                {
                    Voice_play(SHU_RU_CUO_WU);                                          
                    DisplayTips(&TIPS_SRCW);                                            //输入错误　
                    UI.step = 0;                                                        //重输入
                } 
            break;
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[1], sizeof(MenuNode_t));                      //返回用户设置菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            if(pswindex==0)                                                             //第一组
            {
                Voice_play(QING_ZAI_SHU_RU_YI_CI);                                      //请再输入一次
                DisplayTips(&TIPS_QZSRYC);                                              //再输入一次密码
                pswindex =  1;                                                          //开始录入第二组秘密
                UI.step = 2;
            }
            else                                                                        //第二组
            {
                if(strncmp((const char *)&Psw[0],(const char *)&Psw[1],16))             //两次密码是否一样
                {
                    DisplayTips(&TIPS_SRCW);                                            //输入错误
                    Voice_play(LIANG_CI_SHU_RU_DE_MA_MA_BU_YI_ZHI);                     //两次输入的密码不一致
                    UI.step = 0;                                                        //重输入
                }
                else if(Pwd_Analyse(&Psw[1][0]))                                        //是否简单
                {
                    Voice_play(MI_MA_GUO_YU_JIAN_DAN);                                  //密码过于简单
                    DisplayTips(&TIPS_SRCW);                                            //输入错误
                    UI.step = 0;                                                        //重输入
                }
                else
                {
                    Voice_play(SHE_ZHI_CHENG_GONG);                                     //设置成功
                    DisplayTips(&TIPS_SZCG);   
                    memcpy(Function_status()->ManageMode.ManagePSW,&Psw[1][0],12);      //复制12位输入的管理密码                        
                    WriteManagePSW(&Psw[1][0]);                                         //保存到EEPROM
                    UI.step = 3;
                }
            }
            pswnum = 0;                                                                 //重新计数
            break;
        default:
            break;
        }
    }
}
//=====================================================================================
//三级菜单
//=====================================================================================
//时间设置
void TimeSetMenu(void)
{
    static Rtc_t Rtc;
    char str[30];
    static bool tog;
    static uint8_t x=0;
    
    switch(UI.step)
    {
    case 0:
        Display_clear_screen();
        Rtc_Get(&Rtc);
        sprintf((char*)str, "%.2d:%.2d:%.2d", Rtc.hour, Rtc.minute, Rtc.second);
        Display_english_str(32, 1, str, false);
        UI.step = 1;
        x = 0;
        break;
    case 1:
        if(++UI.Tcount > (300 / TIME_BASIC_MS))
        {
            UI.Tcount = 0;

            if(tog)
                Display_english_str((4+x) * 8, 1, "_", false);
            else
            {
                sprintf((char*)str, "%.2d:%.2d:%.2d", Rtc.hour, Rtc.minute, Rtc.second);
                Display_english_str(32, 1, str, false);
            }
            tog = !tog; 
        }
        break;
    case 2:                                                                              //设置失败
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            memcpy((void *)&UI, &Menu_tab[2], sizeof(MenuNode_t));                      //返回系统设置菜单
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        if(Tuch_status()->val <10)
        {
            switch(x)                                                                   //“_”图标X坐标
            {
                case 0:x=1;Rtc.hour-=(Rtc.hour/10)*10;Rtc.hour += Tuch_status()->val*10;break;  
                case 1:x=3;Rtc.hour-=(Rtc.hour%10);Rtc.hour += Tuch_status()->val;break;   
                case 3:x=4;Rtc.minute-=(Rtc.minute/10)*10;Rtc.minute += Tuch_status()->val*10;break;
                case 4:x=6;Rtc.minute-=(Rtc.minute%10);Rtc.minute += Tuch_status()->val;break;   
                case 6:x=7;Rtc.second-=(Rtc.second/10)*10;Rtc.second += Tuch_status()->val*10;break;
                case 7:x=0;Rtc.second-=(Rtc.second%10);Rtc.second += Tuch_status()->val;break;                
                default:x=0;break;
            }
            sprintf((char*)str, "%.2d:%.2d:%.2d", Rtc.hour, Rtc.minute, Rtc.second);
            Display_english_str(32, 1, str, false);
        }
        
        switch(Tuch_status()->val)
        {
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[2], sizeof(MenuNode_t));                      //返回系统设置菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            if(Rtc_Set(&Rtc))                                                           //设置时间
            {
                Voice_play(SHE_ZHI_SHI_BAI);                                            //设置失败
                DisplayTips(&TIPS_SZSB);
            }
            else 
            {
                Voice_play(SHE_ZHI_CHENG_GONG);                                         //设置成功
                DisplayTips(&TIPS_SZCG);                                                            
            }
            UI.step = 2; 
            break;
        default:
            break;
        }
    }
}
//日期设置
void DateSetMenu(void)
{
    static Rtc_t Rtc;
    char str[30];
    static bool tog;
    static uint8_t x=0;
    switch(UI.step)
    {
    case 0:
        Display_clear_screen();
        Rtc_Get(&Rtc);
        sprintf((char*)str, "%.4d-%.2d-%.2d", Rtc.year, Rtc.month, Rtc.day);
        Display_english_str(24, 1, str, false);
        UI.step = 1;
        x = 0;
        break;
    case 1:
        if(++UI.Tcount > (300 / TIME_BASIC_MS))
        {
            UI.Tcount = 0;
            if(tog)
                Display_english_str((5+x) * 8, 1, "_", false);
            else
            {
                sprintf((char*)str, "%.4d-%.2d-%.2d", Rtc.year, Rtc.month, Rtc.day);
                Display_english_str(24, 1, str, false);
            }
            tog = !tog;
        }
        break;
    case 2:                                                                             //设置失败
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            memcpy((void *)&UI, &Menu_tab[2], sizeof(MenuNode_t));                      //返回系统设置菜单
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        if(Tuch_status()->val <10)
        {
            switch(x)                                                                   //“_”图标X坐标
            {
                case 0:x=1;Rtc.year-=((Rtc.year%100)/10)*10; Rtc.year += Tuch_status()->val*10;break;     
                case 1:x=3;Rtc.year-=((Rtc.year%100)%10);  Rtc.year += Tuch_status()->val;break;       
                case 3:if(Tuch_status()->val<2) {Rtc.month-=((Rtc.month/10)*10);Rtc.month += Tuch_status()->val*10;x=4;}break;
                case 4:x=6;Rtc.month-=(Rtc.month%10);  Rtc.month += Tuch_status()->val;break;
                case 6:if(Tuch_status()->val<4){Rtc.day-=((Rtc.day/10)*10);Rtc.day += Tuch_status()->val*10;x=7;}break;
                case 7:x=0;Rtc.day-=(Rtc.day%10); Rtc.day += Tuch_status()->val;break;              
                default:x=0;break;
            }
            sprintf((char*)str, "%.4d-%.2d-%.2d", Rtc.year, Rtc.month, Rtc.day);
            Display_english_str(24, 1, str, false);
        }
        switch(Tuch_status()->val)
        {
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[2], sizeof(MenuNode_t));                      //返回系统设置菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            if(Rtc_Set(&Rtc))                                                           //设置日期
            {
                Voice_play(SHE_ZHI_SHI_BAI);                                            //设置失败
                DisplayTips(&TIPS_SZSB);
            }
            else 
            {
                Voice_play(SHE_ZHI_CHENG_GONG);                                         //设置成功
                DisplayTips(&TIPS_SZCG);                                                            
            }
            UI.step = 2; 
            break;
        default:
            break;
        }
    }
}
//语音设置
void VoiceSetMenu(void)
{
    OptionRecord_t optionTemp;
    static uint8_t playStep = 0;
    switch(UI.step)
    {
    case 0:
        Voice_play(Voice_menu[0].SpeakAddr);                                            //第一次播放 语音模式请按
        DisplayItem(0, Voice_menu, 2);
        UI.step = 1;
        playStep = 0;
        break;
    case 1:
        if(UI.autoRead)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)                                                        //播放数字状态
                {
                    Voice_play(YU_YIN_1+UI.slect_index);                               //播放数字
                    if(UI.slect_index ==1)                                             //0-1菜单条目数
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)                                                   //播放菜单状态
                {
                    Voice_play(Voice_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, Voice_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, Voice_menu);
                }
                else if(playStep==2)                                                   //播放结尾状态
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);        //返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;                                                   //停止自动播放
                }
            }
        }
        break;
    case 2:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            memcpy((void *)&UI, &Menu_tab[2], sizeof(MenuNode_t));                      //返回系统设置菜单
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
            Function_status()->ManageMode.Voice = MUTE_OFF;
            ReadOptionRecord(&optionTemp);                                              //先读设置
            optionTemp.Voice = MUTE_OFF;                                                //改
            WriteOptionRecord(&optionTemp);                                             //后写
            Voice_play(SHE_ZHI_CHENG_GONG);                                             //设置成功
            DisplayTips(&TIPS_SZCG);                                                    //语音模式
            UI.step = 2;
            break;
        case TUCH_KEY2:
            Function_status()->ManageMode.Voice = MUTE_ON;
            ReadOptionRecord(&optionTemp);                                              //先读设置
            optionTemp.Voice = MUTE_ON;                                                 //改
            WriteOptionRecord(&optionTemp);                                             //后写 
            Voice_play(SHE_ZHI_CHENG_GONG);                                             //设置成功
            DisplayTips(&TIPS_SZCG);                                                    //静音模式
            UI.step = 2;
            break;     
        case TUCH_KEY8:                                                                 //上翻
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, Voice_menu);
            if(--UI.slect_index < 0)UI.slect_index = 1;
            DisplayItemSelect(true, UI.slect_index, Voice_menu);
            break;
        case TUCH_KEY0:                                                                 //下翻
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, Voice_menu);
            if(++UI.slect_index > 1)UI.slect_index = 0;
            DisplayItemSelect(true, UI.slect_index, Voice_menu);
            break;
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[2], sizeof(MenuNode_t));                      //返回系统设置菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            if(UI.slect_index==0)Function_status()->ManageMode.Voice = MUTE_OFF;
            else Function_status()->ManageMode.Voice = MUTE_ON;
            Voice_play(SHE_ZHI_CHENG_GONG);                                             //设置成功
            DisplayTips(&TIPS_SZCG);
            UI.step = 2;
            break;
        default:
            break;
        }
    }
}
//语言设置
void LanguageSetMenu(void)
{
    OptionRecord_t optionTemp;
    static uint8_t playStep = 0;
    switch(UI.step)
    {
    case 0:                                             
        Voice_play(Language_menu[0].SpeakAddr);                                         //第一次播放 中文请按
        DisplayItem(0, Language_menu, 2);
        UI.step = 1;
        playStep = 0;
        break;
    case 1:
        if(UI.autoRead)
        {
            if(Voice_Status()==VOICE_ST_IDLE)
            {
                if(playStep==0)                                                        //播放数字状态
                {
                    Voice_play(YU_YIN_1+UI.slect_index);                               //播放数字
                    if(UI.slect_index ==1)                                             //0-1菜单条目数
                        playStep = 2;
                    else 
                        playStep = 1;
                }
                else if(playStep==1)                                                   //播放菜单状态
                {
                    Voice_play(Language_menu[UI.slect_index+1].SpeakAddr);
                    playStep = 0; 
                    
                    DisplayItemSelect(false, UI.slect_index, Language_menu);
                    UI.slect_index++;
                    DisplayItemSelect(true, UI.slect_index, Language_menu);
                }
                else if(playStep==2)                                                   //播放结尾状态
                {
                    Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);        //返回上级菜单请按*号键
                    playStep = 0;
                    UI.autoRead = 0;                                                   //停止自动播放
                }
            }
        }
        break;
    case 2:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            memcpy((void *)&UI, &Menu_tab[2], sizeof(MenuNode_t));                      //返回系统设置菜单
        }
        break; 
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY1:
            Function_status()->ManageMode.Language = CHN;
            Voice_play(SHE_ZHI_CHENG_GONG);                                             //设置成功
            DisplayTips(&TIPS_SZCG);                                                    //中文模式
            ReadOptionRecord(&optionTemp);                                              //先读设置
            optionTemp.Language = CHN;                                                  //改
            WriteOptionRecord(&optionTemp);                                             //后写
            UI.step = 2;
            break;
        case TUCH_KEY2:
            Function_status()->ManageMode.Language = EN;
            Voice_play(SHE_ZHI_CHENG_GONG);                                             //设置成功
            DisplayTips(&TIPS_SZCG);                                                    //英文模式
            ReadOptionRecord(&optionTemp);                                              //先读设置
            optionTemp.Language = EN;                                                   //改
            WriteOptionRecord(&optionTemp);                                             //后写 
            UI.step = 2;
            break;    
        case TUCH_KEY8:                                                                 //上翻
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, Language_menu);
            if(--UI.slect_index < 0)UI.slect_index = 1;
            DisplayItemSelect(true, UI.slect_index, Language_menu);
            break;
        case TUCH_KEY0:                                                                 //下翻
            UI.autoRead = 0;
            DisplayItemSelect(false, UI.slect_index, Language_menu);
            if(++UI.slect_index > 1)UI.slect_index = 0;
            DisplayItemSelect(true, UI.slect_index, Language_menu);
            break;
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[2], sizeof(MenuNode_t));                      //返回系统设置菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            if(UI.slect_index ==0)Function_status()->ManageMode.Language = CHN;
            else Function_status()->ManageMode.Language = EN;
            Voice_play(SHE_ZHI_CHENG_GONG);                                             //设置成功
            DisplayTips(&TIPS_SZCG);                                                    //模式
            UI.step = 2;
            break;
        default:
            break;
        }
    }
}
//=====================================================================================
//三级菜单
//=====================================================================================
//报序列号
bool ReadSnNuber(void)
{
    static char SN[sizeof(LockRecord.SysSN)];                                           //防伪码 为空时提示请刷防伪码
    static uint8_t step=0;
    static uint8_t i;
    static char *str;
    
    if(step==0)
    {
        Record_Read((uint32_t)&LockRecord.SysSN,SN,sizeof(LockRecord.SysSN));           //读取防伪码
        str = &SN[0];
        i = 0;
        step = 1;
        Display_clear_screen();                                                         //清屏
    }
    else if(step==1)
    {
        if(Voice_Status() == VOICE_ST_IDLE)
        {
            if(*str!=0)
            {
                Display_char(0*3+i++*8,0,*str,false);                                   //一个个显示
                if(*str==0x30)
                    Voice_play(YU_YIN_0);                                               //播放数字0
                else if((*str>0x30) && (*str<0x3A))
                    Voice_play((*str-0x31)+YU_YIN_1);                                   //播放数字1-9
                else if((*str>0x40) && (*str<0x5B))
                    Voice_play((*str-0x41)+YU_YIN_A);                                   //播放数字A-Z
                str++;
            }
            else
             {
                step = 0;
                return true;
            }
        }        
    }
    return false;
}
//系统查询 防伪码
void SysRecordMenu(void)
{
    static char SN[sizeof(LockRecord.SysSN)];                                           //防伪码
    static char *str;
    static uint8_t i;
    switch(UI.step)
    {
    case 0:
        Display_clear_screen();
        Record_Read((uint32_t)&LockRecord.SysSN,SN,sizeof(LockRecord.SysSN));           //读取防伪码
        if(SN[0]==0x00)                                                                 //防伪码为空的，提示请刷防伪码
        {
            UI.step = 3;
        }
        else
        {
            str = &SN[0];
            UI.step = 1;
            i = 0;            
        }
        break;
    case 1:
        if(Voice_Status() == VOICE_ST_IDLE)
        {
            if(*str!=0)
            {
                Display_char(0*3+i++*8,0,*str,false);                                   //一个个显示
                if(*str==0x30)
                    Voice_play(YU_YIN_0);                                               //播放数字0
                else if((*str>0x30) && (*str<0x3A))
                    Voice_play((*str-0x31)+YU_YIN_1);                                   //播放数字1-9
                else if((*str>0x40) && (*str<0x5B))
                    Voice_play((*str-0x41)+YU_YIN_A);                                   //播放数字A-Z
                str++;
            }
            else 
            {
                Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);             //返回上级菜单请按*号键
                UI.step = 2;
            }
        }
        break;
    case 2:                                                                             //等待退出
        if(Voice_Status() == VOICE_ST_IDLE)
        {
            memcpy((void *)&UI, &Menu_tab[3], sizeof(MenuNode_t));                      //返回记录查询菜单
        }
        break;
    case 3:                                                                             //请刷防伪码
        if(Voice_Status() == VOICE_ST_IDLE)
        {
            DisplayTips(&TIPS_QSFWM); 
            Voice_play(QING_SHUA_FANG_WEI_MA);                                          //请刷防伪码
            UI.step = 2;                                                                //跳到等待状态
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[3], sizeof(MenuNode_t));                      //返回记录查询菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            break;
        default:
            break;
        }
    }
}

//index为显示选择指引 Item为记录 Y为坐标0或2 select为是否选中
void DisplayRecordItem(uint8_t index,DoordRecord_t *Item,uint8_t Y,bool select)                    
{
    char str[30];
    //----------------------------------------------------------------------------------
    if(Function_status()->ManageMode.Language==CHN)                                     //中文显示
    {
        if(Item->Class==C_PSW)
            Display_chinese_str(0, Y, TIPS_MM.TipsChinese, select);                     //密码
        else if(Item->Class==C_FIG)
            Display_chinese_str(0, Y, TIPS_ZW.TipsChinese, select);                     //指纹
        else if(Item->Class==C_CARD)
            Display_chinese_str(0, Y, TIPS_KP.TipsChinese, select);                     //卡片
    }
    else                                                                                //英文显示
    {
        if(Item->Class==C_PSW)
            Display_english_str(0, Y,"PIN ", select);                                   //密码
        else if(Item->Class==C_FIG)
            Display_english_str(0, Y,"FP  ", select);                                   //指纹
        else if(Item->Class==C_CARD)
            Display_english_str(0, Y,"Card", select);                                   //卡片
     } 
    sprintf(str, ":%.2d         ",(Item->ID&0x7F)); 
    Display_english_str(8*4, Y, str, select);                                           //显示ID 加空格用于反显
    sprintf(str, "%.3d %.2d-%.2d %.2d:%.2d ",index,Item->Time.month,Item->Time.day,Item->Time.hour,Item->Time.minute);
    Display_english_str(0, Y+1, str, select);                                           //显示序号 开门时间
    //----------------------------------------------------------------------------------  
}
//顺序查询
void OrderRecordMenu(void)
{
    DoordRecord_t doorTemp;
    static bool view=false;
    static bool upDown;                                                                         //上下标记
    static uint8_t totalItem=0;                                                                 //总记录数
    static int16_t selectIndex=0;                                                               //选中的指引
    uint16_t i;
    
    Rtc_t rtcTemp;
    switch(UI.step)
    {
    case 0:
        //------------------------------------------------------------------------------------
        //密码:00(ID)
        //002 06-01 20:09
        //指纹:00(ID)
        //003 06-01 14:53      
//        doorTemp.ID = 0x80;
//        doorTemp.Class = C_CARD;
//        Rtc_Get((Rtc_t *)&doorTemp.Time);
//        Record_Write((uint32_t)&LockRecord.DoodrRecord[0],&doorTemp,sizeof(DoordRecord_t));    //写一个测试
//        
//        doorTemp.ID = 0x81;
//        doorTemp.Class = C_FIG;
//        Rtc_Get((Rtc_t *)&doorTemp.Time);
//        Record_Write((uint32_t)&LockRecord.DoodrRecord[1],&doorTemp,sizeof(DoordRecord_t));    //写一个测试 
//        
//        doorTemp.ID = 0x82;
//        doorTemp.Class = C_PSW;
//        Rtc_Get((Rtc_t *)&doorTemp.Time);
//        Record_Write((uint32_t)&LockRecord.DoodrRecord[2],&doorTemp,sizeof(DoordRecord_t));    //写一个测试 
//        
//        doorTemp.ID = 0x83;
//        doorTemp.Class = C_PSW;
//        Rtc_Get((Rtc_t *)&doorTemp.Time);
//        Record_Write((uint32_t)&LockRecord.DoodrRecord[3],&doorTemp,sizeof(DoordRecord_t));    //写一个测试 
//        
//        
//        doorTemp.ID = 0x80;
//        doorTemp.Class = C_FIG;
//        Rtc_Get((Rtc_t *)&doorTemp.Time);
//        Record_Write((uint32_t)&LockRecord.DoodrRecord[4],&doorTemp,sizeof(DoordRecord_t));    //写一个测试 
//        
//        
//        doorTemp.ID = 0x82;
//        doorTemp.Class = C_PSW;
//        Rtc_Get((Rtc_t *)&doorTemp.Time);
//        Record_Write((uint32_t)&LockRecord.DoodrRecord[5],&doorTemp,sizeof(DoordRecord_t));    //写一个测试  
//        
//        doorTemp.ID = 0x80;
//        doorTemp.Class = C_PSW;
//        Rtc_Get((Rtc_t *)&doorTemp.Time);
//        Record_Write((uint32_t)&LockRecord.DoodrRecord[6],&doorTemp,sizeof(DoordRecord_t));    //写一个测试  
        //------------------------------------------------------------------------------------
        totalItem = 0;                                                                          //总数清0
        selectIndex = 0;                                                                        //选中第一个
        for(i=0;i<LEN_DOOR_RECORD;i++)                                                          //查找所有记录                                                 
        {
            Record_Read((uint32_t)&LockRecord.DoodrRecord[i],&doorTemp,sizeof(DoordRecord_t));
            if(doorTemp.Class!=0)totalItem +=1;                                                 //总数 
        }
        if(totalItem ==0)
            DisplayTips(&TIPS_KMJLWK);                                                          //开门记录为空   
        else if(totalItem==1)                                                                   //只有1条
        { 
            Display_clear_screen();                                                             //只有1行时要清屏
            Record_Read((uint32_t)&LockRecord.DoodrRecord[0],&doorTemp,sizeof(DoordRecord_t));
            DisplayRecordItem(selectIndex,&doorTemp,0,true);                                    
        }
        else                                                                                    //2条以上
        {            
            Record_Read((uint32_t)&LockRecord.DoodrRecord[0],&doorTemp,sizeof(DoordRecord_t));
            DisplayRecordItem(selectIndex,&doorTemp,0,true);                                    //选择第1条
            Record_Read((uint32_t)&LockRecord.DoodrRecord[1],&doorTemp,sizeof(DoordRecord_t));
            DisplayRecordItem(selectIndex+1,&doorTemp,2,false);                  
        }
        UI.step = 1;
        break;
    case 1:
        if(Voice_Status() == VOICE_ST_IDLE)
        {
            Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);                         //返回上级菜单请按*号键
            UI.step = 2;
        }
        break;
    case 2:                                                                                     //更新显示状态
        if(view)                                                                    
        {
            view = false;                                                                                                                                       
            if(totalItem > 1)                                                                    //只有1条 按上下键不动作  2条以上 才动作
            {
                if(upDown)                                                                       //上翻
                {
                    if(--selectIndex<0)                                                         
                    {
                        selectIndex = (totalItem-1);
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[selectIndex-1],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(selectIndex-1,&doorTemp,0,false);                              
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[selectIndex],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(selectIndex,&doorTemp,2,true); 
                    }
                    else
                    {
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[selectIndex],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(selectIndex,&doorTemp,0,true);                              
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[selectIndex+1],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(selectIndex+1,&doorTemp,2,false); 
                    }
                }
                else                                                                             //下翻
                {
                    if(++selectIndex > (totalItem-1))                                       
                    {
                        selectIndex=0;   
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[selectIndex],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(selectIndex,&doorTemp,0,true);                              
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[selectIndex+1],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(selectIndex+1,&doorTemp,2,false); 
                    }
                    else                                                                    
                    {
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[selectIndex-1],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(selectIndex-1,&doorTemp,0,false);                              
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[selectIndex],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(selectIndex,&doorTemp,2,true); 
                    }             
                }
            }
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY8:                                                                 //上翻
            UI.step = 2;                                                                //跳到更新显示状态
            upDown = true;                                                              //Up
            view = true;
            break;
        case TUCH_KEY0:                                                                 //下翻
            UI.step = 2;                                                                //跳到更新显示状态
            upDown = false;                                                             //Down
            view = true;
            break;  
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[3], sizeof(MenuNode_t));                      //返回记录查询菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            break;
        default:
            break;
        }
    }
}
//编号查询
void NumRecordMenu(void)
{
    static uint8_t BianHao[2];                                                          //编号
    char str[30];
    DoordRecord_t doorTemp;                                                             //记录内容
    uint8_t i;
    static uint8_t RecordIDindex[LEN_DOOR_RECORD];                                      //找出指引 最多200条都是同一ID的
    static uint8_t totalItem = 0;                                                       //相同ID记录的总数
    static int16_t selectIndex = 0;
    static bool view=false;
    static bool upDown;     
    switch(UI.step)
    {
    case 0:
        Voice_play(TIPS_QSRLWBHYJHJJS.SpeakAddr);                                       //请输入两位编号以#号键结束
        DisplayTips(&TIPS_QSRLWBHYJHJJS);
        UI.step = 1;
        memset(BianHao,0,2);                                                            //清
        memset(RecordIDindex,0,LEN_DOOR_RECORD);                                        //
        totalItem = 0;
        selectIndex = 0;
        break;
    case 1:
        if(Voice_Status() == VOICE_ST_IDLE)
        {
            Voice_play(FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN);                 //返回上级菜单请按*号键
            UI.step = 2;
        }
        break;
    case 2:                                                                             //输入编号状态
        if(Voice_Status() == VOICE_ST_IDLE)
        {
            
        }
        break;
    case 3:                                                                             //显示记录为空状态
        if(++UI.Tcount > (1000 / TIME_BASIC_MS))
        {
            UI.Tcount = 0;
            UI.step = 0;
        }
        break;
    case 4:                                                                             //显示记录状态
        if(view)
        {
            view=false;
            if(totalItem>1)                                                             //两条以上才有上下键
            {
                if(upDown)                                                              //上翻
                {
                    if(--selectIndex<0)                                                         
                    {
                        selectIndex = (totalItem-1);
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[selectIndex-1]],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(RecordIDindex[selectIndex-1],&doorTemp,0,false);                              
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[selectIndex]],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(RecordIDindex[selectIndex],&doorTemp,2,true); 
                    }
                    else
                    {
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[selectIndex]],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(RecordIDindex[selectIndex],&doorTemp,0,true);                              
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[selectIndex+1]],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(RecordIDindex[selectIndex+1],&doorTemp,2,false); 
                    }
                }
                else                                                                    //下翻
                {
                    if(++selectIndex > (totalItem-1))                                       
                    {
                        selectIndex=0;   
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[selectIndex]],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(RecordIDindex[selectIndex],&doorTemp,0,true);                              
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[selectIndex+1]],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(RecordIDindex[selectIndex+1],&doorTemp,2,false); 
                    }
                    else                                                                    
                    {
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[selectIndex-1]],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(RecordIDindex[selectIndex-1],&doorTemp,0,false);                              
                        Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[selectIndex]],&doorTemp,sizeof(DoordRecord_t));
                        DisplayRecordItem(RecordIDindex[selectIndex],&doorTemp,2,true); 
                    } 
                }
            }
        }
        break;
    case 5:
        if(Voice_Status()==VOICE_ST_IDLE)                                               //输入错误
        {
            UI.step = 0;
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY0:    
        case TUCH_KEY1:
        case TUCH_KEY2:
        case TUCH_KEY3:
        case TUCH_KEY4:
        case TUCH_KEY5:
        case TUCH_KEY6:
        case TUCH_KEY7:
        case TUCH_KEY8:
        case TUCH_KEY9:
            if(UI.step==1 || UI.step==2)
            {
                sprintf(str,"%d",Tuch_status()->val);
                if((BianHao[0]&0x80)== 0x00) 
                {
                    BianHao[0] = 0x80+Tuch_status()->val;
                    Display_english_str(56, 2,str, false);
                }
                else if((BianHao[1]&0x80)== 0x00)
                {
                    BianHao[1] = 0x80+Tuch_status()->val;
                    Display_english_str(64, 2,str, false);
                }
            }  
            else if(UI.step==4)                                                         //在显示记录状态下按上下键
            {
                if(Tuch_status()->val==TUCH_KEY0)                                       //下键
                {
                    upDown = false;
                    view  = true;  
                }
                else if(Tuch_status()->val==TUCH_KEY8)                                  //上键
                {
                    upDown = true;
                    view = true;
                }  
            }
            break;  
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[3], sizeof(MenuNode_t));                      //返回记录查询菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            if(((BianHao[0])!=0)&& (BianHao[1]!=0))                                     //编号是否正确 10位必须是个位必须有数
            {
                totalItem = 0;
                memset(RecordIDindex,0,LEN_DOOR_RECORD);
                for(i=0;i<LEN_DOOR_RECORD;i++)
                {
                   Record_Read((uint32_t)&LockRecord.DoodrRecord[i],&doorTemp,sizeof(DoordRecord_t)); 
                   if(doorTemp.ID == ((((BianHao[0]-0x80)*10)+(BianHao[1]-0x80))|0x80)  ) //找出所有ID相同的记录指引
                   {
                        RecordIDindex[totalItem++] = i;                                  //记下指引
                   }
                }
                if(totalItem==1)
                {
                    Display_clear_screen();                                             //先清屏
                    Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[0]],&doorTemp,sizeof(DoordRecord_t)); 
                    DisplayRecordItem(RecordIDindex[0],&doorTemp,0,true);               //显示第一行  
                    UI.step = 4;                                                        //跳到显示记录状态
                }
                else if(totalItem>1)
                {
                    Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[0]],&doorTemp,sizeof(DoordRecord_t)); 
                    DisplayRecordItem(RecordIDindex[0],&doorTemp,0,true);               //显示第一行
                    Record_Read((uint32_t)&LockRecord.DoodrRecord[RecordIDindex[1]],&doorTemp,sizeof(DoordRecord_t)); 
                    DisplayRecordItem(RecordIDindex[1],&doorTemp,2,false);              //显示第二行 
                    UI.step = 4;                                                        //跳到显示记录状态
                    selectIndex = 0;                                                         
                }
                else                                                                    //没有找到记录
                {
                    DisplayTips(&TIPS_KMJLWK);                                          //开门记录为空
                    UI.step = 3;                                                        //返回   
                }
            }
            else
            {
                Voice_play(SHU_RU_CUO_WU);                                              //输入错误 
                DisplayTips(&TIPS_SRCW);                                                //输入错误 
                UI.step = 5;                                                            //返回               
            }   
            break;                                                  
        default:
            break;
        }
    }
}
//=====================================================================================
//三级菜单
//=====================================================================================
void JinNetMenu(void)
{
    switch(UI.step)
    {
    case 0:
        DisplayTips(&TIPS_SZCG);                                                        //配置成功
        Voice_play(PEI_WANG_ZHONG_QING_SHAO_HOU);                                       //配网中请稍后
        UI.step = 1;
        break;
    case 1:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            Voice_play(YI_RU_WANG);                                                     //已入网
            UI.step = 2;
        }
        break;
    case 2:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            memcpy((void *)&UI, &Menu_tab[4], sizeof(MenuNode_t));                     //返回扩展功能菜单
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[4], sizeof(MenuNode_t));                      //返回扩展功能菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            break;
        default:
            break;
        }
    }
}

void QuitNetMenu(void)
{
    switch(UI.step)
    {
    case 0:
        DisplayTips(&TIPS_SZCG);                                                        //配置成功
        Voice_play(TUI_WANG_ZHONG_QING_SHAO_HOU);                                       //退网中请稍后
        UI.step = 1;
        break;
    case 1:
        if(Voice_Status()==VOICE_ST_IDLE)
        {
            memcpy((void *)&UI, &Menu_tab[4], sizeof(MenuNode_t));                      //返回扩展功能菜单
        }
        break;
    default:
        break;
    }
    if(Tuch_status()->even)
    {
        switch(Tuch_status()->val)
        {
        case TUCH_KEY_ESC:                                                              //*返回
            memcpy((void *)&UI, &Menu_tab[4], sizeof(MenuNode_t));                      //返回扩展功能菜单
            break;
        case TUCH_KEY_ENTER:                                                            //#确认
            break;
        default:
            break;
        }
    }
}
//=====================================================================================
//三级菜单
//=====================================================================================


