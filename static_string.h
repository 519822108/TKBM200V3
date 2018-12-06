#ifndef STATIC_STRING_H
#define STATIC_STRING_H
#include <QString>
#include "config.h"

static QString msg_alart_level[4];// = {QString("无报警\0"),QString("一级报警\0"),QString("二级报警\0"),QString("三级报警\0")};
static QString msg_alart_list[8][8];/* = { {QString("绝缘过低\t"),QString("内CAN故障\t"),QString("从板故障\t"),QString("EEP故障\t")},
                                 {QString("单体过压\t"),QString("单体欠压\t"),QString("总压过高\t"),QString("总压过低\t")},
                                 {QString("电池过温\t"),QString("电池低温\t"),QString("温差过大\t"),QString("压差过大\t")},
                                 {QString("SOC过低\t"),QString("HALL离线\t"),QString("SOC跳变\t"),QString("时钟离线\t")},
                                 {QString("充电过流\t"),QString("放电过流\t"),QString("采集线断裂\t"),QString("SOH过低\t")}
                            };*/
static QString msg_connector_state[16];// = {QString("自检中\0"),QString("接触器吸合\0"),QString("单体过压保护\0"),QString("单体欠压保护\0"),\
                                          QString("总压过高保护\0"),QString("总压过低保护"),QString("电池过温保护\0"),QString("电池低温保护\0"),\
                                          QString("压差过大保护\0"),QString("温差过大保护\0"),QString("充电过流保护\0"),QString("放电过流保护\0"),\
                                          QString("绝缘过低保护\0"),QString("从板离线保护\0"),QString("上位机强制闭合\0"),QString("上位机强制断开\0"),\
};

static QString msg_chg_state[4];// = {QString("放电状态中\0"),QString("充电枪连接,未充电\0"),QString("充电枪连接,充电中\0"),QString("非充电枪,充电中\0")};

static QString msg_cc2_state[2];// = {QString("未可靠连接\0"),QString("可靠连接\0")};
static const QString msg_global_ver[2] = {QString("27930-2111"),QString("27930-2015")};
static QString msg_need_mode[4];// = {QString("禁止充电\0"),QString("恒压充电\0"),QString("恒流充电\0"),QString("ERROR")};

static QString msg_chg_stage[8];
static QString msg_bms_stop_chg[9];// = {QString("达到总压上限\0"),QString("达到单体上限\0"),QString("受到充电机CST"),QString("绝缘电阻过低\0"),\
                                      QString("电池过温\0"),QString("CC2信号异常\0"),QString("电流异常\0"),QString("从板离线\0"),QString(" ")};
static QString msg_chg_stop_err[6];// = {QString("达到充电设定\0"),QString("人工中止\0"),QString("故障中止\0"),QString("BMS主动中止\0"),QString(" ")};
static QString msg_chg_stop_fat[9];// = {QString("充电机过温\0"),QString("连接器故障\0"),QString("充电机内部过温\0"),QString("电量不能传送\0"),\
                                     QString("急停故障\0"),QString("电流不能匹配\0"),QString("电压异常\0"),QString("其它故障\0"),QString(" ")};
static const QString msg_bms_rcv_err[7] = {QString("CRM error"),QString("CML error"),QString("CRO error"),QString("CCS error"),QString("CST error"),QString("CSD error"),QString(" ")};
static const QString msg_chg_rcv_err[8] = {QString("BRM error"),QString("BCP error"),QString("BRO error"),QString("BCS error"),QString("BCL error"),QString("BST error"),QString("BSD error"),QString(" ")};
static QString msg_state_enable[2];// = {QString("使能\0"),QString("禁止\0")};
static QString msg_ac_chg_state_a[8];// = {QString("未开始\0"),QString("充电中\0"),QString("接触器断开,充电停止\0"),QString("报警,充电停止\0"),\
                                     QString("单体达到充电停止条件,充电停止\0"),QString("总压达到充电停止条件,充电停止\0"),QString("接口未可靠连接,充电停止\0"),QString(" ")};
static QString msg_ac_chger_out_state[8];// = {QString("未开始\0"),QString("充电中\0"),QString("充电停止\0"),QString("充电停止\0"),QString("充电停止\0"),QString("充电停止\0"),QString(" ")};
static QString msg_over_chg_acc_flag[2];// = {QString("未完成\0"),QString("完成\0")};

static QString msg_cnt_force_ctrol[4];// = {QString("00H:不控制\0"),QString("A0H:强制闭合\0"),QString("A1H:强制断开\0")};
static QString msg_vcu_can_alarm[2];// = {QString("00H:不控制\0"),QString("A0H:屏蔽报警\0")};
static QString msg_bms_run_state[8];// = {QString("模块A错误\0"),QString("模块B错误\0"),QString("模块C错误\0"),QString("模块D错误\0"),\
                                            QString("EEPROM错误\0"),QString("reserved"),QString("reserved"),QString("测试模式\0")};
static QString param_ac_byte = "参数校验字节\0";
#endif // STATIC_STRING_H
