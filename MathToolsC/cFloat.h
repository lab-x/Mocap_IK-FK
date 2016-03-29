#ifndef __MathToolsC__cFloat__
#define __MathToolsC__cFloat__


//#define USE_FLOAT64_IN_C




#ifdef USE_FLOAT64_IN_C
    typedef double cFloat ;
    #define cSqrt sqrt


#else
    typedef float cFloat ;
    #define cSqrt sqrtf


#endif






#endif

