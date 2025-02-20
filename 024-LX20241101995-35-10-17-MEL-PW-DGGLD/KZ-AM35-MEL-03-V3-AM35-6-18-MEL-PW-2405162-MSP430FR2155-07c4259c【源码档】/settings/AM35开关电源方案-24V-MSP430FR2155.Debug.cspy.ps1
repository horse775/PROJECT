﻿param([String]$debugfile = "");

# This powershell file has been generated by the IAR Embedded Workbench
# C - SPY Debugger, as an aid to preparing a command line for running
# the cspybat command line utility using the appropriate settings.
#
# Note that this file is generated every time a new debug session
# is initialized, so you may want to move or rename the file before
# making changes.
#
# You can launch cspybat by typing Powershell.exe -File followed by the name of this batch file, followed
# by the name of the debug file (usually an ELF / DWARF or UBROF file).
#
# Read about available command line parameters in the C - SPY Debugging
# Guide. Hints about additional command line parameters that may be
# useful in specific cases :
#   --download_only   Downloads a code image without starting a debug
#                     session afterwards.
#   --silent          Omits the sign - on message.
#   --timeout         Limits the maximum allowed execution time.
#


if ($debugfile -eq "")
{
& "D:\\common\bin\cspybat" -f "D:\001-MY  PROJECT\024-LX20241101995-35-10-17-MEL-PW-DGGLD\KZ-AM35-MEL-03-V3-AM35-6-18-MEL-PW-2405162-MSP430FR2155-07c4259c【源码档】\settings\AM35开关电源方案-24V-MSP430FR2155.Debug.general.xcl" --backend -f "D:\001-MY  PROJECT\024-LX20241101995-35-10-17-MEL-PW-DGGLD\KZ-AM35-MEL-03-V3-AM35-6-18-MEL-PW-2405162-MSP430FR2155-07c4259c【源码档】\settings\AM35开关电源方案-24V-MSP430FR2155.Debug.driver.xcl" 
}
else
{
& "D:\\common\bin\cspybat" -f "D:\001-MY  PROJECT\024-LX20241101995-35-10-17-MEL-PW-DGGLD\KZ-AM35-MEL-03-V3-AM35-6-18-MEL-PW-2405162-MSP430FR2155-07c4259c【源码档】\settings\AM35开关电源方案-24V-MSP430FR2155.Debug.general.xcl" --debug_file=$debugfile --backend -f "D:\001-MY  PROJECT\024-LX20241101995-35-10-17-MEL-PW-DGGLD\KZ-AM35-MEL-03-V3-AM35-6-18-MEL-PW-2405162-MSP430FR2155-07c4259c【源码档】\settings\AM35开关电源方案-24V-MSP430FR2155.Debug.driver.xcl" 
}
