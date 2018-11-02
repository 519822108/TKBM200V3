/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *cb_type;
    QLabel *label_2;
    QComboBox *cb_num;
    QLabel *label_3;
    QComboBox *cb_rate;
    QLabel *label_4;
    QComboBox *cb_port;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pb_ok;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QStringLiteral("LoginDialog"));
        LoginDialog->resize(275, 99);
        gridLayout = new QGridLayout(LoginDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(LoginDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        cb_type = new QComboBox(LoginDialog);
        cb_type->setObjectName(QStringLiteral("cb_type"));

        gridLayout->addWidget(cb_type, 0, 1, 1, 1);

        label_2 = new QLabel(LoginDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 2, 1, 1);

        cb_num = new QComboBox(LoginDialog);
        cb_num->setObjectName(QStringLiteral("cb_num"));

        gridLayout->addWidget(cb_num, 0, 3, 1, 1);

        label_3 = new QLabel(LoginDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        cb_rate = new QComboBox(LoginDialog);
        cb_rate->setObjectName(QStringLiteral("cb_rate"));

        gridLayout->addWidget(cb_rate, 1, 1, 1, 1);

        label_4 = new QLabel(LoginDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 1, 2, 1, 1);

        cb_port = new QComboBox(LoginDialog);
        cb_port->setObjectName(QStringLiteral("cb_port"));

        gridLayout->addWidget(cb_port, 1, 3, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(39, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pb_ok = new QPushButton(LoginDialog);
        pb_ok->setObjectName(QStringLiteral("pb_ok"));

        horizontalLayout->addWidget(pb_ok);

        horizontalSpacer_2 = new QSpacerItem(63, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 4);


        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QApplication::translate("LoginDialog", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("LoginDialog", "\350\256\276\345\244\207:", Q_NULLPTR));
        cb_type->clear();
        cb_type->insertItems(0, QStringList()
         << QApplication::translate("LoginDialog", "USB_CAN_E_U", Q_NULLPTR)
         << QApplication::translate("LoginDialog", "USB_CAN_2E_U", Q_NULLPTR)
        );
        label_2->setText(QApplication::translate("LoginDialog", "\347\274\226\345\217\267:", Q_NULLPTR));
        cb_num->clear();
        cb_num->insertItems(0, QStringList()
         << QApplication::translate("LoginDialog", "CAN0", Q_NULLPTR)
         << QApplication::translate("LoginDialog", "CAN1", Q_NULLPTR)
        );
        label_3->setText(QApplication::translate("LoginDialog", "\346\263\242\347\211\271\347\216\207:", Q_NULLPTR));
        cb_rate->clear();
        cb_rate->insertItems(0, QStringList()
         << QApplication::translate("LoginDialog", "250Kb/s", Q_NULLPTR)
         << QApplication::translate("LoginDialog", "125Kb/s", Q_NULLPTR)
         << QApplication::translate("LoginDialog", "500Kb/s", Q_NULLPTR)
        );
        label_4->setText(QApplication::translate("LoginDialog", "\347\253\257\345\217\243\345\217\267:", Q_NULLPTR));
        cb_port->clear();
        cb_port->insertItems(0, QStringList()
         << QApplication::translate("LoginDialog", "Port0", Q_NULLPTR)
         << QApplication::translate("LoginDialog", "Port1", Q_NULLPTR)
        );
        pb_ok->setText(QApplication::translate("LoginDialog", "\347\241\256\345\256\232", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
