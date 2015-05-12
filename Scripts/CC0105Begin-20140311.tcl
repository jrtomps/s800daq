# Begin sequence for s800 DAQ
# $CCUSB controller serial number is CC0105

set CCUSB CC0105
global runinfo
source ~/operations/daq/usb/Configs/s800daqinithresholds.tcl
# Copy configuration files if writing to disk
if {$runinfo(save)} {
	file copy -force ~/operations/daq/usb/Configs/s800daqinithresholds.tcl $runinfo(path)/run$runinfo(number)/s800daqinithresholds.tcl
	file copy -force ~/operations/daq/usb/Configs/s800trigger.tcl $runinfo(path)/run$runinfo(number)/s800trigger.tcl
	file copy -force ~/operations/daq/usb/Configs/s800shpini.tcl $runinfo(path)/run$runinfo(number)/s800shpini.tcl
	file copy -force ~/operations/daq/usb/Configs/s800cfdini.tcl $runinfo(path)/run$runinfo(number)/s800cfdini.tcl
	file copy -force ~/operations/daq/usb/Configs/s800crdc1v.tcl $runinfo(path)/run$runinfo(number)/s800crdc1v.tcl
	file copy -force ~/operations/daq/usb/Configs/s800crdc2v.tcl $runinfo(path)/run$runinfo(number)/s800crdc2v.tcl
	file copy -force ~/operations/daq/usb/Configs/s800tppacv.tcl $runinfo(path)/run$runinfo(number)/s800tppacv.tcl
	file copy -force ~/operations/daq/usb/Configs/s800tdcdelayini.tcl $runinfo(path)/run$runinfo(number)/s800tdcdelayini.tcl
	foreach filename [glob "$runinfo(path)/run$runinfo(number)/*"] {
		file attributes $filename -permissions 0440
	}
}
if {$runinfo(copy)} {
	file copy -force ~/operations/daq/usb/Configs/s800daqinithresholds.tcl $runinfo(copypath)/run$runinfo(number)/s800daqinithresholds.tcl
	file copy -force ~/operations/daq/usb/Configs/s800trigger.tcl $runinfo(copypath)/run$runinfo(number)/s800trigger.tcl
	file copy -force ~/operations/daq/usb/Configs/s800shpini.tcl $runinfo(copypath)/run$runinfo(number)/s800shpini.tcl
	file copy -force ~/operations/daq/usb/Configs/s800cfdini.tcl $runinfo(copypath)/run$runinfo(number)/s800cfdini.tcl
	file copy -force ~/operations/daq/usb/Configs/s800crdc1v.tcl $runinfo(copypath)/run$runinfo(number)/s800crdc1v.tcl
	file copy -force ~/operations/daq/usb/Configs/s800crdc2v.tcl $runinfo(copypath)/run$runinfo(number)/s800crdc2v.tcl
	file copy -force ~/operations/daq/usb/Configs/s800tppacv.tcl $runinfo(copypath)/run$runinfo(number)/s800tppacv.tcl
	file copy -force ~/operations/daq/usb/Configs/s800tdcdelayini.tcl $runinfo(copypath)/run$runinfo(number)/s800tdcdelayini.tcl
	foreach filename [glob "$runinfo(copypath)/run$runinfo(number)/*"] {
		file attributes $filename -permissions 0440
	}
}

