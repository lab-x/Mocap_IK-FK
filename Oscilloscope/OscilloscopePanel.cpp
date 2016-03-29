
#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QMessageBox>
#include "OscilloscopePanel.h"


OscilloscopePanel::OscilloscopePanel(OscilloscopeCore* core ,
           QWidget * parent, Qt::WindowFlags f) : QWidget(parent,f)
{
    this->Core = core ;
    SelectedCoreSuccessfully = false ;
    ChannelConfigWindow = NULL ;
    CfgWidgets = NULL ;
    CfgWidgetsCount = 3 ;
    ChCfgWindowShown = false ;
    AdvancedChCfgSelected = false ;
    ChCfgWindowPos = QPoint(100,50);
    setChannelConfigWindowTitle(QString("channels configure"));

    if ( this->core()->selected() )
    {
        QMessageBox::warning(this,"error","failed to create an OscilloscopePanel , there may already exist such a panel for this scope core\n",QMessageBox::Ok,QMessageBox::Ok);
        deInit();
        return ;
    }
    else
    {
        this->core()->setSelect(true);
        SelectedCoreSuccessfully = true ;
    }


    UI.setupUi(this)  ;

    connectXaxisRelatedSlots();
    connectExtraFuntionsRelatedSlots();

    updateCoreInformation();
    updatePanelInformation();
}


void OscilloscopePanel::deInit()
{
    if ( SelectedCoreSuccessfully )
        this->core()->setSelect(false);
}


OscilloscopePanel::~OscilloscopePanel()
{
    releaseChCfgWindow();
    deInit();
}





void OscilloscopePanel::setChannelsAmount()               // for channelsAmountButton
{
    QMessageBox::information (this,"attention","this function is not supported in this version\n",QMessageBox::Ok,QMessageBox::Ok);
    UI.channelAmountLineEdit->setText(QString("%1").arg(core()->channelsCount()));
}

void OscilloscopePanel::expandBuffer()                    // for expandBufferButton
{
    QMessageBox::information (this,"attention","this function is not supported in this version\n",QMessageBox::Ok,QMessageBox::Ok);

}

void OscilloscopePanel::changeColor()              // for channelColorButton
{
    QMessageBox::information (this,"attention","this function is not supported in this version\n",QMessageBox::Ok,QMessageBox::Ok);
}


void OscilloscopePanel::resetAllChannels()                // for resetAllChannelsButton
{
    core()->resetAllChannels();
}

void OscilloscopePanel::lockAllChannels()                // for lockAllChannelsButton
{
    core()->lockAllChannels();
}


void OscilloscopePanel::setXScaleLines()                 // for xScaleLinesLineEdit
{
    int lines ;

    bool ok ;
    lines = UI.xScaleLinesLineEdit->text().toInt(&ok) ;
    if ( ok && lines > 0 )
    {
        core()->setScaleLinesInX(lines);
    }
    else
    {
        disconnect(UI.xScaleLinesLineEdit,SIGNAL(editingFinished()),this,SLOT(setXScaleLines()));

        QMessageBox::warning (this,"error","the number of scale lines must be a positive integer\n",QMessageBox::Ok,QMessageBox::Ok);
        UI.xScaleLinesLineEdit->setText(QString("%1").arg(core()->scaleLinesInX()));

        connect(UI.xScaleLinesLineEdit,SIGNAL(editingFinished()),this,SLOT(setXScaleLines()));
    }
}

void OscilloscopePanel::setYScaleLines()                  // for yScaleLinesLineEdit
{
    int lines ;
    bool ok ;
    lines = UI.yScaleLinesLineEdit->text().toInt(&ok) ;
    if ( ok && lines > 0 )
    {
        core()->setScaleLinesInY(lines);
    }
    else
    {
        disconnect(UI.yScaleLinesLineEdit,SIGNAL(editingFinished()),this,SLOT(setYScaleLines()));

        QMessageBox::warning (this,"error","the number of scale lines must be a positive integer\n",QMessageBox::Ok,QMessageBox::Ok);
        UI.yScaleLinesLineEdit->setText(QString("%1").arg(core()->scaleLinesInY()));

        connect(UI.xScaleLinesLineEdit,SIGNAL(editingFinished()),this,SLOT(setXScaleLines()));
    }
}


