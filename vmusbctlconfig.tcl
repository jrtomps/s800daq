

package require cvmusb
package require controlscript
package require Itcl
package require crdcxlm72
package require ppacxlm72

# Firmware locations
set crdc(firmware) /user/s800/server/fpga/crdc5v.bit
set ppac(firmware) /user/s800/server/fpga/ppac2v.bit
set stamp(firmware) /user/s800/server/fpga/stamp64.bit
set delay(firmware) /user/s800/server/fpga/gd16.bit

# Initialize the timestamp with firmware
#set xlmTstampSlot 4
set xlmTstampSlot 16
Module create xlm xlmTStampFw
Module config xlmTStampFw -base [expr $xlmTstampSlot<<27]
Module config xlmTStampFw -firmware $stamp(firmware)

## Create the script driver that will initialize the ppacs and crdcs
#controlscript ctlScript_ -controllertype ccusb
#ctlScript_ config -initscript Scripts/VM0079Init.tcl
#
#Module create tcl ctlScript
#Module config ctlScript -ensemble ctlScript_




