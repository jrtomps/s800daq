
CXX = g++ -std=c++11

all : libvmusbtstamp.so

libvmusbtstamp.so: tstamp.cpp
	${CXX} -o tstamp.o -c tstamp.cpp -I${DAQROOT}/include -fPIC
	${CXX} -shared -Wl,-rpath=${PWD} -o $@ tstamp.o -fPIC
