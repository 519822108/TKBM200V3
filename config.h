#ifndef CONFIG_H
#define CONFIG_H

#define FILE_PATH_CONFIG    "/config/BMS_config.xlsx"
#define FILE_CONFIG_COL_START   1               //配置文件数据区列起始坐标
#define FILE_CONFIG_COL_END     7               //配置文件数据区列终止坐标
#define FILE_CONFIG_ROW_START   2               //配置文件数据区行起始坐标
#define FILE_CONFIG_ROW_END     266             //配置文件数据区行终止坐标,此处不定

#define CONFIG_COL_PARAM_NUM    1               //参数编号的列号
#define CONFIG_COL_PARAM_NAME   2               //参数数据名的列号
#define CONFIG_COL_PARAM_LEN    4               //参数所占字节数的列号
#define CONFIG_COL_PARAM_FORMAT 2               //参数显示格式的列号
#define CONFIG_COL_PARAM_NOTE   7               //参数注释的列号
#define CONFIG_COL_PARAM_RATE   5               //参数比例
#define CONFIG_COL_PARAM_OFF    6               //参数偏移

#define FILE_CONFIG_LIST_MAX_LEN    512         //配置参数最大数目

#define CAN_RECV_BUFF_LEN_MAX       512         //CAN buff 长度
#define LIST_WIDGET_MSG_LEN_MAX     256         //辅助列表框报文显示最大行数

#define THIS_SOFT_VERSION           "TB-BM200 V3.0"

#define CHG_STAGE_ARRAY_SIZE        50

#define SUB_MAIN_MSG1_ID            0x0841F7F4     //主板数据1
#define SUB_MAIN_MSG2_ID            0x0842F7F4     //主板数据2
#define JURYUAN_MSG_ID              0x0851F4A5     //绝缘板数据

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

#endif // CONFIG_H
