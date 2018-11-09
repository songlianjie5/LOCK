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
#include "main.h"

const item_t main_menu[]={
    "1.", "用户设置　　　", "User settings ", YONG_HU_SHE_ZHI_QING_AN,
    "2.", "系统设置　　　", "System setting", XI_TONG_SHE_ZHI_QING_AN,
    "3.", "记录查询　　　", "Records query ", JI_LU_CHA_XUN_QING_AN,
    "4.", "扩展功能　　　", "Extended func ", KUO_ZHAN_GONG_NENG_QING_AN,
};
const item_t user_menu[]= {
    "1.", "添加用户密码　", "Add user PIN  ", TIAN_JIA_YONG_HU_MI_MA_QING_AN,
    "2.", "添加用户指纹　", "Add user FP   ", TIAN_JIA_YONG_HU_ZHI_WEN_QING_AN,
    "3.", "添加用户卡片　", "Add user card ", TIAN_JIA_YONG_HU_KA_PIAN_QING_AN,
    "4.", "删除用户密码　", "Del user PIN  ", SHAN_CHU_YONG_HU_MI_MA_QING_AN,
    "5.", "删除用户指纹　", "Del User FP   ", SHAN_CHU_YONG_HU_ZHI_WEN_QING_AN,
    "6.", "删除用户卡片　", "Del user card ", SHAN_CHU_YONG_HU_KA_PIAN_QING_AN,
    "7.", "设置开门模式　", "Set Open Mode ", SHE_ZHI_KAI_MEN_MO_SHI_QING_AN,
    "9.", "修改管理密码　", "Mod master PIN", XIU_GAI_GUAN_LI_MI_MA_QING_AN,
};
const item_t sys_menu[]={
    "1.", "时间设置　　　", "Time settings ", SHI_JIAN_SHE_ZHI_QING_AN,
    "2.", "日期设置　　　", "Date settings ", RI_QI_SHE_ZHI_QING_AN,
    "3.", "语音设置　　　", "Voice Settings", YU_YIN_SHE_ZHI_QING_AN,
    "4.", "语言设置　　　", "Language Set  ", YU_YAN_SHE_ZHI_QING_AN,
};
const item_t record_menu[]={
    "1.", "系统查询　　　", "System query  ", XI_TONG_CHA_XUN_QING_AN,
    "2.", "顺序查询　　　", "By order      ", SHUN_XU_CHA_XUN_QING_AN,
    "3.", "编号查询　　　", "By number     ", BIAN_HAO_CHA_XUN_QING_AN,
};
const item_t ext_menu[]={
    "1.", "加入网络　　　", "Add network ", JIA_RU_WANG_LUO,
    "2.", "退出网络　　　", "Del network ", TUI_CHU_WANG_LUO,
};

const item_t Del_menu[]={
    "1.", "单个删除　　　", "Del single    ", DAN_GE_SAHN_CHU_QING_AN,
    "2.", "全部删除　　　", "Del all       ", QUAN_BU_SHAN_CHU_QING_AN,
};
const item_t Mode_menu[]={
    "1.", "常用模式　　　", "Normal Mode   ", CHANG_YONG_MO_SHI_QING_AN,
    "2.", "安全模式　　　", "Safe Mode     ", AN_QUAN_MO_SHI_QING_AN,
};
const item_t Voice_menu[]={
    "1.", "语音模式　　　", "Vioce Mode on ", YU_YIN_MO_SHI_QING_AN,
    "2.", "静音模式　　　", "Vioce Mode off", JING_YIN_MO_SHI_QING_AN,
};
const item_t Language_menu[]={
    "1.", "中文模式　　　", "Chinese       ", ZHONG_WEN_QING_AN,
    "2.", "英文模式　　　", "English       ", YING_WEN_QING_AN,
};

//验证失败
const tips_t TIPS_YZSB={"验证失败","Verify Failed",YAN_ZHENG_SHI_BAI};

//已退出管理模式
const tips_t TIPS_YTCGLMS={"已退出管理模式","Exit Master Mode",YI_TUI_CHU_GUAN_LI_MO_SHI};

//请输入用户密码以#号键结束
const tips_t TIPS_QSRYHMMYJHJJS={"请输入密码","Input PIN code",QING_SHU_RU_YONG_HU_MI_MA_YI_JING_HAO_JIAN_JIE_SHU};

//请输入两位编号以#号键结束
const tips_t TIPS_QSRLWBHYJHJJS={"请输入编号","Input number",QING_SHU_RU_LIANG_WEI_BIAN_HAO_YI_JING_HAO_JIAN_JIE_SHU};

//输入错误
const tips_t TIPS_SRCW={"输入错误","Input error",SHU_RU_CUO_WU};

