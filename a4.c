/* a4.c
 * CSC Spring 2024
 * 
 * Student name:  Matthew Laforce
 * Student UVic ID:  V01019219
 * Date of completed work:  April 5, 2024
 *
 *
 * Code provided for Assignment #4
 *
 * Author: Mike Zastre (2022-Nov-22)
 *
 * This skeleton of a C language program is provided to help you
 * begin the programming tasks for A#4. As with the previous
 * assignments, there are "DO NOT TOUCH" sections. You are *not* to
 * modify the lines within these section.
 *
 * You are also NOT to introduce any new program-or file-scope
 * variables (i.e., ALL of your variables must be local variables).
 * YOU MAY, however, read from and write to the existing program- and
 * file-scope variables. Note: "global" variables are program-
 * and file-scope variables.
 *
 * UNAPPROVED CHANGES to "DO NOT TOUCH" sections could result in
 * either incorrect code execution during assignment evaluation, or
 * perhaps even code that cannot be compiled.  The resulting mark may
 * be zero.
 */


/* =============================================
 * ==== BEGINNING OF "DO NOT TOUCH" SECTION ====
 * =============================================
 */

#define __DELAY_BACKWARD_COMPATIBLE__ 1
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DELAY1 0.000001
#define DELAY3 0.01

#define PRESCALE_DIV1 8
#define PRESCALE_DIV3 64
#define TOP1 ((int)(0.5 + (F_CPU/PRESCALE_DIV1*DELAY1))) 
#define TOP3 ((int)(0.5 + (F_CPU/PRESCALE_DIV3*DELAY3)))

#define PWM_PERIOD ((long int)500)

volatile long int count = 0;
volatile long int slow_count = 0;


ISR(TIMER1_COMPA_vect) {
	count++;
}


ISR(TIMER3_COMPA_vect) {
	slow_count += 5;
}

/* =======================================
 * ==== END OF "DO NOT TOUCH" SECTION ====
 * =======================================
 */

/* *********************************************
 * **** BEGINNING OF "STUDENT CODE" SECTION ****
 * *********************************************
 */

// Name global variables for readability
#define OFF ((int)0)
#define ON ((int)1)

// ================================================================================
/*
PART A: led_state()

This code uses a switch to split inputs into four main cases, with LED values 
0 through 3 for the rightmost to leftmost LEDs (respectively). These cases are 
then split further into 'ON' and 'OFF' states - when state is 'ON', do the same
to the LED.

As a minor form of error handling, if 'LED' isn't an integer ranging from 0 to 
3, this function will do nothing.
*/

void led_state(uint8_t LED, uint8_t state) {
	switch (LED) {
	// "Rightmost LED"  [LED 0 / Bit 7]
		case 0:
			if (state == ON) {
				// State on - LED on
				PORTL |= 0b10000000;
			} else {
				// State off - LED off
				PORTL &= 0b01111111;
			}
		break;
	// "Right-Mid LED"  [LED 1 / Bit 5]
		case 1:
			if (state == ON) {
				PORTL |= 0b00100000;
			} else {
				PORTL &= 0b11011111;
			}
		break;
	// "Left-Mid LED"  [LED 2 / Bit 3]
		case 2:
			if (state == ON) {
				PORTL |= 0b00001000;
				
			} else {
				PORTL &= 0b11110111;
			}
		break;
	// "Leftmost LED"  [LED 3 / Bit 1]
		case 3:
			if (state == ON) {
				PORTL |= 0b00000010;
			} else {
				PORTL &= 0b11111101;
			}
			
	// Any other LED input is ignored
	}
}
// ================================================================================
/*
PART B: SOS()

This version of 'SOS' uses a 'for' loop with an increment: it iterates until the
index equals 'length'. Inside the loop, it examines each bit in the 'light[]' 
array value in position 'index', detecting which bits should be on or off using 
a bitwise '&' operation. Finally, this function pauses for 'duration[]' at 
'index', using '_delay_ms'.

Note: This code is generic enough to work in Part E without alteration, so
I recycled this code down there.
*/

