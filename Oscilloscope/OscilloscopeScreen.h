#ifndef _OSCILLOSCOPE_SCREEN_H_
#define _OSCILLOSCOPE_SCREEN_H_


#include "stdint.h"

#include <QWidget>
#include <QRect>
#include <QImage>
#include <QLineF>
#include <QPainter>
#include <QPen>
#include <QColor>

#include "OscilloscopeCore.h"


class OscilloscopeScreen : public QWidget
{
public:
    OscilloscopeScreen(QWidget * parent = 0, Qt::WindowFlags f = 0) : QWidget(parent,f){Core=NULL;}
    OscilloscopeScreen(OscilloscopeCore*core , QWidget * parent = 0, Qt::WindowFlags f = 0);
	~OscilloscopeScreen();

    OscilloscopeCore *  core() {return Core;}
    void setCore(OscilloscopeCore *core){Core=core;}

protected:
	void paintEvent(QPaintEvent *event);

private:
    OscilloscopeCore * Core ;


private:
    void drawScaleLines(QPainter& painter);
    void drawWaveByTimeStamp(QPainter&painter,int channel);


private:
    // 已经被抛弃的函数们, say good bye to them .
#define Compile_Obsolete_Funtions_For_OscilloscopeScreen 1 //0
#if  Compile_Obsolete_Funtions_For_OscilloscopeScreen

    void drawWaveBySampleIndex(QPainter& painter,int channel);

#endif

};



#endif
