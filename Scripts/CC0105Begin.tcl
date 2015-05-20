# Begin sequence for s800 DAQ
# $CCUSB controller serial number is CC0105

set CCUSB $::Globals::aController
global runinfo
#source /user/s800/operations/daq/usb/Configs/s800daqinithresholds.tcl
source Scripts/s800daqinithresholds.tcl
# Copy configuration files if writing to disk
#if {$runinfo(save)} {
#	file copy -force /user/s800/operations/daq/usb/Configs/s800daqinithresholds.tcl $runinfo(path)/run$runinfo(number)/s800daqinithresholds.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800trigger.tcl $runinfo(path)/run$runinfo(number)/s800trigger.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800shpini.tcl $runinfo(path)/run$runinfo(number)/s800shpini.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800cfdini.tcl $runinfo(path)/run$runinfo(number)/s800cfdini.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800crdc1v.tcl $runinfo(path)/run$runinfo(number)/s800crdc1v.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800crdc2v.tcl $runinfo(path)/run$runinfo(number)/s800crdc2v.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800tppacv.tcl $runinfo(path)/run$runinfo(number)/s800tppacv.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800tdcdelayini.tcl $runinfo(path)/run$runinfo(number)/s800tdcdelayini.tcl
#	foreach filename [glob "$runinfo(path)/run$runinfo(number)/*"] {
#		file attributes $filename -permissions 0440
#	}
#}
#if {$runinfo(copy)} {
#	file copy -force /user/s800/operations/daq/usb/Configs/s800daqinithresholds.tcl $runinfo(copypath)/run$runinfo(number)/s800daqinithresholds.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800trigger.tcl $runinfo(copypath)/run$runinfo(number)/s800trigger.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800shpini.tcl $runinfo(copypath)/run$runinfo(number)/s800shpini.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800cfdini.tcl $runinfo(copypath)/run$runinfo(number)/s800cfdini.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800crdc1v.tcl $runinfo(copypath)/run$runinfo(number)/s800crdc1v.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800crdc2v.tcl $runinfo(copypath)/run$runinfo(number)/s800crdc2v.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800tppacv.tcl $runinfo(copypath)/run$runinfo(number)/s800tppacv.tcl
#	file copy -force /user/s800/operations/daq/usb/Configs/s800tdcdelayini.tcl $runinfo(copypath)/run$runinfo(number)/s800tdcdelayini.tcl
#	foreach filename [glob "$runinfo(copypath)/run$runinfo(number)/*"] {
#		file attributes $filename -permissions 0440
#	}
#}

# Module declarations
#if {[lsearch [itcl::find objects] nimout] == -1} {ACAENV262 nimout $VMUSB 0x444400}
if {[lsearch [itcl::find objects] trigger] == -1} {ATrigger2367 trigger $CCUSB 17}
#if {[lsearch [itcl::find objects] cfd] == -1} {ACAENC808 cfd $CCUSB 21}
#if {[lsearch [itcl::find objects] hcfd1] == -1} {ACAENC671 hcfd1 $CCUSB 12}
#if {[lsearch [itcl::find objects] hcfd2] == -1} {ACAENC671 hcfd2 $CCUSB 14}
if {[lsearch [itcl::find objects] scint] == -1} {ALeCroy4300B scint $CCUSB 18}
if {[lsearch [itcl::find objects] ionch] == -1} {APhillips71xx ionch $CCUSB 6}
if {[lsearch [itcl::find objects] hodo1] == -1} {APhillips71xx hodo1 $CCUSB 7}
if {[lsearch [itcl::find objects] hodo2] == -1} {APhillips71xx hodo2 $CCUSB 8}
if {[lsearch [itcl::find objects] hodopat] == -1} {ALeCroy4448 hodopat $CCUSB 13}
if {[lsearch [itcl::find objects] s800tdc] == -1} {APhillips71xx s800tdc $CCUSB 19}
#if {[lsearch [itcl::find objects] labrtdc] == -1} {APhillips71xx labrtdc $CCUSB 22}
if {[lsearch [itcl::find objects] s800adc] == -1} {APhillips71xx s800adc $CCUSB 23}
#

puts "Executing begin sequence of CAMAC crate"
flush stdout

# This method assumes filename points to an "old" type Tcl file defining parameters
# in an array called "TRIGGER"
proc initATrigger2367 {device node filename} {
	set parameters [list PCDelay PCWidth SCDelay SCWidth PSDelay CCWidth SSDelay Bypasses PDFactor SDFactor TriggerBox ClockBox Inspect1 Inspect2 Inspect3 Inspect4 ADCWidth QDCWidth TDCWidth CoincidenceWidth]
        set Fpara [list 16 16 16 16 16 16 16 16 16 16 16 16 17 17 17 17 18 18 18 18]
        set Apara [list 0 1 2 3 4 5 6 7 8 9 10 12 0 1 2 3 0 1 2 3]
	source $filename
	for {set i 0} {$i < [llength $parameters]} {incr i} {
		$device simpleWrite24 $node [lindex $Apara $i] [lindex $Fpara $i] $TRIGGER([lindex $parameters $i])
	}
}
puts "\tInitializing Trigger2367"
# Temporarily removed to achieve continuous time stamp mode (Sam's experiment)
trigger Go 0; # Inhibit triggers and time stamp clock
trigger Clear; # Clear latches, trigger register and time stamp
trigger Enable 0; # Enable external sync signal
initATrigger2367 $CCUSB 17 /user/s800/operations/daq/usb/Configs/s800trigger.tcl
#trigger Select 1; # 0: internal clock 1: external clock

