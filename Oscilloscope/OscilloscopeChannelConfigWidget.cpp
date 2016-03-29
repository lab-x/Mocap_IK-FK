
#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDialog>
#include "math.h"
#include "OscilloscopeChannelConfigWidget.h"

// 与 sampleTypeComboBox下拉框中的内容顺序一致
const  OscilloscopeChannel::OriginalDataType OscilloscopeChannelConfigWidget::SampleTypeList[] = {
    OscilloscopeChannel::Original_INT8 ,
    OscilloscopeChannel::Original_UINT8 ,
    OscilloscopeChannel::Original_INT16 ,
    OscilloscopeChannel::Original_UINT16 ,
    OscilloscopeChannel::Original_INT32 ,
    OscilloscopeChannel::Original_UINT32 ,
    OscilloscopeChannel::Original_FLOAT32 ,
    OscilloscopeChannel::Original_FLOAT64
};

OscilloscopeChannelConfigWidget::OscilloscopeChannelConfigWidget(OscilloscopeCore* core ,
           bool advance, QWidget * parent, Qt::WindowFlags f) : QWidget(parent,f)
{
    this->Core = core ;
    Advance = advance ;
    SelectedChannel = 0 ;

    UI.setupUi(this,Advance)  ;
    connectChannelRelatedSlots();
    updateChannelComboBox();

    updateChannelInformation();

//    UI.channelGroupBox->installEventFilter(this);
}


void OscilloscopeChannelConfigWidget::deInit()
{
    if ( SelectedChannel > 0 )
        this->core()->channel(SelectedChannel-1)->setSelect(false);
}


OscilloscopeChannelConfigWidget::~OscilloscopeChannelConfigWidget()
{
    deInit();
}


void OscilloscopeChannelConfigWidget::switchChannnel (int index )       // for channelComboBox
{
    if ( index == selectedChannel() )
        return ;

    if ( index == 0 )
    {
        this->core()->channel(selectedChannel()-1)->setSelect(false);
        SelectedChannel = 0 ;
        setDefault();
        return ;
    }

    if (  false==core()->channel(index-1)->configurable() )
    {
        QMessageBox::warning(this,"error","the selected channel is not configurable\n",QMessageBox::Ok,QMessageBox::Ok);
        UI.channelComboBox->setCurrentIndex(selectedChannel());
        return ;
    }

    if (  core()->channel(index-1)->selected()  )
    {
        QMessageBox::warning(this,"error","the selected channel has been selected in other panels\n",QMessageBox::Ok,QMessageBox::Ok);
        UI.channelComboBox->setCurrentIndex(selectedChannel());
        return ;
    }

    if (selectedChannel()!=0)   // 0代表没有选中任何通道
        this->core()->channel(selectedChannel()-1)->setSelect(false);

    SelectedChannel = index ;
    if ( SelectedChannel > 0 )
        this->core()->channel(SelectedChannel-1)->setSelect(true);

    updateChannelInformation();

}


void OscilloscopeChannelConfigWidget::setDefault()
{
    disconnectChannelRelatedSlots();

    UI.showChannelCheckBox->setCheckState(Qt::Unchecked);
    UI.showChannelCheckBox->setEnabled(false);

    UI.yRangeLineEdit->setText(QString(""));
    UI.yRangeLineEdit->setEnabled(false);

    UI.yRefLineEdit->setText(QString(""));
    UI.yRefLineEdit->setEnabled(false);

    UI.yOffsetSlider->setMaximum(1000);UI.yOffsetSlider->setMinimum(-1000);
    UI.yStretchSlider->setValue(UI.yStretchSlider->minimum());
    UI.yStretchSlider->setEnabled(false);
    UI.yStretchLineEdit->setText(QString("1"));
    UI.yStretchLineEdit->setEnabled(false);

    UI.yStretchSlider->setMaximum(990);UI.yStretchSlider->setMinimum(0);
    UI.yOffsetSlider->setValue(0);
    UI.yOffsetSlider->setEnabled(false);
    UI.yOffsetLineEdit->setText(QString("0"));
    UI.yOffsetLineEdit->setEnabled(false);

    UI.descripLineEdit->setText(QString(""));
    UI.descripLineEdit->setEnabled(false);

    if ( Advance )
    {
        UI.showDelayLineEdit->setText(QString("0"));
        UI.showDelayLineEdit->setEnabled(false);

        UI.channelResetButton->setEnabled(false);
        UI.sampleTypeComboBox->setCurrentIndex(findSampleTypeIndex(OscilloscopeChannel::Original_FLOAT64));
        UI.sampleTypeComboBox->setEnabled(false);
    }

    UI.channelGroupBox->setTitle(QString("Please Select a Channel"));

    connectChannelRelatedSlots();
}



