#ifndef _OSCILLOSCOPE_CORE_H_
#define _OSCILLOSCOPE_CORE_H_


#include "stdint.h"

#include "OscilloscopeChannel.h"

#include <QColor>


class OscilloscopeCore
{

public :
	OscilloscopeCore();
	~OscilloscopeCore();

	// 返回通道数量
	int channelsCount() {return ChannelsCount ;}
    //void setChannelCount(int count){};    // 暂时未加此功能。

    // 返回横向刻度线的数量（的一半）。（与Y轴相交刻度线的多少条）
    int scaleLinesInY() { return ScaleLinesInY;}
    void setScaleLinesInY(int lines) {ScaleLinesInY=lines;}

    // 返回纵向刻度线的数量。（与X轴相交刻度线的多少条）
    int scaleLinesInX() { return ScaleLinesInX;}
    void setScaleLinesInX(int lines) {ScaleLinesInX=lines;}


	// 返回一个channel的指针。
	OscilloscopeChannel* channel(int i) {return Channels+i;}
	
	// 设置XRange.表示视窗宽度对应的时间轴长度,单位为相邻样点的时间戳间隔。或者说视窗中显示的总样点数(减一)
    void setXRange(int range) {XRange=abs(range);}
	int xRange() {return XRange;}

	// 设置波形X方向偏移。代表时间轴偏移的长度，单位为相邻样点的时间戳间隔。或者说偏移的样点数。只能为负。
    void setXOffset(int offset) { XOffset=-abs(offset);}
    int xOffset() {return XOffset;}

    // // 本示波器内核是否已被示波器配置窗口选中。一个内核只能被一个配置窗口选中。
    bool selected() {return Selected;}
    void setSelect(bool select) { Selected = select ; }

    // 背景颜色
    unsigned int bGColor() {return BGColor;}

    // 坐标轴颜色
    unsigned int axisColor() {return AxisColor;}


    // 设置在显示时各通道之间是按照时间戳同步的方式显示，还是各通道使用自己的时间戳。
    bool timeStampSyn() {return TimeStampSyn;}
    void setTimeStampSyn(bool syn){TimeStampSyn=syn;}

    // 返回示波器的最新时间戳。就是查询各通道的时间戳，然后返回最大的值。仅当使用时间戳同步功能时使用。
    t_TimeStamp latestTimeStamp();
    // 复位所有通道，包括时间戳归0。
    void resetAllChannels();
    // 锁定所有通道
    void lockAllChannels();

private:

    int XRange ;		// 视窗宽度对应的时间轴长度,单位为相邻样点的时间戳间隔。相邻样点的时间戳间隔一般为1ms.
	int XOffset	;	// 波形X方向偏移。代表时间轴偏移的长度，单位为相邻样点的时间戳间隔。或者说偏移的样点数。只能为负。	

	unsigned int BGColor ;		// 背景颜色
	unsigned int AxisColor ;	// 坐标轴颜色

    bool Selected ;     // 本示波器内核是否已被示波器配置窗口选中。一个内核只能被一个配置窗口选中。


    bool TimeStampSyn ;         // 设置各通道时间戳是否同步。设置同步后，任何一个通道添加数据时，其他通道的时间戳也会跟着更新。


	// 通道数量
	int ChannelsCount ;
    int ScaleLinesInX ;
    int ScaleLinesInY ;

	static const int MAX_CHANNELS_IN_EACH_SCOPE = 6 ;
    static const int SCALE_LINES_IN_Y = 4 ; // Y轴0点两边各4条，共8条。
    static const int SCALE_LINES_IN_X = 10 ;


    OscilloscopeChannel Channels[MAX_CHANNELS_IN_EACH_SCOPE] ;	//通道

};


#endif