# Module declarations
#if {[lsearch [itcl::find objects] nimout] == -1} {ACAENV262 nimout $VMUSB 0x444400}
if {[lsearch [itcl::find objects] trigger] == -1} {ATrigger2367 trigger $CCUSB 17}
if {[lsearch [itcl::find objects] cfd] == -1} {ACAENC808 cfd $CCUSB 21}
#if {[lsearch [itcl::find objects] cfd1] == -1} {AOrtecCCF8000 cfd1 $CCUSB 14}
#if {[lsearch [itcl::find objects] cfd2] == -1} {AOrtecCCF8000 cfd2 $CCUSB 15}
#if {[lsearch [itcl::find objects] hcfd1] == -1} {ARIS13080 hcfd1 $CCUSB 16}
#if {[lsearch [itcl::find objects] hcfd2] == -1} {ARIS13080 hcfd2 $CCUSB 17}
if {[lsearch [itcl::find objects] hcfd1] == -1} {ACAENC671 hcfd1 $CCUSB 12}
if {[lsearch [itcl::find objects] hcfd2] == -1} {ACAENC671 hcfd2 $CCUSB 14}
if {[lsearch [itcl::find objects] scint] == -1} {ALeCroy4300B scint $CCUSB 18}
if {[lsearch [itcl::find objects] ionch] == -1} {APhillips71xx ionch $CCUSB 6}
if {[lsearch [itcl::find objects] hodo1] == -1} {APhillips71xx hodo1 $CCUSB 7}
if {[lsearch [itcl::find objects] hodo2] == -1} {APhillips71xx hodo2 $CCUSB 8}
if {[lsearch [itcl::find objects] hodopat] == -1} {ALeCroy4448 hodopat $CCUSB 13}
if {[lsearch [itcl::find objects] s800tdc] == -1} {APhillips71xx s800tdc $CCUSB 19}
if {[lsearch [itcl::find objects] labrtdc] == -1} {APhillips71xx labrtdc $CCUSB 22}
if {[lsearch [itcl::find objects] s800adc] == -1} {APhillips71xx s800adc $CCUSB 23}

if {[lsearch [itcl::find objects] scaler] == -1} {ALeCroy4434 scaler $CCUSB 16}

puts -nonewline "Executing begin sequence of CAMAC crate controlled by $CCUSB... "
flush stdout

# Toggle time stamp clock to false (set veto)
#nimout WriteNIMLevel 1
# Controller initialization
$CCUSB SetTriggerDelay 25
$CCUSB SetLAMMask 0; # Set trigger on NIM1
$CCUSB SetBufferLength 4096; # Maximum buffer length
$CCUSB SetHeader 1; # We want the number of event information
$CCUSB SetForceDump 1; # Force dump scaler buffers
$CCUSB SetLatchEvent 1; # Allow events encountered during scaler readout to be processed
#$CCUSB SetRepeatDelay 4; # Necessary to read 4434 scalers correctly
$CCUSB SetLAMTimeout 10; # LAM timeout set to 50 µs
$CCUSB SetRegister 5 0x100; # Set O2 to Event
$CCUSB SetRegister 6 0x10;		# Enable scaler A
$CCUSB SetRegister 6 0x1000;	# Enable scaler B
$CCUSB SetRegister 6 0x20;		# Reset scaler A
$CCUSB SetRegister 6 0x2000;	# Reset scaler B
#$CCUSB SetRegister 6 [expr 1 + (2<<8)]; # Set Input of scaler A to NIM I1, scaler B to NIM I2
#$CCUSB SetRegister 6 [expr 4 + (2<<8)]; # Set Input of scaler A to event, scaler B to NIM I2
$CCUSB SetRegister 6 [expr 4 + (5<<8)]; # Set Input of scaler A to event, scaler B to term of scaler A, O1=Busy, O3=End of Event
#$CCUSB SetRegister 6 [expr 2 + (5<<8)]; # Set Input of scaler A to NIM I2, scaler B to term of scaler A
$CCUSB SetScalerFrequency 2; # in units of 0.5 s

# Modules initializations
#CCUSBRead $CCUSB 29 9 24; # Set Inhibit while programming modules
#CCUSBRead $CCUSB 28 9 29; # Clear Camac crate

trigger Go 0; # Inhibit triggers and time stamp clock
trigger Clear; # Clear latches, trigger register and time stamp
trigger Enable 0; # Enable external sync signal
trigger Init ~/operations/daq/usb/Configs/s800trigger.tcl
trigger Select 2; # 0: internal clock 1: external clock

