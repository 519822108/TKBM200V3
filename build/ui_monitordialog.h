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
    QTextBrowser *tbr_msg;
    QListWidget *lw_fid;
    QLineEdit *lineEdit;
    QTextBrowser *tbr_id;

    void setupUi(QDialog *MonitorDialog)
    {
        if (MonitorDialog->objectName().isEmpty())
            MonitorDialog->setObjectName(QStringLiteral("MonitorDialog"));
        MonitorDialog->resize(360, 159);
        tbr_msg = new QTextBrowser(MonitorDialog);
        tbr_msg->setObjectName(QStringLiteral("tbr_msg"));
        tbr_msg->setGeometry(QRect(0, 0, 231, 160));
        lw_fid = new QListWidget(MonitorDialog);
        new QListWidgetItem(lw_fid);
        new QListWidgetItem(lw_fid);
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

        retranslateUi(MonitorDialog);

        QMetaObject::connectSlotsByName(MonitorDialog);
    } // setupUi

    void retranslateUi(QDialog *MonitorDialog)
    {
        MonitorDialog->setWindowTitle(QApplication::translate("MonitorDialog", "Dialog", Q_NULLPTR));
        tbr_msg->setHtml(QApplication::translate("MonitorDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0x19ff0804: 01 02 03 04 05 06 07 08</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", Q_NULLPTR));

        const bool __sortingEnabled = lw_fid->isSortingEnabled();
        lw_fid->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = lw_fid->item(1);
        ___qlistwidgetitem->setText(QApplication::translate("MonitorDialog", "0x19ff0805", Q_NULLPTR));
        QListWidgetItem *___qlistwidgetitem1 = lw_fid->item(2);
        ___qlistwidgetitem1->setText(QApplication::translate("MonitorDialog", "0x12345678", Q_NULLPTR));
        lw_fid->setSortingEnabled(__sortingEnabled);

        lineEdit->setText(QApplication::translate("MonitorDialog", "ID\350\241\250", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MonitorDialog: public Ui_MonitorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MONITORDIALOG_H
