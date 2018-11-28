#include "monitordialog.h"
#include "ui_monitordialog.h"
#include <QDesktopWidget>
#include <data_organize.h>
#include "string.h"
#include <iostream>
#include <QList>
#include <QMutex>
#include "config.h"

QMutex ext_mutex;
extern QList<VCI_CAN_OBJ> vec_buff;

MonitorDialog::MonitorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("CAN 监视");

    QDesktopWidget *thisDesk = QApplication::desktop();
    QRect rect = thisDesk->availableGeometry();
    this->move(rect.width()-this->width()-14,rect.height()-this->height()-40);

    memset(&msg_id_list,0,sizeof(struct id_list));
    msg_id_list.len = 0;
    board_id_list.len = 0;
    filter_id = 0;
    temp_array.clear();

    int flag = windowFlags();
    flag &= (~Qt::WindowContextHelpButtonHint);

    this->setWindowFlags(Qt::WindowFlags(flag));

    timer_100 = new QTimer(this);
    connect(timer_100,&QTimer::timeout,this,&MonitorDialog::timer100_timeout);
    timer_100->start(100);
}

void MonitorDialog::timer100_timeout()
{
    int i,j;
    QString temp_str;

    ext_mutex.lock();
    foreach(VCI_CAN_OBJ obj,vec_buff){
        for(j=0;j<msg_id_list.len;j++){
            if(obj.ID == msg_id_list.id[j]) break;
        }
        if(j==msg_id_list.len){
            msg_id_list.id[j] = obj.ID;
            msg_id_list.len++;
            temp_str = QString("%1").arg(obj.ID,8,16,QChar('0'));
            temp_str = temp_str.toUpper();
            ui->lw_fid->addItem(temp_str);
        }

        if(CONG_BOARD_STATE_ID == (obj.ID&0xFFFFFFF0)){
            for(j=0;j<board_id_list.len;j++)
                if(obj.Data[0] == board_id_list.id[j]) break;
            if(j == board_id_list.len){
                board_id_list.id[j] = obj.Data[0];
                board_id_list.len++;
                ui->lw_bid->addItem(QString("%1").arg(obj.Data[0]));
            }
        }

        if(obj.ID == filter_id || filter_id == 0){
            temp_str = QString("%1: ").arg(obj.ID,8,16,QChar('0'));
            for(int k=0;k<obj.DataLen;k++){
                temp_str += QString("%1 ").arg(obj.Data[k],2,16,QChar('0'));
            }
            temp_str = temp_str.toUpper();
            temp_array.push_back(temp_str);
            if(temp_array.size() > LIST_WIDGET_MSG_LEN_MAX){
                temp_array.pop_front();
            }
        }
    }

    if(vec_buff.isEmpty() == false){
        ui->lw_msg->clear();
        ui->lw_msg->addItems(QStringList(temp_array.toList()));
        ui->lw_msg->scrollToBottom();
        vec_buff.clear();
    }
    ext_mutex.unlock();
}

MonitorDialog::~MonitorDialog()
{
    delete ui;
}

void MonitorDialog::on_lw_fid_itemDoubleClicked(QListWidgetItem *item)
{
    bool ok;
    QString temp = item->text();
    if(temp.isEmpty() || temp.isNull()){
        filter_id = 0;
    }else{
        filter_id = temp.toInt(&ok,16);
    }
    ui->lw_fid->clear();
    ui->lw_fid->addItem(QString(""));
    memset(&msg_id_list,0,sizeof(struct id_list));
}

void MonitorDialog::on_lw_bid_itemDoubleClicked(QListWidgetItem *item)
{
    bool ok;
    QString sid = item->text();
    unsigned char id = (unsigned char)sid.toInt(&ok,10);
    ui->lw_bid->clear();
    board_id_list.len = 0;
    if(ok)
        emit sig_send_board_id(id);
}

void MonitorDialog::slots_unit_outline()
{
    ui->lw_bid->clear();
    board_id_list.len = 0;
}
