#include <stdint.h>
struct float{
  uint8_t signif;
  uint8_t exp;
  uint8_t sign;
}

float add(float a, float b){
  uint8_t exp;
  uint8_t first;
  uint8_t first_s;
  uint8_t second;
  uint8_t second_s;
  float out;
  out.signif=0;

  if(a.exp<b.exp){
    exp=b.exp;
    first=b.signif;
    first_s=b.sign;
    second=a.signif<<(b.exp-a.exp);
    second_s=a.sign;
  }else if(a.exp>b.exp){
    exp=a.exp;
    first=a.signif;
    first_s=a.sign;
    second=b.signif<<(a.exp-b.exp);
    second_s=b.sign;
  }else{
    exp=a.exp;
    first=a.signif;
    first_s=a.sign;
    second=b.signif;
    second_s=b.sign;
  }
  out.exp=exp;
  if(first_s==second_s){
    out.sign=first_s;
    out.signif=first+second;
  }else{
    if(first>second){
      out.signif = first-second;
      out.sign = first_s;
    }else if(second>first){
      out.signif = second-first;
      out.sign = second_s;
    }else{
      out.sign = 1;
      out.signif = 0;
    }
  }
  return out;
}

//===============================================================

// Nie do koñca sprytny kod który oryginalnie u¿ywa³ signed intów i wywali siê na unsigned XD
#include <stdio.h>

struct Float {
	unsigned short sign; //zero for negative, one for positive
	int significand;
	int exp; //base is 10
	unsigned short expSign;
};

int intPow(int n, int pow) {
	int i = 0;
	int res = 1;
	for(i=0;i<pow;i++)
		res = res * n;
	return res;
}

void printFloat(struct Float floatt) {
	int exp = floatt.exp;
	if(exp>0) { // if exp is higher, we want to see the number as H U G E
		printf("%s%d", floatt.sign?"":"-",floatt.significand * intPow(10, exp));
	} else { // otherwise, we use cool M A T H  N O T A T I O N
		exp = exp*(-1);
		int before = floatt.significand / intPow(10, exp);
		int after = floatt.significand - before * intPow(10, exp);
		printf("%s%d.%de%d", floatt.sign?"":"-",before, after, floatt.exp);
	}
}

struct Float addFloat(struct Float f1, struct Float f2) {
	struct Float res;
	int multiplyer;

	if(f1.exp>f2.exp) { // if F1 has bigger exp, we need to multiply its significand to play well with lower exp.
		multiplyer = f1.exp - f2.exp;
		f1.significand = f1.significand * intPow(10, multiplyer);  // TODO - this will fuck up when multiplyer < 0
		f1.exp = f2.exp;
	}
	else if (f1.exp<f2.exp) {
		multiplyer = f2.exp - f1.exp;
		f2.significand = f2.significand * intPow(10, multiplyer); 
		f2.exp = f1.exp;
	}

	res.exp = f1.exp;

	// TODO  - is there a way to make it less ugly, assuming we use unsigned int?
	if(!f1.sign && !f2.sign) {
		res.significand = f1.significand + f2.significand; 
		res.sign = 0;
	} else if (f1.sign && !f2.sign)  {
		if(f1.significand > f2.significand) {
			res.significand = f1.significand - f2.significand;
			res.sign = 1;
		} else {
			res.significand = f2.significand - f1.significand;
			res.sign = 0;
		}
	} else if (!f1.sign && f2.sign)  {
		if(f2.significand > f1.significand) {
			res.significand = f2.significand - f1.significand;
			res.sign = 1;
		} else {
			res.significand = f1.significand - f2.significand;
			res.sign = 0;
		}
	} else {
		res.significand = f1.significand + f2.significand; 
		res.sign = 1;
	}

	return res;
}

struct Float subFloat(struct Float f1, struct Float f2) {
	f2.sign = !f2.sign;
	return addFloat(f1, f2);
}

struct Float mulFloat(struct Float f1, struct Float f2) {
	struct Float res;

	res.sign = !(f1.sign ^ f2.sign);
	res.exp = f1.exp + f2.exp;
	res.significand = f1.significand * f2.significand;

	return res;
}

struct Float divFloat(struct Float f1, struct Float f2) {
	struct Float res;

	res.sign = !(f1.sign ^ f2.sign);
	res.exp = f1.exp - f2.exp;
	res.significand = f1.significand / f2.significand;

	return res;
}

int main() {
	printf("How cool is that!\n");

	struct Float myFloat;
	myFloat.sign = 1;
	myFloat.significand = 255;
	myFloat.exp = -3;
	printFloat(myFloat);

	printf(", ");

	struct Float float2;
	float2.sign = 1;
	float2.significand = 23;
	float2.exp = 5;

	printFloat(float2);

	printf("\n add: ");

	printFloat(addFloat(myFloat, float2));

	printf("\n sub: ");

	printFloat(subFloat(myFloat, float2));

	printf("\n mul: ");

	printFloat(mulFloat(myFloat, float2));

	printf("\n div: ");

	printFloat(divFloat(myFloat, float2));

}

