/********************************************************************************
** Form generated from reading UI file 'tkbmwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TKBMWIDGET_H
#define UI_TKBMWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TkbmWidget
{
public:

    void setupUi(QWidget *TkbmWidget)
    {
        if (TkbmWidget->objectName().isEmpty())
            TkbmWidget->setObjectName(QStringLiteral("TkbmWidget"));
        TkbmWidget->resize(400, 300);

        retranslateUi(TkbmWidget);

        QMetaObject::connectSlotsByName(TkbmWidget);
    } // setupUi

    void retranslateUi(QWidget *TkbmWidget)
    {
        TkbmWidget->setWindowTitle(QApplication::translate("TkbmWidget", "TkbmWidget", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TkbmWidget: public Ui_TkbmWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TKBMWIDGET_H
