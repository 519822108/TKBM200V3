/********************************************************************************
** Form generated from reading UI file 'monitordialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MONITORDIALOG_H
#define UI_MONITORDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_MonitorDialog
{
public:
    QListWidget *lw_fid;
    QLineEdit *lineEdit;
    QListWidget *lw_msg;
    QListWidget *lw_bid;

    void setupUi(QDialog *MonitorDialog)
    {
        if (MonitorDialog->objectName().isEmpty())
            MonitorDialog->setObjectName(QStringLiteral("MonitorDialog"));
        MonitorDialog->resize(339, 159);
        lw_fid = new QListWidget(MonitorDialog);
        new QListWidgetItem(lw_fid);
        lw_fid->setObjectName(QStringLiteral("lw_fid"));
        lw_fid->setGeometry(QRect(230, 0, 70, 160));
        lw_fid->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lw_fid->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lineEdit = new QLineEdit(MonitorDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setEnabled(false);
        lineEdit->setGeometry(QRect(300, 0, 40, 20));
        lineEdit->setAlignment(Qt::AlignCenter);
        lw_msg = new QListWidget(MonitorDialog);
        lw_msg->setObjectName(QStringLiteral("lw_msg"));
        lw_msg->setGeometry(QRect(0, 0, 230, 160));
        lw_msg->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lw_msg->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lw_bid = new QListWidget(MonitorDialog);
        lw_bid->setObjectName(QStringLiteral("lw_bid"));
        lw_bid->setGeometry(QRect(300, 20, 40, 140));
        lw_bid->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lw_bid->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        retranslateUi(MonitorDialog);

        QMetaObject::connectSlotsByName(MonitorDialog);
    } // setupUi

    void retranslateUi(QDialog *MonitorDialog)
    {
        MonitorDialog->setWindowTitle(QApplication::translate("MonitorDialog", "Dialog", Q_NULLPTR));

        const bool __sortingEnabled = lw_fid->isSortingEnabled();
        lw_fid->setSortingEnabled(false);
        lw_fid->setSortingEnabled(__sortingEnabled);

        lineEdit->setText(QApplication::translate("MonitorDialog", "ID\350\241\250", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MonitorDialog: public Ui_MonitorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MONITORDIALOG_H
