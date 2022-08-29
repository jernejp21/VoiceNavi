# UTF-8

from ctypes import *
import math
import time
import sys
import numpy
import matplotlib.pyplot as plt

from ctypes import *
from dwfconstants import *
import time
import sys

if sys.platform.startswith("win"):
    dwf = cdll.dwf
elif sys.platform.startswith("darwin"):
    dwf = cdll.LoadLibrary("/Library/Frameworks/dwf.framework/dwf")
else:
    dwf = cdll.LoadLibrary("libdwf.so")

hdwf = c_int()
sts = c_byte()
IsEnabled = c_bool()
usbVoltage = c_double()
usbCurrent = c_double()
auxVoltage = c_double()
auxCurrent = c_double()

version = create_string_buffer(16)
dwf.FDwfGetVersion(version)
print("DWF Version: "+str(version.value))

print("Opening first device")
dwf.FDwfDeviceOpen(c_int(-1), byref(hdwf))

if hdwf.value == 0:
    print("failed to open device")
    szerr = create_string_buffer(512)
    dwf.FDwfGetLastErrorMsg(szerr)
    print(str(szerr.value))
    quit()

# set up analog IO channel nodes
# enable positive supply
dwf.FDwfAnalogIOChannelNodeSet(hdwf, c_int(0), c_int(0), c_double(True))
# set voltage to 5 V
dwf.FDwfAnalogIOChannelNodeSet(hdwf, c_int(0), c_int(1), c_double(3.3))
# master enable
dwf.FDwfAnalogIOEnableSet(hdwf, c_int(True))

time.sleep(2)

pulseTime = c_double(0.050)  # 50 ms
SW1 = c_int(7)
SW2 = c_int(6)
SW3 = c_int(5)
SW4 = c_int(4)
SW5 = c_int(3)
SW6 = c_int(2)
SW7 = c_int(1)
SW8 = c_int(0)
SW9 = c_int(12)
SW10 = c_int(13)
SW11 = c_int(14)
SW12 = c_int(15)
OP = c_int(9)
STOP = c_int(8)
BUSY = c_int(10)

HIGH = c_int(2)
LOW = c_int(1)

for channel in range(0, 16):
    # 1=DwfDigitalOutIdleLow, low when not running
    dwf.FDwfDigitalOutIdleSet(hdwf, c_int(channel), HIGH)

dwf.FDwfDigitalOutRunSet(hdwf, pulseTime)  # pulseTime run
dwf.FDwfDigitalOutRepeatSet(hdwf, c_int(1))  # once
dwf.FDwfDigitalOutCounterInitSet(hdwf, SW1, c_int(0), c_int(0))  # initialize low on start
#dwf.FDwfDigitalOutCounterInitSet(hdwf, SW3, c_int(0), c_int(0))  # initialize low on start
#dwf.FDwfDigitalOutCounterInitSet(hdwf, OP, c_int(0), c_int(0))  # initialize low on start
dwf.FDwfDigitalOutCounterInitSet(hdwf, STOP, c_int(1), c_int(1))  # initialize low on start

# low/high count zero, no toggle during run
dwf.FDwfDigitalOutCounterSet(hdwf, SW1, c_int(0), c_int(0))
dwf.FDwfDigitalOutEnableSet(hdwf, SW1, c_int(1))
#dwf.FDwfDigitalOutCounterSet(hdwf, SW3, c_int(0), c_int(0))
#dwf.FDwfDigitalOutEnableSet(hdwf, SW3, c_int(1))
#dwf.FDwfDigitalOutCounterSet(hdwf, OP, c_int(0), c_int(0))
#dwf.FDwfDigitalOutEnableSet(hdwf, OP, c_int(1))

dwf.FDwfDigitalOutEnableSet(hdwf, STOP, c_int(1))
dwf.FDwfDigitalOutCounterSet(hdwf, STOP, c_int(0), c_int(0))

'''oscilo start'''
hzDI = c_double()
dwf.FDwfDigitalInInternalClockInfo(hdwf, byref(hzDI))
print("DigitanIn base freq: "+str(hzDI.value/1e6)+"MHz")
# sample rate = system frequency / divider, 1kHz
freq = 1000
div = int(hzDI.value/freq)
print("Freq is: " + str(freq))
dwf.FDwfDigitalInDividerSet(hdwf, c_int(div))

# 16bit per sample format
dwf.FDwfDigitalInSampleFormatSet(hdwf, c_int(16))
# set number of sample to acquire
cSamples = 1024
rgbSamples = (c_uint16*cSamples)()
dwf.FDwfDigitalInBufferSizeSet(hdwf, c_int(cSamples))
dwf.FDwfDigitalInTriggerSourceSet(hdwf, c_ubyte(3))  # trigsrcDetectorDigitalIn
dwf.FDwfDigitalInTriggerPositionSet(hdwf, c_int(int(cSamples/4*3-1)))
dwf.FDwfDigitalInTriggerSet(hdwf, c_int(0), c_int(0), c_int(0), c_int(1 << 7))  # DIO7 falling edge

swDetectResult = 0
opDetectResult = 0

numberOfTests = 500
testNr = 0
while testNr < numberOfTests:
    # begin acquisition
    dwf.FDwfDigitalInConfigure(hdwf, c_bool(0), c_bool(1))
    print("Waiting for acquisition {0}".format(testNr + 1))

    while True:
        dwf.FDwfDigitalInStatus(hdwf, c_int(1), byref(sts))
        if sts.value == DwfStateArmed.value:
            break

    # Start switch
    dwf.FDwfDigitalOutConfigure(hdwf, c_int(1))

    while True:
        dwf.FDwfDigitalInStatus(hdwf, c_int(1), byref(sts))
        #print("Status:", str(sts.value))
        if sts.value == DwfStateDone.value:  # done
            break

    time.sleep(0.2)
    dwf.FDwfDigitalOutIdleSet(hdwf, STOP, LOW)
    time.sleep(0.2)
    dwf.FDwfDigitalOutIdleSet(hdwf, STOP, HIGH)

    # get samples, byte size
    dwf.FDwfDigitalInStatusData(hdwf, rgbSamples, 2*cSamples)
    data = numpy.fromiter(rgbSamples, dtype=numpy.uint16)

    sw1_data = data & (1 << 7)
    sw2_data = data & (1 << 5)
    busy_data = data & (1 << 10)

    sw1_data = sw1_data >> 7
    sw2_data = sw2_data >> 5
    busy_data = busy_data >> 10

    #T = cSamples/freq
    #t = numpy.linspace(-T/4, T*3/4, cSamples)
    #plt.plot(t, sw1_data, 'r')
    #plt.plot(t, sw2_data, 'g')
    #plt.plot(t, busy_data, 'b')
    #plt.show()

    # Check test results
    if(sw1_data.min() == 0 and sw1_data.max() == 1):
        #ok
        #print("SW1 puls detected")
        swDetectResult +=1
    if(busy_data.min() == 0 and busy_data.max() == 1):
        #ok
        #print("OP signal detected")
        opDetectResult +=1

    testNr += 1

print()
print("Pulse length: {0}".format(pulseTime))
print("SW1 puls detected {0} times".format(swDetectResult))
print("OP signal detected {0} times".format(opDetectResult))
print()

dwf.FDwfDeviceCloseAll()
