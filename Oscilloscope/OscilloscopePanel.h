/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef OscilloscopePanel_H
#define OscilloscopePanel_H

#include "OscilloscopePanelUI.h"
#include "OscilloscopeCore.h"
#include "OscilloscopeChannel.h"
#include "OscilloscopeChannelConfigWidget.h"
#include <QPoint>
#include <QString>

class OscilloscopePanel : public QWidget
{
    Q_OBJECT
public:
    OscilloscopePanel(OscilloscopeCore* core, QWidget * parent = NULL, Qt::WindowFlags f = NULL);
    ~OscilloscopePanel();

    typedef OscilloscopeChannelConfigWidget ScopeChCfgWidget ;

    OscilloscopeCore* core(){return Core;}
    bool isValidPanel(){return SelectedCoreSuccessfully;}


    bool hasAdvancedChCfg() ;
    bool isChCfgWindowActive() ;
    int  cfgWidgetsCount(){return CfgWidgetsCount;}

    Ui::oscilloscopePanel* ui(){return &UI;}

    void setChannelConfigWindowTitle(QString str) {ChannelConfigWindowTitle=str;}
    QString channelConfigWindowTitle() {return ChannelConfigWindowTitle;}

public slots:

    // UI slots

    // Extra functions related .
    void setChannelsAmount() ;              // for channelsAmountButton
    void expandBuffer();                    // for expandBufferButton
    void changeColor() ;                    // for changeColorButton
    void resetAllChannels();                // for resetAllChannelsButton
    void lockAllChannels();                // for lockAllChannelsButton
    void setXScaleLines();                  // for xScaleLinesLineEdit
    void setYScaleLines();                  // for yScaleLinesLineEdit
    void setTimeStampSync(int state);       // for timeStampSyncCheckBox
    void setAdvancedChannelConfig(int state);//for advancedChannelConfigCheckBox
    void showChannelConfig(int state);               // for showChannelConfigCheckBox
    void configWidgetsAmount();             // for configWidgetsAmountLineEdit

    // X-axis related .
    void setXRange(int value) ;             // for xRangeSlider
    void setXRange() ;                      // for xRangeLineEdit
    void setXOffset(int value) ;            // for xOffsetSlider
    void setXOffset() ;                     // for xOffsetLineEdit

    // UI slots over .

private:
    void deInit();
    void updateCoreInformation();
    void updatePanelInformation();

    void releaseChCfgWindow();
    void constructChCfgWindow();


    void connectXaxisRelatedSlots() ;
    void disconnectXaxisRelatedSlots() ;
\
    void connectExtraFuntionsRelatedSlots();
    void disconnectExtraFuntionsRelatedSlots();

private:
    Ui::oscilloscopePanel UI ;
    OscilloscopeCore* Core ;
    bool SelectedCoreSuccessfully ;

    QWidget* ChannelConfigWindow ;
    ScopeChCfgWidget** CfgWidgets ;
    QString  ChannelConfigWindowTitle ;

    int CfgWidgetsCount ;
    bool ChCfgWindowShown ;
    bool AdvancedChCfgSelected ;
    QPoint ChCfgWindowPos ;

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // UI_WIDGET_H
