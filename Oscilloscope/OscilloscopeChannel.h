#ifndef _OSCILLOSCOPE_CHANNEL_H_
#define _OSCILLOSCOPE_CHANNEL_H_


#include "stdint.h"

#include <QColor>

typedef int32_t t_TimeStamp ;


class OscilloscopeChannel
{

public:
	OscilloscopeChannel();
	~OscilloscopeChannel();



	// 函数描述：向通道中添加一个样点。样点值为value，样点时间戳为startTimeStamp。
    // startTimeStamp为0时表示使用默认的时间戳.
    // 出现错误返回false (一般是类型错误).
    bool addSample ( double value , t_TimeStamp startTimeStamp = 0 ) ;
    bool addSample ( float value , t_TimeStamp startTimeStamp = 0 ) ;
    bool addSample ( int32_t value , t_TimeStamp startTimeStamp = 0 ) ;
    bool addSample ( int16_t value , t_TimeStamp startTimeStamp = 0 ) ;
    bool addSample ( int8_t value , t_TimeStamp startTimeStamp = 0 ) ;
    bool addSample ( uint32_t value , t_TimeStamp startTimeStamp = 0 ) ;
    bool addSample ( uint16_t value , t_TimeStamp startTimeStamp = 0 ) ;
    bool addSample ( uint8_t value , t_TimeStamp startTimeStamp = 0 ) ;


    // 函数描述：向通道中添加count个样点，samples指向要添加的数据。本函数适用于所有基本数据类型，函数内部不做类型检查！！
    // 参数DeltaTime表示相邻的两个样点添加到Samples数组中后，二者之间的时间戳间隔。如果DeltaTime为0，则使用默认的时间戳间隔
    // 返回类型本来是void，但为了跟addSample函数保持一致，所以返回bool。当前版本此函数总是返回true。
    bool addSamples ( void* samples , int count , t_TimeStamp startTimeStamp = 0 , t_TimeStamp DeltaTime = 0  );


    // 函数描述：读取time时刻通道的采样值。
    // 如果rightTypeData被设置，样点的数值还会以样点所属的类型拷贝到rightTypeData中。
    // 如果通道的样点数组中所有样点的时间戳都不等于time，则返回通道中时间戳距离time最近的左右两个采样值
    // 进行线性插值后的结果。如果返回值是插值运算的结果，则会设置interpolation标志位。
    // 如果通道中没有符合要求的数据，则返回0，并设置noValid标志。
    double sampleValueAtTime(t_TimeStamp time, void* rightTypeData = NULL , bool* noValid=NULL , bool* interpolation = NULL );	// 有待更新

    // 函数描述：读取到LatestValidPos的索引偏移为offsetToLatest处的样点数据值(以double类型返回)。
    // 如果rightTypeData被设置，样点的数值还会以样点所属的类型拷贝到rightTypeData中。
    // 如果time被设置，样点的时间戳会赋值给time.
    // 如果偏移为offsetToLatest处样点的valid为false，则返回0并设置noValid标志。
    double sampleValueAtOffset(uint offsetToLatest, void* rightTypeData = NULL , t_TimeStamp *time=NULL, bool* noValid=NULL );




	// 返回样点数组的大小
	int samplesSize();
 	// 返回每个样点的大小（占用的字节数），与SamplesType对应
	int sizeOfEachSample ();	
	typedef enum OriginalDataType_t
	{
        Original_INT8 = 0 ,
		Original_UINT8 ,	
		Original_INT16 ,	
		Original_UINT16 ,
		Original_INT32 ,
		Original_UINT32 ,
		Original_FLOAT32 ,
		Original_FLOAT64 
	}OriginalDataType;


	// 设置样点的数据类型
    OriginalDataType samplesType() {return SamplesType ;}
    void setSamplesType(OriginalDataType newType){	// 修改样点类型时会先清除所有样点的数据。
        if ( newType != SamplesType ){
            resetSamples () ;
            SamplesType = newType ;
        }
    }

    // 设置是否锁定样点的数据类型
    bool samplesTypeLocked() {return SamplesTypeLocked;}
    void unlockSamplesType() {SamplesTypeLocked=false;}
    void lockSamplesType() {SamplesTypeLocked=true;}


	// 设置是否显示通道波形
    void setShow(bool show=true) {Show = show ;}
    bool show(){return Show;};

	// 设置Y轴数值范围/量程。真正的量程范围是 ( -Range , Range ) 
	void setYRange(double range) {YRange = range;}
	double yRange() {return YRange;}

