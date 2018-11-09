/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef _VOICE_H_
#define _VOICE_H_
    
#include "main.h"
    
#define HAUN_YING_YIN                                           1//欢迎音
#define AN_JIAN_YIN_DI                                          2//按键音Di
#define ZHI_WEN_YIN_DO                                          3//指纹音Do
#define BAO_JING_YIN                                            4//报警音
#define YU_YIN_A                                                5//A
#define YU_YIN_B                                                6//B
#define YU_YIN_C                                                7//C
#define YU_YIN_D                                                8//D
#define YU_YIN_E                                                9//E
#define YU_YIN_F                                                10//F
#define YU_YIN_G                                                11//G
#define YU_YIN_H                                                12//H
#define YU_YIN_I                                                13//I
#define YU_YIN_J                                                14//J
#define YU_YIN_K                                                15//K
#define YU_YIN_L                                                16//L
#define YU_YIN_M                                                17//M
#define YU_YIN_N                                                18//N
#define YU_YIN_O                                                19//O
#define YU_YIN_P                                                20//P
#define YU_YIN_Q                                                21//Q
#define YU_YIN_R                                                22//R 
#define YU_YIN_S                                                23//S
#define YU_YIN_T                                                24//T
#define YU_YIN_U                                                25//U
#define YU_YIN_V                                                26//V
#define YU_YIN_W                                                27//W
#define YU_YIN_X                                                28//X
#define YU_YIN_Y                                                29//Y
#define YU_YIN_Z                                                30//Z
#define YU_YIN_1                                                31//一
#define YU_YIN_2                                                32//二
#define YU_YIN_3                                                33//三
#define YU_YIN_4                                                34//四
#define YU_YIN_5                                                35//五
#define YU_YIN_6                                                36//六
#define YU_YIN_7                                                37//七
#define YU_YIN_8                                                38//八
#define YU_YIN_9                                                39//九
#define YU_YIN_0                                                40//零
#define YAN_ZHENG_SHI_BAI                                       41//验证失败
#define YI_TUI_CHU_GUAN_LI_MO_SHI                               42//已退出管理模式
#define QING_SHU_RU_YONG_HU_MI_MA_YI_JING_HAO_JIAN_JIE_SHU      43//请输入用户密码以#号键结束
#define QING_SHU_RU_LIANG_WEI_BIAN_HAO_YI_JING_HAO_JIAN_JIE_SHU 44//请输入两位编号以#号键结束
#define SHU_RU_CUO_WU                                           45//输入错误
#define MI_MA_GUO_YU_JIAN_DAN                                   46//密码过于简单
#define SHE_ZHI_SHI_BAI                                         47//设置失败
#define QING_CHONG_XIN_SHU_RU                                   48//请重新输入
#define QING_NA_KAI_SHOU_ZHI_ZAI_FANG_YI_CI                     49//请拿开手指，再放一次
#define TIAN_JIA_SHI_BAI                                        50//添加失败
#define TIAN_JIA_CHENG_GONG                                     51//添加成功
#define FAN_HUI_SHANG_JI_CAI_DAN_QING_AN_XING_HAO_JIAN          52//返回上级菜单请按*号键
#define SHAN_CHU_SHI_BAI                                        53//删除失败
#define SHAN_CHU_CHENG_GONG                                     54//删除成功
#define ZHI_WEN_KU_WEI_KONG                                     55//指纹库为空
#define BIAN_HAO_BU_CUN_ZAI                                     56//编号不存在
#define QING_JI_SHI_XIU_GAI_GUAN_LI_MI_MA                       57//请及时修改管理密码
#define YI_JIN_RU_GUAN_LI_MO_SHI                                58//已进入管理模式
#define YONG_HU_SHE_ZHI_QING_AN                                 59//用户设置请按
#define XI_TONG_SHE_ZHI_QING_AN                                 60//系统设置请按
#define JI_LU_CHA_XUN_QING_AN                                   61//记录查询请按
#define KUO_ZHAN_GONG_NENG_QING_AN                              62//扩展功能请按
#define TIAN_JIA_YONG_HU_MI_MA_QING_AN                          63//添加用户密码请按
#define TIAN_JIA_YONG_HU_ZHI_WEN_QING_AN                        64//添加用户指纹请按
#define TIAN_JIA_YONG_HU_KA_PIAN_QING_AN                        65//添加用户卡片请按
#define SHAN_CHU_YONG_HU_KA_PIAN_QING_AN                        66//删除用户卡片请按
#define SHAN_CHU_YONG_HU_MI_MA_QING_AN                          67//删除用户密码请按
#define SHAN_CHU_YONG_HU_ZHI_WEN_QING_AN                        68//删除用户指纹请按
#define SHE_ZHI_KAI_MEN_MO_SHI_QING_AN                          69//设置开门模式请按
#define XIU_GAI_GUAN_LI_MI_MA_QING_AN                           70//修改管理密码请按
#define SHI_JIAN_SHE_ZHI_QING_AN                                71//时间设置请按
#define RI_QI_SHE_ZHI_QING_AN                                   72//日期设置请按
#define XI_TONG_CHA_XUN_QING_AN                                 73//系统查询请按
#define SHUN_XU_CHA_XUN_QING_AN                                 74//顺序查询请按
#define BIAN_HAO_CHA_XUN_QING_AN                                75//编号查询请按
#define WU_XIAN_MO_KUAI_QING_AN                                 76//无线模块请按
#define JIA_RU_WANG_LUO                  					   77//加入网络请按
#define TUI_CHU_WANG_LUO                                      78//退出网络请按
#define QUAN_BU_SHAN_CHU_QING_AN                                79//全部删除请按
#define DAN_GE_SAHN_CHU_QING_AN                                 80//单个删除请按
#define CHANG_YONG_MO_SHI_QING_AN                               81//常用模式请按
#define AN_QUAN_MO_SHI_QING_AN                                  82//安全模式请按
#define YU_YIN_MO_SHI_QING_AN                                   83//语音模式请按
#define JING_YIN_MO_SHI_QING_AN                                 84//静音模式请按
#define YU_YIN_SHE_ZHI_QING_AN                                  85//语音设置请按
#define YU_YAN_SHE_ZHI_QING_AN                                  86//语言设置请按
#define ZHONG_WEN_QING_AN                                       87//中文请按
#define YING_WEN_QING_AN                                        88//英文请按
#define YAN_ZHENG_CHENG_GONG_QING_KAI_MEN                        89//验证成功,请开门
#define DIAN_LIANG_DI_QING_GENG_HUAN_DIAN_CHI                   90//电量低请更换电池
#define BIAN_HAO_YI_CUN_ZAI                                     91//编号已存在
#define ZHI_WEN_YI_CUN_ZAI                                      92//指纹已存在
#define SHE_ZHI_CHENG_GONG                                      93//设置成功
#define XI_TONG_YI_SUO_DING_QING_SHAO_HOU_ZAI_SHI               94//系统已锁定，请稍候再试
#define QING_SHU_RU_BIAN_HAO_YI_JING_HAO_JIAN_JIE_SHU           95//请输入编号以#号键结束
#define QING_ZAI_SHU_RU_YI_CI                                   96//请再输入一次
#define QING_AN_ZHI_WEN                                         97//请按指纹
#define WEI_TIAN_JIA_ZHI_WEN                                    98//未添加指纹
#define WEI_SHE_ZHI_MI_MA                                       99//未设置密码
#define QING_TIAN_JIA_GUAN_LI_MI_MA_YI_JING_HAO_JIAN_JIE_SHU    100//请添加管理密码以#号键结束
#define WEI_TIAN_JIA_YONG_HU_KA                                 101//未添加用户卡
#define QING_SHUA_FANG_WEI_MA                                   102//请刷防伪码
#define SHAN_CHU_SHI_BAI_WEI_AN_QUAN_MO_SHI                     103//删除失败，为安全模式
#define YI_HUI_FU_DAO_CHU_CHANG_SHE_ZHI                         104//已恢复到出厂设置
#define ZHNEG_ZAI_HUI_FU_QING_SHAO_HOU                          105//正在恢复，请稍后
#define QING_XIU_GAI_GUAN_LI_MI_MA                              106//请修改管理密码
#define QING_AN_YAO_KONG_QI                                     107//请按遥控器
#define QING_SHUA_KA                                            108//请刷卡
#define QING_SHU_RU_ZHI_WEN_HUO_SHU_RU_LIANG_WEI_BAIN_HAO       109//请输入指纹或输入两位编号，以#号键结束
#define QING_SHUA_KA_HUO_SHU_RU_LIANG_WEI_BAIN_HAO              110//请刷卡或输入两位编号，以#号键结束
#define KA_PIAN_KU_WEI_KONG                                     111//卡片库为空
#define KA_PIAN_KU_YI_MAN                                       112//卡片库已满
#define KA_PIAN_YI_CUN_ZAI                                      113//卡片已存在
#define ZHI_WEN_KU_YI_MAN                                       114//指纹库已满
#define MI_MA_KU_YI_MAN                                         115//密码库已满
#define MI_MA_KU_WEI_KONG                                       116//密码库为空
#define FEI_ZHENG_CHANG_KAI_SUO                                 117//非正常开锁
#define QING_SHUA_KA_HUO_AN_ZHI_WEN                             118//请刷卡或按指纹
#define QING_SHU_RU_MI_MA_HUO_KA_PIAN                           119//请输入密码或卡片
#define QING_SHU_RU_MI_MA_HUO_ZHI_WEN                           120//请输入密码或指纹

