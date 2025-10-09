echo CMake build generator: %1

cmake -G %1 ^
  "-DTU_Bln361095BoostCppLibRootDir=%2"