//密码过于简单
const tips_t TIPS_MMGYJD={"密码过于简单","PIN too simple",MI_MA_GUO_YU_JIAN_DAN};

//设置失败
const tips_t TIPS_SZSB={"设置失败","Setting failed",SHE_ZHI_SHI_BAI};

//设置成功
const tips_t TIPS_SZCG={"设置成功","Setting succeed",SHE_ZHI_CHENG_GONG};

//请重新输入
const tips_t TIPS_QCXSR={"请重新输入","Input again",QING_CHONG_XIN_SHU_RU};

//请拿开手指，再放一次
const tips_t TIPS_QNKSZZFYC={"再放一次","Scan again",QING_NA_KAI_SHOU_ZHI_ZAI_FANG_YI_CI};

//请向左微微移动手指
const tips_t TIPS_QXZWWYDSZ={"向左微移手指","FP moves left",QING_XIANG_ZUO_WEI_WEI_YI_DONG_SHOU_ZHI};

//请向右微微移动手指
const tips_t TIPS_QXYWWYDSZ={"向右微移手指","FP moves right",QING_XIANG_YOU_WEI_WEI_YI_DONG_SHOU_ZHI};

//请向上微微移动手指
const tips_t TIPS_QXSWWYDSZ={"向上微移手指","FP moves up",QING_XIANG_SHANG_WEI_WEI_YI_DONG_SHOU_ZHI};

//请向下微微移动手指
const tips_t TIPS_QXXWWYDSZ={"向下微移手指","FP moves down",QING_XIANG_XAI_WEI_WEI_YI_DONG_SHOU_ZHI};

//添加失败
const tips_t TIPS_TJSB={"添加失败","Add failed",TIAN_JIA_SHI_BAI};

//添加成功
const tips_t TIPS_TJCG={"添加成功","Add succeed",TIAN_JIA_CHENG_GONG};

//删除失败
const tips_t TIPS_SCSB={"删除失败","Del failed",SHAN_CHU_SHI_BAI};

//删除成功
const tips_t TIPS_SCCG={"删除成功","Del success",SHAN_CHU_CHENG_GONG};

//指纹库为空
const tips_t TIPS_ZWKWK={"指纹库为空","FP lib empty",ZHI_WEN_KU_WEI_KONG};

//编号不存在
const tips_t TIPS_BHBCZ={"编号不存在","Num not exist",BIAN_HAO_BU_CUN_ZAI};

//卡片不存在
const tips_t TIPS_KPBCZ={"卡片不存在","Card not exist",0};

//已进入管理模式
const tips_t TIPS_YJRGLMS={"已进入管理模式","Master Mode",YI_JIN_RU_GUAN_LI_MO_SHI};

//验证成功
const tips_t TIPS_YZCG={"验证成功","Verify succeed",YAN_ZHENG_CHENG_GONG_QING_KAI_MEN};

//电量低请更换电池
const tips_t TIPS_DLDQGHDC={"电量低请更换电池","Low power",DIAN_LIANG_DI_QING_GENG_HUAN_DIAN_CHI};

//编号已存在
const tips_t TIPS_BHYCZ={"编号已存在","Num exist",BIAN_HAO_YI_CUN_ZAI};

//指纹已存在
const tips_t TIPS_ZWYCZ={"指纹已存在","FP exist",ZHI_WEN_YI_CUN_ZAI};

//请输入手指
const tips_t TIPS_QSRSZ={"请输入手指","Input Fp",QING_AN_ZHI_WEN};

//高音量模式
const tips_t TIPS_GYMS={"高音量模式","High volume",GAO_YIN_LIANG_MO_SHI};

//低音量模式
const tips_t TIPS_DYMS={"低音量模式","Low volume",DI_YIN_LIANG_MO_SHI};

//静音模式
const tips_t TIPS_JYMS={"静音模式","Voice guide off",JING_YIN_MO_SHI};

//系统已锁定，请稍候再试
const tips_t TIPS_XTYSD={"系统已锁定","System locked",XI_TONG_YI_SUO_DING_QING_SHAO_HOU_ZAI_SHI};

//输入错误报警
const tips_t TIPS_SRCWBJ={"输入错误报警","In Put Error",0};

//请稍候再试
const tips_t TIPS_QSHZS={"请稍后再试","Try It Later",XI_TONG_YI_SUO_DING_QING_SHAO_HOU_ZAI_SHI};

//密码已存在
const tips_t TIPS_MMYCZ={"密码已存在","Pwd exists",0};

//卡片已存在
const tips_t TIPS_MKYCZ={"卡片已存在","Card exists",KA_PIAN_YI_CUN_ZAI};

