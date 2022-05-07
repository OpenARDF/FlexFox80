Version X1_8_3
Hardware Compatibility: Rev X1
Changes:
o Implements simple RSSI averaging filter above a certain RSSI-delta threshold
o Allows 6 threshold settings to be selected using TON parameter: TON 1, TON 2, ... TON 6.

Version X1_8_2
Hardware Compatibility: Rev X1
Changes:
o Adds the "TON" command for turning on/off audio RSSI
o TON -1, TON 0, TON 1, reverse audio freq RSSI, turn off audio RSSI, and enable normal audio freq RSSI, respectively
o TON settings get saved by "P" perm command.

Version X1.8.1
Hardware Compatibility: Rev X1
Changes:
o Includes attenuation, 2m preamp, and 80m preamp (VGA) settings to the
list of those that get "permed"
o Moved more receiver functionality to receiver.c module.


Version X1.8.0 - 29 Dec 2017
Hardware Compatibility: Rev X1
Changes:
o Fixes an issue the could result in uC reset if PORTB Pin 2 gets pulled low
o Added a compile flag allowing watchdog timer to be disabled (default is enabled)


Version 0.7.15 - 11 Dec 2017
Hardware Compatibility: Rev X1
Changes:
o Added functionality to the existing PRE (preamp) command, allowing
it to set the 80m front end VGA DAC controlling its gain (0-255)
o Added driver files for DAC081C085.


Version 0.7.14 - 2 Dec 2017
Hardware Compatibility: Rev X1
Changes:
o Adds "PRE" command to support turning 2m preamp on/off
o Attenuation now is settable from [0 to 100] with 100=max attenuation
o Minor bug fixes


Version 0.7.11 - 29 Nov 2017
Hardware Compatibility: Rev X1
Changes:
o Fixes issues with setting and restoring volume settings.
o Tone volume setting now ranges from 0 to 15, like Main volume.


Version 0.7.10 - 25 Nov 2017
Hardware Compatibility: Rev X1
Changes:
o Add the "A" command for setting receiver attenuation
o Improves filtering of RSSI when "SS" command is used
o Fixes a strange optimizer issue with the "BND" command by disabling
optimization for that command.