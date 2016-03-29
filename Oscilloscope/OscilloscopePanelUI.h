/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OSCILLOSCOPEPANEL_H
#define UI_OSCILLOSCOPEPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_oscilloscopePanel
{
public:
    QGroupBox *xAxisConfigGroupBox;
    QWidget *layoutWidget;
    QVBoxLayout *xAxisConfigGroupBoxLayout;
    QHBoxLayout *xRangeLayout;
    QLabel *xRangeLabel;
    QLineEdit *xRangeLineEdit;
    QSlider *xRangeSlider;
    QHBoxLayout *xOffsetLayout;
    QLabel *xOffsetLabel;
    QLineEdit *xOffsetLineEdit;
    QSlider *xOffsetSlider;
    QGroupBox *magicFunctionsGroupBox;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *magicFunctionsGroupBoxLayout;
    QLabel *justCallMeLabel;
    QHBoxLayout *channelsAmountLayout;
    QLineEdit *channelAmountLineEdit;
    QPushButton *channelsAmountButton;
    QSpacerItem *verticalSpacer;
    QPushButton *expandBufferButton;
    QPushButton *changeColorButton;
    QPushButton *resetAllChannelsButton;
    QPushButton *lockAllChannelsButton;
    QHBoxLayout *xScaleLinesLayout;
    QLabel *xScaleLinesLabel;
    QLineEdit *xScaleLinesLineEdit;
    QHBoxLayout *yScaleLinesLayout;
    QLabel *yScaleLinesLabel;
    QLineEdit *yScaleLinesLineEdit;
    QCheckBox *timeStampSyncCheckBox;
    QCheckBox *advancedChannelConfigCheckBox;
    QCheckBox *showChannelConfigCheckBox;
    QHBoxLayout *configWidgetsAmountLayout;
    QLabel *configWidgetsAmountLabel;
    QLineEdit *configWidgetsAmountLineEdit;

    void setupUi(QWidget *oscilloscopePanel)
    {
        if (oscilloscopePanel->objectName().isEmpty())
            oscilloscopePanel->setObjectName(QStringLiteral("oscilloscopePanel"));
        oscilloscopePanel->resize(290, 610);
        oscilloscopePanel->setMinimumSize(QSize(290, 610));
        oscilloscopePanel->setMaximumSize(QSize(290, 610));
        xAxisConfigGroupBox = new QGroupBox(oscilloscopePanel);
        xAxisConfigGroupBox->setObjectName(QStringLiteral("xAxisConfigGroupBox"));
        xAxisConfigGroupBox->setGeometry(QRect(10, 450, 271, 151));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(xAxisConfigGroupBox->sizePolicy().hasHeightForWidth());
        xAxisConfigGroupBox->setSizePolicy(sizePolicy);
        layoutWidget = new QWidget(xAxisConfigGroupBox);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(11, 20, 241, 124));
        xAxisConfigGroupBoxLayout = new QVBoxLayout(layoutWidget);
        xAxisConfigGroupBoxLayout->setSpacing(6);
        xAxisConfigGroupBoxLayout->setContentsMargins(11, 11, 11, 11);
        xAxisConfigGroupBoxLayout->setObjectName(QStringLiteral("xAxisConfigGroupBoxLayout"));
        xAxisConfigGroupBoxLayout->setContentsMargins(0, 0, 0, 0);
        xRangeLayout = new QHBoxLayout();
        xRangeLayout->setSpacing(6);
        xRangeLayout->setObjectName(QStringLiteral("xRangeLayout"));
        xRangeLabel = new QLabel(layoutWidget);
        xRangeLabel->setObjectName(QStringLiteral("xRangeLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(xRangeLabel->sizePolicy().hasHeightForWidth());
        xRangeLabel->setSizePolicy(sizePolicy1);
        xRangeLabel->setMinimumSize(QSize(0, 20));
        xRangeLabel->setMaximumSize(QSize(16777215, 20));

        xRangeLayout->addWidget(xRangeLabel);

        xRangeLineEdit = new QLineEdit(layoutWidget);
        xRangeLineEdit->setObjectName(QStringLiteral("xRangeLineEdit"));
        sizePolicy1.setHeightForWidth(xRangeLineEdit->sizePolicy().hasHeightForWidth());
        xRangeLineEdit->setSizePolicy(sizePolicy1);
        xRangeLineEdit->setMinimumSize(QSize(0, 20));
        xRangeLineEdit->setMaximumSize(QSize(16777215, 20));

        xRangeLayout->addWidget(xRangeLineEdit);


        xAxisConfigGroupBoxLayout->addLayout(xRangeLayout);

        xRangeSlider = new QSlider(layoutWidget);
        xRangeSlider->setObjectName(QStringLiteral("xRangeSlider"));
        xRangeSlider->setMinimumSize(QSize(0, 30));
        xRangeSlider->setMaximumSize(QSize(16777215, 30));
        xRangeSlider->setOrientation(Qt::Horizontal);

        xAxisConfigGroupBoxLayout->addWidget(xRangeSlider);

        xOffsetLayout = new QHBoxLayout();
        xOffsetLayout->setSpacing(6);
        xOffsetLayout->setObjectName(QStringLiteral("xOffsetLayout"));
        xOffsetLabel = new QLabel(layoutWidget);
        xOffsetLabel->setObjectName(QStringLiteral("xOffsetLabel"));
        sizePolicy1.setHeightForWidth(xOffsetLabel->sizePolicy().hasHeightForWidth());
        xOffsetLabel->setSizePolicy(sizePolicy1);
        xOffsetLabel->setMinimumSize(QSize(0, 20));
        xOffsetLabel->setMaximumSize(QSize(16777215, 20));

        xOffsetLayout->addWidget(xOffsetLabel);

        xOffsetLineEdit = new QLineEdit(layoutWidget);
        xOffsetLineEdit->setObjectName(QStringLiteral("xOffsetLineEdit"));
        sizePolicy1.setHeightForWidth(xOffsetLineEdit->sizePolicy().hasHeightForWidth());
        xOffsetLineEdit->setSizePolicy(sizePolicy1);
        xOffsetLineEdit->setMinimumSize(QSize(0, 20));
        xOffsetLineEdit->setMaximumSize(QSize(16777215, 20));

        xOffsetLayout->addWidget(xOffsetLineEdit);


        xAxisConfigGroupBoxLayout->addLayout(xOffsetLayout);

        xOffsetSlider = new QSlider(layoutWidget);
        xOffsetSlider->setObjectName(QStringLiteral("xOffsetSlider"));
        xOffsetSlider->setMinimumSize(QSize(0, 30));
        xOffsetSlider->setMaximumSize(QSize(16777215, 30));
        xOffsetSlider->setOrientation(Qt::Horizontal);

        xAxisConfigGroupBoxLayout->addWidget(xOffsetSlider);

        magicFunctionsGroupBox = new QGroupBox(oscilloscopePanel);
        magicFunctionsGroupBox->setObjectName(QStringLiteral("magicFunctionsGroupBox"));
        magicFunctionsGroupBox->setGeometry(QRect(10, 10, 271, 411));
        sizePolicy.setHeightForWidth(magicFunctionsGroupBox->sizePolicy().hasHeightForWidth());
        magicFunctionsGroupBox->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(magicFunctionsGroupBox);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        magicFunctionsGroupBoxLayout = new QVBoxLayout();
        magicFunctionsGroupBoxLayout->setSpacing(6);
        magicFunctionsGroupBoxLayout->setObjectName(QStringLiteral("magicFunctionsGroupBoxLayout"));
        justCallMeLabel = new QLabel(magicFunctionsGroupBox);
        justCallMeLabel->setObjectName(QStringLiteral("justCallMeLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(justCallMeLabel->sizePolicy().hasHeightForWidth());
        justCallMeLabel->setSizePolicy(sizePolicy2);
        justCallMeLabel->setMinimumSize(QSize(0, 20));
        justCallMeLabel->setMaximumSize(QSize(16777215, 20));

        magicFunctionsGroupBoxLayout->addWidget(justCallMeLabel);

        channelsAmountLayout = new QHBoxLayout();
        channelsAmountLayout->setSpacing(6);
        channelsAmountLayout->setObjectName(QStringLiteral("channelsAmountLayout"));
        channelAmountLineEdit = new QLineEdit(magicFunctionsGroupBox);
        channelAmountLineEdit->setObjectName(QStringLiteral("channelAmountLineEdit"));
        sizePolicy1.setHeightForWidth(channelAmountLineEdit->sizePolicy().hasHeightForWidth());
        channelAmountLineEdit->setSizePolicy(sizePolicy1);
        channelAmountLineEdit->setMinimumSize(QSize(0, 20));
        channelAmountLineEdit->setMaximumSize(QSize(16777215, 20));

        channelsAmountLayout->addWidget(channelAmountLineEdit);

        channelsAmountButton = new QPushButton(magicFunctionsGroupBox);
        channelsAmountButton->setObjectName(QStringLiteral("channelsAmountButton"));
        channelsAmountButton->setMinimumSize(QSize(0, 30));

        channelsAmountLayout->addWidget(channelsAmountButton);


        magicFunctionsGroupBoxLayout->addLayout(channelsAmountLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        magicFunctionsGroupBoxLayout->addItem(verticalSpacer);

        expandBufferButton = new QPushButton(magicFunctionsGroupBox);
        expandBufferButton->setObjectName(QStringLiteral("expandBufferButton"));
        expandBufferButton->setMinimumSize(QSize(0, 30));

        magicFunctionsGroupBoxLayout->addWidget(expandBufferButton);

        changeColorButton = new QPushButton(magicFunctionsGroupBox);
        changeColorButton->setObjectName(QStringLiteral("changeColorButton"));
        changeColorButton->setMinimumSize(QSize(0, 30));

        magicFunctionsGroupBoxLayout->addWidget(changeColorButton);

        resetAllChannelsButton = new QPushButton(magicFunctionsGroupBox);
        resetAllChannelsButton->setObjectName(QStringLiteral("resetAllChannelsButton"));
        resetAllChannelsButton->setMinimumSize(QSize(0, 30));

        magicFunctionsGroupBoxLayout->addWidget(resetAllChannelsButton);


        lockAllChannelsButton = new QPushButton(magicFunctionsGroupBox);
        lockAllChannelsButton->setObjectName(QStringLiteral("lockAllChannelsButton"));
        lockAllChannelsButton->setMinimumSize(QSize(0, 30));

        magicFunctionsGroupBoxLayout->addWidget(lockAllChannelsButton);


        xScaleLinesLayout = new QHBoxLayout();
        xScaleLinesLayout->setSpacing(6);
        xScaleLinesLayout->setObjectName(QStringLiteral("xScaleLinesLayout"));
        xScaleLinesLabel = new QLabel(magicFunctionsGroupBox);
        xScaleLinesLabel->setObjectName(QStringLiteral("xScaleLinesLabel"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(20);
        sizePolicy3.setHeightForWidth(xScaleLinesLabel->sizePolicy().hasHeightForWidth());
        xScaleLinesLabel->setSizePolicy(sizePolicy3);
        xScaleLinesLabel->setMinimumSize(QSize(0, 20));

        xScaleLinesLayout->addWidget(xScaleLinesLabel);

        xScaleLinesLineEdit = new QLineEdit(magicFunctionsGroupBox);
        xScaleLinesLineEdit->setObjectName(QStringLiteral("xScaleLinesLineEdit"));
        sizePolicy1.setHeightForWidth(xScaleLinesLineEdit->sizePolicy().hasHeightForWidth());
        xScaleLinesLineEdit->setSizePolicy(sizePolicy1);
        xScaleLinesLineEdit->setMinimumSize(QSize(0, 20));
        xScaleLinesLineEdit->setMaximumSize(QSize(16777215, 20));

        xScaleLinesLayout->addWidget(xScaleLinesLineEdit);


        magicFunctionsGroupBoxLayout->addLayout(xScaleLinesLayout);

        yScaleLinesLayout = new QHBoxLayout();
        yScaleLinesLayout->setSpacing(6);
        yScaleLinesLayout->setObjectName(QStringLiteral("yScaleLinesLayout"));
        yScaleLinesLabel = new QLabel(magicFunctionsGroupBox);
        yScaleLinesLabel->setObjectName(QStringLiteral("yScaleLinesLabel"));
        sizePolicy3.setHeightForWidth(yScaleLinesLabel->sizePolicy().hasHeightForWidth());
        yScaleLinesLabel->setSizePolicy(sizePolicy3);
        yScaleLinesLabel->setMinimumSize(QSize(0, 20));

        yScaleLinesLayout->addWidget(yScaleLinesLabel);

        yScaleLinesLineEdit = new QLineEdit(magicFunctionsGroupBox);
        yScaleLinesLineEdit->setObjectName(QStringLiteral("yScaleLinesLineEdit"));
        sizePolicy1.setHeightForWidth(yScaleLinesLineEdit->sizePolicy().hasHeightForWidth());
        yScaleLinesLineEdit->setSizePolicy(sizePolicy1);
        yScaleLinesLineEdit->setMinimumSize(QSize(0, 20));
        yScaleLinesLineEdit->setMaximumSize(QSize(16777215, 20));

        yScaleLinesLayout->addWidget(yScaleLinesLineEdit);


        magicFunctionsGroupBoxLayout->addLayout(yScaleLinesLayout);

        timeStampSyncCheckBox = new QCheckBox(magicFunctionsGroupBox);
        timeStampSyncCheckBox->setObjectName(QStringLiteral("timeStampSyncCheckBox"));
        sizePolicy2.setHeightForWidth(timeStampSyncCheckBox->sizePolicy().hasHeightForWidth());
        timeStampSyncCheckBox->setSizePolicy(sizePolicy2);

        magicFunctionsGroupBoxLayout->addWidget(timeStampSyncCheckBox);

        advancedChannelConfigCheckBox = new QCheckBox(magicFunctionsGroupBox);
        advancedChannelConfigCheckBox->setObjectName(QStringLiteral("advancedChannelConfigCheckBox"));

        magicFunctionsGroupBoxLayout->addWidget(advancedChannelConfigCheckBox);

        showChannelConfigCheckBox = new QCheckBox(magicFunctionsGroupBox);
        showChannelConfigCheckBox->setObjectName(QStringLiteral("showChannelConfigCheckBox"));

        magicFunctionsGroupBoxLayout->addWidget(showChannelConfigCheckBox);

        configWidgetsAmountLayout = new QHBoxLayout();
        configWidgetsAmountLayout->setSpacing(6);
        configWidgetsAmountLayout->setObjectName(QStringLiteral("configWidgetsAmountLayout"));
        configWidgetsAmountLabel = new QLabel(magicFunctionsGroupBox);
        configWidgetsAmountLabel->setObjectName(QStringLiteral("configWidgetsAmountLabel"));
        sizePolicy3.setHeightForWidth(configWidgetsAmountLabel->sizePolicy().hasHeightForWidth());
        configWidgetsAmountLabel->setSizePolicy(sizePolicy3);
        configWidgetsAmountLabel->setMinimumSize(QSize(0, 20));

        configWidgetsAmountLayout->addWidget(configWidgetsAmountLabel);

        configWidgetsAmountLineEdit = new QLineEdit(magicFunctionsGroupBox);
        configWidgetsAmountLineEdit->setObjectName(QStringLiteral("configWidgetsAmountLineEdit"));
        sizePolicy1.setHeightForWidth(configWidgetsAmountLineEdit->sizePolicy().hasHeightForWidth());
        configWidgetsAmountLineEdit->setSizePolicy(sizePolicy1);
        configWidgetsAmountLineEdit->setMinimumSize(QSize(0, 20));
        configWidgetsAmountLineEdit->setMaximumSize(QSize(16777215, 20));

        configWidgetsAmountLayout->addWidget(configWidgetsAmountLineEdit);


        magicFunctionsGroupBoxLayout->addLayout(configWidgetsAmountLayout);


        verticalLayout_2->addLayout(magicFunctionsGroupBoxLayout);


        retranslateUi(oscilloscopePanel);

        QMetaObject::connectSlotsByName(oscilloscopePanel);
    } // setupUi

    void retranslateUi(QWidget *oscilloscopePanel)
    {
        oscilloscopePanel->setWindowTitle(QApplication::translate("oscilloscopePanel", "Widget", 0));
        xAxisConfigGroupBox->setTitle(QApplication::translate("oscilloscopePanel", "X-axis Configure", 0));
        xRangeLabel->setText(QApplication::translate("oscilloscopePanel", "XRange", 0));
        xOffsetLabel->setText(QApplication::translate("oscilloscopePanel", "XOffset", 0));
        magicFunctionsGroupBox->setTitle(QApplication::translate("oscilloscopePanel", "Magic Powerful Suprising Functions ! ", 0));
        justCallMeLabel->setText(QApplication::translate("oscilloscopePanel", "Just tell me how many channels you want !", 0));
        channelsAmountButton->setText(QApplication::translate("oscilloscopePanel", "Give me channels Now!!", 0));
        expandBufferButton->setText(QApplication::translate("oscilloscopePanel", "I want to expand the buffer ", 0));
        changeColorButton->setText(QApplication::translate("oscilloscopePanel", "I want to change the Colors", 0));
        resetAllChannelsButton->setText(QApplication::translate("oscilloscopePanel", "Reset All Channels", 0));
        lockAllChannelsButton->setText(QApplication::translate("oscilloscopePanel", "Lock All Channels", 0));
        xScaleLinesLabel->setText(QApplication::translate("oscilloscopePanel", "Scale Lines in X", 0));
        yScaleLinesLabel->setText(QApplication::translate("oscilloscopePanel", "Scale Lines in Y", 0));
        timeStampSyncCheckBox->setText(QApplication::translate("oscilloscopePanel", "Time stamp sync in ALL channels", 0));
        advancedChannelConfigCheckBox->setText(QApplication::translate("oscilloscopePanel", "advanced channels-configure", 0));
        showChannelConfigCheckBox->setText(QApplication::translate("oscilloscopePanel", "show channels-configure window", 0));
        configWidgetsAmountLabel->setText(QApplication::translate("oscilloscopePanel", "amount of channel-config-widgets", 0));
    } // retranslateUi

};

namespace Ui {
    class oscilloscopePanel: public Ui_oscilloscopePanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OSCILLOSCOPEPANEL_H
