#include <msp430.h> 
/// RB 2023 badge low-level entry point.
/**
 ** The main source module is concerned with the basic setup of the MCU and
 ** onboard GPIO and peripherals. It also contains the main program loop of
 ** the entire badge code. Aside from initialization of peripherals and the
 ** other badge drivers, this module has a core purpose of calling events in
 ** (primarily) the badge.c module.
 **
 ** The basic split in responsibility between the badge.c and main.c modules
 ** is that main.c detects, prioritizes, and clears flags set from
 ** interrupts; it then calls the appropriate function in badge.c so that
 ** badge.c can behave in a more event-driven way, with the underlying MSP430
 ** hardware and registers abstracted away by main.c for the most part.
 **
 ** \file main.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2023 George Louthan @duplico. MIT License.
 */

// MSP430 main header
#include <msp430fr2633.h>

// C headers
#include <stdint.h>

// Accessory headers
#include <driverlib.h>

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Board basics

	// Mid-level drivers

	// Application functionality

	return 0;
}