#define YI_KAI_MEN                                              126//已开门
#define YI_GUAN_MEN                                             127//已关门
#define YI_QI_DONG_BU_FANG                                      128//已启动布防
#define YI_FAN_SUO                                              129//已反锁
#define MEN_WEI_SHANG_SUO                                       130//门未上锁
#define PEI_WANG_ZHONG_QING_SHAO_HOU                            131//配网中请稍后
#define TUI_WANG_ZHONG_QING_SHAO_HOU                            132//退网中请稍后
#define YU_YIN_MO_SHI                                           133//语音模式
#define JING_YIN_MO_SHI                                         134//静音模式
#define GAO_YIN_LIANG_MO_SHI                                    135//高音量模式
#define DI_YIN_LIANG_MO_SHI                                     136//低音量模式
#define SUO_TI_YI_CHANG                                         137//锁体异常
#define YI_RU_WANG                                              138//已入网
#define HUAN_YING_HUI_JIA                                       139//欢迎回家
#define ZI_DONG_MO_SHI                                          143//自动模式
#define SHOU_DONG_MO_SHI                                        144//手动模式

#define KAI_QI_LAN_YA_QING_AN                                   145//开启蓝牙请按
#define GUAN_BI_LAN_YA_QING_AN                                  146//关闭蓝牙请按
#define LIAN_JIE_LAN_YA_QING_AN                                 147//连接蓝牙请按
#define DUAN_KAI_LAN_YA_QING_AN                                 148//断开蓝牙请按
#define LAN_YA_YI_KAI_QI                                        149//蓝牙已开启
#define LAN_YA_YI_GUAN_BI                                       150//蓝牙已关闭
#define LAN_YA_YI_LIAN_JIE                                      151//蓝牙已连接
#define LAN_YA_SHE_ZHI_QING_AN                                  152//蓝牙设置请按

