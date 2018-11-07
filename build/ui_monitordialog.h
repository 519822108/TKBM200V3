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
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_MonitorDialog
{
public:
    QListWidget *lw_fid;
    QLineEdit *lineEdit;
    QTextBrowser *tbr_id;
    QListWidget *lw_msg;

    void setupUi(QDialog *MonitorDialog)
    {
        if (MonitorDialog->objectName().isEmpty())
            MonitorDialog->setObjectName(QStringLiteral("MonitorDialog"));
        MonitorDialog->resize(360, 159);
        lw_fid = new QListWidget(MonitorDialog);
        new QListWidgetItem(lw_fid);
        lw_fid->setObjectName(QStringLiteral("lw_fid"));
        lw_fid->setGeometry(QRect(230, 0, 91, 160));
        lineEdit = new QLineEdit(MonitorDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setEnabled(false);
        lineEdit->setGeometry(QRect(320, 0, 40, 20));
        lineEdit->setAlignment(Qt::AlignCenter);
        tbr_id = new QTextBrowser(MonitorDialog);
        tbr_id->setObjectName(QStringLiteral("tbr_id"));
        tbr_id->setGeometry(QRect(320, 20, 40, 140));
        lw_msg = new QListWidget(MonitorDialog);
        lw_msg->setObjectName(QStringLiteral("lw_msg"));
        lw_msg->setGeometry(QRect(0, 0, 230, 160));

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
