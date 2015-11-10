
.PHONY: clean
.PHONY: all

CXX = g++

ifdef ROOTSYS
CFLAGS = -c -g -Wall
# IFLAGS += -I $(ROOTSYS)/include
# #ROOTLIBS = $(ROOTLIBS)
# ROOTLIBS = $(shell $(ROOTSYS)/bin/root-config --nonew --glibs) -lMinuit -lSpectrum
# ROOTLIBS := $(filter-out -lNew,$(ROOTLIBS))
# ROOTLIBS := $(filter-out -lpthread,$(ROOTLIBS))

LDFLAGS  := `root-config --libs`
CPPFLAGS := `root-config --cflags`
ROOTLIBS  = -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic -lMinuit -lSpectrum

else
all:
	@echo "ROOT not found!"
endif




all: generator_talys

# generator_dicebox: *.o
# 	$(CXX) $(IFLAGS) $(ROOTLIBS) $^ -o $@
# 
# %.o: %.cc
# 	$(CXX) $(IFLAGS) $(CFLAGS) $(ROOTLIBS) $^

# generator_talys: *.o
# 	$(CXX) $(LDFLAGS) $(ROOTLIBS) $^ -o $@
generator_talys: *.o
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cc
	$(CXX) $(CFLAGS) $(CPPFLAGS) $^

	
clean:
	rm -f *.o *.so generator_talys


