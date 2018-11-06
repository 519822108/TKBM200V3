#include "tkbmwidget.h"
#include "ui_tkbmwidget.h"
#include "iostream"
#include <QDesktopWidget>
#include <QAxObject>
#include <QFile>
#include <QMessageBox>
#include <QThread>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "string.h"


TkbmWidget::TkbmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TkbmWidget)
{
    ui->setupUi(this);
    QDesktopWidget *thisDesk = QApplication::desktop();
    QRect rect = thisDesk->availableGeometry();
    this->move((rect.width()-this->width())/2,(rect.height()-this->height())/2-20);
    this->ui_init();
    this->set_eep_config();

}

void TkbmWidget::ui_init(void)
{
    ui->tb_breif->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_breif->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m3->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m4->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m4->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_temp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_temp->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_disconnect->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_disconnect->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_version->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_version->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_run_state->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_run_state->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_chg_info->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_chg_info->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_chg_err->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_chg_err->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_need->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_need->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_ctl_info->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_ctl_info->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_each_hi->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_each_hi->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_each_lo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_each_lo->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_event->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_event->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_brief->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_brief->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_eep_file->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_eep_file->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_state->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_state->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_eep_content->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_eep_content->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TkbmWidget::set_eep_config(void)
{
    this->read_excel_data();
    this->set_eeprom_table();
}

void TkbmWidget::read_excel_data()
{
    QString file_path = QApplication::applicationDirPath() + FILE_PATH_CONFIG;
    QFile config_file(file_path);
    eep_config.len = 0;
    if(config_file.exists() == false){
        QMessageBox::information(this,"error",QString("缺少配置文件:\n[%1]\n程序关闭").arg(file_path));
        this->~TkbmWidget();
    }

    QAxObject excel("Excel.Application");
    excel.setProperty("Visible",false);
    QAxObject *pWorkbooks = excel.querySubObject("WorkBooks");
    pWorkbooks->dynamicCall("Open (Const QString&)",config_file.fileName());
    QAxObject *pWorkBook = excel.querySubObject("ActiveWorkBook");
    QAxObject *pSheet = pWorkBook->querySubObject("Sheets(int)",1);
    QAxObject *pCell;
    QVariant cell_val;
    QString val;
    do{
        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_NUM);     //获取参数编号
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        if(val.isEmpty() == true || val.isNull() == true){
            break;
        }
        eep_config.param[eep_config.len].s_num = val.toInt();

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_NAME);     //获取参数名
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].s_name = val;
   #if 0
        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_LEN);     //获取数据长度
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].o_byte_len = val.toInt();

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_FORMAT);     //获取显示格式
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].o_dis_format = val.toInt();
   #endif

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_NOTE);     //获取参数注释
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].s_note = val;

        eep_config.len++;
    }while(true);
    pWorkBook->dynamicCall("Close(Boolen)",false);
    excel.dynamicCall("Quit(void)");
}

void TkbmWidget::set_eeprom_table()
{
    int i;
    int row_num;
    for(i=0;i<eep_config.len;i++){
        row_num = ui->tb_eep_file->rowCount();
        ui->tb_eep_file->insertRow(row_num);
        ui->tb_eep_file->setVerticalHeaderItem(i,new QTableWidgetItem(QString("%1").arg(i)));
        ui->tb_eep_file->setItem(i,0,new QTableWidgetItem(eep_config.param[i].s_name));
    }
}

TkbmWidget::~TkbmWidget()
{
    delete ui;
}

void TkbmWidget::on_tb_eep_file_cellClicked(int row, int column)
{
    ui->tbs_note->setText(eep_config.param[row].s_note);
}
