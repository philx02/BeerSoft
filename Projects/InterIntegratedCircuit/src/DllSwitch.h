#pragma once

#ifdef WIN32
  #ifdef InterIntegratedCircuit_EXPORTS
    #define INTERINTEGRATEDCIRCUIT_LIB __declspec(dllexport)
  #else
    #define INTERINTEGRATEDCIRCUIT_LIB __declspec(dllimport)
  #endif
#else
  #define INTERINTEGRATEDCIRCUIT_LIB
#endif
