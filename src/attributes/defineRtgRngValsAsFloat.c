
///Definition of variables must/should be in a source(.cpp) file to ensure they
/// are done only once.
TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) TU_Bln361095SMARTmonUse(BestRtgVal)
  = 1.0;

TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) TU_Bln361095SMARTmonUse(
  AtLeastWarnRtgVal) = 0.0;

TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) TU_Bln361095SMARTmonUse(WorstRtgVal)
  = -1.0;
TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) TU_Bln361095SMARTmonUse(RtgValRng) =
  //BestRtgVal + 
  //abs(TU_Bln361095SMARTmonUse(WorstRtgVal) )
  2.0;
