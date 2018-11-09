#ifndef STATIC_STRING_H
#define STATIC_STRING_H
#include <QString>

static QString msg_alart_level[4] = {QString("无报警\0"),QString("一级报警\0"),QString("二级报警\0"),QString("三级报警\0")};
static QString msg_alart_list[5][4] = { {QString("绝缘过低\t"),QString("内CAN故障\t"),QString("从板故障\t"),QString("EEP故障\t")},
                                 {QString("单体过压\t"),QString("单体欠压\t"),QString("总压过高\t"),QString("总压过低\t")},
                                 {QString("电池过温\t"),QString("电池低温\t"),QString("温差过大\t"),QString("压差过大\t")},
                                 {QString("SOC过低\t"),QString("HALL离线\t"),QString("SOC跳变\t"),QString("时钟离线\t")},
                                 {QString("充电过流\t"),QString("放电过流\t"),QString("采集线断裂\t"),QString("SOH过低\t")}
                            };
static QString msg_connector_state[16] = {QString("自检中\0"),QString("接触器吸合\0"),QString("单体过压保护\0"),QString("单体欠压保护\0"),
                                          QString("总压过高保护\0"),QString("总压过低保护"),QString("电池过温保护\0"),QString("电池低温保护\0"),
                                          QString("压差过大保护\0"),QString("温差过大保护\0"),QString("充电过流保护\0"),QString("放电过流保护\0"),
                                          QString("绝缘过低保护\0"),QString("从板离线保护\0"),QString("上位机强制闭合\0"),QString("上位机强制断开\0"),
};

static QString msg_chg_state[4] = {QString("放电状态中\0"),QString("充电枪连接,未充电\0"),QString("充电枪连接,充电中\0"),QString("非充电枪,充电中\0")};

#endif // STATIC_STRING_H
