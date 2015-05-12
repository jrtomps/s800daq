

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
#Module create xlm xlmTStampFw
#Module config xlmTStampFw -base [expr $xlmTstampSlot<<27]
#Module config xlmTStampFw -firmware $stamp(firmware)

# Initialize the crdc 1 firmware
#set crdc1_Slot 10 
#Module create xlm crdc1_FwLoad
#Module config crdc1_FwLoad -base [expr $crdc1_Slot<<27]
#Module config crdc1_FwLoad -firmware $crdc(firmware)

# Initialize the crdc 2 firmware
set crdc2_Slot 2
Module create xlm crdc2_FwLoad
Module config crdc2_FwLoad -base [expr $crdc2_Slot<<27]
Module config crdc2_FwLoad -firmware $crdc(firmware)

after 1000

# Create the script driver that will initialize the ppacs and crdcs
controlscript ctlScript_ -controllertype vmusb
ctlScript_ configure -initscript Scripts/VM0079Init.tcl

Module create tcl ctlScript
Module config ctlScript -ensemble ctlScript_