void OscilloscopePanel::setTimeStampSync(int state)       // for timeStampSyncCheckBox
{
    if ( state == Qt::Checked )
    {
        core()->setTimeStampSyn(true);
    }
    else if ( state == Qt::Unchecked )
    {
        core()->setTimeStampSyn(false);
    }
}

void OscilloscopePanel::setAdvancedChannelConfig(int state)    //for advancedChannelConfigCheckBox
{
    if ( isChCfgWindowActive() )
    {
        disconnect(UI.advancedChannelConfigCheckBox,SIGNAL(stateChanged(int)),this,SLOT(setAdvancedChannelConfig(int)));
        QMessageBox::warning(this,QString("error"),QString("you should not change the channel-config-widgets type when the channel-config-window is active"),QMessageBox::Ok,QMessageBox::Ok);
        UI.advancedChannelConfigCheckBox->setCheckState(hasAdvancedChCfg()?Qt::Checked:Qt::Unchecked);
        connect(UI.advancedChannelConfigCheckBox,SIGNAL(stateChanged(int)),this,SLOT(setAdvancedChannelConfig(int)));
        return ;
    }

    if ( state == Qt::Checked )
    {
        AdvancedChCfgSelected = true ;
    }
    else if (state == Qt::Unchecked)
    {
        AdvancedChCfgSelected = false ;
    }
}

void OscilloscopePanel::releaseChCfgWindow()
{
    if ( CfgWidgets )
    {
        for (int i=0;i<cfgWidgetsCount();i++)
        {
            if ( CfgWidgets[i] )
                delete CfgWidgets[i] ;
        }
        delete CfgWidgets ;
        CfgWidgets = NULL ;
    }
    if ( ChannelConfigWindow )
    {
        ChCfgWindowPos = ChannelConfigWindow->pos() ;
        delete ChannelConfigWindow ;
        ChannelConfigWindow = NULL ;
    }
}

void OscilloscopePanel::constructChCfgWindow()
{
    ChannelConfigWindow = new QWidget ;
    int widgetsCount = cfgWidgetsCount() ;
    bool advance = hasAdvancedChCfg() ;
    int eachWidgetHeight , eachWidgetWidth ;

    if (advance)
    {
        eachWidgetHeight = Ui_oscilloscopeChannelConfigWidget::HeightForAdvance ;
        eachWidgetWidth = Ui_oscilloscopeChannelConfigWidget::WidthForAdvance ;
    }
    else
    {
        eachWidgetHeight = Ui_oscilloscopeChannelConfigWidget::HeightForNonAdvance ;
        eachWidgetWidth = Ui_oscilloscopeChannelConfigWidget::WidthForNonAdvance ;
    }
    ChannelConfigWindow->resize( 20+(eachWidgetWidth*widgetsCount), 10+(eachWidgetHeight) ) ;
    ChannelConfigWindow->setMinimumSize(20+(eachWidgetWidth*widgetsCount), 10+(eachWidgetHeight) ) ;
    ChannelConfigWindow->setMaximumSize(20+(eachWidgetWidth*widgetsCount), 10+(eachWidgetHeight) ) ;
    ChannelConfigWindow->move(ChCfgWindowPos);


    ChannelConfigWindow->setWindowTitle( channelConfigWindowTitle() ) ;
    ChannelConfigWindow->show();


    CfgWidgets = new OscilloscopeChannelConfigWidget* [widgetsCount] ;


    for(int i=0;i<widgetsCount;i++)
    {
        CfgWidgets[i] = new OscilloscopeChannelConfigWidget(core(),advance,ChannelConfigWindow) ;
        CfgWidgets[i]->move(10+i*eachWidgetWidth,10);
        CfgWidgets[i]->show();
    }

    for(int i=0,ch=0 ;i<widgetsCount;i++)
    {
        while ( ch < core()->channelsCount() )
        {
            if ( core()->channel(ch)->configurable() && !(core()->channel(ch)->selected()) )
            {
                CfgWidgets[i]->ui()->channelComboBox->setCurrentIndex(ch+1);
                ch++;
                break ;
            }
            ch ++ ;
        }

        if ( ch < core()->channelsCount() )
            continue ;
        else
            break ;
    }

    ChannelConfigWindow->installEventFilter(this);
}

