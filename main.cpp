#include "tkbmwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TkbmWidget w;
    w.show();

    return a.exec();
}