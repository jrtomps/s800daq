#
#package require evbcallouts
#
#::EVBC::useEventBuilder
#
#::EVBC::registerRingSource tcp://spdaq48/rawccusb libccusbtstamp.so 0  "S800 CAMAC"
#::EVBC::registerRingSource tcp://spdaq48/rawvmusb libvmusbtstamp.so 1  "S800 VMUSB"
#
#proc OnStart {} {
#::EVBC::initialize -restart false -glombuild true -glomdt 10 -destring s800built
#}