bool OscilloscopePanel::eventFilter(QObject *object, QEvent *event)
{
    if ( object==ChannelConfigWindow && event->type()==QEvent::Close )
    {
        UI.showChannelConfigCheckBox->setCheckState(Qt::Unchecked);
        return true ;
    }
    return false ;
}


void OscilloscopePanel::showChannelConfig(int state)      // for showChannelConfigCheckBox
{
    if ( state == Qt::Checked )
    {
        if ( isChCfgWindowActive() )
            return ;

        releaseChCfgWindow() ;

        constructChCfgWindow();

        UI.advancedChannelConfigCheckBox->setEnabled(false);
        UI.configWidgetsAmountLineEdit->setEnabled(false);

        ChCfgWindowShown = true ;

    }
    else if (state == Qt::Unchecked)
    {
        if ( ! isChCfgWindowActive() )
            return ;

        releaseChCfgWindow() ;
        UI.advancedChannelConfigCheckBox->setEnabled(true);
        UI.configWidgetsAmountLineEdit->setEnabled(true);

        ChCfgWindowShown = false ;
    }
}

void OscilloscopePanel::configWidgetsAmount()             // for configWidgetsAmountLineEdit
{

    if ( isChCfgWindowActive() )
    {
        disconnect(UI.configWidgetsAmountLineEdit,SIGNAL(editingFinished()),this,SLOT(configWidgetsAmount()));
        QMessageBox::warning(this,QString("error"),QString("you should not change the amount of channel-config-widgets when the channel-config-window is active"),QMessageBox::Ok,QMessageBox::Ok);
        UI.configWidgetsAmountLineEdit->setText(QString("%1").arg(cfgWidgetsCount()));
        connect(UI.configWidgetsAmountLineEdit,SIGNAL(editingFinished()),this,SLOT(configWidgetsAmount()));
        return ;
    }

    int amount ;
    bool ok ;
    amount = UI.configWidgetsAmountLineEdit->text().toInt(&ok);
    int min =1 ;
    int max = 6 ;

    if ( !ok || amount<min )
    {
        disconnect(UI.configWidgetsAmountLineEdit,SIGNAL(editingFinished()),this,SLOT(configWidgetsAmount()));
        QMessageBox::warning(this,QString("error"),QString("the amount of channel-config-widgets should be a integer between %1 and %2").arg(min).arg(max),QMessageBox::Ok,QMessageBox::Ok);
        UI.configWidgetsAmountLineEdit->setText(QString("%1").arg(cfgWidgetsCount()));
        connect(UI.configWidgetsAmountLineEdit,SIGNAL(editingFinished()),this,SLOT(configWidgetsAmount()));
        return ;
    }

    if ( amount>max )
    {
        disconnect(UI.configWidgetsAmountLineEdit,SIGNAL(editingFinished()),this,SLOT(configWidgetsAmount()));
        QMessageBox::warning(this,QString("error"),QString("are you kidding me sir ? if i do give you so much widgets, the width of your screen will \"hold不住\"...lol\nthe amount of channel-config-widgets should be a integer between %1 and %2").arg(min).arg(max),QMessageBox::Ok,QMessageBox::Ok);
        UI.configWidgetsAmountLineEdit->setText(QString("%1").arg(cfgWidgetsCount()));
        connect(UI.configWidgetsAmountLineEdit,SIGNAL(editingFinished()),this,SLOT(configWidgetsAmount()));
        return ;
    }

    CfgWidgetsCount = amount ;

}

void OscilloscopePanel::setXRange(int value)              // for xRangeSlider
{
    core()->setXRange(value);
    UI.xRangeLineEdit->setText(QString("%1").arg(core()->xRange()));
}

