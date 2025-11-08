///Link to _this_ file and include the header file:so it builds faster
/// than compiling all source files that include the header files after it was
/// changed.

///Stefan Gebauer's(TU Berlin matriculation number 361095) ~"cmnSrc" repository:
 #include <dataType\charStr\stringize.h>///TU_Bln361095stringize

///this project's "SMARTmon" repository:
 #include "SMARTvalueRater.hpp"///TU_Bln361095SMARTmonUse(BestRtgVal)
 //#include "SMARTattrValRtngDecl.hpp"///enum TU_Bln361095SMARTmonDef(Rating)

#pragma message("__cplusplus version:" \
  TU_Bln361095expandAndStringize(__cplusplus) )

#if __cplusplus >= 201103L///std::is_same_v(...), decltype(...)
#pragma message("decltype(TU_Bln361095SMARTmonUse(BestRtgVal) ):" \
  TU_Bln361095expandAndStringize(decltype(TU_Bln361095SMARTmonUse(BestRtgVal) )\
  ) )
  //#if decltype(TU_Bln361095SMARTmonUse(BestRtgVal) ) == float)
  #if std::is_same_v< \
    decltype(TU_Bln361095SMARTmonUse(BestRtgVal) ) \
    /*TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)*/ \
    , float> \
      ::value == true
    #include <attributes/defineRtgRngValsAsFloat.c>
  #endif
#else///C++ version < C++ 11
  #include <attributes/defineRtgRngValsAsFloat.c>
#endif
