#ifndef PHYSICS_UTIL_ROOT_LOADED
#define PHYSICS_UTIL_ROOT_LOADED

#include <TSystem.h>

namespace physics {
// suppress the ROOT signal handlers (they interfere with debugging tools, code
// analyzers and manual signal handlers (e.g. in a DAQ).)
//
// inline definition so we don't need to link the root libraries when not needed
inline void root_suppress_signals() {
  gSystem->ResetSignal(kSigChild);
  gSystem->ResetSignal(kSigBus);
  gSystem->ResetSignal(kSigSegmentationViolation);
  gSystem->ResetSignal(kSigIllegalInstruction);
  gSystem->ResetSignal(kSigSystem);
  gSystem->ResetSignal(kSigPipe);
  gSystem->ResetSignal(kSigAlarm);
  gSystem->ResetSignal(kSigUrgent);
  gSystem->ResetSignal(kSigFloatingException);
  gSystem->ResetSignal(kSigWindowChanged);
}
}

#endif
