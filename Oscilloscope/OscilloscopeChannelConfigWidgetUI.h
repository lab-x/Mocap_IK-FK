/********************************************************************************
** Form generated from reading UI file 'o.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OSCILLOSCOPECHANNELCONFIGWIDGET_H
#define UI_OSCILLOSCOPECHANNELCONFIGWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
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

class Ui_oscilloscopeChannelConfigWidget
{
public:
    QGroupBox *channelGroupBox;
    QWidget *layoutWidget;
    QVBoxLayout *channelGroupBoxLayout;
    QHBoxLayout *channelSelectLayout;
    QComboBox *channelComboBox;
    QCheckBox *showChannelCheckBox;
    QHBoxLayout *yRangeAndRefLayout;
    QLabel *yRangeLabel;
    QLineEdit *yRangeLineEdit;
    QLabel *yRefLabel;
    QLineEdit *yRefLineEdit;
    QHBoxLayout *showDelayLayout;
    QLabel *showDelayLabel;
    QLineEdit *showDelayLineEdit;
    QHBoxLayout *yStretchAndYoffsetLayout;
    QVBoxLayout *yStretchLayout;
    QHBoxLayout *yStretchSubLayout1;
    QSpacerItem *yStretchSpacer1;
    QSlider *yStretchSlider;
    QSpacerItem *yStretchSpacer2;
    QHBoxLayout *yStretchSubLayout2;
    QLabel *yStretchLabel;
    QLineEdit *yStretchLineEdit;
    QVBoxLayout *yOffsetLayout;
    QHBoxLayout *yOffsetSubLayout1;
    QSpacerItem *yOffsetSpacer1;
    QSlider *yOffsetSlider;
    QSpacerItem *yOffsetSpacer2;
    QHBoxLayout *yOffsetSubLayout2;
    QLabel *yOffsetLabel;
    QLineEdit *yOffsetLineEdit;

    QHBoxLayout *descripLayout;
    QLabel *descripLabel;
    QLineEdit *descripLineEdit;

    QPushButton *channelResetButton;
    QPushButton *channelLockButton ;
    QHBoxLayout *sampleTypeSelectLayout;
    QLabel *sampleTypeLabel;
    QComboBox *sampleTypeComboBox;

    static const int HeightForNonAdvance = 220 ;
    static const int WidthForNonAdvance = 225 ;
    static const int HeightForAdvance = 346 ;
    static const int WidthForAdvance = 225 ;


    void setupUi(QWidget *oscilloscopeChannelConfigWidget,bool advanced=true)
    {
        setupUi_stage1(oscilloscopeChannelConfigWidget,advanced) ;

        if ( !advanced )
        {
            oscilloscopeChannelConfigWidget->resize(WidthForNonAdvance,HeightForNonAdvance);
            oscilloscopeChannelConfigWidget->setMinimumSize(WidthForNonAdvance,HeightForNonAdvance);
            oscilloscopeChannelConfigWidget->setMaximumSize(WidthForNonAdvance,HeightForNonAdvance);

            channelGroupBox->setGeometry(QRect(0, 0, 221, 220));
            layoutWidget->setGeometry(QRect(10, 20, 204, 195));
        }
        else
        {
            oscilloscopeChannelConfigWidget->resize(WidthForAdvance,HeightForAdvance);
            oscilloscopeChannelConfigWidget->setMinimumSize(WidthForAdvance,HeightForAdvance);
            oscilloscopeChannelConfigWidget->setMaximumSize(WidthForAdvance,HeightForAdvance);
        }

        retranslateUi(oscilloscopeChannelConfigWidget,advanced);
    }


    void setupUi_stage1(QWidget *oscilloscopeChannelConfigWidget,bool advanced)
    {
        if (oscilloscopeChannelConfigWidget->objectName().isEmpty())
            oscilloscopeChannelConfigWidget->setObjectName(QStringLiteral("oscilloscopeChannelConfigWidget"));
        oscilloscopeChannelConfigWidget->resize(225, 346);
        oscilloscopeChannelConfigWidget->setMinimumSize(QSize(0, 0));
        channelGroupBox = new QGroupBox(oscilloscopeChannelConfigWidget);
        channelGroupBox->setObjectName(QStringLiteral("channelGroupBox"));
        channelGroupBox->setGeometry(QRect(0, 0, 221, 341));
        layoutWidget = new QWidget(channelGroupBox);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 20, 204, 311));
        channelGroupBoxLayout = new QVBoxLayout(layoutWidget);
        channelGroupBoxLayout->setSpacing(6);
        channelGroupBoxLayout->setContentsMargins(11, 11, 11, 11);
        channelGroupBoxLayout->setObjectName(QStringLiteral("channelGroupBoxLayout"));
        channelGroupBoxLayout->setContentsMargins(0, 0, 0, 0);
        channelSelectLayout = new QHBoxLayout();
        channelSelectLayout->setSpacing(6);
        channelSelectLayout->setObjectName(QStringLiteral("channelSelectLayout"));
        channelComboBox = new QComboBox(layoutWidget);
        channelComboBox->setObjectName(QStringLiteral("channelComboBox"));
        channelComboBox->setMinimumSize(QSize(0, 20));
        channelComboBox->setMaximumSize(QSize(16777215, 20));

        channelSelectLayout->addWidget(channelComboBox);

        showChannelCheckBox = new QCheckBox(layoutWidget);
        showChannelCheckBox->setObjectName(QStringLiteral("showChannelCheckBox"));
        showChannelCheckBox->setMinimumSize(QSize(0, 20));
        showChannelCheckBox->setMaximumSize(QSize(16777215, 20));

        channelSelectLayout->addWidget(showChannelCheckBox);


        channelGroupBoxLayout->addLayout(channelSelectLayout);

        yRangeAndRefLayout = new QHBoxLayout();
        yRangeAndRefLayout->setSpacing(6);
        yRangeAndRefLayout->setObjectName(QStringLiteral("yRangeAndRefLayout"));
        yRangeLabel = new QLabel(layoutWidget);
        yRangeLabel->setObjectName(QStringLiteral("yRangeLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(yRangeLabel->sizePolicy().hasHeightForWidth());
        yRangeLabel->setSizePolicy(sizePolicy);
        yRangeLabel->setMinimumSize(QSize(0, 20));
        yRangeLabel->setMaximumSize(QSize(16777215, 20));

        yRangeAndRefLayout->addWidget(yRangeLabel);

        yRangeLineEdit = new QLineEdit(layoutWidget);
        yRangeLineEdit->setObjectName(QStringLiteral("yRangeLineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(yRangeLineEdit->sizePolicy().hasHeightForWidth());
        yRangeLineEdit->setSizePolicy(sizePolicy1);
        yRangeLineEdit->setMinimumSize(QSize(0, 20));
        yRangeLineEdit->setMaximumSize(QSize(16777215, 20));

        yRangeAndRefLayout->addWidget(yRangeLineEdit);

        yRefLabel = new QLabel(layoutWidget);
        yRefLabel->setObjectName(QStringLiteral("yRefLabel"));
        sizePolicy.setHeightForWidth(yRefLabel->sizePolicy().hasHeightForWidth());
        yRefLabel->setSizePolicy(sizePolicy);
        yRefLabel->setMinimumSize(QSize(0, 20));
        yRefLabel->setMaximumSize(QSize(16777215, 20));

        yRangeAndRefLayout->addWidget(yRefLabel);

        yRefLineEdit = new QLineEdit(layoutWidget);
        yRefLineEdit->setObjectName(QStringLiteral("yRefLineEdit"));
        sizePolicy1.setHeightForWidth(yRefLineEdit->sizePolicy().hasHeightForWidth());
        yRefLineEdit->setSizePolicy(sizePolicy1);
        yRefLineEdit->setMinimumSize(QSize(0, 20));
        yRefLineEdit->setMaximumSize(QSize(16777215, 20));

        yRangeAndRefLayout->addWidget(yRefLineEdit);


        channelGroupBoxLayout->addLayout(yRangeAndRefLayout);


        yStretchAndYoffsetLayout = new QHBoxLayout();
        yStretchAndYoffsetLayout->setSpacing(6);
        yStretchAndYoffsetLayout->setObjectName(QStringLiteral("yStretchAndYoffsetLayout"));
        yStretchLayout = new QVBoxLayout();
        yStretchLayout->setSpacing(6);
        yStretchLayout->setObjectName(QStringLiteral("yStretchLayout"));
        yStretchSubLayout1 = new QHBoxLayout();
        yStretchSubLayout1->setSpacing(6);
        yStretchSubLayout1->setObjectName(QStringLiteral("yStretchSubLayout1"));
        yStretchSpacer1 = new QSpacerItem(18, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        yStretchSubLayout1->addItem(yStretchSpacer1);

        yStretchSlider = new QSlider(layoutWidget);
        yStretchSlider->setObjectName(QStringLiteral("yStretchSlider"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(yStretchSlider->sizePolicy().hasHeightForWidth());
        yStretchSlider->setSizePolicy(sizePolicy2);
        yStretchSlider->setMinimumSize(QSize(50, 80));
        yStretchSlider->setMaximumSize(QSize(50, 90));
        yStretchSlider->setOrientation(Qt::Vertical);

        yStretchSubLayout1->addWidget(yStretchSlider);

        yStretchSpacer2 = new QSpacerItem(18, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        yStretchSubLayout1->addItem(yStretchSpacer2);


        yStretchLayout->addLayout(yStretchSubLayout1);

        yStretchSubLayout2 = new QHBoxLayout();
        yStretchSubLayout2->setSpacing(6);
        yStretchSubLayout2->setObjectName(QStringLiteral("yStretchSubLayout2"));
        yStretchLabel = new QLabel(layoutWidget);
        yStretchLabel->setObjectName(QStringLiteral("yStretchLabel"));
        sizePolicy1.setHeightForWidth(yStretchLabel->sizePolicy().hasHeightForWidth());
        yStretchLabel->setSizePolicy(sizePolicy1);
        yStretchLabel->setMinimumSize(QSize(0, 20));
        yStretchLabel->setMaximumSize(QSize(16777215, 20));

        yStretchSubLayout2->addWidget(yStretchLabel);

        yStretchLineEdit = new QLineEdit(layoutWidget);
        yStretchLineEdit->setObjectName(QStringLiteral("yStretchLineEdit"));
        sizePolicy1.setHeightForWidth(yStretchLineEdit->sizePolicy().hasHeightForWidth());
        yStretchLineEdit->setSizePolicy(sizePolicy1);
        yStretchLineEdit->setMinimumSize(QSize(0, 20));
        yStretchLineEdit->setMaximumSize(QSize(16777215, 20));

        yStretchSubLayout2->addWidget(yStretchLineEdit);


        yStretchLayout->addLayout(yStretchSubLayout2);


        yStretchAndYoffsetLayout->addLayout(yStretchLayout);

        yOffsetLayout = new QVBoxLayout();
        yOffsetLayout->setSpacing(6);
        yOffsetLayout->setObjectName(QStringLiteral("yOffsetLayout"));
        yOffsetSubLayout1 = new QHBoxLayout();
        yOffsetSubLayout1->setSpacing(6);
        yOffsetSubLayout1->setObjectName(QStringLiteral("yOffsetSubLayout1"));
        yOffsetSpacer1 = new QSpacerItem(18, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        yOffsetSubLayout1->addItem(yOffsetSpacer1);

        yOffsetSlider = new QSlider(layoutWidget);
        yOffsetSlider->setObjectName(QStringLiteral("yOffsetSlider"));
        sizePolicy2.setHeightForWidth(yOffsetSlider->sizePolicy().hasHeightForWidth());
        yOffsetSlider->setSizePolicy(sizePolicy2);
        yOffsetSlider->setMinimumSize(QSize(50, 80));
        yOffsetSlider->setMaximumSize(QSize(50, 90));
        yOffsetSlider->setOrientation(Qt::Vertical);

        yOffsetSubLayout1->addWidget(yOffsetSlider);

        yOffsetSpacer2 = new QSpacerItem(18, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        yOffsetSubLayout1->addItem(yOffsetSpacer2);


        yOffsetLayout->addLayout(yOffsetSubLayout1);

        yOffsetSubLayout2 = new QHBoxLayout();
        yOffsetSubLayout2->setSpacing(6);
        yOffsetSubLayout2->setObjectName(QStringLiteral("yOffsetSubLayout2"));
        yOffsetLabel = new QLabel(layoutWidget);
        yOffsetLabel->setObjectName(QStringLiteral("yOffsetLabel"));
        sizePolicy1.setHeightForWidth(yOffsetLabel->sizePolicy().hasHeightForWidth());
        yOffsetLabel->setSizePolicy(sizePolicy1);
        yOffsetLabel->setMinimumSize(QSize(0, 20));
        yOffsetLabel->setMaximumSize(QSize(16777215, 20));

        yOffsetSubLayout2->addWidget(yOffsetLabel);

        yOffsetLineEdit = new QLineEdit(layoutWidget);
        yOffsetLineEdit->setObjectName(QStringLiteral("yOffsetLineEdit"));
        sizePolicy1.setHeightForWidth(yOffsetLineEdit->sizePolicy().hasHeightForWidth());
        yOffsetLineEdit->setSizePolicy(sizePolicy1);
        yOffsetLineEdit->setMinimumSize(QSize(0, 20));
        yOffsetLineEdit->setMaximumSize(QSize(16777215, 20));

        yOffsetSubLayout2->addWidget(yOffsetLineEdit);


        yOffsetLayout->addLayout(yOffsetSubLayout2);


        yStretchAndYoffsetLayout->addLayout(yOffsetLayout);


        channelGroupBoxLayout->addLayout(yStretchAndYoffsetLayout);




        descripLayout = new QHBoxLayout();
        descripLayout->setSpacing(6);
        descripLayout->setObjectName(QStringLiteral("descripLayout"));
        descripLabel = new QLabel(layoutWidget);
        descripLabel->setObjectName(QStringLiteral("descripLabel"));
        sizePolicy.setHeightForWidth(descripLabel->sizePolicy().hasHeightForWidth());
        descripLabel->setSizePolicy(sizePolicy);
        descripLabel->setMinimumSize(QSize(0, 20));
        descripLabel->setMaximumSize(QSize(16777215, 20));

        descripLayout->addWidget(descripLabel);

        descripLineEdit = new QLineEdit(layoutWidget);
        descripLineEdit->setObjectName(QStringLiteral("descripLineEdit"));
        sizePolicy1.setHeightForWidth(descripLineEdit->sizePolicy().hasHeightForWidth());
        descripLineEdit->setSizePolicy(sizePolicy1);
        descripLineEdit->setMinimumSize(QSize(0, 20));
        descripLineEdit->setMaximumSize(QSize(16777215, 20));

        descripLayout->addWidget(descripLineEdit);


        channelGroupBoxLayout->addLayout(descripLayout);





if ( ! advanced )
    return ;


        showDelayLayout = new QHBoxLayout();
        showDelayLayout->setSpacing(6);
        showDelayLayout->setObjectName(QStringLiteral("showDelayLayout"));
        showDelayLabel = new QLabel(layoutWidget);
        showDelayLabel->setObjectName(QStringLiteral("showDelayLabel"));
        sizePolicy.setHeightForWidth(showDelayLabel->sizePolicy().hasHeightForWidth());
        showDelayLabel->setSizePolicy(sizePolicy);
        showDelayLabel->setMinimumSize(QSize(0, 20));
        showDelayLabel->setMaximumSize(QSize(16777215, 20));

        showDelayLayout->addWidget(showDelayLabel);

        showDelayLineEdit = new QLineEdit(layoutWidget);
        showDelayLineEdit->setObjectName(QStringLiteral("showDelayLineEdit"));
        sizePolicy1.setHeightForWidth(showDelayLineEdit->sizePolicy().hasHeightForWidth());
        showDelayLineEdit->setSizePolicy(sizePolicy1);
        showDelayLineEdit->setMinimumSize(QSize(0, 20));
        showDelayLineEdit->setMaximumSize(QSize(16777215, 20));

        showDelayLayout->addWidget(showDelayLineEdit);


        channelGroupBoxLayout->addLayout(showDelayLayout);


        channelResetButton = new QPushButton(layoutWidget);
        channelResetButton->setObjectName(QStringLiteral("channelResetButton"));
        channelResetButton->setMinimumSize(QSize(0, 30));
        channelResetButton->setMaximumSize(QSize(16777215, 30));

        channelGroupBoxLayout->addWidget(channelResetButton);

        channelLockButton = new QPushButton(layoutWidget);
        channelLockButton->setObjectName(QStringLiteral("channelLockButton"));
        channelLockButton->setMinimumSize(QSize(0, 30));
        channelLockButton->setMaximumSize(QSize(16777215, 30));

        channelGroupBoxLayout->addWidget(channelLockButton);


        sampleTypeSelectLayout = new QHBoxLayout();
        sampleTypeSelectLayout->setSpacing(6);
        sampleTypeSelectLayout->setObjectName(QStringLiteral("sampleTypeSelectLayout"));
        sampleTypeLabel = new QLabel(layoutWidget);
        sampleTypeLabel->setObjectName(QStringLiteral("sampleTypeLabel"));
        sizePolicy.setHeightForWidth(sampleTypeLabel->sizePolicy().hasHeightForWidth());
        sampleTypeLabel->setSizePolicy(sizePolicy);
        sampleTypeLabel->setMinimumSize(QSize(0, 20));
        sampleTypeLabel->setMaximumSize(QSize(16777215, 20));

        sampleTypeSelectLayout->addWidget(sampleTypeLabel);

        sampleTypeComboBox = new QComboBox(layoutWidget);
        sampleTypeComboBox->setObjectName(QStringLiteral("sampleTypeComboBox"));
        sampleTypeComboBox->setMinimumSize(QSize(0, 20));
        sampleTypeComboBox->setMaximumSize(QSize(16777215, 20));

        sampleTypeSelectLayout->addWidget(sampleTypeComboBox);


        channelGroupBoxLayout->addLayout(sampleTypeSelectLayout);

    } // setupUi

    void retranslateUi(QWidget *oscilloscopeChannelConfigWidget,bool advanced)
    {
        oscilloscopeChannelConfigWidget->setWindowTitle(QApplication::translate("oscilloscopeChannelConfigWidget", "Widget", 0));
        channelGroupBox->setTitle(QApplication::translate("oscilloscopeChannelConfigWidget", "GroupBox", 0));
        channelComboBox->clear();
        channelComboBox->insertItems(0, QStringList()
         << QApplication::translate("oscilloscopeChannelConfigWidget", "CH1", 0)
        );
        showChannelCheckBox->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "Show", 0));
        yRangeLabel->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "YRange", 0));
        yRefLabel->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "YRef  ", 0));
        yStretchLabel->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "YStretch", 0));
        yOffsetLabel->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "Yoffset ", 0));
        descripLabel->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "Description", 0));


if (!advanced)
    return ;
        showDelayLabel->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "ShowDelay", 0));
        channelResetButton->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "Reset Channel", 0));
        channelLockButton->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "Lock Channel", 0));
        sampleTypeLabel->setText(QApplication::translate("oscilloscopeChannelConfigWidget", "SampleType", 0));
        sampleTypeComboBox->clear();
        sampleTypeComboBox->insertItems(0, QStringList()
         << QApplication::translate("oscilloscopeChannelConfigWidget", "INT8", 0)
         << QApplication::translate("oscilloscopeChannelConfigWidget", "UINT8", 0)
         << QApplication::translate("oscilloscopeChannelConfigWidget", "INT16", 0)
         << QApplication::translate("oscilloscopeChannelConfigWidget", "UINT16", 0)
         << QApplication::translate("oscilloscopeChannelConfigWidget", "INT32", 0)
         << QApplication::translate("oscilloscopeChannelConfigWidget", "UINT32", 0)
         << QApplication::translate("oscilloscopeChannelConfigWidget", "FLOAT32", 0)
         << QApplication::translate("oscilloscopeChannelConfigWidget", "FLOAT64", 0)
        );
        sampleTypeComboBox->setCurrentText(QApplication::translate("oscilloscopeChannelConfigWidget", "INT8", 0));
    } // retranslateUi

};

namespace Ui {
    class oscilloscopeChannelConfigWidget: public Ui_oscilloscopeChannelConfigWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OSCILLOSCOPECHANNELCONFIGWIDGET_H
