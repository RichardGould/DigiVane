Digital version of analog Weather Station wind vane ... WIP

Eight slotted opto's thru an 8 bit 8574 i2c to a Wenos D1 Mini with a slotted disk capable of triggering two opto's giving 16 unique values.

the opto's are radially spread over 360 degrees and connected clockwise such that opto 1 is bit 0, opto 8 is bit 7

data ouput is published via MQTT where :

MiS/VANE/Data = $,00003,29996,000,000,000,000,000,000,000,000,000,000,000,000,000,296,000,000,000,#

$  is identifier
field 1  is sequence number
field 2  is milliseconds of monitoring
field 3  is number of undefined reading, a reading of zero from 8574
field 4 - 20  is a count of the particular opto
#  is identifier

Firmware update is via AudinoOTA via Arduino IDE
