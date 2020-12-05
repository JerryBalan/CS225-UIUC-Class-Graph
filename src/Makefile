EXENAME = main
OBJS = main.o fileIO.o classes.o graph.o disjoint.o fdgOutput.o

CXX = clang++
CXXFLAGS = $(CS225) -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -pedantic
LD = clang++
LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi -lm

# Custom Clang version enforcement logic:
ccred=$(shell echo -e "\033[0;31m")
ccyellow=$(shell echo -e "\033[0;33m")
ccend=$(shell echo -e "\033[0m")

IS_EWS=$(shell hostname | grep "ews.illinois.edu")
IS_CORRECT_CLANG=$(shell clang -v 2>&1 | grep "version 6")
ifneq ($(strip $(IS_EWS)),)
ifeq ($(strip $(IS_CORRECT_CLANG)),)
CLANG_VERSION_MSG = $(error $(ccred) On EWS, please run 'module load llvm/6.0.1' first when running CS225 assignments. $(ccend))
endif
else
ifneq ($(strip $(SKIP_EWS_CHECK)),True)
CLANG_VERSION_MSG = $(warning $(ccyellow) Looks like you are not on EWS. Be sure to test on EWS before the deadline. $(ccend))
endif
endif

.PHONY: all test clean output_msg

all : $(EXENAME)

output_msg: ; $(CLANG_VERSION_MSG)

$(EXENAME) : output_msg $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXENAME)

main.o : main.cpp fileIO.h
	$(CXX) $(CXXFLAGS) main.cpp

fileIO.o : fileIO.cpp fileIO.h
	$(CXX) $(CXXFLAGS) fileIO.cpp

disjoint.o: ../structures/disjoint.cpp ../structures/disjoint.h
	$(CXX) $(CXXFLAGS) ../structures/disjoint.cpp

graph.o: ../structures/graph.cpp ../structures/graph.h
	$(CXX) $(CXXFLAGS) ../structures/graph.cpp

classes.o : classes.cpp classes.h fileIO.h ../structures/graph.h
	$(CXX) $(CXXFLAGS) classes.cpp

fdgOutput.o : fdgOutput.cpp fdgOutput.h ../structures/graph.h ../structures/PNG.h
	$(CXX) $(CXXFLAGS) fdgOutput.cpp



clean :
	-rm -f *.o $(EXENAME) test
