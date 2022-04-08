# VoiceNavi
For operation manuals, refer to [VoiceNavi](http://www.voicenavi.co.jp/).

## Supported devices

- 5A2
- 5F2
- 5F1
- 5F9IH

To switch between different devices, use MS1<sup>*1</sup> and MS2<sup>*1</sup> solder pads.

MS2 | MS1 | Device
:---: | :---: | :---:
Open | Open | 5A2
Open | Closed | 5F2
Closed | Open | 5F1
Closed | Closed | 5F9IH

## Volume adjustment

If SW2 is put into position 1, 1st song in flash will continuously play and VR3 volume can be adjusted. If position 3 is selected, 1st song in flash will continuously play and VR4 volume can be adjusted. This can be used to adjust volume. Normal operation mode is position 2.

## How to use (on Windows)
To edit source code, use [E2 Studio](https://www.renesas.com/us/en/software-tool/e-studio). Version used for this project is 2021-04 (22.1.0).

**IMPORTANT! Use GCC compiler version 8.3.0.202004. Do not use 8.3.202102 or newer. Compiling doesn't work in 202102 version.**

For updating FW, use [Renesas Flash Programmer V3](https://www.renesas.com/us/en/software-tool/renesas-flash-programmer-programming-gui#download).
 - Power off the device and set **boot** jumper.
 - Move SBPOW1 switch from **Host** to **Slave**.
 - Plug in USB cable and connect to PC.
 - Power on the device. To check if the device is recognised by PC, go to Device Manager and look at Ports.

 ![device manager](Images/Ports.png)
 - In Renesas Flash Programmer, create new project.

 ![new project](Images/RFP_new_project.png)

 - Under *Tool* select *Com port*, under *Tool Details* select port which corresponds to RX USB Boot (Device Manager).
 - Click connect to check if you can connect to device. ID Code is all **F**s.
 - In operation tab, make sure you have **little endian** selected.
 - *Program File* is VoiceNavi.mot, located in VoiceNavi/HardwareDebug. If file or folder is missing, compile the project from E2 Studio or download the file from releases page.
 - Press *Start* button to begin download.
 - After download is completed, turn off power, unplug USB cable, move SBPOW1 back to **Host** and remove **boot** jumper.

## Copy from USB drive to internal flash procedure
To copy data from USB drive to flash, next procedure must be done. Before inserting USB drive, make sure it is formatted to FAT (FAT16, FAT32, exFAT) format and all needed wav files and only one wpj file is on USB drive. If this is not true, error will occur during copy procedure.

**Copy procedure:**
- Turn off power.
- Insert USB flash drive.
- Turn on power.
- During copy process, USB LED (green LED) will be turned on.
- If copying was successful, USB LED will blink in 0,5 s intervals. If errors occur, see error table.
- Turn off power.
- Remove USB flash drive.
- Turn on power.

## Errors table
If error occurs during any time when using VoiceNavi, check error table for details what the cause may be.

**LED colours and names:**
- ALARM - red (LED4)
- POWER - amber (LED1)
- BUSY - yellow (LED2)
- USB - green (LED3)

<table>
<thead>
  <tr>
    <th colspan="2">LED status</th>
    <th>Error type</th>
    <th>Possible cause</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>USB LED</td>
    <td>ALARM LED</td>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td>ON</td>
    <td>ON</td>
    <td>File system error</td>
    <td>USB is formatted in wrong file system. Use FAT file system.<br>Flash overflow.</td>
  </tr>
  <tr>
    <td>ON</td>
    <td>blinking (500 ms)</td>
    <td>Wave file error</td>
    <td>WAV file is missing.<br>WPJ file is missing.<br>WAV file is in stereo.</td>
  </tr>
  <tr>
    <td>OFF</td>
    <td>blinking (100 ms)</td>
    <td>Flash ECC error</td>
    <td>Bit error was detected and not corrected. Possibly deteriorated flash.</td>
  </tr>
  <tr>
    <td>OFF</td>
    <td>blinking (500 ms)</td>
    <td>Empty flash error</td>
    <td>No data in flash. Copy data from USB to internal flash.</td>
  </tr>
</tbody>
</table>

## Notes
Above described procedures apply to normal operation mode. If JP1 jumper is soldered, device is in debug mode. Device mode allows user to switch between board types using DIP switch.

*1 - In debug mode use DIP switch 7 and 8. MS1 is 7, MS2 is 8

