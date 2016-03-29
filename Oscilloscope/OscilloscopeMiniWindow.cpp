
#include "OscilloscopeMiniWindow.h"

void OscilloscopeMiniWindow::updateScopeWindow()
{
    if ( NULL == scope() )
        return ;


    OscilloscopeCore *core = scope()->core() ;

    ui()->scopeWidget->update();

    // 每个示波器只使用前三个通道，且这三个通道的样点数据类型必须相同，且只能是INT16和FLOAT32两种类型。
    QString str1 = "----" ;
    QString str2 = "----" ;
    QString str3 = "----" ;

    if ( OscilloscopeChannel::Original_FLOAT32 ==
        core->channel(0)->samplesType() )
    {
        float ch1,ch2,ch3 ;
        bool ch1NotValid, ch2NotValid, ch3NotValid ;
        core->channel(0)->sampleValueAtOffset(0,&ch1,NULL,&ch1NotValid) ;
        core->channel(1)->sampleValueAtOffset(0,&ch2,NULL,&ch2NotValid) ;
        core->channel(2)->sampleValueAtOffset(0,&ch3,NULL,&ch3NotValid) ;

        if ( ! ch1NotValid )
        {
            if (ch1<0) str1="";
            else str1=" ";
            str1 += QString("%1").arg(ch1) ;
        }

        if ( ! ch2NotValid )
        {
            if (ch2<0) str2="";
            else str2=" ";
            str2 += QString("%1").arg(ch2) ;
        }

        if ( ! ch3NotValid )
        {
            if (ch3<0) str3="";
            else str3=" ";
            str3 += QString("%1").arg(ch3) ;
        }

    }
    else if ( OscilloscopeChannel::Original_INT16 ==
              core->channel(0)->samplesType() )
    {
        int16_t ch1,ch2,ch3 ;
        bool ch1NotValid, ch2NotValid, ch3NotValid ;
        core->channel(0)->sampleValueAtOffset(0,&ch1,NULL,&ch1NotValid) ;
        core->channel(1)->sampleValueAtOffset(0,&ch2,NULL,&ch2NotValid) ;
        core->channel(2)->sampleValueAtOffset(0,&ch3,NULL,&ch3NotValid) ;

        if ( ! ch1NotValid )
        {
            if (ch1<0) str1="";
            else str1=" ";
            str1 += QString("%1").arg(ch1) ;
        }

        if ( ! ch2NotValid )
        {
            if (ch2<0) str2="";
            else str2=" ";
            str2 += QString("%1").arg(ch2) ;
        }

        if ( ! ch3NotValid )
        {
            if (ch3<0) str3="";
            else str3=" ";
            str3 += QString("%1").arg(ch3) ;
        }
    }

    ui()->scopeLabel1->setText(QString("|%1:%2").arg(labelName(0)).arg(str1));
    ui()->scopeLabel2->setText(QString("|%1:%2").arg(labelName(1)).arg(str2));
    ui()->scopeLabel3->setText(QString("|%1:%2").arg(labelName(2)).arg(str3));
}

void OscilloscopeMiniWindow::on_scopeCheckBox_stateChanged ( int state )
{
    if ( NULL == scope() )
        return ;

    if ( state == Qt::Checked )
    {
        scope()->show();
    }
    else if ( state == Qt::Unchecked )
    {
        scope()->close();
    }
}






