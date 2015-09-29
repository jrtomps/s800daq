

set DAQROOT $::env(DAQROOT)
lappend auto_path [file join $DAQROOT TclLibs]

package require readoutscript
package require Itcl
package require crdcxlm72
package require ppacxlm72
package require gd16xlm72
package require caenn568b
package require caenv288


## -- TAGS --
set vmusbCrateTag 0xE800
set tstampStart 0x5803
set tstampFinish 0xF803
set crdc1Start 0xcfdc
set crdc1Finish 0xffdc
set crdc2Start 0xcfdd
set crdc2Finish 0xffdd
set ppacStart 0x5870
set ppacFinish 0xf870
set mtdcStart 0x0ddc
set mtdcFinish 0xfddc

marker create vmusbTag $vmusbCrateTag
marker create tstampStartTag $tstampStart
marker create tstampFinishTag $tstampFinish
marker create mtdcStartTag $mtdcStart
marker create mtdcFinishTag $mtdcFinish


#################
# -- XLMV72's: CRDCs, PPACs, Delay box --
# -- IC/Hodoscope CAEN shappers --
#
# Script drivers for XLM (CRDCs, PPACs, Delay box), and Hodocope/IC shapers 
# It runs the VM0079Begin, VM0079Event, and VM0079End scripts
readoutscript rdoScript -controllertype vmusb
rdoScript configure -initscript Scripts/VM0079Begin.tcl; # Configuration files for modules 
rdoScript configure -rdolistscript Scripts/VM0079Event.tcl; # Include readout of XLMV72 modules (CRDCs and TPPACs)
#rdoScript config -onendlist Scripts/VM0079End.tcl
addtcldriver rdoScript



#################
# -- TDC --
#itcl::body AMesytecMTDC32::Init {} {
#	SetIRQVector 0
#	SetIRQLevel 0
#	SetMultiEvent 3
#	SetMaxTransfer 1
#	SetDataLength 2
#	SetResolution 6; # 62.5 ps per channel
#	SetBank0Trigger 1
#	SetBank0Start 15384; # -1000 ns (16384 corresponds to 0)
#	SetBank0Width 1000; # Window with (ns)
#	ResetFIFO
#}
mtdc create tdc -base 0x40000000
mtdc config tdc -ipl 0 -vector 0
mtdc config tdc -maxtransfers 1
mtdc config tdc -datalen 32
mtdc config tdc -resolution 62.5ps
mtdc config tdc -bank0triggersource Tr0
mtdc config tdc -bank0winstart 14384; # -2000 ns (16384 corresponds to 0)
mtdc config tdc -bank0winwidth 4000 ;# Window with (ns)





#################
# --VM-USB --
## Crate Controller initialization
#$VMUSB SetBufferLength 13000
#$VMUSB SetBufferMode 0
#$VMUSB SetHeader 1
#$VMUSB SetBulkBuffers 0
#$VMUSB SetForceDump 1
#$VMUSB SetTriggerDelay 110
#$VMUSB SetScalerPeriod 0
#$VMUSB SetScalerFrequency 2
#
#$VMUSB SetNIMs 0x00010600; # Set input of scaler A to NIM I1 and input of scaler B to term of A, O1=Busy, O2=End of Event
#$VMUSB SetNIMs 0x80CC0000; # Enable and reset scaler A & B

# VM-USB configuration, buffer lend
vmusb create ctlr 
vmusb config ctlr -bufferlength 13k
# vmusb config ctlr -bulktransfermode default
vmusb config ctlr -forcescalerdump on
vmusb config ctlr -readscalers on -incremental no
vmusb config ctlr -scalera nimi1 -scalerb carry; # Set scaler A to I1 and scaler B to A overflow



#################
# -- XLM Time stamp --
set tstampSlot 4
XLMTimestamp create tstamp -base [expr $tstampSlot<<27]
XLMTimestamp config tstamp -firmware /user/s800/server/fpga/stamp64.bit
XLMTimestamp config tstamp -loadfirmware off


#################
# Script driver for running VM0079Scaler.tcl
readoutscript sclrScript -controllertype vmusb
sclrScript configure -rdolistscript Scripts/VM0079Scaler.tcl
addtcldriver sclrScript








##########################################################################################
# -- Define stack to include all VME modules (XLMs, shapers, TDC, VM-USB controller)
set rdoList [list vmusbTag \
                  ctlr \
		  tstampStartTag \
		  tstamp \
                  tstampFinishTag \
                  rdoScript \
		  mtdcStartTag \
		  tdc \
		  mtdcFinishTag ]

stack create eventStack
stack config eventStack -modules $rdoList ;# Stack is made of rdoList modules
stack config eventStack -trigger nim1
stack config eventStack -delay 110

##########################################################################################
# -- Define stack to include scalers
stack create sclrStack
stack config sclrStack -trigger scaler -modules sclrScript -period 1

