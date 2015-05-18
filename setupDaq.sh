#!/bin/bash


if test $# -ne 1 ; then
	echo "Usage: ./setupDaq.sh VSN"
	echo ""
	echo "   VSN - version of NSCLDAQ to use (i.e. 11.0-001)"
	exit 1
fi 

unset DAQROOT
source /scratch/$1/daqsetup.bash

#export DAQROOT=/scratch/$1
#export DAQBIN=$DAQROOT/bin
#export DAQLIB=$DAQROOT/lib

export TCLLIBPATH="$TCLLIBPATH $DAQROOT/TclLibs $DAQLIB"

echo "Set the following environment variables"
echo "DAQROOT = $DAQROOT" 
echo "DAQBIN  = $DAQBIN" 
echo "DAQLIB  = $DAQLIB" 
echo "TCLLIBPATH  = $TCLLIBPATH" 

