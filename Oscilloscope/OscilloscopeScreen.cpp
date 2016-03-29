
#include "OscilloscopeCore.h"
#include "OscilloscopeScreen.h"
#include "stdio.h"
#include <QDebug>



OscilloscopeScreen::OscilloscopeScreen(OscilloscopeCore*core, QWidget * parent, Qt::WindowFlags f)
                            :   QWidget(parent,f)
{
    Core = core ;
}

OscilloscopeScreen::~OscilloscopeScreen()
{

}

void OscilloscopeScreen::drawScaleLines(QPainter& painter)
{
    // 画刻度线
    QPen pen ;
    pen.setStyle(Qt::DotLine);
    pen.setColor(core()->axisColor());
    pen.setWidth(1);
    painter.setPen(pen);

    int height = this->height() / 2 ;	// y轴有正负，所以高度除以2
    int width = this->width() ;			// x轴无正负。

    // 横线
    for(int i=0;i<core()->scaleLinesInY();i++)
    {
        int deltaY = i*height / (core()->scaleLinesInY()) ;
        QLineF line ( 0, height-deltaY , width , height-deltaY );
        painter.drawLine(line);
        QLineF line2 ( 0, height+deltaY , width , height+deltaY );
        painter.drawLine(line2);
    }

    // 竖线
    for(int i=0;i<core()->scaleLinesInX();i++)
    {
        int deltaX = i*width / core()->scaleLinesInX() ;
        QLineF line ( deltaX, 0 , deltaX , height*2 );
        painter.drawLine(line);
    }
}




void OscilloscopeScreen::paintEvent(QPaintEvent *event)
{
	QImage image = QImage ( this->width(), this->height(), QImage::Format_ARGB32 );

    if ( core() )
    {
        image.fill(core()->bGColor());	// 填充背景色
        QPainter painter (&image);

        // 画刻度线
        drawScaleLines(painter) ;
    //qDebug()<<"刻度线画完";

        // 画所有波的波形
        for (int i=0;i<core()->channelsCount();i++)
        {
            if ( core()->channel(i)->show() )
            {
                drawWaveByTimeStamp(painter,i) ;
            }
        }
    }

    else
    {
        image.fill(qRgb(0,0,0));	// 填充黑色
    }


	QPainter painter2(this);
	QRect rect(0,0,this->width(),this->height());  	//创建绘图区域
	painter2.drawImage(rect,image);                    //绘图，drawImage有很多重载函数，可根据自己的需要调用相应函数 
}



void OscilloscopeScreen::drawWaveByTimeStamp(QPainter&painter,int channel)
{
//static int a = 0 ;
//a++;
//qDebug()<<"drawing in drawWaveByTimeStamp "<<a;

    int i = channel ;
    int height = this->height() / 2 ;	// y轴有正负，所以高度除以2
    int width = this->width() ;			// x轴无正负。
    double middleY = height-core()->channel(i)->yOffset()*height ;

    QPen pen ;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(core()->channel(i)->color()));

    // 画通道的参考线。
    pen.setWidth(5);
    painter.setPen(pen);
    QLineF line(0, middleY, width/35, middleY);
    painter.drawLine(line);
    QLineF line2(width,middleY,width-width/35,middleY);
    painter.drawLine(line2);
//qDebug()<<"通道"<<i+1<<"的参考线画完";

    // 开始画波形
    int xOffset = - core()->xOffset() ;
    double deltaX = (double)width / core()->xRange() ;	// 两个相邻时间戳在视窗中显示时的X坐标间隔，单位是像素。

    t_TimeStamp leftTimeStamp  ;    // 视窗最左边所显示的样点对应的时间戳
    t_TimeStamp rightTimeStamp  ;   // 视窗最右边所显示的样点对应的时间戳

    if (core()->timeStampSyn())
        rightTimeStamp = core()->latestTimeStamp() ;      // 各通道时间戳同步时使用。
    else
        rightTimeStamp = core()->channel(i)->latestValidTimeStamp() ;   // 各通道时间戳异步时使用。

    rightTimeStamp -= xOffset ;
    leftTimeStamp = rightTimeStamp - core()->xRange() ;

//qDebug()<<"rightTimeStamp is "<<rightTimeStamp<<", leftTimeStamp is "<<leftTimeStamp<<", xOffset is "<<xOffset ;

    t_TimeStamp showDelay = core()->channel(i)->showDelay() ;
    int rightSampleIndex = core()->channel(i)->getIndexByTimeStamp( rightTimeStamp - showDelay ) ;
    if ( rightSampleIndex < 0 )
    {
        // 如果通道所有样点的时间戳都比 ( rightTimeStamp - showDelay ) 大，或者通道中没有任何有效样点时，就会出现rightSampleIndex<0。
        // 如果通道中没有任何有效样点，自然就不用画这个通道。
        // 如果通道所有样点的时间戳都比( rightTimeStamp - showDelay ) 大，说明即使画出这个通道的波形，在视窗中也看不到，
        // 这时就直接跳过，也不画这个通道。
        return ;
    }

    // 从右往左画波形。startSampleIndex表示第一个要画的样点的索引
    int startSampleIndex = core()->channel(i)->getNextValidSampleIndex(rightSampleIndex) ;

    int curIndex , nextIndex ;
    double curX , curY , nextX , nextY ;

    // 计算第一个要画的点在视窗中的坐标（Y轴未拉伸前的坐标），单位是像素。
    curIndex = startSampleIndex ;
    curX = ( core()->channel(i)->Samples[curIndex].timeStamp + showDelay - leftTimeStamp ) * deltaX ;
    curY = core()->channel(i)->sampleValue(curIndex)  -  core()->channel(i)->yRef() ;
    curY = curY	/ core()->channel(i)->yRange() * height ;

    // 画波形。
    // 画波形的停止条件：找不到下一个要画的样点（所有样点已画完）,
    //         或当前所画样点已超出视窗左边，即横坐标小于0（说明没有必要再画了，再画也看不到）。