cfd EnableOnlyChannels [list 0 1 6 7 8 9 10 14]
for {set i 0} {$i < 8} {incr i} {cfd SetThreshold $i $CFD1([format "thr%.2d" $i])}
cfd SetWidth 0 $CFD1(widthA)
cfd SetDeadTime 0 0
for {set i 0} {$i < 8} {incr i} {cfd SetThreshold [expr $i+8] $CFD2([format "thr%.2d" $i])}
cfd SetWidth 1 $CFD2(widthA)
cfd SetDeadTime 1 0

#set thr ""; for {set i 0} {$i < 8} {incr i} {lappend thr $CFD1([format "thr%.2d" $i])}
#cfd1 WriteThresholds $thr
#cfd1 WriteWidths [list $CFD1(widthA) $CFD1(widthB)]
#set thr ""; for {set i 0} {$i < 8} {incr i} {lappend thr $CFD2([format "thr%.2d" $i])}
#cfd2 WriteThresholds $thr
#cfd2 WriteWidths [list $CFD2(widthA) $CFD2(widthB)]

#set thr ""; for {set i 0} {$i < 16} {incr i} {lappend thr $Hodo_CFD1([format "thr%.2d" $i])}
#set wal ""; for {set i 0} {$i < 16} {incr i} {lappend wal $Hodo_CFD1([format "walk%.2d" $i])}
#hcfd1 InitCsI
#hcfd1 Polarity 0
#hcfd1 Fraction 0
#hcfd1 Thresholds $thr
#hcfd1 Walks $wal

hcfd1 Enable 0 0xFF
hcfd1 Enable 1 0xFF
for {set i 0} {$i < 16} {incr i 2} {hcfd1 Threshold $i 7}
for {set i 1} {$i < 16} {incr i 2} {hcfd1 Threshold $i 7}
for {set i 0} {$i < 16} {incr i} {hcfd1 Delay $i 255}
hcfd1 Threshold 3 8
hcfd1 Threshold 15 9

#set thr ""; for {set i 0} {$i < 16} {incr i} {lappend thr $Hodo_CFD2([format "thr%.2d" $i])}
#set wal ""; for {set i 0} {$i < 16} {incr i} {lappend wal $Hodo_CFD2([format "walk%.2d" $i])}
#hcfd2 InitCsI
#hcfd2 Polarity 0
#hcfd2 Fraction 0
#hcfd2 Thresholds $thr
#hcfd2 Walks $wal

hcfd2 Enable 0 0xFF
hcfd2 Enable 1 0xFF
for {set i 0} {$i < 16} {incr i 2} {hcfd2 Threshold $i 7}
for {set i 1} {$i < 16} {incr i 2} {hcfd2 Threshold $i 7}
for {set i 0} {$i < 16} {incr i} {hcfd2 Delay $i 255}
hcfd2 Threshold 9 9
hcfd2 Threshold 13 8

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
		for {set i 0} {$i < 16} {incr i} {lappend peds [expr $n([format "ped%.2d" $i])%65536]}
		$module WritePedestals $peds
	} else {
		$module WritePedestals {0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0}
	}
	if {[info exists n(lth00)]} {
		for {set i 0} {$i < 16} {incr i} {lappend lths $n([format "lth%.2d" $i])}
		$module WriteLowerThresholds $lths
	} else {
		$module WriteLowerThresholds {10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10}
	}
	if {[info exists n(uth00)]} {
		for {set i 0} {$i < 16} {incr i} {lappend uths $n([format "uth%.2d" $i])}
		$module WriteUpperThresholds $uths
	} else {
		$module WriteUpperThresholds {0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa 0xffa}
	}
}

InitPhillips71xx ionch IC_ADC
InitPhillips71xx hodo1 Hodo1_ADC
InitPhillips71xx hodo2 Hodo2_ADC
InitPhillips71xx s800adc S800_ADC
InitPhillips71xx s800tdc TIMES
#InitPhillips71xx labrtdc LABR

hodopat Clear
scaler Init

puts "Done."
flush stdout
