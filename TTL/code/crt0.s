// we need different crt0.s depending if we're compiling for CD
// or otherwise.

#ifdef CDROM
  #include "crt0-boot.s"
#else
  #include "crt0-atwinmon.s"
#endif
