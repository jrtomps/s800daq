# Init sequence for s800 DAQ
# $CCUSB controller serial number is CC0105

set CCUSB CC0105
set trigger(firmware) ~/server/fpga/usbtrig.bit
set trigger(configuration) ~/operations/daq/usb/Configs/s800trigger.tcl

# Module declarations
if {[lsearch [itcl::find objects] trigger] == -1} {ATrigger2367 trigger $CCUSB 17}

# Here we are configuring the trigger module
puts "Please wait. Writing firmware to trigger ULM2367 module FPGA."
trigger Configure $trigger(firmware)

# Now load the latest configuration from the saved file
trigger Init $trigger(configuration)

# Now start the GUI if it isn't up yet
if {![winfo exists .trigger]} {
	toplevel .trigger
	trigger SetupGUI .trigger $trigger(configuration)
	trigger ReadStatus
}
