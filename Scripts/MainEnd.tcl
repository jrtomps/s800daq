# Main End sequence for s800 DAQ

# Temporarily removed to achieve continuous time stamp mode (Sam's experiment) ????????????
#after 100
trigger Go 0

## Unfreeze GUIs if they exist
#if {[winfo exists .trigger]} {
#	trigger LockGUI 0
#}
#if {[winfo exists .gd16]} {
#	delay UnlockGD16
#}
