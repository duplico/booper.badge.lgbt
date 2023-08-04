![Image](booper_transparent.png)

Drawing by [Moeka3](https://www.fiverr.com/moeka3)

# What is this badge?
I'm `booper.badge.lgbt`! Just a simple indie electronic badge for DEF CON 31.

At first, the team decided to take the year off of badge making, but then they got FOMO in mid-July, and made a last-minute badge. No prototypes, no testing, just YOLO. I hope you enjoy!

## How does it work?
Pretty simple! It just hangs out and looks cute, and also looks for its friends ("queerdar!"). The more of its friends it sees, the happier it gets, and the faster the lights above the eyes blink.

You can also boop the heart to make a cute face.

Press and hold the heart to adjust the LED brightness.

# Is this a queercon badge?
No. The `badge.lgbt` team also works on badges for Queercon, but this one's not officially related to Queercon at all. It's our own project!

# Weren't you going to make more floppies?
Yeah, and we really are still planning to make another batch and issue a code update. We've actually fixed a few bugs in the main `badge.lgbt` repo since shipping for 2021. But parts availability for some critical components have been very, very bad the last two years. We hope to bring it back for 2024. Wish us luck...

# Technical notes
## Power-on self-test codes
A limited number of fault codes can be displayed on startup if the badge detects a problem. To read the code, point the tentacles to the left. Read the digit displays from top to bottom in this orientation.

* `Id` - Badge ID number is unassigned. The badge will still mostly work, but the radio will not transmit a valid ID for queerdar behavior. It can still boop, and can still receive other badges' queerdar beacons.
* `RF` - Radio fault. The processor cannot communicate properly with the radio module, and radio functionality will not work. Check the RFM75 module physically for damage and proper connection of its solder joints.
* `Fr` - Frequency calibration lost. It should do this automatically, but it will loop until completed. Place it close to other working badges to complete the calibration, then power cycle the board after its frequency selection is completed. See the section below for more detail on that process.

## Radio frequency calibration
We have observed some inconsistencies between individual examples of the RFM75 radio modules used on these badges. For that reason, part of the assembly process involved a frequency calibration that attempted to select the best frequency configuration to ensure compatibility between all boards. If field failures occur where the frequency calibration may need to be repeated, this can be done by pressing and holding the heart button for 9 seconds. The brightness will change 9 times. After the 9th change the brightness should be the same as when the button was first pressed. Release at this point, and the eyes will show some strange animation for approximately one minute. After one minute, the frequency calibration is completed. It is recommended to place the badge very close to multiple other badges during this process.

The new center frequency will be displayed on the LEDs. Orient the board with tentacles to the left, and the upper LED display will show the 10s digit, and the lower display will show the 1s digit. The selected center frequency will be 2.4XX MHz where the XX is replaced by the 2-digit value shown on after calibration.

## Solder composition
These boards are assembled using Chip Quik Sn42/Bi57.6/Ag0.4 low-temperature lead-free solder. This Bismuth-based solder has a low melting temperature (138°C vs 183°C for Sn63/Pb37 solder), and lower toxicity. The back of the board is assembled with Chip Quik SMDLTFP solder paste, and the front is assembled either with the same paste or with SMD2SWLT wire solder of the same alloy.

Note that tin-bismuth solder can exhibit undesirable thermal properties if contaminated with lead. If you need to rework these boards, please consider doing so in a way that prevents such contamination, either by using a clean tip, avoiding use of lead completely, or thoroughly cleaning the existing solder from the boards before using a different alloy for rework.

A small number of boards were used as prototypes as well, and were assembled with 63/37 leaded solder. These boards have a 22 AWG purple wire on the back. All boards with white or black 30 AWG wires on the back are built with the lead-free process.
