/********************************************************************************
** Form generated from reading UI file 'WatchDialog.ui'
**
** Created: Sun 24. Jun 10:21:13 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WATCHDIALOG_H
#define UI_WATCHDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *labelDate0;
    QLabel *labelTime0;
    QLabel *labelTime1;
    QLabel *label_3;
    QLabel *labelDate1;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *labelPriceMax;
    QLabel *label_6;
    QLabel *labelVolumeSum;
    QLabel *labelPriceMin;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *labelPriceDif;
    QLabel *labelVolumeSup;
    QLabel *label_9;
    QLabel *labelVolumeDem;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *labelProfit;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *labelDensitySup;
    QLabel *labelDensityDem;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(136, 345);
        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 20, 51, 16));
        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 40, 51, 16));
        labelDate0 = new QLabel(Dialog);
        labelDate0->setObjectName(QString::fromUtf8("labelDate0"));
        labelDate0->setGeometry(QRect(60, 20, 61, 16));
        labelTime0 = new QLabel(Dialog);
        labelTime0->setObjectName(QString::fromUtf8("labelTime0"));
        labelTime0->setGeometry(QRect(60, 40, 61, 16));
        labelTime1 = new QLabel(Dialog);
        labelTime1->setObjectName(QString::fromUtf8("labelTime1"));
        labelTime1->setGeometry(QRect(60, 100, 61, 16));
        label_3 = new QLabel(Dialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 100, 51, 16));
        labelDate1 = new QLabel(Dialog);
        labelDate1->setObjectName(QString::fromUtf8("labelDate1"));
        labelDate1->setGeometry(QRect(60, 70, 61, 16));
        label_4 = new QLabel(Dialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(0, 70, 51, 16));
        label_5 = new QLabel(Dialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(0, 130, 46, 14));
        labelPriceMax = new QLabel(Dialog);
        labelPriceMax->setObjectName(QString::fromUtf8("labelPriceMax"));
        labelPriceMax->setGeometry(QRect(70, 130, 61, 16));
        label_6 = new QLabel(Dialog);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(0, 200, 41, 16));
        labelVolumeSum = new QLabel(Dialog);
        labelVolumeSum->setObjectName(QString::fromUtf8("labelVolumeSum"));
        labelVolumeSum->setGeometry(QRect(50, 200, 61, 16));
        labelPriceMin = new QLabel(Dialog);
        labelPriceMin->setObjectName(QString::fromUtf8("labelPriceMin"));
        labelPriceMin->setGeometry(QRect(70, 150, 61, 16));
        label_7 = new QLabel(Dialog);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(0, 150, 46, 14));
        label_8 = new QLabel(Dialog);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(0, 170, 51, 16));
        labelPriceDif = new QLabel(Dialog);
        labelPriceDif->setObjectName(QString::fromUtf8("labelPriceDif"));
        labelPriceDif->setGeometry(QRect(70, 170, 61, 16));
        labelVolumeSup = new QLabel(Dialog);
        labelVolumeSup->setObjectName(QString::fromUtf8("labelVolumeSup"));
        labelVolumeSup->setGeometry(QRect(50, 220, 61, 16));
        label_9 = new QLabel(Dialog);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(0, 240, 41, 16));
        labelVolumeDem = new QLabel(Dialog);
        labelVolumeDem->setObjectName(QString::fromUtf8("labelVolumeDem"));
        labelVolumeDem->setGeometry(QRect(50, 240, 61, 16));
        label_10 = new QLabel(Dialog);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(0, 220, 41, 16));
        label_11 = new QLabel(Dialog);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(0, 270, 41, 16));
        labelProfit = new QLabel(Dialog);
        labelProfit->setObjectName(QString::fromUtf8("labelProfit"));
        labelProfit->setGeometry(QRect(50, 270, 61, 16));
        label_12 = new QLabel(Dialog);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(0, 290, 51, 16));
        label_13 = new QLabel(Dialog);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(0, 310, 51, 16));
        labelDensitySup = new QLabel(Dialog);
        labelDensitySup->setObjectName(QString::fromUtf8("labelDensitySup"));
        labelDensitySup->setGeometry(QRect(60, 290, 71, 16));
        labelDensityDem = new QLabel(Dialog);
        labelDensityDem->setObjectName(QString::fromUtf8("labelDensityDem"));
        labelDensityDem->setGeometry(QRect(60, 310, 71, 16));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "Date0", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "Time0", 0, QApplication::UnicodeUTF8));
        labelDate0->setText(QApplication::translate("Dialog", "Date0", 0, QApplication::UnicodeUTF8));
        labelTime0->setText(QApplication::translate("Dialog", "Time0", 0, QApplication::UnicodeUTF8));
        labelTime1->setText(QApplication::translate("Dialog", "Time1", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Dialog", "Time1", 0, QApplication::UnicodeUTF8));
        labelDate1->setText(QApplication::translate("Dialog", "Date1", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Dialog", "Date1", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Dialog", "Price Max", 0, QApplication::UnicodeUTF8));
        labelPriceMax->setText(QApplication::translate("Dialog", "Price0", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Dialog", "Volume", 0, QApplication::UnicodeUTF8));
        labelVolumeSum->setText(QApplication::translate("Dialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        labelPriceMin->setText(QApplication::translate("Dialog", "Price0", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Dialog", "Price Min", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Dialog", "Price Dif,%", 0, QApplication::UnicodeUTF8));
        labelPriceDif->setText(QApplication::translate("Dialog", "Price0", 0, QApplication::UnicodeUTF8));
        labelVolumeSup->setText(QApplication::translate("Dialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Dialog", "Vol Dem", 0, QApplication::UnicodeUTF8));
        labelVolumeDem->setText(QApplication::translate("Dialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Dialog", "Vol Sup", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Dialog", "Profit", 0, QApplication::UnicodeUTF8));
        labelProfit->setText(QApplication::translate("Dialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Dialog", "Dens Sup", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("Dialog", "Dens Dem", 0, QApplication::UnicodeUTF8));
        labelDensitySup->setText(QApplication::translate("Dialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        labelDensityDem->setText(QApplication::translate("Dialog", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WATCHDIALOG_H
