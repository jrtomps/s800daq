


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

marker create vmusbTag $vmusbCrateTag
marker create tstampStartTag $tstampStart
marker create tstampFinishTag $tstampFinish

# Script driver for running the VM0079Begin, VM0079Event, and
# VM0079End scripts. These handle the crdc and ppacs
readoutscript rdoScript -controllertype vmusb
rdoScript configure -initscript Scripts/VM0079Begin.tcl
rdoScript configure -rdolistscript Scripts/VM0079Event.tcl
#rdoScript config -onendlist Scripts/VM0079End.tcl
addtcldriver rdoScript

# create the mtdc driver
#mtdc create tdc -base 0x40000000

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
vmusb config ctlr -scalera nimi1 -scalerb carry


set tstampSlot 4
XLMTimestamp create tstamp -base [expr $tstampSlot<<27]
XLMTimestamp config tstamp -firmware /user/s800/server/fpga/stamp64.bit

set rdoList [list vmusbTag \
                  ctlr \
		  tstampStartTag \
		  tstamp \
                  tstampFinishTag \
                  rdoScript ]

stack create eventStack
stack config eventStack -modules $rdoList
stack config eventStack -trigger nim1
stack config eventStack -delay 110


# Script driver for running VM0079Scaler.tcl
readoutscript sclrScript -controllertype vmusb
sclrScript configure -rdolistscript Scripts/VM0079Scaler.tcl
addtcldriver sclrScript

stack create sclrStack
stack config sclrStack -trigger scaler -modules sclrScript -period 1

