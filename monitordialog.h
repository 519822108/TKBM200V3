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

public slots:
    void timer100_timeout();

private slots:
    void on_lw_fid_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MonitorDialog *ui;
    QVector<QString> temp_array;
    QTimer *timer_100;

    int temp_cp_msg_len;
    struct id_list{
        unsigned int id[128];
        int len;
    }msg_id_list;
    int filter_id;
    char board_id_list[128];
};

#endif // MONITORDIALOG_H
