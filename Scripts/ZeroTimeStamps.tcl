set CCUSB CC0105
set VMUSB VM0079

if {[lsearch [itcl::find objects] trigger] == -1} {ATrigger2367 trigger $CCUSB 20}
if {[lsearch [itcl::find objects] stamp] == -1} {ATimeStampXLM72 stamp $VMUSB 4}

# First inhibit time stamps so they reset at the same time if clock is ticking
trigger Go 0
after 100
trigger Clear
stamp Init
after 100
#trigger Go 1

