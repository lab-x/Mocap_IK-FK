
#include "Oscilloscope.h"
#include <QDebug>



Oscilloscope::Oscilloscope(OscilloscopeCore *core, bool functionWindowVisible , QWidget * parent, Qt::WindowFlags f)
                                          : QWidget(parent,f)
{
    if (core)
    {
        Core = core ;
        CoreIsLocal = false ;
    }
    else
    {
        Core = new OscilloscopeCore() ;
        CoreIsLocal = true ;
    }

    Screen = new OscilloscopeScreen ( Core ) ;
    Panel = new OscilloscopePanel (Core ) ;

    uiSetup(functionWindowVisible);
    setScreenTitle(QString("Scope Screen"));


    Panel ->setParent(scopeConfigWidgetsParent);
    Panel->move(0,0);
    Screen->setParent(scopeSceenParent);
    Screen->setGeometry(10,20,scopeSceenParent->width()-20,scopeSceenParent->height()-40);


    ScanTimer = new QTimer(Screen) ;

    connect(ScanTimer,SIGNAL(timeout()),Screen,SLOT(update()));
    ScanTimer->setInterval(40);
    ScanTimer->start();
}

Oscilloscope::~Oscilloscope()
{
    delete Panel ;
    delete Screen ;

    if ( CoreIsLocal )
        delete Core ;
}


void Oscilloscope::resizeEvent(QResizeEvent*event)
{
    int height = this->height() ;
    int width = this->width() ;

    functionWidgetsParent->move(Panel->width() + WidgetsGap ,
                                height - WidgetsGap - getFuncWidgetsParentHeight() );

    scopeSceenParent->resize(width-2*WidgetsGap-Panel->width() , height-3*WidgetsGap-getFuncWidgetsParentHeight() );
    scopeSceenParent->move( Panel->width() + WidgetsGap , WidgetsGap ) ;


    Screen->setGeometry(10,20,scopeSceenParent->width()-20,scopeSceenParent->height()-40);

    QWidget::resizeEvent(event);
}

void Oscilloscope::closeEvent(QCloseEvent *event)
{
    // 先关掉附属窗口，再正常关闭
    panel()->ui()->showChannelConfigCheckBox->setChecked(false);
    QWidget::closeEvent(event);
}


void Oscilloscope::uiSetup(bool functionWindowVisible)
{

    // 主窗口设置
    setFuntionWindowVisible(functionWindowVisible) ; // 该函数中会设置窗口的最小尺寸 ，所以下面只设置最大尺寸
    setMaximumSize(QSize(16777215, 16777215));
    resize(minimumSize());
    move(300,0);
    setWindowTitle(QApplication::translate("Oscilloscope", "Oscilloscope Simulator", 0));


    // 示波器面板窗口设置
    scopeConfigWidgetsParent = new QWidget(this);
    scopeConfigWidgetsParent->setMinimumSize(QSize(Panel->width(), Panel->height()));
    scopeConfigWidgetsParent->setMaximumSize(QSize(Panel->width(), Panel->height()));
    scopeConfigWidgetsParent->resize(scopeConfigWidgetsParent->minimumSize());
    scopeConfigWidgetsParent->move(0,0);


    // 功能窗口设置
    functionWidgetsParent = new QGroupBox(this);
    functionWidgetsParent->setMinimumSize(getFuncWidgetsParentWidth(),getFuncWidgetsParentHeight());
    functionWidgetsParent->setMaximumSize(getFuncWidgetsParentWidth(),getFuncWidgetsParentHeight());
    functionWidgetsParent->resize(functionWidgetsParent->minimumSize());
    functionWidgetsParent->move(Panel->width() + WidgetsGap ,
                                height() - WidgetsGap - getFuncWidgetsParentHeight() );
    functionWidgetsParent->setTitle(QApplication::translate("Oscilloscope", "Functions", 0));


    // 示波器屏幕窗口设置。
    scopeSceenParent = new QGroupBox(this);
    scopeSceenParent->setMinimumSize(QSize(minScopeScreenParentWidth, minScopeScreenParentHeight));
    scopeSceenParent->setMaximumSize(QSize(16777215, 16777215));
    scopeSceenParent->resize(width()-2*WidgetsGap-Panel->width() , height()-3*WidgetsGap-getFuncWidgetsParentHeight() );
    scopeSceenParent->move( Panel->width() + WidgetsGap , WidgetsGap ) ;
    scopeSceenParent->setTitle(QApplication::translate("Oscilloscope", "Scope Screen", 0));


}

