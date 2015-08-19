INSTDIR=/usr/opt/nscldaq/11.0-005

include $(INSTDIR)/filterkit/FilterIncludes




#
#  Add your own compilation/link flags here:

USERCXXFLAGS=-std=c++11 -O2
USERCCFLAGS=$(USERCCFLAGS)
USERLDFLAGS= -Wl,-rpath=$(INSTDIR)/lib

#
#  Add the names of objects you need here if you modified the name of the driver file, 
#  this should also reflect thtat.
#
#OBJECTS = CS800Filter.o SkeletonMain.o
OBJECTS = CS800Filter.o FragmentIndex.o SkeletonMain.o
#OBJECTS = CFragCounterFilter.o FragmentIndex.o SkeletonMain.o
#OBJECTS = CDuplicateFilter.o SkeletonMain.o
#OBJECTS = CCaesarFilter.o SkeletonMain.o

# 
#  Modify the line below to provide the name of the library you are trying to build
#  it must be of the form libsomethingorother.so
#

USERFILTER = UserFilter

all : $(USERFILTER) unittests check

$(USERFILTER): $(OBJECTS) 
	   $(CXX) $(OBJECTS) -o $@ $(USERLDFLAGS) $(LDFLAGS)


 
clean:
	-rm -f *.o 

unittests : TestRunner.o filtertests.o CS800Filter.o FragmentIndex.o \
            RingItemFromFile.o
	$(CXX) -I$(shell pwd) -o $@ $^ $(USERLDFLAGS) $(LDFLAGS) -lcppunit

check : 
	./unittests
