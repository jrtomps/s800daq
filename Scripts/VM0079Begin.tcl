# Begin sequence for s800 DAQ
# VMUSB controller serial number is VM0079

set VMUSB $::Globals::aController

# Create module instances if they don't exist

# delay was renamed to delayBox to avoid conflict with "delay" command
# used for defining standalone stack delays
if {[lsearch [itcl::find objects] delayBox] == -1} {AGD16XLM72 delayBox $VMUSB 7}
if {[lsearch [itcl::find objects] crdc1] == -1} {ACrdcXLM72 crdc1 $VMUSB 10}
if {[lsearch [itcl::find objects] crdc2] == -1} {ACrdcXLM72 crdc2 $VMUSB 13}
if {[lsearch [itcl::find objects] ppac] == -1} {APpacXLM72 ppac $VMUSB 16}
if {[lsearch [itcl::find objects] caen] == -1} {ACAENV288 caen $VMUSB 0x200000}
if {[lsearch [itcl::find objects] icshp] == -1} {ACAENN568B icshp caen 1}
if {[lsearch [itcl::find objects] hodoshp1] == -1} {ACAENN568B hodoshp1 caen 2}
if {[lsearch [itcl::find objects] hodoshp2] == -1} {ACAENN568B hodoshp2 caen 3}

# Modules initializations
puts "Initializing CRDC #1"
crdc1 Init /user/s800/operations/daq/usb/Configs/s800crdc1v.tcl CRDC1_PADS
puts "Initializing CRDC #2"
crdc2 Init /user/s800/operations/daq/usb/Configs/s800crdc2v.tcl CRDC2_PADS
puts "Initializing PPAC" 
ppac Init /user/s800/operations/daq/usb/Configs/s800tppacv.tcl TRACKI_PADS
puts "Initializing GD16 delay"
delayBox Init /user/s800/operations/daq/usb/Configs/s800tdcdelayini.tcl TDCDELAY


# N568B shapers
puts "Initializing IC Shaper "
icshp Init /user/s800/operations/daq/usb/Configs/s800shpini.tcl IC_SHP
puts "Initializing Hodo #1 Shaper "
hodoshp1 Init /user/s800/operations/daq/usb/Configs/s800shpini.tcl Hodo_SHP1
puts "Initializing Hodo #2 Shaper "
hodoshp2 Init /user/s800/operations/daq/usb/Configs/s800shpini.tcl Hodo_SHP2

puts "Done"
flush stdout
