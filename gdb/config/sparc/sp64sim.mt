# Target: SPARC64 (with simulator)
# solib.o and procfs.o taken out for now.  We don't have shared libraries yet,
# and the elf version requires procfs.o but the a.out version doesn't.
# Then again, having procfs.o in a target makefile fragment seems wrong.
TDEPFILES = sparc-tdep.o $(SIMFILES)
TM_FILE= tm-sp64.h

# Need gcc for long long support.
CC = gcc

MH_CFLAGS = -I${srcdir}/../sim/sp64
SIMFILES = remote-sim.o ../sim/sp64/libsim.a

# The simulator uses the sqrt() function.
TM_CLIBS = -lm
