#ifndef CONFIG_H
#define CONFIG_H
#include <QString>

#define FILE_PATH_CONFIG    "/config/BMS_config.xlsx"
#define FILE_PATH_ARRTXT    "/config/ctxt.xml"
#define FILE_CONFIG_COL_START   1               //配置文件数据区列起始坐标
#define FILE_CONFIG_COL_END     7               //配置文件数据区列终止坐标
#define FILE_CONFIG_ROW_START   2               //配置文件数据区行起始坐标
#define FILE_CONFIG_ROW_END     266             //配置文件数据区行终止坐标,此处不定

#define CONFIG_COL_PARAM_NUM    1               //参数编号的列号
#define CONFIG_COL_PARAM_NAME   2               //参数数据名的列号
#define CONFIG_COL_PARAM_LEN    4               //参数所占字节数的列号
#define CONFIG_COL_PARAM_FORMAT 3               //参数显示格式的列号
#define CONFIG_COL_PARAM_NOTE   7               //参数注释的列号
#define CONFIG_COL_PARAM_RATE   5               //参数比例
#define CONFIG_COL_PARAM_OFF    6               //参数偏移

#define FILE_CONFIG_LIST_MAX_LEN    512         //配置参数最大数目
#define EEPROM_ARRAY_LENGTH         512         //EEPROM长度
#define EEPROM_SEND_DATA_LEN        7           //发送eeprom数据报文数据长度
#define EEPROM_DATA_RECV_MARK       46         //eeprom数据报文桢数
#define EEPROM_DATA_SEND_MARK       38          //eeprom数据发送帧数
#define EEPROM_DATA_LENGTH          266         //EEPROM数据长度
#define EEPROM_DATA_DATLENG         (EEPROM_DATA_LENGTH-1)
#define EEP_XLS_COL_LENGTH          16
#define EEPROM_DATA_EXCEL_ROW_S     2
#define EEPROM_DATA_EXCEL_COL_S     2
#define EEP_TB_FILE                 2
#define EEP_TB_RECV                 1
#define EEP_TB_PREX_LENG            3

#define CONFIG_USE_FILE_CTXTXML     1           //启用配置文件配置部分参数

static int VOLTAG_SAVE_TIME   =     20;          //电压数据保存时间间隔    *50ms
static int VOLTAG_IS_SAVE     =     1;           //是否存储电压数据
static int VOLTAG_SAVE_MODE   =     0;           //电压保存模式
static int IS_DATA_ARRY_CLEAR =     0;           //是否执行数据保存数组清空,不清空则屏幕不闪烁但数据会残留

#define CAN_RECV_BUFF_LEN_MAX   512         //CAN buff 长度
#define LIST_WIDGET_MSG_LEN_MAX 256        //辅助列表框报文显示最大行数

static QString THIS_SOFT_VERSION = "TB-BM200 V3.0";

#define BREIF_INFO_FIGURE_NAME      "MInfo"
#define SAVE_AS_ONE_DATA_NAME       "Value"

#define CHG_STAGE_ARRAY_SIZE        50
#define CHG_STAGE_ARRAY_SIZE_ORG    7

static unsigned int SUB_MAIN_MSG1_ID  =  0x0841F7F4;     //主板数据1
static unsigned int SUB_MAIN_MSG2_ID  =  0x0842F7F4;     //主板数据2
static unsigned int JURYUAN_MSG_ID    =  0x0851F4A5;     //绝缘板数据

static unsigned int CONG_BOARD_STATE_ID = 0x0821F4C0;      //从板状态信息
static unsigned int CONG_BOARD_TEMP_ID  = 0x0822F4C0;      //从板温度信息
#define CONG_BOARD_EACH_TEMP        6               //每个从板温度保温带有温度数据数
static unsigned int CONG_BOARD_VOL_ID   = 0x0823F4C0;      //从板电压信息
#define CONG_BOARD_EACH_VOL         3               //每个从板电压报文带有的电压数据数

#define CONG_BOARD_LIMIT_VOL_LEN    6               //单体电压最高最低保存长度

