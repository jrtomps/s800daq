# Main Begin sequence for s800 DAQ

set CCUSB $::Globals::aController

# Module declarations
if {[lsearch [itcl::find objects] trigger] == -1} {ATrigger2367 trigger $CCUSB 17}

## Choose whether or not to send slow control buffers
#global runinfo
#set runinfo(slowcontrol) 1
#
## Freeze GUIs if they exist
#if {[winfo exists .trigger]} {
#	trigger LockGUI 1
#}
#if {[winfo exists .gd16]} {
#	delay LockGD16
#}

# Trigger clear and go after some fixed time (here 1ms)
trigger Clear
after 1
trigger Go 1
