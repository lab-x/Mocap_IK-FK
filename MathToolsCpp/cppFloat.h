#ifndef __MathtoolsCpp__CppFloat__
#define __MathtoolsCpp__CppFloat__


//#define USE_FLOAT64_IN_CPP




#ifdef USE_FLOAT64_IN_CPP
    typedef double Float ;
    #define Sqrt sqrt


#else
    typedef float Float ;
    #define Sqrt sqrtf


#endif






#endif

