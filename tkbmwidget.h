#ifndef TKBMWIDGET_H
#define TKBMWIDGET_H

#include <QWidget>

namespace Ui {
class TkbmWidget;
}

class TkbmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TkbmWidget(QWidget *parent = nullptr);
    ~TkbmWidget();

private:
    Ui::TkbmWidget *ui;
};

#endif // TKBMWIDGET_H