static unsigned int UPME_SET_MAIN_PARAM_ID = 0x0831F4F7;      //上位机设置主板参数ID

static int BMS_UNIT_OUTLINE_CNT   =   100;     //从板超时离线计数,5s
static long voltag_store_file_len_max = 10;      //电压数据保存文件最大字节数 *1Mbyte
static int BMS_BAT_COUNT_IN_EXCEL = 256;        //预定义EXCEl表格中电池总数
static int EXCEL_FILENAME_FORMAT = 3;           //保存ECXCEL文件名时间精度,0-day,1-hours,2-mimutes,3-second
#define MBYTE_TO_BYTE               1048576     //1Mbyte 转字节
#define SUB_MODULE_NUM_MAX          4       //从板最大模块数
#define EACH_MODULE_CHINNEL         12      //每个模块采集数
#define TOTAL_MODULE_LENGTH         (SUB_MODULE_NUM_MAX*EACH_MODULE_CHINNEL)        //单板单体电压通道最大数
#define SUB_TEMPRATURE_NUM_MAX      16      //从板最大温度采集数
#define SUB_TEMP_DISPLAY_ROW        4       //温度显示行数
#define SUB_TEMP_DISPLAY_COL        4       //温度显示烈数
#define BMS_MAX_CONG_LINE           16      //从板最大连接数

#define MSG_SUMMARY_LIST_LENGTH     32      //概述数组总长度
#define MSG_SUMMARY_LIST_HERTICK    1       //概述 心跳
#define MSG_SUMMARY_LIST_SOC        2       //概述 SOC
#define MSG_SUMMARY_LIST_TVOL       3       //概述 总电压
#define MSG_SUMMARY_LIST_TCUR       4       //概述 总电流
#define MSG_SUMMARY_LIST_CCON       5       //概述 从板连接数
#define MSG_SUMMARY_LIST_CHGST      6       //充放电状态
#define MSG_SUMMARY_LIST_ACCAI      7       //实际容量
#define MSG_SUMMARY_LIST_OFCAT      8       //补偿容量
#define MSG_SUMMARY_LIST_RESIS      9       //绝缘电阻
#define MSG_SUMMARY_LIST_NORSV      10      //绝缘检测电压
#define MSG_SUMMARY_LIST_RSVTIC     11      //绝缘检测心跳
#define MSG_SUMMARY_LIST_RSV0       12      //保留
#define MSG_SUMMARY_LIST_MTPV       13      //最高单体电压
#define MSG_SUMMARY_LIST_MVPS       14      //最高单体位置
#define MSG_SUMMARY_LIST_MLPV       15      //最低单体电压
#define MSG_SUMMARY_LIST_MLPS       16      //最低单体位置
#define MSG_SUMMARY_LIST_MTTP       17      //最高温度
#define MSG_SUMMARY_LIST_MLTP       18      //最低温度
#define MSG_SUMMARY_LIST_RSV1       19      //保留
#define MSG_SUMMARY_LIST_CNST       20      //接触器状态

#define MSG_CHG_SUMMARY_LENGTH      32      //充电信息概述表最大长度
#define MSG_CHG_SUMMARY_CHG_STAGE   0       //充电阶段
#define MSG_CHG_SUMMARY_VOL_NEED    1       //电压需求
#define MSG_CHG_SUMMARY_CUR_NEED    2       //电流需求
#define MSG_CHG_SUMMARY_MODE_NEED   3       //模式需求
#define MSG_CHG_SUMMARY_IIME_NEED   4       //剩余充电时间
#define MSG_CHG_SUMMARY_CONNTEMP1   5       //插座温度1
#define MSG_CHG_SUMMARY_CONNTEMP2   6       //插座温度2
#define MSG_CHG_SUMMARY_CC2_STATE   7       //CC2状态
#define MSG_CHG_SUMMARY_CC2_FRQ     8       //CC2频率
#define MSG_CHG_SUMMARY_CHG_VER     9      //充电国际版本
#define MSG_CHG_SUMMARY_OUT_VOL     10      //输出电压
#define MSG_CHG_SUMMARY_OUT_CUR     11      //输出电流
#define MSG_CHG_SUMMARY_OUT_POW     12      //输出能量
#define MSG_CHG_SUMMARY_CHG_TIM     13      //充电时间
#define MSG_CHG_SUMMARY_VOL_MAX     14      //CHG最高输出电压
#define MSG_CHG_SUMMARY_VOL_MIN     15      //CHG最低输出电压
#define MSG_CHG_SUMMARY_CUR_MAX     16      //CHG最高输出电压
#define MSG_CHG_SUMMARY_CUR_MIN     17      //CHG最低输出电流

