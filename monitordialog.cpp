#include "monitordialog.h"
#include "ui_monitordialog.h"
#include <QDesktopWidget>
#include <data_organize.h>
#include "string.h"
#include <iostream>
#include <QList>

extern struct recv_data can_data;
VCI_CAN_OBJ last_msg;
VCI_CAN_OBJ temp_cp_msg[512];
MonitorDialog::MonitorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("CAN 监视");
    QDesktopWidget *thisDesk = QApplication::desktop();
    QRect rect = thisDesk->availableGeometry();
    this->move(rect.width()-this->width()-14,rect.height()-this->height()-40);
    memset(&last_msg,0,sizeof(VCI_CAN_OBJ));
    msg_id_list.len = 0;
    filter_id = 0;

    QTimer *timer100 = new QTimer(this);
    connect(timer100,&QTimer::timeout,this,&MonitorDialog::timer100_timeout);
    timer100->start(11);
}
static int tempstamp;
void MonitorDialog::timer100_timeout()
{
    int i,j,arry_len=0;
    QString temp_str;
    QList<QString> temp_array;

    memcpy(temp_cp_msg,&can_data.data[temp_cp_msg_len],(can_data.len)*sizeof(VCI_CAN_OBJ));
    temp_cp_msg_len = can_data.len;
    for(i=0;i<temp_cp_msg_len;i++){
        if(last_msg.TimeStamp >= temp_cp_msg[i].TimeStamp)    continue;       //如果不是最新的报文则跳过
        memcpy(&last_msg,&temp_cp_msg[i],sizeof(VCI_CAN_OBJ));

        for(j=0;j<msg_id_list.len;j++){
            if(last_msg.ID == msg_id_list.id[j]) break;
        }
        if(j==msg_id_list.len){
            msg_id_list.id[j] = last_msg.ID;
            msg_id_list.len++;
            temp_str = QString("%1").arg(last_msg.ID,8,16,QChar('0'));
            temp_str = temp_str.toUpper();
            ui->lw_fid->addItem(temp_str);
        }
        if(last_msg.ID == filter_id || filter_id==0){
            temp_str = QString("%1: ").arg(last_msg.ID,8,16,QChar('0'));
            for(int k=0;k<last_msg.DataLen;k++){
                temp_str += QString("%1 ").arg(last_msg.Data[k],2,16,QChar('0'));
            }
            temp_str = temp_str.toUpper();
            temp_array.push_back(temp_str);
            arry_len++;
        }
        if(ui->lw_msg->count() > LIST_WIDGET_MSG_LEN_MAX){
//            for(j=0;j<ui->lw_msg->count() - LIST_WIDGET_MSG_LEN_MAX;j++)
                ui->lw_msg->takeItem(0);
        }

    }

    ui->lw_msg->addItems(temp_array);
    temp_array.clear();
    if(tempstamp < can_data.len)
        ui->lw_msg->scrollToBottom();
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
}
