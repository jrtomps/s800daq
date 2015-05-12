


package require readoutscript

# Script driver for running the VM0079Begin, VM0079Event, and
# VM0079End scripts. These handle the crdc and ppacs
readoutscript rdoScript -controllertype vmusb
rdoScript config -initscript Scripts/VM0079Begin.tcl
rdoScript config -rdolist Scripts/VM0079Event.tcl
rdoScript config -onendlist Scripts/VM0079End.tcl
addtcldriver rdoScript

# create the mtdc driver
mtdc create tdc -base 0x40000000

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

# Script driver for running VM0079Scaler.tcl
readoutscript sclrScript -controllertype vmusb
sclrScript config -rdolist Scripts/VM0079Scaler.tcl
addtcldriver sclrScript


stack create eventStack
stack config eventStack -type event 
stack config eventStack -modules [list ctlr rdoScript tdc]
stack config eventStack -trigger nimi1
stack config eventStack -delay 110


stack create sclrStack
stack config sclrStack -type scaler -modules sclrScript -period 1