    // 设置Y的参考线，绝对值最好不大于YRange。
	void setYRef ( double ref ) { YRef = ref;}	
	double yRef() {return YRef; }

	// 设置Y轴偏移。绝对值小于1，代表偏移量与视窗高度的比值
	void setYOffset ( double offset ) {YOffset = offset;}	
	double yOffset() {return YOffset; }

	// 设置Y轴的拉伸比例。绝对值不能大于1 ,为负时表示反相。
	void setYStretchRatio(double stretch) {YStretchRatio = stretch;}
	double yStretchRatio () {return YStretchRatio;}

	// 设置通道的波形颜色
	void setColor(int color) {Color=color;}
	int color() {return Color;}	

    // 选择是否允许对通道进行配置。
    bool configurable() {return Configurable ;}
    void setConfigurable(bool configurable) {Configurable=configurable;}

    // 通道配置窗口选中通道、查询通道是否被配置窗口选中。一个通道同一时刻只能被一个配置窗口选中。
    bool selected() {return Selected ;}
    void setSelect(bool selected) {Selected=selected;}

    // 设置通道的描述
    const char* description(){return Description ; }
    int descriptionBufLen(){return sizeof(Description);}
    void setDescription(const char*des ) {
        if (des==NULL||strlen(des)==0) {Description[0]='\0';return;}
        int cpyLen = strlen(des)<(descriptionBufLen()-1)?strlen(des):(descriptionBufLen()-1) ;
        memcpy(Description,des,cpyLen);
        Description[cpyLen] = '\0' ;
    }
    // 设置是否锁定通道的描述
    bool descriptionLocked(){return LockDescription;}
    void setLockDescription(bool lock) {LockDescription = lock;}


    // 设置显示延时。返回值为最终设定的延时值，正常情况下返回值与delay相等，除非delay比通道的最新时间戳还大，这时返回最新时间戳。
    t_TimeStamp setShowDelay(t_TimeStamp delay){
        if ( delay > latestValidTimeStamp() )
            delay = latestValidTimeStamp() ;
        ShowDelay = delay ;
        return ShowDelay ;
    }
    t_TimeStamp showDelay(){return ShowDelay;}



    // 锁定样点数组，锁定后样点数组就固定了，不能再向其中添加样点
    void lockSamples(){SamplesLocked=true;}
//    void unlockSamples(){SamplesLocked=false;}    // 不设置此函数。要解锁就复位reset
    bool samplesLocked(){return SamplesLocked;}

    // 将所有样点复位（清除），LatestValidPos也会被设成-1.并且Latest时间戳归零。同时还会解锁样点数组
    void reset();

    // 返回时间戳最新的样点在FIFO中的位置； 返回对应的最新时间戳；
    int latestValidPos() {return LatestValidPos ;}
    t_TimeStamp latestValidTimeStamp(){return LatestValidTimeStamp;}



private:

    // 计算startIndex偏移offset后的索引值。这个函数只是做索引计算，而不检查对应索引处的数据是否有意义!!
    // startIndex必须是合理的值，即必须是非负的而且小于通道样点数组的大小。否则返回-1.
    // offset可为正亦可为负，但其绝对值应小于样点数组的长度，否则返回-1.
    // 返回值即为要求的索引值。正确的索引值都是非负的，返回-1说明参数不合理。
    int getSampleIndex ( int startIndex , int offset ) ;		

    // 计算从index1到index2之间的索引距离。这个函数只是做索引计算，而不检查对应索引处的数据是否有意义!!
    // index1和index2必须是合理的索引值，否则返回-1.
    // 正常情况下，返回值都为非负，表示从index1到index2之间的索引距离。
    int getIndexDistance(int index1,int index2) ;

    // 根据一个索引值求它的上一个或下一个样点的索引值。等价于调用 getSampleIndex(startIndex,1或-1)。
	int getPreSampleIndex (int startIndex) ;
	int getNextSampleIndex (int startIndex) ;    


