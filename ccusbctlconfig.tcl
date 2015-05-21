
# CCUSB Slow controls config script


set DAQROOT $::env(DAQROOT)
lappend auto_path [file join $DAQROOT TclLibs]

package require controlscript
package require trigger2367

controlscript script -controllertype ccusb
script configure -initscript Scripts/CC0105Init.tcl

#Module create tcl ctlScript
#Module config ctlScript -ensemble script
