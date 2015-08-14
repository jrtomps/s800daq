
CXX = g++ -std=c++11

all : libccusbtstamp.so

libccusbtstamp.so: tstamp.cpp
	${CXX} -o tstamp.o -c tstamp.cpp -fPIC -I${DAQROOT}/include
	${CXX} -shared -Wl,-rpath=${PWD} -fPIC -o $@ tstamp.o