void SOS() {
    uint8_t light[] = {
        0x1, 0, 0x1, 0, 0x1, 0,
        0xf, 0, 0xf, 0, 0xf, 0,
        0x1, 0, 0x1, 0, 0x1, 0,
        0x0
    };
    int duration[] = {
        100, 250, 100, 250, 100, 500,
        250, 250, 250, 250, 250, 500,
        100, 250, 100, 250, 100, 250,
        250
    };
	int length = 19;
	for (int index = 0; index < length; index++) {
	// Examine (Bit 0 / LED 0); call 'led_state'
		if ((light[index] & 0b00000001) != 0b00000000) {
			led_state(0, ON);
		} else {
			led_state(0, OFF);
		}
	// Repeat for (Bit 1 / LED 1)
		if ((light[index] & 0b00000010) != 0b00000000) {
			led_state(1, ON);
		} else {
			led_state(1, OFF);
		}
	// Repeat for (Bit 2 / LED 2)
		if ((light[index] & 0b00000100) != 0b00000000) {
			led_state(2, ON);
		} else {
			led_state(2, OFF);
		}
	// Lastly, repeat for (Bit 3 / LED 3)
		if ((light[index] & 0b00001000) != 0b00000000) {
			led_state(3, ON);
		} else {
			led_state(3, OFF);
		}
	// Delay for 'duration[]' using '_delay_ms', then loop
		_delay_ms(duration[index]);
	}
}
// ================================================================================
/*
PART C: glow()

This glow() function closely follows the provided psudocode. Cycles are broken 
into three parts:

1.) An LED "on" stage (count < glow_threshold);
2.) An LED "off" stage (glow_threshold <= count < PWM_PERIOD);
3.) A "reset" stage, which sets count to 0 reseting the cycle. 
*/
void glow(uint8_t LED, float brightness) {
	long int glow_threshold = PWM_PERIOD * brightness;
	int glowing = ON;
	while (glowing) {
		if (count < glow_threshold) {
			// Handle "on-time" for the LED
			led_state(LED, ON);
		} else if (count < PWM_PERIOD) {
			// Handle "off-time" for the LED
			led_state(LED, OFF);
		} else {
			// Cycle finished - reset 'count' to '0', loop again
			count = 0;
		}
	}
}
// ================================================================================
/*
PART D: pulse_glow()

There are two major parts to this function, which correspond to the two timers. 

1.) A lighting cycle, tied to 'count' and 'PWM_PERIOD'. This cycle does two main 
	things. One, it increments or decrements 'intensity', depending on 'increasing';
	and two, it uses 'intensity' to calculate a duty cycle which it uses to handle 
	the lighting.
	
2.) An inversion cycle, tied to 'slow_count' and 'full_cycle'. This cycle flips 
	'increasing', changing the behavior of the lighting cycle. It also resets
	'count' and 'slow_count'.
	
Note: some of my values, such as 'full_cycle' and 'interval', seem quite
strange mathematically. In practice on the lab computers, I found these values
seemed to line up best with the assignment video!
*/

void pulse_glow(uint8_t LED) {
	
	// Create a timing variable
	long int full_cycle = 5650;
	// Create intensity variables
	float interval = 0.0055;
	float intensity = 0;
	// Create logical variables
	long int glow_threshold = 0;
	int increasing = ON;
	// Make an infinite loop
	int glowing = ON;
	while (glowing) {
		
	// 1. LIGHTING CYCLE - uses count + PWM_PERIOD
		if (count < glow_threshold) {
			// Handle the "lit-up" portion of the duty cycle
			led_state(LED, ON);
		} else if (count < PWM_PERIOD) {
			// Handle the "off" portion of the duty cycle
			led_state(LED, OFF);
		} else {
			// Update the interval of the duty cycle
			if (increasing == ON) {
				intensity += interval;
			} else {
				intensity -= interval;
			}
			glow_threshold = PWM_PERIOD * intensity;
			count = 0;
		}
	// 2. INVERSION CYCLE - uses full_cycle + slow_count
		if (slow_count >= full_cycle) {
			if (increasing == ON) {
			// Maxed brightness - turn off 'increasing'
				increasing = OFF;
			} else {
			// LED off - turn on 'increasing'
				increasing = ON;
			}
			// Reset the timers
			slow_count = 0;
			count = 0;
		}
	}
}
// ================================================================================
/*
BONUS: light_show()

Concerning the logic I used here, it is essentially just Part B a second time. I
actually copy/pasted the code over.

However, the hardest part here was getting the LED flash values from the video 
along with the long/short timings! I figure this gets pretty close to what is shown 
in the video: hopefully close enough.
*/

