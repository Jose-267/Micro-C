/* ================== main.c ================== */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "adc.h"
#include "servo.h"

/* Promedia ‘samples’ lecturas del canal ADC indicado */
static uint16_t ADC_read_avg(uint8_t channel, uint8_t samples)
{
	uint32_t sum = 0;
	for (uint8_t i = 0; i < samples; i++)
	sum += ADC_read(channel);

	return (uint16_t)(sum / samples);
}

int main(void)
{
	/* ---------- Variables ---------- */
	uint16_t adc0, adc1, adc2;           /* potenciómetros            */
	uint16_t dutyA_us, dutyB_us, dutyC_us;/* pulsos para los servos   */

	/* ---------- Inicialización ---------- */
	ADC_init();          /* conversor A/D                          */
	servo_init();        /* Timer 1: servos A (PB1) y B (PB2)      */
	servoC_init();       /* Timer 2: servo  C (PD3)                */

	/* ---------- Bucle principal ---------- */
	while (1)
	{
		/* 1. Leer potenciómetros (promediar 4 muestras) */
		adc0 = ADC_read_avg(0, 4);   /* pot 0 → servo B (PB2) */
		adc1 = ADC_read_avg(1, 4);   /* pot 1 → servo A (PB1) */
		adc2 = ADC_read_avg(2, 4);   /* pot 2 → servo C (PD3) */

		/* 2. Mapear ADC→anchos de pulso                    */
		/*    Servos A y B: 1,0-5,0 ms  (1000-5000 µs)      */
		dutyB_us = ((uint32_t)adc0 * 4000UL) / 1023UL + 1000UL;
		dutyA_us = ((uint32_t)adc1 * 4000UL) / 1023UL + 1000UL;

		/*    Servo C: 1,0-2,5 ms  (1000-2500 µs)           */
		dutyC_us = ((uint32_t)adc2 * 1500UL) / 1023UL + 1000UL;

		/* 3. Actualizar servos                            */
		servoB_setDuty(dutyB_us);
		servoA_setDuty(dutyA_us);
		servoC_setDuty(dutyC_us);

		/* 4. Esperar un frame (~20 ms)                    */
		_delay_ms(20);
	}
}