void OscilloscopePanel::setXRange()                     // for xRangeLineEdit
{
    int range ;
    bool ok ;
    range = UI.xRangeLineEdit->text().toInt(&ok);
    int max = UI.xRangeSlider->maximum() ;
    int min = UI.xRangeSlider->minimum() ;

    if ( ok && range<=max && range>=min )
    {
        disconnect(UI.xRangeSlider,SIGNAL(valueChanged(int)),this,SLOT(setXRange(int)));
        core()->setXRange(range);
        UI.xRangeSlider->setValue(range);
        connect(UI.xRangeSlider,SIGNAL(valueChanged(int)),this,SLOT(setXRange(int)));
    }
    else
    {
        disconnect(UI.xRangeLineEdit,SIGNAL(editingFinished()),this,SLOT(setXRange()));
        QMessageBox::warning(this,QString("error"),QString("the value of XRange must be a integer whose value is between %1 and %2").arg(min).arg(max),QMessageBox::Ok,QMessageBox::Ok);
        UI.xRangeLineEdit->setText(QString("%1").arg(core()->xRange()));
        connect(UI.xRangeLineEdit,SIGNAL(editingFinished()),this,SLOT(setXRange()));
    }
}

void OscilloscopePanel::setXOffset(int value)             // for xOffsetSlider
{
    core()->setXOffset(value);
    UI.xOffsetLineEdit->setText(QString("%1").arg(-core()->xOffset()));
}

void OscilloscopePanel::setXOffset()                    // for xOffsetLineEdit
{
    int offset ;
    bool ok ;
    offset = - UI.xOffsetLineEdit->text().toInt(&ok);
    int min = UI.xOffsetSlider->minimum();
    int max = UI.xOffsetSlider->maximum();

    if ( ok && offset<=max && offset>=min )
    {
        disconnect(UI.xOffsetSlider,SIGNAL(valueChanged(int)),this,SLOT(setXOffset(int)));
        core()->setXOffset(offset);
        UI.xOffsetSlider->setValue(offset);
        connect(UI.xOffsetSlider,SIGNAL(valueChanged(int)),this,SLOT(setXOffset(int)));
    }
    else
    {
        disconnect(UI.xOffsetLineEdit,SIGNAL(editingFinished()),this,SLOT(setXOffset()));
        QMessageBox::warning(this,QString("error"),QString("the value of XOffset must be a integer whose value is between %1 and %2").arg(-min).arg(-max),QMessageBox::Ok,QMessageBox::Ok);
        UI.xOffsetLineEdit->setText(QString("%1").arg(core()->xOffset()));
        connect(UI.xOffsetLineEdit,SIGNAL(editingFinished()),this,SLOT(setXOffset()));
    }
}


bool OscilloscopePanel::hasAdvancedChCfg()
{
    return AdvancedChCfgSelected ;
}


bool OscilloscopePanel::isChCfgWindowActive()
{
    return ChCfgWindowShown ;
}


void OscilloscopePanel::updatePanelInformation()
{
    disconnectExtraFuntionsRelatedSlots();
    disconnectXaxisRelatedSlots();

    if ( hasAdvancedChCfg() )
        UI.advancedChannelConfigCheckBox->setCheckState(Qt::Checked);
    else
        UI.advancedChannelConfigCheckBox->setCheckState(Qt::Unchecked);

    if ( isChCfgWindowActive() )
        UI.showChannelConfigCheckBox->setCheckState(Qt::Checked);
    else
        UI.showChannelConfigCheckBox->setCheckState(Qt::Unchecked);

    UI.configWidgetsAmountLineEdit->setText(QString("%1").arg(cfgWidgetsCount()));

    connectXaxisRelatedSlots();
    connectExtraFuntionsRelatedSlots();
}

void OscilloscopePanel::updateCoreInformation()
{
    disconnectExtraFuntionsRelatedSlots();
    disconnectXaxisRelatedSlots();


    int range = core()->xRange() ;
    int offset = core()->xOffset();

    UI.xRangeSlider->setMaximum(core()->channel(0)->samplesSize()*2);
    UI.xRangeSlider->setMinimum(10);
    UI.xRangeSlider->setValue(range);
    UI.xRangeLineEdit->setText(QString("%1").arg(range));

    UI.xOffsetSlider->setMaximum(0);
    UI.xOffsetSlider->setMinimum(-core()->channel(0)->samplesSize()*2);
    UI.xOffsetSlider->setValue(offset);
    UI.xOffsetLineEdit->setText(QString("%1").arg(-offset));

    UI.channelAmountLineEdit->setText(QString("%1").arg(core()->channelsCount()));
    UI.xScaleLinesLineEdit->setText(QString("%1").arg(core()->scaleLinesInX()));
    UI.yScaleLinesLineEdit->setText(QString("%1").arg(core()->scaleLinesInY()));

    if( core()->timeStampSyn() )
        UI.timeStampSyncCheckBox->setCheckState(Qt::Checked);
    else
        UI.timeStampSyncCheckBox->setCheckState(Qt::Unchecked);

    connectXaxisRelatedSlots();
    connectExtraFuntionsRelatedSlots();
}