//密码错误
const tips_t TIPS_MMCW={"密码错误","Wrong PIN code",0};

//密码无效
const tips_t TIPS_MMWX={"密码无效","Invalid PIN code",0};

//请再输入一次
const tips_t TIPS_QZSRYC={"请再输入一次","Enter Again",QING_ZAI_SHU_RU_YI_CI};

//请按指纹
const tips_t TIPS_QAZW={"请按指纹","Input FP",QING_AN_ZHI_WEN};

//请刷卡或按指纹
const tips_t TIPS_QSRZWHKP={"请刷卡或按指纹","FP or card",QING_SHUA_KA_HUO_AN_ZHI_WEN};

//未添加指纹
const tips_t TIPS_WTJZW={"未添加指纹","None FP",WEI_TIAN_JIA_ZHI_WEN};

//未设置密码
const tips_t TIPS_WSZMM={"未设置密码","None PIN code",WEI_SHE_ZHI_MI_MA};

//未添加用户卡
const tips_t TIPS_WTJYHK={"未添加用户卡","None card",WEI_TIAN_JIA_YONG_HU_KA};

//请添加管理密码以#号键结束
const tips_t TIPS_QTJGLMM={"请添加管理密码","Add Master PIN",QING_TIAN_JIA_GUAN_LI_MI_MA_YI_JING_HAO_JIAN_JIE_SHU};

//请刷防伪码
const tips_t TIPS_QSFWM={"请刷防伪码","Scan Code",QING_SHUA_FANG_WEI_MA};

//请重新刷卡
const tips_t TIPS_QCXSK={"请重新刷卡","Swipe Again",0};

//已恢复到出厂设置
const tips_t TIPS_YHFDCCSZ={"已恢复到出厂设置","Factory reset",YI_HUI_FU_DAO_CHU_CHANG_SHE_ZHI};

//正在恢复，请稍后
const tips_t TIPS_ZZHFQSH={"请稍后","Wait…",ZHNEG_ZAI_HUI_FU_QING_SHAO_HOU};

//请修改管理密码
const tips_t TIPS_QXGGLMM={"请修改管理密码","Modify PIN code",QING_XIU_GAI_GUAN_LI_MI_MA};

//请按遥控器
const tips_t TIPS_QAYKQ={"请按遥控器","Press remote",QING_AN_YAO_KONG_QI};

//请刷卡
const tips_t TIPS_QSK={"请刷卡","Contact card",QING_SHUA_KA};

//请输入指纹或输入两位编号，以#号键结束
const tips_t TIPS_QSRZWHSRLWBHQJHJJS={"输入指纹或编号","PIN or FP",QING_SHU_RU_ZHI_WEN_HUO_SHU_RU_LIANG_WEI_BAIN_HAO};

//请刷卡或输入两位编号，以#号键结束
const tips_t TIPS_QSKHSRLWBHYJHJJS={"请刷卡或输入编号","PIN or card",QING_SHUA_KA_HUO_SHU_RU_LIANG_WEI_BAIN_HAO};

//卡片库为空
const tips_t TIPS_KPKWK={"卡片库为空","Cards empty",KA_PIAN_KU_WEI_KONG};

//卡片库已满
const tips_t TIPS_KPKYM={"卡片库已满","Cards full",KA_PIAN_KU_YI_MAN};

//卡片已存在
const tips_t TIPS_KPYCZ={"卡片已存在","Card exist",KA_PIAN_YI_CUN_ZAI};

//已清空所有卡片
const tips_t TIPS_YQKSYKP={"已清空所有卡片","Empty Card",0};

//指纹库已满
const tips_t TIPS_ZWKYM={"指纹库已满","FP Lib full",ZHI_WEN_KU_YI_MAN};

//密码库已满
const tips_t TIPS_MMKYM={"密码库已满","PIN Lib Full",MI_MA_KU_YI_MAN};

//密码库为空
const tips_t TIPS_MMKWK={"密码库为空","PIN Lib Empty",MI_MA_KU_WEI_KONG};

//非正常开锁
const tips_t TIPS_FZCKS={"非正常开锁","Open error",FEI_ZHENG_CHANG_KAI_SUO};

//防撬报警
const tips_t TIPS_FQBJ={"防撬报警","Tamper Alarm",0};

//门锁异常
const tips_t TIPS_MSYC={"门锁异常","Error",0};

//布防报警
const tips_t TIPS_BFBJ={"布防报警","Defence Alarm",0};

//非正常开锁
const tips_t TIPS_JXYSKM={"非正常开锁","Machine Alarm",FEI_ZHENG_CHANG_KAI_SUO};