//qDebug()<<"drawing wave of channel "<<i;
    pen.setWidth(1);
    painter.setPen(pen);
    while ( curX > 0 )
    {
        nextIndex = core()->channel(i)->getPreValidSampleIndex(curIndex) ;
        if ( nextIndex == curIndex )    // 说明已经找不到下一个需要画的数据了
            break ;

        nextX = ( core()->channel(i)->Samples[nextIndex].timeStamp + showDelay - leftTimeStamp ) * deltaX ;
        nextY = core()->channel(i)->sampleValue(nextIndex)  -  core()->channel(i)->yRef() ;
        nextY = nextY / core()->channel(i)->yRange() * height ;

        // 用减号是因为屏幕坐标与狄卡尔坐标的y轴方向相反
        QLineF line(curX, middleY-curY/core()->channel(i)->yStretchRatio(), nextX, middleY-nextY/core()->channel(i)->yStretchRatio());
        painter.drawLine(line);

        curIndex = nextIndex ;
        curX = nextX ;
        curY = nextY ;
    }
}




/****************************************************************************************/

// 已经被抛弃的函数们, say good bye to them .

#if  Compile_Obsolete_Funtions_For_OscilloscopeScreen

void OscilloscopeScreen::drawWaveBySampleIndex(QPainter &painter,int channel)
{
    int i = channel ;
    QPen pen ;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(core()->channel(i)->color()));

    int height = this->height() / 2 ;	// y轴有正负，所以高度除以2
    int width = this->width() ;			// x轴无正负。
    double middleY = height-core()->channel(i)->yOffset()*height ;

    // 画通道的参考线。
    pen.setWidth(5);
    painter.setPen(pen);
    QLineF line(0, middleY, width/35, middleY);
    painter.drawLine(line);
    QLineF line2(width,middleY,width-width/35,middleY);
    painter.drawLine(line2);
//qDebug()<<"通道"<<i+1<<"的参考线画完";

    // 开始画波形
    int xOffset = - core()->xOffset() ;
    double deltaX = (double)width / core()->xRange() ;	// 两个相邻样点在视窗中显示时的X坐标间隔，单位是像素。
    int samplesSize = core()->channel(i)->samplesSize();
    int rightSampleIndex ;  // 显示在视窗可见部分中的最右边的样点的索引
    int startSampleIndex ;  // 第一个要画的样点（可能已超出视窗的可视范围）的索引。画波形时是从右往左画

    if ( xOffset>samplesSize-1 )
    {
        // 如果偏移量超出了通道的数据量，则不需要再画这个通道。
        return  ;
    }

    rightSampleIndex =  ( core()->channel(i)->latestValidPos() - xOffset + samplesSize ) % samplesSize ;
    startSampleIndex = core()->channel(i)->getNextValidSampleIndex (rightSampleIndex) ;

    if ( startSampleIndex == -1 )
    {
        // 如果最右边没有可用样点，说明该通道没有数据。不需要画这个通道。
        return  ;
    }

    int curIndex , nextIndex ;
    double curX , curY , nextX , nextY ;
    curIndex = startSampleIndex ;
    curX = width + core()->channel(i)->getIndexDistance(rightSampleIndex,curIndex) * deltaX ;		// 第一个要画的点在视窗中的坐标，单位是像素。
    curY = core()->channel(i)->sampleValue(curIndex)  -  core()->channel(i)->yRef() ;
    curY = curY	/ core()->channel(i)->yRange() * height ;

    // 画波形。
    // 画波形的停止条件：找不到下一个要画的样点（所有样点已画完）,
    //         或当前所画样点已超出视窗左边，即横坐标小于0（说明没有必要再画了，再画也看不到）。
    pen.setWidth(1);
    painter.setPen(pen);
    while ( curX > 0 )
    {
        nextIndex = core()->channel(i)->getPreValidSampleIndex(curIndex) ;
        if ( nextIndex == curIndex )    // 说明已经找不到下一个需要画的数据了
            break ;

        nextX = width - core()->channel(i)->getIndexDistance(nextIndex,rightSampleIndex) * deltaX ;
        nextY = core()->channel(i)->sampleValue(nextIndex)  -  core()->channel(i)->yRef() ;
        nextY = nextY / core()->channel(i)->yRange() * height ;

        // 用减号是因为屏幕坐标与狄卡尔坐标的y轴方向相反
        QLineF line(curX, middleY-curY/core()->channel(i)->yStretchRatio(), nextX, middleY-nextY/core()->channel(i)->yStretchRatio());
        painter.drawLine(line);

        curIndex = nextIndex ;
        curX = nextX ;
        curY = nextY ;
    }
}


#endif
