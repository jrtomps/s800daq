set VMUSB VM0079
set CCUSB CC0105
global runinfo
set trigger(configuration) ~/operations/daq/usb/Configs/s800trigger.tcl
set gd16(file) ~/operations/daq/usb/Configs/s800tdcdelayini.tcl
set gd16(moduleName) TDCDELAY

# Module declarations
if {[lsearch [itcl::find objects] trigger] == -1} {ATrigger2367 trigger $CCUSB 17}
if {[lsearch [itcl::find objects] delay] == -1} {AGD16XLM72 delay $VMUSB 7}

# Start the trigger GUI if it isn't up yet
if {![winfo exists .trigger]} {
	toplevel .trigger
	trigger SetupGUI .trigger $trigger(configuration)
	trigger ReadConfigFile
	if {[string equal $runinfo(state) active]} {trigger LockGUI 1}
}

# Start the Gate and Delay GUI if it isn't up yet
if {![winfo exists .gd16]} {
	toplevel .gd16
	delay SetupGUI .gd16
	delay ReadFileGD16
	if {[string equal $runinfo(state) active]} {delay LockGD16}
}
