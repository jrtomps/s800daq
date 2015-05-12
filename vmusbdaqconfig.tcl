


package require readoutscript

readoutscript rdoScript -controllertype vmusb
rdoScript config -initscript Scripts/VM0079Begin.tcl
rdoScript config -rdolist Scripts/VM0079Event.tcl
rdoScript config -onendlist Scripts/VM0079End.tcl
addtcldriver rdoScript

readoutscript sclrScript -controllertype vmusb
sclrScript config -rdolist Scripts/VM0079Scaler.tcl
addtcldriver sclrScript


stack create eventStack
stack config eventStack -type event -modules rdoScript -trigger nimi1


stack create sclrStack
stack config sclrStack -type scaler -modules sclrScript -period 1