	// 根据一个索引值找他前/后第一个有效样点的索引值。
    // 如果参数startIndex的值不合理，直接返回-1。
	// 如果在startIndex的前/后没有找到有效点，则：
    //		如果startIndex处的数据本身是有效的，则返回startIndex ;
    //		如果startIndex处的数据本身非有效，则返回 -1 .
    // 这两个函数与getPreSampleIndex/getNextSampleIndex的一个重要区别在于，下面这两个函数会检查对应索引处的数据
    // 是否有意义。如 getPreValidSampleIndex，他会保证返回的索引处数据的时间戳不会比startIndex大；相反的，
    // getNextValidSampleIndex会保证返回的索引处时间戳不会比startIndex小。注意，有些情况下这两个函数还可能会将
    // startIndex原数返回。
	int getPreValidSampleIndex (int startIndex);
	int getNextValidSampleIndex (int startIndex);




    // 其功能与sampleValueInOffset函数类似，只不过第一个参数不是到LatestValidPos的偏移，而是一个Samples数组的索引。
	// 这个函数只在内部使用。
    double sampleValue(int i, void* rightTypeData = NULL , t_TimeStamp *time=NULL, bool* noValid=NULL );	// 有待更新



    // 将所有样点复位（清除），LatestValidPos也会被设成-1.Latest时间戳保留。
	void resetSamples();



    // 清除索引值在startIndex与endIndex之间的样点数据。清除区域包括startIndex但不包括endIndex。
    void clearSamples(int startIndex, int endIndex ) ;



    // 返回通道样点数组中时间戳等于timeStamp的有效样点的索引。如果没有等于
    // timeStamp的时间戳，就找时间戳距离timeStamp最近且小于timeStamp
    // 的样点的索引。如果所有样点的时间戳都比timeStamp大，则返回-1；
    // 如果找不到任何有效样点，也返回-1；
    int getIndexByTimeStamp(t_TimeStamp timeStamp);

    // 将采样数数据转化成double输出
    double sampleToDouble ( void*data ) ;



private:		//  types define
	typedef union {
		signed char m_int8 ;
		unsigned char m_uint8 ;
		signed short m_int16 ;
		unsigned short m_uint16 ;
		signed int m_int32 ;
		unsigned int m_uint32 ;
		float m_float32 ;
		double m_float64 ;
	} OriginalData ;

	typedef struct {

		OriginalData sample ;
        t_TimeStamp timeStamp ;
		bool valid ;

	} SamplingPoint ;

private:
	double YRange ;	//	量程。真正的量程范围是 ( -Range , Range ) 
	double YRef ;		// 画波形时的参考线。绝对值小于YRange 。
	double YOffset  ;	// 波形Y方向偏移。绝对值小于1，代表偏移量与视窗高度的比值
	double YStretchRatio ; 		// Y轴伸展比率，绝对值不能大于1 。为负时表示反相。
	unsigned int Color ;	// 波形颜色
	bool Show ;	// 是否显示
    t_TimeStamp ShowDelay ; // 显示时延迟的时间戳。

    bool Configurable ; // 是否允许对通道进行配置。
    bool Selected ; // 是否已被通道配置窗口选中。一个通道同一时刻只能被一个配置窗口选中。

	OriginalDataType SamplesType ;		// 样点的数据类型
    bool SamplesTypeLocked ;      // 锁定样点的数据类型

    bool SamplesLocked ;  // 锁定样点数组。样点数组被锁定后就无法再想其中添加样点

static const int MAX_SAMPLES_IN_EACH_CHANNEL = 5000 ;

	SamplingPoint Samples[MAX_SAMPLES_IN_EACH_CHANNEL] ;	// 样点数组,FIFO
	int SamplesSize ;
    int LatestValidPos ;	// 最新的有效样点在FIFO中的位置
    t_TimeStamp LatestValidTimeStamp ;  // 最新的有效样点的时间戳

    char Description[64] ;
    bool LockDescription ;


    friend class OscilloscopeCore ;
    friend class OscilloscopeScreen ;



    // 已经被抛弃的函数们, say good bye to them .
#define Compile_Obsolete_Funtions_For_OscilloscopeChannel 1 //0
#if  Compile_Obsolete_Funtions_For_OscilloscopeChannel

private:

    t_TimeStamp DeltaTimeStamp ;	// 连续添加样点时（不单独指定每一个样点的时间戳的话），新添样点两两间默认的时间戳间隔
    int calculateSampleIndexByTimeStamp(t_TimeStamp& timeStamp) ;

public:
    // 设置deltaTimeStamp，即Samples数组中相邻两个元素的时间戳的差值。
    void setDeltaTimeStamp(t_TimeStamp newDelta);	// 改变deltaTimeStamp时会首先清除所有样点的数据。
    t_TimeStamp deltaTimeStamp() ;


#endif

};

#endif
