#include "tkbmwidget.h"
#include "ui_tkbmwidget.h"

TkbmWidget::TkbmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TkbmWidget)
{
    ui->setupUi(this);
}

TkbmWidget::~TkbmWidget()
{
    delete ui;
}