void OscilloscopeChannelConfigWidget::updateChannelInformation()
{

    int index = selectedChannel()-1 ;
    if ( index < 0 )
    {
        setDefault() ;
        return ;
    }

    disconnectChannelRelatedSlots();

    UI.showChannelCheckBox->setEnabled(true);
    UI.yRangeLineEdit->setEnabled(true);
    UI.yRefLineEdit->setEnabled(true);
    UI.yStretchSlider->setEnabled(true);
    UI.yStretchLineEdit->setEnabled(true);
    UI.yOffsetSlider->setEnabled(true);
    UI.yOffsetLineEdit->setEnabled(true);

    if ( core()->channel(index)->descriptionLocked() )
        UI.descripLineEdit->setEnabled(false);
    else
        UI.descripLineEdit->setEnabled(true);


    if ( Advance )
    {
        UI.showDelayLineEdit->setEnabled(true);
        UI.channelResetButton->setEnabled(true);
        if ( core()->channel(index)->samplesTypeLocked() )
            UI.sampleTypeComboBox->setEnabled(false);
        else
            UI.sampleTypeComboBox->setEnabled(true);
    }


    if ( this->core()->channel(index)->show() )
        UI.showChannelCheckBox->setCheckState(Qt::Checked);
    else
        UI.showChannelCheckBox->setCheckState(Qt::Unchecked);

    QString str ;

    str = QString("%1").arg(core()->channel(index)->yRange());
    UI.yRangeLineEdit->setText(str);
    str = QString("%1").arg(core()->channel(index)->yRef());
    UI.yRefLineEdit->setText(str);

    UI.yOffsetSlider->setMaximum(1000);UI.yOffsetSlider->setMinimum(-1000);
    UI.yOffsetSlider->setValue(core()->channel(index)->yOffset()*1000);
    UI.yOffsetLineEdit->setText(QString("%1").arg(core()->channel(index)->yOffset()));

    UI.yStretchSlider->setMaximum(990);UI.yStretchSlider->setMinimum(0);
    UI.yStretchSlider->setValue ( 1000 - core()->channel(index)->yStretchRatio()*1000);
    UI.yStretchLineEdit->setText(QString("%1").arg(core()->channel(index)->yStretchRatio()));


    QString descripStr = QString::fromUtf8(core()->channel(selectedChannel()-1)->description()) ;
    UI.descripLineEdit->setText(descripStr);


    if (Advance)
    {
        UI.sampleTypeComboBox->setCurrentIndex ( findSampleTypeIndex ( core()->channel(index)->samplesType() ));
        UI.showDelayLineEdit->setText(QString("%1").arg(core()->channel(index)->showDelay()));
    }


    setGroupBoxTitle(descripStr) ;

    connectChannelRelatedSlots();
}

void OscilloscopeChannelConfigWidget::setGroupBoxTitle(QString &str)
{
    if(str.isEmpty())
        UI.channelGroupBox->setTitle(QString("none description"));
    else
        UI.channelGroupBox->setTitle(str);
}

void OscilloscopeChannelConfigWidget::switchShow (int state)            // for showChannelCheckBox
{
    int index = selectedChannel() - 1 ;
    if(state==Qt::Unchecked)
        core()->channel(index)->setShow(false);
    else if(state==Qt::Checked)
        core()->channel(index)->setShow(true);
}


