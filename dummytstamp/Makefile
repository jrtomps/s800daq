
CXX = g++ -std=c++11

all : libdummytstamp.so

libdummytstamp.so: dummytstamp.cpp
	${CXX} -o dummytstamp.o -c dummytstamp.cpp -I${DAQROOT}/include
	${CXX} -shared -Wl,-rpath=${PWD} -o $@ dummytstamp.o
