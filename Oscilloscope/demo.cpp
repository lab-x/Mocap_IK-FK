

#include <QApplication>
#include "Oscilloscope.h"
#include "OscilloscopeChannelConfigWidget.h"
#include "math.h"


#define TEST_OSCILLOSCOPE
#ifdef TEST_OSCILLOSCOPE


//#define OTHER
//#define ZHENG_XIAN
#define PPM


int main(int argc,char**argv)
{
	QApplication app(argc, argv);


#ifdef PPM

    Oscilloscope * scope = new Oscilloscope(false) ;

    scope->core()->channel(0)->setShow(true) ;
    scope->core()->channel(0)->setDescription("PPM");
    scope->core()->channel(0)->setYRange(2);

    scope->core()->channel(0)->addSample ( 1.0 , 1 ) ;
    scope->core()->channel(0)->addSample ( 1.0 , 250) ;
    scope->core()->channel(0)->addSample ( 0.0 , 251) ;
    scope->core()->channel(0)->addSample ( 0.0 , 400) ;


    scope->core()->channel(0)->addSample ( 1.0 , 401 ) ;
    scope->core()->channel(0)->addSample ( 1.0 , 570) ;
    scope->core()->channel(0)->addSample ( 0.0 , 571) ;
    scope->core()->channel(0)->addSample ( 0.0 , 600) ;


    scope->core()->channel(0)->addSample ( 1.0 , 601 ) ;
    scope->core()->channel(0)->addSample ( 1.0 , 730) ;
    scope->core()->channel(0)->addSample ( 0.0 , 731) ;
    scope->core()->channel(0)->addSample ( 0.0 , 800) ;


    scope->core()->channel(0)->addSample ( 1.0 , 801 ) ;
    scope->core()->channel(0)->addSample ( 1.0 , 990) ;
    scope->core()->channel(0)->addSample ( 0.0 , 991) ;
    scope->core()->channel(0)->addSample ( 0.0 , 1000) ;


    scope->core()->channel(0)->addSample ( 1.0 , 1001 ) ;
    scope->core()->channel(0)->addSample ( 1.0 , 1148) ;
    scope->core()->channel(0)->addSample ( 0.0 , 1149) ;
    scope->core()->channel(0)->addSample ( 0.0 , 2000) ;


    scope->show() ;
#endif



#ifdef  ZHENG_XIAN

    Oscilloscope * scope = new Oscilloscope(false) ;

    scope->core()->setXOffset(0) ;

    scope->core()->channel(0)->setLockSamplesType(true) ;

    scope->core()->channel(1)->setConfigurable(false);
    scope->core()->channel(2)->setConfigurable(false);

    scope->core()->channel(3)->setShow(true) ;
    scope->core()->channel(3)->setDescription("正弦波");
    scope->core()->channel(3)->setLockDescription(true);

    int addMax = 2000 ;
    for(int i=0;i<addMax;i++)
    {
        #define DIVIDE (100.0)
        scope->core()->channel(3)->addSample ( sin(i/DIVIDE) ) ;
        if(i<addMax/2)
            scope->core()->channel(0)->addSample ( sin(i/DIVIDE) ) ;
    }



    scope->show() ;
#endif


#ifdef OTHER

    OscilloscopeCore* core = new OscilloscopeCore() ;

    core->channel(3)->setConfigurable(false);
    core->channel(4)->setConfigurable(false);

    core->channel(0)->setDescription("加速度计X轴");
    core->channel(1)->setDescription("加速度计Y轴");
    core->channel(2)->setDescription("加速度计Z轴");


    OscilloscopeChannelConfigWidget* configWidget = new OscilloscopeChannelConfigWidget(core,false);

    configWidget->show();

#endif

	return app.exec();
}





#endif
