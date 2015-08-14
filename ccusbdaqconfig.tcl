
# CCUSB DAQ config script

set DAQROOT $::env(DAQROOT)
lappend auto_path [file join $DAQROOT TclLibs]
lappend auto_path [file join $DAQROOT lib]

package require readoutscript
package require Itcl
package require trigger2367
package require lecroy4300b
package require phillips71xx
package require lecroy4448

marker create crateTag 0xC800
marker create sclr4434Tag  0x4434

#################
# --CC-USB --
## Crate Controller initialization
#
#$CCUSB SetTriggerDelay 25
#$CCUSB SetLAMMask 0; # Set trigger on NIM1
#$CCUSB SetBufferLength 4096; # Maximum buffer length
# not doing because it is unused in CCUSBReadout
#$CCUSB SetHeader 1; # We want the number of event information
#$CCUSB SetForceDump 1; # Force dump scaler buffers
#$CCUSB SetLatchEvent 1; # Allow events encountered during scaler readout to be processed
#$CCUSB SetLAMTimeout 10; # LAM timeout set to 50 µs
#$CCUSB SetRegister 5 0x100; # Set O2 to Event
#$CCUSB SetRegister 6 0x10;		# Enable scaler A
#$CCUSB SetRegister 6 0x1000;	# Enable scaler B
#$CCUSB SetRegister 6 0x20;		# Reset scaler A
#$CCUSB SetRegister 6 0x2000;	# Reset scaler B
#$CCUSB SetRegister 6 [expr 4 + (5<<8)]; # Set Input of scaler A to event, scaler B to term of scaler A, O1=Busy, O3=End of Event
#$CCUSB SetScalerFrequency 2; # in units of 0.5 s
ccusb create ctlr
ccusb config ctlr -bufferlength 4096
ccusb config ctlr -readscalers on -forcescalerdump on 
ccusb config ctlr -out1 busy 
ccusb config ctlr -out2 event
ccusb config ctlr -out3 busyend
ccusb config ctlr -scalera event -scalerb carrya ;# Set Input of scaler A to event, scaler B to term of scaler A,




#################
# Script driver for trigger module, FERA, Phillips ADCs and TDCs
# (Trigger, Scintillators ADC, hodoscope and IC ADCs, S800 TDC...)
# It runs the CC0105Begin and CC0105Event scripts
#
readoutscript rdoScript -controllertype ccusb
rdoScript configure -initscript Scripts/CC0105Begin.tcl
rdoScript configure -rdolistscript Scripts/CC0105Event.tcl
#rdoScript configure -onendscript Scripts/CC0105Event.tcl
addtcldriver rdoScript


#################
# The script that sets the trigger Go bit to 1 at run start
# and also sets it to 0 at the end of the run
readoutscript startStopScript -controllertype ccusb
startStopScript configure -initscript Scripts/MainBegin.tcl
startStopScript configure -onendscript Scripts/MainEnd.tcl
addtcldriver startStopScript



##########################################################################################
# -- Define stack to include all CAMAC modules (Trigger, ADCs, TDC, CC-USB controller)
set rdoModules [list crateTag \
                     ctlr \
	             rdoScript \
		     startStopScript ]



stack create rdoStack
stack config rdoStack -type event -modules $rdoModules
stack config rdoStack -delay 25 ;
stack config rdoStack -lams 0 ; # trigger on NIM1
stack config rdoStack -lamtimeout 10 ; # 50us



##########################################################################################
# -- Define stack to include scalersLeCroy4434 create sclr -slot 16
LeCroy4434 create sclr -slot 16
LeCroy4434 config sclr -incremental false

set sclrModules [list crateTag \
		      sclr4434Tag \
		      sclr ]

stack create sclrStack
stack config sclrStack -type scaler 
stack config sclrStack -modules $sclrModules
stack config sclrStack -period 1



