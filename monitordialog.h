#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>
#include <QTimer>
#include <ControlCAN.h>
#include <QListWidgetItem>
#include <list>

namespace Ui {
class MonitorDialog;
}

class MonitorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MonitorDialog(QWidget *parent = nullptr);
    ~MonitorDialog();
    struct id_list{
        unsigned int id[128];
        int len;
    }msg_id_list,board_id_list;

public slots:
    void timer100_timeout();
    void slots_unit_outline();

private slots:
    void on_lw_fid_itemDoubleClicked(QListWidgetItem *item);

    void on_lw_bid_itemDoubleClicked(QListWidgetItem *item);

signals:
    void sig_send_board_id(int id);

private:
    Ui::MonitorDialog *ui;
    QVector<QString> temp_array;
    QTimer *timer_100;

    int temp_cp_msg_len;


    int filter_id;
};

#endif // MONITORDIALOG_H
