
#include "OscilloscopeChannel.h"
#include "stdio.h"
#include <QDebug>





OscilloscopeChannel::OscilloscopeChannel()
{
    setConfigurable(true);
    setSelect(false);

    YRange = 1.0 ; //  代表 +- 1.0
    YRef = 0.0 ;
    YOffset = 0.0 ;
    YStretchRatio = 1.0 ; // 不拉伸。
    ShowDelay = 0 ;

    SamplesType = Original_FLOAT64 ;
    unlockSamplesType() ;

    setDescription(NULL);
    setLockDescription(false) ;

    DeltaTimeStamp = 1 ;        // 已被遗弃。


    SamplesSize = MAX_SAMPLES_IN_EACH_CHANNEL ;


    reset();
}

OscilloscopeChannel::~OscilloscopeChannel()
{

}

void OscilloscopeChannel::reset()
{

	resetSamples();

    LatestValidTimeStamp = 0 ;

    SamplesLocked = false ;

}

void OscilloscopeChannel::resetSamples()
{
	for ( int i=0; i<samplesSize() ; i++ )
	{
		Samples[i].valid = false ;
        Samples[i].timeStamp = 0 ;
	}
    LatestValidPos = -1 ;
}


int OscilloscopeChannel::samplesSize()
{
    return SamplesSize ;
}


int OscilloscopeChannel::getIndexByTimeStamp(t_TimeStamp timeStamp)
{
    // 这个函数可以优化，提高效率。

    int index = latestValidPos() ;

    while (1)
    {
        if ( index < 0 )    // 通道样点数组中找不到有效样点时会出现index=-1;
            return -1 ;

        if ( Samples[index].timeStamp <= timeStamp )
            return index ;

        int nextIndex = getPreValidSampleIndex(index) ;
        if ( nextIndex == index )
            return -1 ; // 说明找不到时间戳比timestamp小的样点。

        index = nextIndex ;
    }
}

int OscilloscopeChannel::getSampleIndex ( int startIndex , int offset )
{
    if ( startIndex < 0 || startIndex > samplesSize()-1 || abs(offset) > samplesSize()-1 )
        return -1 ;

	int index = startIndex ;
	index += offset ;
	if (index<0)
		index += samplesSize();

	return index % samplesSize() ;
}

int OscilloscopeChannel::getPreSampleIndex (int startIndex)
{
    if ( startIndex < 0 || startIndex > samplesSize()-1 )
        return -1 ;

	if (startIndex==0)
		return samplesSize() - 1 ;
	else
		return startIndex - 1 ; 
}

int OscilloscopeChannel::getNextSampleIndex(int startIndex)
{
    if ( startIndex < 0 || startIndex > samplesSize()-1 )
        return -1 ;

	startIndex ++ ;
	if (startIndex==samplesSize())
		return 0 ;
	else
		return startIndex; 
}


int OscilloscopeChannel::getPreValidSampleIndex(int startIndex)
{
    if ( startIndex < 0 || startIndex > samplesSize()-1 )
        return -1 ;

    int defaultRet = -1 ;
	if ( Samples[startIndex].valid )
        defaultRet = startIndex ;

	int index = startIndex ;
	while ( 1 )
	{
		index = getPreSampleIndex ( index ) ;

        if ( index == LatestValidPos )
            return defaultRet ;		// 表明该数前面已经没有可用的样点了。

		if ( Samples[index].valid )
			return index ;
	}
}

int OscilloscopeChannel::getNextValidSampleIndex(int startIndex)
{
    if ( startIndex < 0 || startIndex > samplesSize()-1 )
        return -1 ;

    int defaultRet = -1 ;
	if ( Samples[startIndex].valid )
        defaultRet = startIndex ;

	int index = startIndex ;
    while ( index != LatestValidPos )
	{
		index = getNextSampleIndex (index) ;

		if ( Samples[index].valid )
			return index ;		
	}

    return defaultRet ;		// 表明该数后面已经没有可用的样点了。
}






void OscilloscopeChannel::clearSamples(int startIndex, int endIndex)
{
	// 清除区域包括 startIndex ,不包括 endIndex 。
	// 注意，如果 endIndex 与 startIndex 相等，不清除任何数据。
	// 如果 endIndex 比 startIndex 小，则清除区域为 [startIndex,samplesSize()) + [0,endIndex)

	int i=startIndex ;
	while ( i !=  endIndex )
	{
		Samples[i].valid = false ;
        Samples[i].timeStamp = 0 ;
		i = getNextSampleIndex(i) ;
	} 
}


bool OscilloscopeChannel::addSample ( double value , t_TimeStamp startTimeStamp)
{
	if ( SamplesType != Original_FLOAT64 )
		return false ;

    return addSamples ( (void*)&value,1,startTimeStamp) ;
}