void light_show() {
	uint8_t light[] = {
		0xf, 0, 0xf, 0,
		0xf, 0, 0x6, 0,
		0x9, 0, 0xf, 0,
		0xf, 0, 0xf, 0,
		0x9, 0, 0x6, 0,
		0x8, 0xc, 0x6, 0x3,
		0x1, 0x3, 0x6, 0xc,
		0x8, 0xc, 0x6, 0x3,
		0x1, 0x3, 0x6, 0,
		0xf, 0, 0xf, 0,
		0x6, 0, 0x6, 0
	};
	int duration[] = {
		185, 185, 185, 185,
		185, 185, 80, 80,
		80, 80, 185, 185,
		185, 185, 185, 185,
		80, 80, 80, 80,
		80, 80, 80, 80,
		80, 80, 80, 80,
		80, 80, 80, 80,
		80, 80, 80, 80,
		185, 185, 185, 185,
		185, 80, 185, 185
	};
	int length = 44;
	for (int index = 0; index < length; index++) {
		// Examine (Bit 0 / LED 0); call 'led_state'
		if ((light[index] & 0b00000001) != 0b00000000) {
			led_state(0, ON);
			} else {
			led_state(0, OFF);
		}
		// Repeat for (Bit 1 / LED 1)
		if ((light[index] & 0b00000010) != 0b00000000) {
			led_state(1, ON);
			} else {
			led_state(1, OFF);
		}
		// Repeat for (Bit 2 / LED 2)
		if ((light[index] & 0b00000100) != 0b00000000) {
			led_state(2, ON);
			} else {
			led_state(2, OFF);
		}
		// Lastly, repeat for (Bit 3 / LED 3)
		if ((light[index] & 0b00001000) != 0b00000000) {
			led_state(3, ON);
			} else {
			led_state(3, OFF);
		}
		// Delay for 'duration[]' using '_delay_ms', then loop
		_delay_ms(duration[index]);
	}
}
// ================================================================================

/* ***************************************************
 * **** END OF FIRST "STUDENT CODE" SECTION **********
 * ***************************************************
 */


/* =============================================
 * ==== BEGINNING OF "DO NOT TOUCH" SECTION ====
 * =============================================
 */

int main() {
    /* Turn off global interrupts while setting up timers. */

	cli();

	/* Set up timer 1, i.e., an interrupt every 1 microsecond. */
	OCR1A = TOP1;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1 << WGM12);
    /* Next two lines provide a prescaler value of 8. */
	TCCR1B |= (1 << CS11);
	TCCR1B |= (1 << CS10);
	TIMSK1 |= (1 << OCIE1A);

	/* Set up timer 3, i.e., an interrupt every 10 milliseconds. */
	OCR3A = TOP3;
	TCCR3A = 0;
	TCCR3B = 0;
	TCCR3B |= (1 << WGM32);
    /* Next line provides a prescaler value of 64. */
	TCCR3B |= (1 << CS31);
	TIMSK3 |= (1 << OCIE3A);


	/* Turn on global interrupts */
	sei();

/* =======================================
 * ==== END OF "DO NOT TOUCH" SECTION ====
 * =======================================
 */


/* *********************************************
 * **** BEGINNING OF "STUDENT CODE" SECTION ****
 * *********************************************
 */

/* // This code could be used to test your work for part A.
	led_state(0, 1);
	_delay_ms(1000);
	led_state(2, 1);
	_delay_ms(1000);
	led_state(1, 1);
	_delay_ms(1000);
	led_state(2, 0);
	_delay_ms(1000);
	led_state(0, 0);
	_delay_ms(1000);
	led_state(1, 0);
	_delay_ms(1000);
*/
	
/// This code could be used to test your work for part B.

	// SOS();

// This code could be used to test your work for part C.

	// glow(0, 0.01);

// This code could be used to test your work for part D.

	// pulse_glow(3);

// This code could be used to test your work for the bonus part.

	// light_show();
	

/* ****************************************************
 * **** END OF SECOND "STUDENT CODE" SECTION **********
 * ****************************************************
 */
}