void OscilloscopeChannelConfigWidget::setYRange ( )                     // for yRangeLineEdit
{
    int index = selectedChannel() - 1 ;

    QString str = UI.yRangeLineEdit->text() ;
    bool ok ;
    double range ;
    range=str.toDouble(&ok);
    if ( ok )
    {
        core()->channel(index)->setYRange(range);
    }
    else
    {
        disconnect(UI.yRangeLineEdit,SIGNAL(editingFinished()),this,SLOT(setYRange()));

        QMessageBox::warning(this,"error","the value of YRange must be a real number\n",QMessageBox::Ok,QMessageBox::Ok);
        str = QString("%1").arg(core()->channel(index)->yRange());
        UI.yRangeLineEdit->setText(str);

        connect(UI.yRangeLineEdit,SIGNAL(editingFinished()),this,SLOT(setYRange()));
    }

}

void OscilloscopeChannelConfigWidget::setDescription()                  // for descripLineEdit
{
    QString str = UI.descripLineEdit->text() ;
    QString oldStr = QString::fromUtf8(core()->channel(selectedChannel()-1)->description()) ;
    int maxStrLen = core()->channel(selectedChannel()-1)->descriptionBufLen() ;

    if (str==oldStr)
        return ;

    disconnect(UI.descripLineEdit,SIGNAL(editingFinished()),this,SLOT(setDescription()));
    if ( QMessageBox::warning(this,QString("confirm"),
              QString("do you want to change the description of this channel?"),
                QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok)
         ==  QMessageBox::Ok  )
    {
        int len = strlen(str.toUtf8().data());
        if ( len < maxStrLen )
        {
            core()->channel(selectedChannel()-1)->setDescription(str.toUtf8().data());
            setGroupBoxTitle(str);
        }
        else
        {
            QMessageBox::warning(this,QString("error"),
                          QString("your input is too long . the size of channel-description should be under %1 bytes(UFT-8 encode)").arg(maxStrLen),
                            QMessageBox::Ok,QMessageBox::Ok) ;
            UI.descripLineEdit->setFocus();
        }
    }
    else
    {
        UI.descripLineEdit->setText(oldStr);
    }
    connect(UI.descripLineEdit,SIGNAL(editingFinished()),this,SLOT(setDescription()));
}

void OscilloscopeChannelConfigWidget::setYRef ()                        // for yRefLineEdit
{
    int index = selectedChannel() - 1 ;

    QString str = UI.yRefLineEdit->text() ;
    bool ok ;
    double ref ;
    ref=str.toDouble(&ok);
    if ( ok )
    {
        core()->channel(index)->setYRef(ref);
    }
    else
    {
        disconnect(UI.yRefLineEdit,SIGNAL(editingFinished()),this,SLOT(setYRef()));

        QMessageBox::warning(this,"error","the value of YRef must be a real number\n",QMessageBox::Ok,QMessageBox::Ok);
        str = QString("%1").arg(core()->channel(index)->yRef());
        UI.yRefLineEdit->setText(str);

        connect(UI.yRefLineEdit,SIGNAL(editingFinished()),this,SLOT(setYRef()));
    }
}

void OscilloscopeChannelConfigWidget::setYOffset(int value)             // for yOffsetSlider
{
    int index = selectedChannel() - 1 ;

    double max=UI.yOffsetSlider->maximum();
    double v = value ;
    double offset = v / max ;
    core()->channel(index)->setYOffset(offset);
    UI.yOffsetLineEdit->setText(QString("%1").arg(core()->channel(index)->yOffset()));
}