#define QING_XIANG_ZUO_WEI_WEI_YI_DONG_SHOU_ZHI               	153//请向左微微移动手指
#define QING_XIANG_YOU_WEI_WEI_YI_DONG_SHOU_ZHI            		154//请向右微微移动手指
#define QING_XIANG_SHANG_WEI_WEI_YI_DONG_SHOU_ZHI         		155//请向上微微移动手指
#define QING_XIANG_XAI_WEI_WEI_YI_DONG_SHOU_ZHI         		156//请向下微微移动手指

#define QING_SHU_RU_GUAN_LI_MI_MA_YI_JING_HAO_JINA_JIE_SHU      157//请输入管理密码后，按井号键
#define YI_JING_HAO_JINA_JIE_SHU         		                158//已#号建结束

#define  SHU_RU_CHENG_GONG        								159//输入成功
#define QING_SHU_RU_GUAN_LI_MA_MA       						160//请输入管理密码

#define QING_SHU_RU_6_12WEI_XIN_MI_MA       					161//请输入6-12位新密码  0806
#define TIAN_JIA_YAO_KONG_QING_AN                               162//添加遥控请按
#define SHAN_CHU_YAO_KONG_QING_AN                               163//删除遥控请按
#define MI_MA_YI_CUN_ZAI       				        			164//密码已存在 0806
#define LIANG_CI_SHU_RU_DE_MA_MA_BU_YI_ZHI 						165//两次输入的密码不一致
    
#define VOICE_ST_BUSY                                              -1
#define VOICE_ST_IDLE                                              0
   
extern int8_t Voice_init(void);
extern void Voice_play(uint16_t val);
extern int8_t Voice_Status(void);

#endif    
/* [] END OF FILE */
