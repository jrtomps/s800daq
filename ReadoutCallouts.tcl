
package require evbcallouts

::EVBC::useEventBuilder

::EVBC::registerRingSource tcp://spdaq48/rawccusb libdummytstamp.so 0  "CCUSB"
::EVBC::registerRingSource tcp://spdaq48/rawvmusb libdummytstamp.so 1  "VMUSB"

proc OnStart {} {
::EVBC::initialize -restart false -glombuild true -glomdt 10 -destring s800built
}