bool OscilloscopeChannel::addSample ( float value , t_TimeStamp startTimeStamp)
{
	if ( SamplesType != Original_FLOAT32 )
		return false ;

    return addSamples ( (void*)&value,1,startTimeStamp) ;
}
bool OscilloscopeChannel::addSample ( int32_t value , t_TimeStamp startTimeStamp)
{
	if ( SamplesType != Original_INT32 )
		return false ;

    return addSamples ( (void*)&value,1,startTimeStamp) ;
}
bool OscilloscopeChannel::addSample ( uint32_t value , t_TimeStamp startTimeStamp )
{
	if ( SamplesType != Original_UINT32 )
		return false ;

    return addSamples ( (void*)&value,1,startTimeStamp) ;
}
bool OscilloscopeChannel::addSample ( int16_t value , t_TimeStamp startTimeStamp )
{
	if ( SamplesType != Original_INT16 )
		return false ;

    return addSamples ( (void*)&value,1,startTimeStamp) ;
}
bool OscilloscopeChannel::addSample ( uint16_t value , t_TimeStamp startTimeStamp )
{
	if ( SamplesType != Original_UINT16 )
		return false ;

    return addSamples ( (void*)&value,1,startTimeStamp) ;
}
bool OscilloscopeChannel::addSample ( int8_t value , t_TimeStamp startTimeStamp)
{
	if ( SamplesType != Original_INT8 )
		return false ;

    return addSamples ( (void*)&value,1,startTimeStamp) ;
}
bool OscilloscopeChannel::addSample ( uint8_t value , t_TimeStamp startTimeStamp )
{
	if ( SamplesType != Original_UINT8 )
		return false ;

    return addSamples ( (void*)&value,1,startTimeStamp) ;
}




bool OscilloscopeChannel::addSamples ( void* samples , int count , t_TimeStamp startTimeStamp, t_TimeStamp DeltaTime)
{
    if ( SamplesLocked )
        return false ;

	int size = sizeOfEachSample () ;
	int i ;
    t_TimeStamp curtimeStamp ;

    if ( DeltaTime == 0 )
    {
        DeltaTime = 1 ;
    }

    if (startTimeStamp==0)
    {
        startTimeStamp = latestValidTimeStamp() + DeltaTime ;
    }

	for(i=0; i<count ; i++ )
	{
        curtimeStamp=startTimeStamp+i*DeltaTime ;
        if ( curtimeStamp < latestValidTimeStamp() )
		{
//            qDebug()<<"time stamp conflict when add sample to channel . ";
			continue ;
		}

        LatestValidPos ++ ;
        LatestValidPos %= samplesSize() ;

        memcpy ( &(Samples[LatestValidPos].sample) , (char*)samples+(i*size) , size ) ;
        Samples[LatestValidPos].timeStamp = curtimeStamp ;
        Samples[LatestValidPos].valid = true ;
        LatestValidTimeStamp = Samples[LatestValidPos].timeStamp ;
	}

	return true ;
}

double OscilloscopeChannel::sampleValueAtOffset (uint offsetToLatest , void* rightTypeData ,t_TimeStamp *time, bool* noValid )
{
	if ( ( offsetToLatest & 0x80000000 ) || ( offsetToLatest >= samplesSize() ) )		// 如果索引太大就设置 noValid
	{
        if ( noValid ) { *noValid = true ; }
		if ( rightTypeData ) { memset ( rightTypeData, 0 , sizeOfEachSample() ) ; }
        if ( time ) { *time = 0 ; }
		return 0.0 ;
	}

	int offset = - (int)offsetToLatest ;
    int index = getSampleIndex(latestValidPos(),offset) ;
    if ( index < 0 )
    {
        if ( noValid ) { *noValid = true ; }
        if ( rightTypeData ) { memset ( rightTypeData, 0 , sizeOfEachSample() ) ; }
        if ( time ) { *time = 0 ; }
        return 0.0 ;
    }

    return sampleValue(index , rightTypeData, time , noValid ) ;
}


double OscilloscopeChannel::sampleValue ( int i , void* rightTypeData ,t_TimeStamp *time, bool* noValid )
{
	if (Samples[i].valid)
	{
        if ( noValid ) { *noValid = false ;}

        if ( rightTypeData ) { memcpy ( rightTypeData, &(Samples[i].sample) , sizeOfEachSample() ) ; }

        if ( time ) { *time = Samples[i].timeStamp ; }

		switch(SamplesType)
		{
		case Original_INT8:	
			return (double)Samples[i].sample.m_int8 ;
		case Original_UINT8:
			return (double)Samples[i].sample.m_uint8 ;
		case Original_INT16:	
			return (double)Samples[i].sample.m_int16 ;
		case Original_UINT16:
			return (double)Samples[i].sample.m_uint16 ;
		case Original_INT32:	
			return (double)Samples[i].sample.m_int32 ;
		case Original_UINT32:
			return (double)Samples[i].sample.m_uint32 ;
		case Original_FLOAT32:	
			return (double)Samples[i].sample.m_float32 ;
		case Original_FLOAT64:
			return (double)Samples[i].sample.m_float64 ;
		default :
			return 0 ;
		}
	}
}