//请刷卡或按指纹
const tips_t TIPS_QSKHAZW={"请刷卡或按指纹","Card or Fp",QING_SHUA_KA_HUO_AN_ZHI_WEN};

//请输入密码或卡片
const tips_t TIPS_QSRMMHKP={"请输入密码或卡片","PIN or Card",QING_SHU_RU_MI_MA_HUO_KA_PIAN};

//请输入密码或指纹
const tips_t TIPS_QSRMMHZW={"请输入密码或指纹","PIN or FP",QING_SHU_RU_MI_MA_HUO_ZHI_WEN};

//开门记录为空
const tips_t TIPS_KMJLWK={"开门记录为空","Record Empty",0};

//密码
const tips_t TIPS_MM={"密码","PIN",0};

//指纹
const tips_t TIPS_ZW={"指纹","FP",0};

//卡片
const tips_t TIPS_KP={"卡片","Card",0};

//机械钥匙
const tips_t TIPS_JXYC ={"机械钥匙","Machine",0};

//遥控开门
const tips_t TIPS_YKKM={"遥控开门","Remote",0};

//一键开启
const tips_t TIPS_YJKQ={"一键开启","One Key",0};

//APP开启
const tips_t TIPS_APP={"手机应用开门","APP",0};

//未知类型
const tips_t TIPS_WZLX={"未知类型","Unknown",0};

//请稍后
const tips_t TIPS_QSH={"请稍后","Wait...",0};

//删除失败，为安全模式
const tips_t TIPS_WAQMS={"为安全模式","Safe Mode",SHAN_CHU_SHI_BAI_WEI_AN_QUAN_MO_SHI};

//遥控已存在
const tips_t TIPS_YKYCZ={"遥控已存在","Remote Exist",0};

//遥控不存在
const tips_t TIPS_YKBCZ={"遥控不存在","Remote Not Exist",0};

//遥控已配满
const tips_t TIPS_YKYPM={"遥控已配满","Remote Add Full",0};

//正在删除密码
const tips_t TIPS_ZZSCMM={"正在删除密码","Delete PIN",0};

//正在删除指纹
const tips_t TIPS_ZZSCZW={"正在删除指纹","Delete FP",0};

//正在删除卡片
const tips_t TIPS_ZZSCKP={"正在删除卡片","Delete Card",0};

//正在删除开门记录
const tips_t TIPS_ZZSCKMJL={"正在删除开门记录","Delete Record",0};

//验证成功,请开门
const tips_t TIPS_QKM={"请开门","Open",YAN_ZHENG_CHENG_GONG_QING_KAI_MEN};

//已关门
const tips_t TIPS_YGM={"已关门","Close",YI_GUAN_MEN};

//蓝牙已开启
const tips_t TIPS_LYKQ={"蓝牙已开启","On Bluetooth",LAN_YA_YI_KAI_QI};

//蓝牙已关闭
const tips_t TIPS_LYGB={"蓝牙已关闭","Off Bluetooth",LAN_YA_YI_GUAN_BI};

//已启动布防
const tips_t TIPS_YQDBF={"已启动布防","Defense Mode on",YI_QI_DONG_BU_FANG};

//已反锁
const tips_t TIPS_YFS={"已反锁","Privacy Mode on",YI_FAN_SUO};

//语音模式
const tips_t TIPS_YYMS={"语音模式","Voice guide on",0};

//自动模式
const tips_t TIPS_ZDMS={"自动模式","Auto Mode",ZI_DONG_MO_SHI};

//手动模式
const tips_t TIPS_SDMS={"手动模式","Manual Mode",SHOU_DONG_MO_SHI};

//门未上锁
const tips_t TIPS_MWSS={"门未上锁","Not locked",MEN_WEI_SHANG_SUO};


void DisplayTips(const tips_t *tip)
{
    Display_clear_screen();
    if(Function_status()->ManageMode.Language==CHN)
    {
        Display_chinese_str((128-strlen(tip->TipsChinese)*8)/2,1,tip->TipsChinese,false);
    }
    else if(Function_status()->ManageMode.Language==EN)
    {
        Display_english_str((128-strlen(tip->TipsEnglish)*8)/2,1,tip->TipsEnglish,false);
    }
}
void DisplayTipsEx(const tips_t *tip,uint8_t y,bool clr)
{
    if(clr)
        Display_clear_screen();
    if(Function_status()->ManageMode.Language==CHN)
    {
        Display_chinese_str((128-strlen(tip->TipsChinese)*8)/2,y,tip->TipsChinese,false);
    }
    else if(Function_status()->ManageMode.Language==EN)
    {
        Display_english_str((128-strlen(tip->TipsEnglish)*8)/2,y,tip->TipsEnglish,false);
    }
}


/* [] END OF FILE */
