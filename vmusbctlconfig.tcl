

package require controlscript

controlscript ctlScript_ -controllertype ccusb
ctlScript_ config -initscript Scripts/VM0079Init.tcl

Module create tcl ctlScript
Module config ctlScript -ensemble ctlScript_
