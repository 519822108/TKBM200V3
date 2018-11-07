#ifndef TKBMWIDGET_H
#define TKBMWIDGET_H

#include <QWidget>
#include "logindialog.h"
#include "monitordialog.h"
#include "data_organize.h"
#include "ctlcan.h"
#include <QThread>
#include <QTimer>

namespace Ui {
class TkbmWidget;
}

class TkbmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TkbmWidget(QWidget *parent = nullptr);
    ~TkbmWidget();

    void ui_init(void);
    void set_eep_config(void);
    void read_excel_data(void);
    void set_eeprom_table(void);
    void comm_timeout(void);

signals:
    void sig_set_cthread_state(CtlCan *pCan);
    void sig_process_exit(void);

private slots:
    void on_tb_eep_file_cellClicked(int row, int column);
    void sig_get_can_param(int dev,int num,int rate,int port);

    void on_rb_exp_dis_clicked();

private:
    Ui::TkbmWidget *ui;
    LoginDialog login_dialog;
    MonitorDialog monitor_dialog;
    struct excel_param_organize_ext eep_config;
    CtlCan *can_bsp;
    QTimer *timer10;

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

protected:
    bool state;
    void run();

private:
    CtlCan *can_bps;
};

#endif // TKBMWIDGET_H
