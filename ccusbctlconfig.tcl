
# CCUSB Slow controls config script


package require controlscript
package require trigger2367

controlscript script -controllertype ccusb
script configure -initscript Scripts/CC0105Init.tcl

#Module create tcl ctlScript
#Module config ctlScript -ensemble script