#cfd EnableOnlyChannels [list 0 1 2 3 4 6 7 8 9 10 14] ;#Enabled additional channels 2,3,4 for 09041B (JP, 03/11/2014))
#for {set i 0} {$i < 8} {incr i} {cfd SetThreshold $i $CFD1([format "thr%.2d" $i])}
#cfd SetWidth 0 $CFD1(widthA)
#cfd SetDeadTime 0 0
#for {set i 0} {$i < 8} {incr i} {cfd SetThreshold [expr $i+8] $CFD2([format "thr%.2d" $i])}
#cfd SetWidth 1 $CFD2(widthA)
#cfd SetDeadTime 1 0
#
#hcfd1 Enable 0 0xFF
#hcfd1 Enable 1 0xFF
#for {set i 0} {$i < 16} {incr i 2} {hcfd1 Threshold $i 7}
#for {set i 1} {$i < 16} {incr i 2} {hcfd1 Threshold $i 7}
#for {set i 0} {$i < 16} {incr i} {hcfd1 Delay $i 255}
#hcfd1 Threshold 3 8
#hcfd1 Threshold 15 9
#
#hcfd2 Enable 0 0xFF
#hcfd2 Enable 1 0xFF
#for {set i 0} {$i < 16} {incr i 2} {hcfd2 Threshold $i 7}
#for {set i 1} {$i < 16} {incr i 2} {hcfd2 Threshold $i 7}
#for {set i 0} {$i < 16} {incr i} {hcfd2 Delay $i 255}
#hcfd2 Threshold 9 9
#hcfd2 Threshold 13 8
#
puts "\tInitializing Lecroy4300B - Scintillator"
scint Clear
scint SetControlRegister 0x7800; # CAMAC LAM enabled; CAMAC sequential read; Ped subtraction enabled
set peds ""; for {set i 0} {$i < 16} {incr i} {lappend peds $SCINT_ENERGY([format "ped%.2d" $i])}
scint SetPedestals $peds

# General procedure to initialize the Phillips ADCs and TDCs
proc InitPhillips71xx {module name} {
	upvar 1 $name n
	$module EnableLAM
	$module SetControlRegister 7
	if {[info exists n(ped00)]} {
	# addWrite32 is called by WritePedestals and converts all numbers to an unsigned integer.
        # Because a negative value in tcl is not converted to an unsigned value in a well-defined
        # manner in C++, we need to play a game. We need to truncate the upper bits so that
        # the lower 12-bits remain. The reason for the massive truncation is that the WritePedestals
        # ensures that the values it receives are properly in the range -4095 to 4095.
	#	for {set i 0} {$i < 16} {incr i} {lappend peds [expr $n([format "ped%.2d" $i])%65536]}
		for {set i 0} {$i < 16} {incr i} {
	          set val $n([format "ped%.2d" $i])
		  lappend peds $val
                } 
		$module WritePedestals $peds
	} else {
		$module WritePedestals {0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0}
	}
	if {[info exists n(lth00)]} {
		for {set i 0} {$i < 16} {incr i} {lappend lths $n([format "lth%.2d" $i])}
		$module WriteLowerThresholds $lths
	} else {
                # this has one extra threshold in it.... can't succeed
#		$module WriteLowerThresholds {10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10}
		$module WriteLowerThresholds {10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10}
	}
	if {[info exists n(uth00)]} {
		for {set i 0} {$i < 16} {incr i} {lappend uths $n([format "uth%.2d" $i])}
		$module WriteUpperThresholds $uths
	} else {
		$module WriteUpperThresholds {0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa}
	}
}

puts "\tInitializing Phillips 71xx - Ion Chamber"
InitPhillips71xx ionch IC_ADC
puts "\tInitializing Phillips 71xx - Hodo 1"
InitPhillips71xx hodo1 Hodo1_ADC
puts "\tInitializing Phillips 71xx - Hodo 2"
InitPhillips71xx hodo2 Hodo2_ADC
puts "\tInitializing Phillips 71xx - S800 ADC"
InitPhillips71xx s800adc S800_ADC
puts "\tInitializing Phillips 71xx - S800 TDC"
InitPhillips71xx s800tdc TIMES

# Not plugged in
#puts "\tInitializing Phillips 71xx - LaBr TDC"
##InitPhillips71xx labrtdc LABR

puts "\tInitializing Lecroy 4448 - Hodoscope pattern"
hodopat Clear

puts "Done."
flush stdout