int OscilloscopeChannel::sizeOfEachSample ()
{
	int size ;
	switch(SamplesType)
	{
	case Original_INT8:
	case Original_UINT8:
		size = 1 ;
		break ;
	case Original_INT16:
	case Original_UINT16:
		size = 2 ;
		break ;
	case Original_INT32:
	case Original_UINT32:
	case Original_FLOAT32:
		size = 4 ;
		break ;
	case Original_FLOAT64:
		size = 8 ;
		break ;
	default:
		size = 0 ;
	}
	return size ;
}

int OscilloscopeChannel::getIndexDistance(int index1,int index2)
{
    if ( index2 < index1 )
    {
        index2+=samplesSize() ;
    }
    return index2 - index1 ;
}


double OscilloscopeChannel::sampleToDouble ( void*data )
{
    OriginalData* sample = (OriginalData*)data ;
    switch(SamplesType)
    {
    case Original_INT8:
        return (double)(sample->m_int8) ;
    case Original_UINT8:
        return (double)(sample->m_uint8) ;
    case Original_INT16:
        return (double)(sample->m_int16) ;
    case Original_UINT16:
        return (double)(sample->m_uint16) ;
    case Original_INT32:
        return (double)(sample->m_int32) ;
    case Original_UINT32:
        return (double)(sample->m_uint32) ;
    case Original_FLOAT32:
        return (double)(sample->m_float32) ;
    case Original_FLOAT64:
        return (double)(sample->m_float64) ;
    default :
        return 0 ;
    }
}


double OscilloscopeChannel::sampleValueAtTime( t_TimeStamp time , void* rightTypeData, bool* noValid , bool* interpolation )
{
    int index = getIndexByTimeStamp(time) ;

    if ( index == -1 || time > latestValidTimeStamp() )
    {
        // 对应的时间戳无效，返回0，并设置 noValid 标志。
        if ( noValid )
            *noValid = true ;
        if (rightTypeData)
            memset(rightTypeData,0,sizeOfEachSample()) ;
        return 0.0 ;
    }

    // 时间戳对应的数据有效
    if (noValid)
        *noValid = false ;

    if ( Samples[index].timeStamp == time )
    {
        // 不用插值，直接返回对应时间戳的数据
        if (interpolation)
            *interpolation = false ;
        memcpy(rightTypeData,&(Samples[index].sample),sizeOfEachSample()) ;
        return sampleToDouble(&(Samples[index].sample));
    }

    // 插值
    if (interpolation)
        *interpolation = true ;

    int leftIndex = index ;
    int rightIndex = getNextValidSampleIndex(leftIndex) ;
    t_TimeStamp leftTime = Samples[leftIndex].timeStamp ;
    t_TimeStamp rightTime = Samples[rightIndex].timeStamp ;
    double leftValue = sampleToDouble(&(Samples[leftIndex].sample)) ;
    double rightValue = sampleToDouble(&(Samples[rightIndex].sample)) ;

    double ratio = (double)(time-leftTime)/(rightTime-leftTime) ;
    double value = leftValue + ratio * ( rightTime-leftTime ) ;

    int8_t value8;
    int16_t value16;
    int32_t value32 ;
    float valuef ;
    int8_t* pValue ;

    switch(SamplesType)
    {
    case Original_INT8:
    case Original_UINT8:
        value8 = (int8_t) value ;
        pValue = (int8_t*)&value8 ;
        break ;
    case Original_INT16:
    case Original_UINT16:
        value16 = (int16_t)value ;
        pValue = (int8_t*)&value16 ;
        break ;
    case Original_INT32:
    case Original_UINT32:
        value32 = (int32_t)value ;
        pValue = (int8_t*)&value32 ;
        break ;
    case Original_FLOAT32:
        valuef = (float)value ;
        pValue = (int8_t*)&valuef ;
        break ;
    case Original_FLOAT64:
        pValue = (int8_t*)&value ;
        break ;
    }
    memcpy(rightTypeData,pValue,sizeOfEachSample());

    return value ;

}




















/****************************************************************************************/

// 已经被抛弃的函数们, say good bye to them .

#if  Compile_Obsolete_Funtions_For_OscilloscopeChannel

int OscilloscopeChannel::calculateSampleIndexByTimeStamp(t_TimeStamp& timeStamp)
{
    t_TimeStamp LoopTime = deltaTimeStamp() * samplesSize() ;
    t_TimeStamp timeMOD = timeStamp % LoopTime ;	// 时间余数
    int index = timeMOD / deltaTimeStamp() ;
    timeStamp -= timeMOD % deltaTimeStamp() ;		// 校正timeStamp .

    if ( index < samplesSize() )
        return index ;
    else
        return index % samplesSize() ;
}


void OscilloscopeChannel::setDeltaTimeStamp(t_TimeStamp newDelta)
{
    if ( newDelta!=0 && newDelta!=DeltaTimeStamp )
    {
        resetSamples();
        DeltaTimeStamp = newDelta ;
    }
}
t_TimeStamp OscilloscopeChannel::deltaTimeStamp()
{
    return DeltaTimeStamp ;
}


#endif
