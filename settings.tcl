set EventLogger /usr/opt/nscldaq/11.0/bin/eventlog
set EventLoggerRing tcp://localhost/0400x
set EventLogUseNsrcsFlag 1
set EventLogAdditionalSources 0
set EventLogUseGUIRunNumber 0
set EventLogUseChecksumFlag 1
set EventLogRunFilePrefix run
set StageArea /user/0400x/stagearea
set run 0
set title {}
set recording 0
set timedRun 0
set duration 0
set dataSources {{host localhost parameters {--daqconfig=/user/s800/converged_daq/vmusbdaqconfig.tcl --ctlconfig=/user/s800/converged_daq/vmusbctlconfig.tcl --ring=rawvmusb --sourceid=1 --timestamplib=/user/s800/converged_daq/libvmusbtstamp.so} path /scratch/11.0-s800conv/bin/VMUSBReadout provider SSHPipe sourceid 0 wdir /user/s800/converged_daq} {host localhost parameters {--daqconfig=/user/s800/converged_daq/ccusbdaqconfig.tcl --ctlconfig=/user/s800/converged_daq/ccusbctlconfig.tcl --ring=rawccusb --sourceid=0 --timestamplib=/user/s800/converged_daq/libccusbtstamp.so} path /scratch/11.0-s800conv/bin/CCUSBReadout provider SSHPipe sourceid 1 wdir /user/s800/converged_daq}}
