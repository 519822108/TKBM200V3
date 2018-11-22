#ifndef DATA_ORGANIZE_H
#define DATA_ORGANIZE_H
#include <config.h>
#include <QString>
#include "ControlCAN.h"
#include <Qt>
#include <QVector>
#include <list>

/*  breif: Excel配置数据到县市区的对应关系
 *  s_ :源文件和显示区共有的成员
 *  o_ :源文件独有的成员
 *  d_ :显示区独有的成员
 */
struct excel_param_organize{
    int             s_num;                  //数据的编号
    QString         s_name;                 //参数名
    QString         s_note;                 //参数注释
    int             o_byte_len;             //参数占有的字节长度
    int             o_dis_format;           //参数的显示格式, 16进制或10进制
    double             o_rate;                 //参数转换系数
    int             o_off;                  //参数转换偏移
    int             d_cur_val;             //参数当前值
    int             d_obj_val;             //参数目标值
};

struct excel_param_organize_ext{               //Excel 参数的表
    int len;
    int pos;
    struct excel_param_organize param[FILE_CONFIG_LIST_MAX_LEN];
};

struct recv_data{                   //CAN数据Buff
    int len;
    VCI_CAN_OBJ data[CAN_RECV_BUFF_LEN_MAX];
};

struct msg_discripte{           //信息描述结构
    QString                 s_val;          //信息值的字符串表示
    int                     i_val;          //信息的值
    enum Qt::GlobalColor    b_color;          //信息背景色
    enum Qt::GlobalColor    f_color;          //信息前景色
};

struct sub_each_board{                          //从板描述
    int bid;                                    //板ID
    int chinnel_start;                          //当前板第一电压通道在系统中的起始位置
    int online_count;                           //在线计数器
    int modu_num;                               //板载模块数
    int sw;                                     //软件版本
    int hw;                                     //硬件版本
    unsigned char sn[6];                        //串号
    unsigned char state;                        //运行状态
    unsigned char htick;                        //心跳计数
    unsigned short disc[SUB_MODULE_NUM_MAX];            //断线信息
    char temp[SUB_TEMPRATURE_NUM_MAX];                  //温度信息
    int reserved;
    int each_volt[TOTAL_MODULE_LENGTH+2];                  //电压信息
    int per_chinnal[SUB_MODULE_NUM_MAX];                //每个模块通道数
};

struct per_battery_info_discription{                    //BMS描述
    int cur_id;                                         //选中的从板ID
    int unit_num;                                       //BMS从板总数
    QVector<struct sub_each_board> each_board;
};

struct eeprom_data_info_discription{
    unsigned char eeprom[EEPROM_ARRAY_LENGTH];
    unsigned char eepcfg[EEPROM_ARRAY_LENGTH];
    unsigned char eepcfg_cp[EEPROM_ARRAY_LENGTH];
    unsigned char recv_mark[EEPROM_DATA_RECV_MARK];
    bool recv_finish_flag;
};

#endif // DATA_ORGANIZE_H
