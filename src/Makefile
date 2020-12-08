# Executable names:
EXE = main
TEST = test

# Add all object files needed for compiling:
EXE_OBJ = main.o
OBJS = main.o fileIO.o fdgOutput.o classes.o structures/graph.o

# Generated files
# CLEAN_RM = testOutput.png testsquaremaze \
# 						*.o *.d tests/*.o tests/*.d cs225/*.d cs225/*.o \
# 						cs225/lodepng/*.o cs225/lodepng/*.d \
# 						cs225/catch/*.o cs225/catch/*.d

# Use the cs225 makefile template:
include structures/cs225.mk


# MP-specific targets
# DSETS_OBJS = testdsets.o
# DSETS_OBJS += dsets.o

testdsets: $(DSETS_OBJS)
	$(LD) $^ $(LDFLAGS) -o $@

MAZE_OBJS = $(filter-out $(EXE_OBJ), $(OBJS))
MAZE_OBJS += testFinalProj.o

testsquaremaze: $(MAZE_OBJS)
	$(LD) $^ $(LDFLAGS) -o $@