#ifndef TKBMWIDGET_H
#define TKBMWIDGET_H

#include <QWidget>
#include "logindialog.h"
#include "monitordialog.h"
#include "data_organize.h"
#include "ctlcan.h"
#include <QThread>
#include <QTimer>
#include <QList>
#include <QMutex>
#include <QTableWidgetItem>
#include <QAxObject>

namespace Ui {
class TkbmWidget;
}
class ReadExcelCfg;
class BatteryStore;

class TkbmWidget : public QWidget
{
    Q_OBJECT

private:

public:
    explicit TkbmWidget(QWidget *parent = nullptr);
    ~TkbmWidget();

    void ui_init(void);
    void data_struct_init(void);
    void set_eep_config(void);
    void read_excel_data(void);
    void set_eeprom_table(void);
    void chg_stage_data_init(void);
    void sub_state_msg_ana(struct sub_each_board *bd,unsigned char data[8]);
    void tb_eeprom_file_setting(unsigned char *data,int col);
    void eeprom_launch_send(bool state);
    void per_bat_vol_limit_cacul();
    void txt_xml_anasys(void);
    unsigned int stringid_to_intid(QString qmsg);
    unsigned char eeprom_after_modify(void);

    QString anasy_alart_msg(unsigned char data[5]);
    QString main_info_msg(unsigned char data,char pos);

signals:
    void sig_set_cthread_state(CtlCan *pCan);
    void sig_process_exit(void);
    void sig_sub_unit_outline(void);
    void sig_get_store_obj(QVector<struct sub_each_board> *send_data,QVector<struct breif_info_for_bms> *breif_info);
    void sig_send_eep_config(struct excel_param_organize_ext *eepConfig);

private slots:
    void on_tb_eep_file_cellClicked(int row, int column);
    void sig_get_can_param(int dev,int num,int rate,int port);
    void comm_timeout(void);                //定时器超时处理通信
    void on_rb_exp_dis_clicked();
    void update_msg_timeout(void);          //定时器超时更新UI
    void update_data_timeout(void);         //定时器超时刷新数据
    void slot_get_board_id(int bid);
    void slot_get_can_error_code(VCI_ERR_INFO errCode);

    void on_pb_sync_clicked();

    void on_pb_modify_clicked();

    void on_tb_ctl_info_cellDoubleClicked(int row, int column);

    void on_pb_ctl_tch_clicked();

    void on_pb_ctl_alarm_clicked();

    void on_le_id_in_editingFinished();

    void on_pb_after_clicked();

    void on_pb_before_clicked();

    void on_pb_mian_into_t_clicked(bool checked);

    void on_pb_eep_read_clicked(bool click);

    void on_pb_out_data_clicked();

    void on_pb_in_data_clicked();

    void on_pb_ensure_clicked();

    void on_tb_eep_file_itemChanged(QTableWidgetItem *item);

    void on_tb_brief_itemChanged(QTableWidgetItem *item);

private:
    Ui::TkbmWidget *ui;
    LoginDialog login_dialog;
    MonitorDialog monitor_dialog;
    CtlCan *can_bsp;
    QTimer *timer10;
    QTimer *timer_100;
    QTimer *timer_5s;
    ReadExcelCfg *read_cfg;
    BatteryStore *bat_store;

    int cnt_ctrl_clk;
    int vcu_alarm;
    int msg_alart_list_line;                    //二维数组msg_alart_list的行长度
    int msg_alart_list_col;                     //二维数组msg_alart_list的列长度
    bool main_is_test_mode;
    bool eeprom_is_send_data;                           //下位机是否发送EEPROM数据
    bool eeprom_recv_data_flag;                          //下位机开始接受EEPROM数据
    bool eeprom_read_btn_state;                         //eeprom读取按钮状态
    bool eeprom_set_launch_setting;                     //EEPROM显示列表是否首次写入
    QMutex save_data_mutex;

    int voltag_store_cnt;                               //电压保存时间计数
    QString file_path;
    VCI_CAN_OBJ eeprom_send_array[EEPROM_DATA_SEND_MARK];
    QVector<struct sub_each_board> store_voltag_data;
    QVector<struct breif_info_for_bms> breif_info;                  //概述信息

    struct excel_param_organize_ext eep_config;
    struct msg_discripte msg_summary[MSG_SUMMARY_LIST_LENGTH];          //概述表格信息结构
    struct msg_discripte msg_alarm;                                     //报警信息结构
    struct msg_discripte msg_chg_summary[MSG_CHG_SUMMARY_LENGTH];       //充电概述表信息结构
    struct msg_discripte msg_chg_err_disc[MSG_CHG_ERROR_CURSOR_LEN];    //充电过程错误描述表
    struct msg_discripte msg_ac_chg_state[MSG_AC_CHG_STATE_LEN];        //交流充电机状态描述表
    struct msg_discripte msg_main_ctl_info[MSG_MAIN_BD_CTRL];           //主板控制信息描述表
    struct msg_discripte msg_bms_run_state_dsc[UNIT_INFO_NUM_MAX];        //从板状态信息
    struct per_battery_info_discription *bms_sub_info;                       //BMS描述
    struct eeprom_data_info_discription eeprom_info;                    //EEPROM 信息描述
    struct each_bat_info_discription limit_vol_info;                    //最高最低电压记录
    struct unit_limit_msg   unit_limit_mast;

protected:
    void closeEvent(QCloseEvent *event);
};

class RecvCan : public QThread
{
    Q_OBJECT

public:
    explicit RecvCan(QObject *parent = nullptr);
    ~RecvCan();

public slots:
    void slot_get_state(CtlCan *pCan);

signals:
    void sig_send_can_error_code(VCI_ERR_INFO errCode);

protected:
    bool state;
    void run();

private:
    CtlCan *can_bps;
};

class BatteryStore : public QThread
{
    Q_OBJECT

public:
    void save_voltag_use_id(QAxObject *pSheets);
    void save_voltag_as_one(QAxObject *pSheets);
    void save_breif_info(QAxObject *pSheets);

public slots:
    void slot_get_store_obj(QVector<struct sub_each_board> *send_data,QVector<struct breif_info_for_bms> *breif_info);

protected:
    void run();

private:
    QVector<struct sub_each_board> bat_unit;
    QVector<struct breif_info_for_bms> bms_breif_info;
};

class ReadExcelCfg : public QThread
{
    Q_OBJECT

public:
    void read_excel_data();

signals:
    void send_progress_status(int status);

public slots:
    void get_eep_config(struct excel_param_organize_ext *eepConfig);

protected:
    void run();
private:
    struct excel_param_organize_ext *eep_config;
};

#endif // TKBMWIDGET_H