//===============================================================

// Oryginalny kod blinkera
//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via DEBUG).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// Definitions visible only within this translation unit.
namespace
{
  // ----- Timing definitions -------------------------------------------------

  // Keep the LED on for 2/3 of a second.
  constexpr Timer::ticks_t BLINK_ON_TICKS = Timer::FREQUENCY_HZ * 3 / 4;
  constexpr Timer::ticks_t BLINK_OFF_TICKS = Timer::FREQUENCY_HZ
      - BLINK_ON_TICKS;
}

// ----- LED definitions ------------------------------------------------------

#if defined(STM32F401xE)

#warning "Assume a NUCLEO-F401RE board, PA5, active high."

// PA5
#define BLINK_PORT_NUMBER         (0)
#define BLINK_PIN_NUMBER          (5)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[1] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER, BLINK_ACTIVE_LOW },
  };

#elif defined(STM32F407xx)

#warning "Assume a STM32F4-Discovery board, PD12-PD15, active high."

#define BLINK_PORT_NUMBER         (3)
#define BLINK_PIN_NUMBER_GREEN    (12)
#define BLINK_PIN_NUMBER_ORANGE   (13)
#define BLINK_PIN_NUMBER_RED      (14)
#define BLINK_PIN_NUMBER_BLUE     (15)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[4] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_ORANGE, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_RED, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_BLUE, BLINK_ACTIVE_LOW },
  };

#elif defined(STM32F411xE)

#warning "Assume a NUCLEO-F411RE board, PA5, active high."

#define BLINK_PORT_NUMBER         (0)
#define BLINK_PIN_NUMBER          (5)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[1] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER, BLINK_ACTIVE_LOW },
  };

#elif defined(STM32F429xx)

#warning "Assume a STM32F429I-Discovery board, PG13-PG14, active high."

#define BLINK_PORT_NUMBER         (6)
#define BLINK_PIN_NUMBER_GREEN    (13)
#define BLINK_PIN_NUMBER_RED      (14)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[2] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_RED, BLINK_ACTIVE_LOW },
  };

#else

#warning "Unknown board, assume PA5, active high."

#define BLINK_PORT_NUMBER         (0)
#define BLINK_PIN_NUMBER          (5)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[1] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER, BLINK_ACTIVE_LOW },
  };

#endif

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{
  // Send a greeting to the trace device (skipped on Release).
  trace_puts("Hello ARM World!");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf("System clock: %u Hz\n", SystemCoreClock);

  Timer timer;
  timer.start ();

  // Perform all necessary initialisations for the LEDs.
  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].powerUp ();
    }

  uint32_t seconds = 0;

  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].turnOn ();
    }

  // First second is long.
  timer.sleep (Timer::FREQUENCY_HZ);

  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].turnOff ();
    }

  timer.sleep (BLINK_OFF_TICKS);

  ++seconds;
  trace_printf ("Second %u\n", seconds);

  if ((sizeof(blinkLeds) / sizeof(blinkLeds[0])) > 1)
    {
      // Blink individual LEDs.
      for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
        {
          blinkLeds[i].turnOn ();
          timer.sleep (BLINK_ON_TICKS);

          blinkLeds[i].turnOff ();
          timer.sleep (BLINK_OFF_TICKS);

          ++seconds;
          trace_printf ("Second %u\n", seconds);
        }

      // Blink binary (it does not though, there originally was code that did that, but it's not that hard to design, isn't it? XD)
      while (1)
        {
//        	  	  for(int i =0; i<rand() % 50;i++){
        	  		  blinkLeds[0].turnOn();
        	  		  if (rand() % 3 != 0) blinkLeds[1].toggle();
        	  		  timer.sleep(rand() % 100 + 30);
        	  		blinkLeds[0].turnOff();
        	  		timer.sleep(rand() % 60);
//        	  	  }
//        	  	for(int i =0; i<20;i++){
//      	  		  blinkLeds[1].turnOn();
//    	  		  timer.sleep(rand() % 100 + 30);
//      	  		blinkLeds[1].turnOff();
//      	  		timer.sleep(40);
//        	  }

          ++seconds;
          trace_printf ("Second %u\n", seconds);
        }
      // Infinite loop, never return.
    }
  else
    {
      while (1)
        {
          blinkLeds[0].turnOn ();
          timer.sleep (300);

          blinkLeds[0].turnOff ();
          timer.sleep (300);

          ++seconds;
          trace_printf ("Second %u\n", seconds);
        }
      // Infinite loop, never return.
    }
}