void OscilloscopeChannelConfigWidget::setYOffset()             // for yOffsetLineEdit
{
    int index = selectedChannel() - 1 ;

    double offset ;
    bool ok ;
    offset = UI.yOffsetLineEdit->text().toDouble(&ok) ;
    if ( ok && fabs(offset)<=1 )
    {
        core()->channel(index)->setYOffset(offset);
        double max=UI.yOffsetSlider->maximum();
        int value = max*offset ;
        disconnect(UI.yOffsetSlider,SIGNAL(valueChanged(int)),this,SLOT(setYOffset(int)));
        UI.yOffsetSlider->setValue(value);
        connect(UI.yOffsetSlider,SIGNAL(valueChanged(int)),this,SLOT(setYOffset(int)));
    }
    else
    {
        disconnect(UI.yOffsetLineEdit,SIGNAL(editingFinished()),this,SLOT(setYOffset()));

        QMessageBox::warning (this,"error","the YOffset must be a real number whose absolute value is not bigger than 1 \n",QMessageBox::Ok,QMessageBox::Ok);
        UI.yOffsetLineEdit->setText(QString("%1").arg(core()->channel(index)->yOffset()));

        connect(UI.yOffsetLineEdit,SIGNAL(editingFinished()),this,SLOT(setYOffset()));
    }
}


void OscilloscopeChannelConfigWidget::setYStretch(int value)            // for yStretchSlider
{
    int index = selectedChannel() - 1 ;

    double v = ( 1000.0 - value ) / 1000.0 ;
    core()->channel(index)->setYStretchRatio(v);
    UI.yStretchLineEdit->setText(QString("%1").arg(core()->channel(index)->yStretchRatio()));
}
void OscilloscopeChannelConfigWidget::setYStretch()            // for yStretchLineEdit
{
    int index = selectedChannel() - 1 ;

    double stretch ;
    bool ok ;
    stretch = UI.yStretchLineEdit->text().toDouble(&ok) ;
    if ( ok && stretch>=0.01 && stretch<=1 )
    {
        core()->channel(index)->setYStretchRatio(stretch);
        int value = 1000 - stretch*1000 ;

        disconnect(UI.yStretchSlider,SIGNAL(valueChanged(int)),this,SLOT(setYStretch(int)));
        UI.yStretchSlider->setValue(value);
        connect(UI.yStretchSlider,SIGNAL(valueChanged(int)),this,SLOT(setYStretch(int)));
    }
    else
    {
        disconnect(UI.yStretchLineEdit,SIGNAL(editingFinished()),this,SLOT(setYStretch()));

        QMessageBox::warning (this,"error","the YStretch must be a real number whose value is between 0.01 and 1 .\n",QMessageBox::Ok,QMessageBox::Ok);
        UI.yOffsetLineEdit->setText(QString("%1").arg(core()->channel(index)->yStretchRatio()));

        connect(UI.yStretchLineEdit,SIGNAL(editingFinished()),this,SLOT(setYStretch()));
    }
}

void OscilloscopeChannelConfigWidget::channelReset()                    // for channelResetButton
{
    int i = selectedChannel() - 1 ;
    core()->channel(i)->reset();
}

void OscilloscopeChannelConfigWidget::channelLock()                    // for channelLockButton
{
    int i = selectedChannel() - 1 ;
    core()->channel(i)->lockSamples();
}

void OscilloscopeChannelConfigWidget::setShowDelay()                    // for showDelayLineEdit
{
    int i = selectedChannel() - 1 ;

    t_TimeStamp delay ;
    bool ok ;
    delay = UI.showDelayLineEdit->text().toInt(&ok) ;

    if ( ok && delay >= 0 )
    {
        delay = core()->channel(i)->setShowDelay(delay) ;
        UI.showDelayLineEdit->setText(QString("%1").arg(core()->channel(i)->showDelay()));
    }
    else
    {
        disconnect(UI.showDelayLineEdit,SIGNAL(editingFinished()),this,SLOT(setShowDelay()));

        QMessageBox::warning (this,"error","the showDelay must be a nonnegative integer number .\n",QMessageBox::Ok,QMessageBox::Ok);
        UI.yOffsetLineEdit->setText(QString("%1").arg(core()->channel(i)->showDelay()));

        connect(UI.showDelayLineEdit,SIGNAL(editingFinished()),this,SLOT(setShowDelay()));
    }
}


