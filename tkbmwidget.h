#ifndef TKBMWIDGET_H
#define TKBMWIDGET_H

#include <QWidget>
#include "logindialog.h"
#include "monitordialog.h"
#include "data_organize.h"

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

private slots:
    void on_tb_eep_file_cellClicked(int row, int column);

private:
    Ui::TkbmWidget *ui;
    LoginDialog login_dialog;
    MonitorDialog monitor_dialog;
    struct excel_param_organize_ext eep_config;
};

#endif // TKBMWIDGET_H
