# Begin sequence for s800 DAQ
# VMUSB controller serial number is VM0079

set VMUSB $::Globals::aController

# Create module instances if they don't exist
if {[lsearch [itcl::find objects] crdc1] == -1} {ACrdcXLM72 crdc1 $VMUSB 10}
if {[lsearch [itcl::find objects] crdc2] == -1} {ACrdcXLM72 crdc2 $VMUSB 13}
if {[lsearch [itcl::find objects] ppac] == -1} {APpacXLM72 ppac $VMUSB 16}
#if {[lsearch [itcl::find objects] caen] == -1} {ACAENV288 caen $VMUSB 0x200000}
#if {[lsearch [itcl::find objects] icshp] == -1} {ACAENN568B icshp caen 1}
#if {[lsearch [itcl::find objects] hodoshp1] == -1} {ACAENN568B hodoshp1 caen 2}
#if {[lsearch [itcl::find objects] hodoshp2] == -1} {ACAENN568B hodoshp2 caen 3}

# Modules initializations
crdc1 Init ~/operations/daq/usb/Configs/s800crdc1v.tcl CRDC1_PADS
crdc2 Init ~/operations/daq/usb/Configs/s800crdc2v.tcl CRDC2_PADS
ppac Init ~/operations/daq/usb/Configs/s800tppacv.tcl TRACKI_PADS


#delay Init ~/operations/daq/usb/Configs/s800tdcdelayini.tcl TDCDELAY

# N568B shapers
#puts -nonewline "IC... "
#flush stdout
#icshp Init ~/operations/daq/usb/Configs/s800shpini.tcl IC_SHP
#puts -nonewline "Hodo1... "
#flush stdout
#hodoshp1 Init ~/operations/daq/usb/Configs/s800shpini.tcl Hodo_SHP1
#puts -nonewline "Hodo2... "
#flush stdout
#hodoshp2 Init ~/operations/daq/usb/Configs/s800shpini.tcl Hodo_SHP2
#

flush stdout
