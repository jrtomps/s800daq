# Init sequence for s800 DAQ
# $CCUSB controller serial number is CC0105

set CCUSB $::Globals::aController
set trigger(firmware) /user/s800/server/fpga/usbtrig.bit
set trigger(configuration) /user/s800/operations/daq/usb/Configs/s800trigger.tcl

# Module declarations
if {[lsearch [itcl::find objects] trigger] == -1} {ATrigger2367 trigger $CCUSB 17}

proc initTrigger2367 {ctlr node filename} {
  set Fpara [list 16 16 16 16 16 16 16 16 16 16 16 16 17 17 17 17 18 18 18 18]
  set Apara [list 0 1 2 3 4 5 6 7 8 9 10 12 0 1 2 3 0 1 2 3]
  set parameters [list PCDelay PCWidth SCDelay SCWidth PSDelay CCWidth SSDelay Bypasses PDFactor SDFactor TriggerBox ClockBox Inspect1 Inspect2 Inspect3 Inspect4 ADCWidth QDCWidth TDCWidth CoincidenceWidth]

  source $filename
  for {set i 0} {$i < [llength $parameters]} {incr i} {
    $ctlr simpleWrite16 $node [lindex $Apara $i] [lindex $Fpara $i] $TRIGGER([lindex $parameters $i])
  }
}
# Here we are configuring the trigger module
puts "Please wait. Writing firmware to trigger ULM2367 module FPGA."
trigger Configure $trigger(firmware)

# Now load the latest configuration from the saved file
puts "Initializing trigger ULM2367 module FPGA."
initTrigger2367 $CCUSB 17 $trigger(configuration)

puts "Done."