void OscilloscopePanel::connectXaxisRelatedSlots()
{
    connect(UI.xRangeLineEdit,SIGNAL(editingFinished()),this,SLOT(setXRange()));
    connect(UI.xRangeSlider,SIGNAL(valueChanged(int)),this,SLOT(setXRange(int)));
    connect(UI.xOffsetLineEdit,SIGNAL(editingFinished()),this,SLOT(setXOffset()));
    connect(UI.xOffsetSlider,SIGNAL(valueChanged(int)),this,SLOT(setXOffset(int)));
}

void OscilloscopePanel::disconnectXaxisRelatedSlots()
{
    disconnect(UI.xRangeLineEdit,SIGNAL(editingFinished()),this,SLOT(setXRange()));
    disconnect(UI.xRangeSlider,SIGNAL(valueChanged(int)),this,SLOT(setXRange(int)));
    disconnect(UI.xOffsetLineEdit,SIGNAL(editingFinished()),this,SLOT(setXOffset()));
    disconnect(UI.xOffsetSlider,SIGNAL(valueChanged(int)),this,SLOT(setXOffset(int)));
}

void OscilloscopePanel::connectExtraFuntionsRelatedSlots()
{
    connect(UI.channelsAmountButton,SIGNAL(clicked()),this,SLOT(setChannelsAmount()));
    connect(UI.changeColorButton,SIGNAL(clicked()),this,SLOT(changeColor()));
    connect(UI.resetAllChannelsButton,SIGNAL(clicked()),this,SLOT(resetAllChannels()));
    connect(UI.lockAllChannelsButton,SIGNAL(clicked()),this,SLOT(lockAllChannels()));
    connect(UI.xScaleLinesLineEdit,SIGNAL(editingFinished()),this,SLOT(setXScaleLines()));
    connect(UI.yScaleLinesLineEdit,SIGNAL(editingFinished()),this,SLOT(setYScaleLines()));
    connect(UI.timeStampSyncCheckBox,SIGNAL(stateChanged(int)),this,SLOT(setTimeStampSync(int)));
    connect(UI.advancedChannelConfigCheckBox,SIGNAL(stateChanged(int)),this,SLOT(setAdvancedChannelConfig(int)));
    connect(UI.showChannelConfigCheckBox,SIGNAL(stateChanged(int)),this,SLOT(showChannelConfig(int)));
    connect(UI.configWidgetsAmountLineEdit,SIGNAL(editingFinished()),this,SLOT(configWidgetsAmount()));
}

void OscilloscopePanel::disconnectExtraFuntionsRelatedSlots()
{
    disconnect(UI.channelsAmountButton,SIGNAL(clicked()),this,SLOT(setChannelsAmount()));
    disconnect(UI.changeColorButton,SIGNAL(clicked()),this,SLOT(changeColor()));
    disconnect(UI.resetAllChannelsButton,SIGNAL(clicked()),this,SLOT(resetAllChannels()));
    disconnect(UI.lockAllChannelsButton,SIGNAL(clicked()),this,SLOT(lockAllChannels()));
    disconnect(UI.xScaleLinesLineEdit,SIGNAL(editingFinished()),this,SLOT(setXScaleLines()));
    disconnect(UI.yScaleLinesLineEdit,SIGNAL(editingFinished()),this,SLOT(setYScaleLines()));
    disconnect(UI.timeStampSyncCheckBox,SIGNAL(stateChanged(int)),this,SLOT(setTimeStampSync(int)));
    disconnect(UI.advancedChannelConfigCheckBox,SIGNAL(stateChanged(int)),this,SLOT(setAdvancedChannelConfig(int)));
    disconnect(UI.showChannelConfigCheckBox,SIGNAL(stateChanged(int)),this,SLOT(showChannelConfig(int)));
    disconnect(UI.configWidgetsAmountLineEdit,SIGNAL(editingFinished()),this,SLOT(configWidgetsAmount()));
}