void OscilloscopeChannelConfigWidget::setSampleType(int index)          // for sampleTypeComboBox
{
    int i = selectedChannel() - 1 ;
    if ( core()->channel(i)->samplesType() == index )
        return ;

    core()->channel(i)->setSamplesType( OscilloscopeChannelConfigWidget::SampleTypeList[index] );
}

void OscilloscopeChannelConfigWidget::updateChannelComboBox()
{
    disconnectChannelRelatedSlots();

    UI.channelComboBox->clear();

    UI.channelComboBox->addItem(QString("NON"));

    int total = core()->channelsCount() ;
    for (int i=0;i<total;i++)
    {
        QString str = QString("CH%1").arg(i+1) ;
        if ( false == core()->channel(i)->configurable() )
            str += "  x";
        UI.channelComboBox->addItem(str);
    }

    UI.channelComboBox->setCurrentIndex(0);
    setDefault();

    connectChannelRelatedSlots();
}

void OscilloscopeChannelConfigWidget::connectChannelRelatedSlots()
{
    connect(UI.channelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(switchChannnel(int)));
    connect(UI.showChannelCheckBox,SIGNAL(stateChanged(int)),this,SLOT(switchShow(int)));
    connect(UI.yRangeLineEdit,SIGNAL(editingFinished()),this,SLOT(setYRange()));
    connect(UI.yRefLineEdit,SIGNAL(editingFinished()),this,SLOT(setYRef()));
    connect(UI.yOffsetSlider,SIGNAL(valueChanged(int)),this,SLOT(setYOffset(int)));
    connect(UI.yOffsetLineEdit,SIGNAL(editingFinished()),this,SLOT(setYOffset()));
    connect(UI.yStretchSlider,SIGNAL(valueChanged(int)),this,SLOT(setYStretch(int)));
    connect(UI.yStretchLineEdit,SIGNAL(editingFinished()),this,SLOT(setYStretch()));
    connect(UI.descripLineEdit,SIGNAL(editingFinished()),this,SLOT(setDescription()));
if(!Advance)
    return ;

    connect(UI.sampleTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setSampleType(int)));
    connect(UI.channelResetButton,SIGNAL(clicked()),this,SLOT(channelReset()));
    connect(UI.channelLockButton,SIGNAL(clicked()),this,SLOT(channelLock()));
    connect(UI.showDelayLineEdit,SIGNAL(editingFinished()),this,SLOT(setShowDelay()));
}
void OscilloscopeChannelConfigWidget::disconnectChannelRelatedSlots()
{
    disconnect(UI.channelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(switchChannnel(int)));
    disconnect(UI.showChannelCheckBox,SIGNAL(stateChanged(int)),this,SLOT(switchShow(int)));
    disconnect(UI.yRangeLineEdit,SIGNAL(editingFinished()),this,SLOT(setYRange()));
    disconnect(UI.yRefLineEdit,SIGNAL(editingFinished()),this,SLOT(setYRef()));
    disconnect(UI.yOffsetSlider,SIGNAL(valueChanged(int)),this,SLOT(setYOffset(int)));
    disconnect(UI.yOffsetLineEdit,SIGNAL(editingFinished()),this,SLOT(setYOffset()));
    disconnect(UI.yStretchSlider,SIGNAL(valueChanged(int)),this,SLOT(setYStretch(int)));
    disconnect(UI.yStretchLineEdit,SIGNAL(editingFinished()),this,SLOT(setYStretch()));
    disconnect(UI.descripLineEdit,SIGNAL(editingFinished()),this,SLOT(setDescription()));

if(!Advance)
    return ;

    disconnect(UI.sampleTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setSampleType(int)));
    disconnect(UI.channelResetButton,SIGNAL(clicked()),this,SLOT(channelReset()));
    disconnect(UI.channelLockButton,SIGNAL(clicked()),this,SLOT(channelLock()));
    disconnect(UI.showDelayLineEdit,SIGNAL(editingFinished()),this,SLOT(setShowDelay()));
}

