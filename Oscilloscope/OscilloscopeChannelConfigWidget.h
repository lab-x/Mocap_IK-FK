/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef OscilloscopeChannelConfigWidget_H
#define OscilloscopeChannelConfigWidget_H

#include "OscilloscopeChannelConfigWidgetUI.h"
#include "OscilloscopeCore.h"
#include "OscilloscopeChannel.h"

class OscilloscopeChannelConfigWidget : public QWidget
{
    Q_OBJECT
public:
    OscilloscopeChannelConfigWidget(OscilloscopeCore* core, bool advance ,
                       QWidget * parent = NULL, Qt::WindowFlags f = NULL);
    ~OscilloscopeChannelConfigWidget();

    OscilloscopeCore* core(){return Core;}

    // SelectedChannel 等于0表示没有选中任何通道。
    int selectedChannel(){return SelectedChannel;}

    Ui::oscilloscopeChannelConfigWidget* ui(){return &UI;}

public slots:
    // UI slots
    void switchChannnel (int index ) ;      // for channelComboBox
    void switchShow (int state) ;           // for showChannelCheckBox
    void setYRange () ;                    // for yRangeLineEdit
    void setYRef () ;                       // for yRefLineEdit

    void setYOffset(int value) ;            // for yOffsetSlider
    void setYOffset();                      // for yOffsetLineEdit

    void setYStretch(int value) ;           // for yStretchSlider
    void setYStretch();                     // for yStretchLineEdit

    void setDescription();                  // for descripLineEdit .

    void setSampleType(int index) ;         // for sampleTypeComboBox
    void channelReset() ;                    // for channelResetButton
    void channelLock();                      // for channelLockButton
    void setShowDelay();                    // for showDelayLineEdit

    void updateChannelComboBox();

private:
    void deInit();
    void updateChannelInformation();

    void setDefault() ;

    void setGroupBoxTitle(QString &str);


    void connectChannelRelatedSlots() ;
    void disconnectChannelRelatedSlots() ;

private:
    Ui::oscilloscopeChannelConfigWidget UI ;
    OscilloscopeCore* Core ;
    bool Advance ;

    int SelectedChannel ;


private:

    static const int SampleTypeListSize = 8 ;
    // 与 sampleTypeComboBox下拉框中的内容顺序一致。具体定义见源文件
    static const OscilloscopeChannel::OriginalDataType SampleTypeList[SampleTypeListSize] ;

    int findSampleTypeIndex(OscilloscopeChannel::OriginalDataType type)
    {
        int i;
        for(i=0;i<SampleTypeListSize;i++)
        {
            if ( SampleTypeList[i] == type )
            {
                return i ;
            }
        }
    }
};

#endif