#define MSG_CHG_ERROR_CURSOR_LEN    8       //充电过程故障原因描述表最大长度
#define MSG_CHG_ERROR_BMS_STOP      0       //BMS停止充电原因
#define MSG_CHG_ERROR_CHG_STOP      1       //充电机停止充电原因
#define MSG_CHG_ERROR_CHG_ERR       2       //充电机停止充电故障
#define MSG_CHG_ERROR_BMS_COM       3       //通信过程MBS错误
#define MSG_CHG_ERROR_CHG_RCV       4       //通信过程CHG接受错误

#define MSG_AC_CHG_STATE_LEN        14      //交流充电状态描述表最大长度
#define MSG_AC_CHG_STATE            0       //充电状态
#define MSG_AC_CHG_ENABLE           1       //充电使能
#define MSG_AC_CHG_VOL_NEED         2       //电压需求
#define MSG_AC_CHG_CUR_NEED         3       //电流需求
#define MSG_AC_CHG_CP_PWM           4       //CP占空比
#define MSG_AC_CHG_CC_STATE         5       //CC状态
#define MSG_AC_CHG_CC_FRQ           6       //CC频率
#define MSG_AC_CHG_OUT_STE          7       //输出状态
#define MSG_AC_CHG_OUT_VOL          8       //输出电压
#define MSG_AC_CHG_OUT_CUR          9       //输出电流

#define MSG_MAIN_BD_CTRL            22      // 主板控制描述结构最大长度
#define MSG_BMS_TIME                0       //BMS时间
#define MSG_LOCAL_TIME              1       //本地时间
#define MSG_SOC_SET                 2       //SCO设置
#define MSG_ACT_CONTAIN             3       //实际容量
#define MSG_OFF_CONTAIN             4       //补偿容量
#define MSG_CNT_FORCE_CTL           5       //接触器强制控制
#define MSG_VCUCAN_ALM_CTL          6       //整车CAN报警控制
#define MSG_OVER_CHG_ACH_FLAG       7       //满电校准完成标志
#define MSG_INTO_OFF_TIME           8       //进入补偿点时间
#define MSG_INTO_30_AC_TIME         9       //进入30%校准点时间
#define MSG_INTO_20_AC_TIME         10       //进入20%校准点时间
#define MSG_INTO_10_AC_TIME         11      //进入10%校准点时间
#define MSG_INTO_1_AC_TIME          12      //进入1%校准点时间
#define MSG_STORE_TIME              13      //存储电量次数
#define MSG_SOC_EXC_BEF             14      //跳变前SOC
#define MSG_SOC_EXC_AFT             15      //跳变后SOC
#define MSG_SOFT_VER                16      //软件版本
#define MSG_HARD_VER                17      //硬件版本
#define MSG_SOFT_MOD_TIME           18      //软件修改时间
#define MSG_SOFT_MOD_NUM            19      //软件修改编号

#define UNIT_INFO_NUM_MAX         4       //单体状态信息数组最大值
#define MODULE_NUM                  0
#define SOFT_VERSION                1
#define HARD_VERSION                2
#define SERIAL_NUMBER               3

static QString EEP_MOD_PASSWD =  "1234" ;       //EEPROM修改密码
#define EEP_MOD_TB_POS              3

#endif // CONFIG_H
