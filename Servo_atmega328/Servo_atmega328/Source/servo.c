/*
 * servo.c
 *
 * Created: 20.7.2018. 21.50.45
 *  Author: Kovacevic
 */ 

#include "common.h"
#include "pwm.h"
#include "servo.h"




uint_least16_t set_servo_angle(uint_least8_t angle, uint_least8_t min, uint_least8_t max)
{
	/*
	 * Funkcija kojom se zadaje pozicija servo motora u stepenima
	 * min i max definisu minimalan i maksimalan ugao
	 * fizicki minimun je faktor ispune 3.1% (0.624ms)
	 * dok je maksimum 11.9% (2.38ms)
	 * 0 stepeni predstavlja 3.1%, a 180 stepeni predstavlja 11.9%
	 * Ovim je obuhvacen i mali zazor do krajnjih pozicija kako bi se 
	 * izbeglo zaglavljivanje sto bi izazvalo veliku struju kroz motor
	 * Pored, dole, hard-coded ogranicenja za min i max ugao,
	 * prosledjeni parametri min i max dodatno osiguravaju da se ne prekoraci
	 * zeljeni opseg kretanja
	 * Eksperimentalna zavisnost ugla od faktora ispune(linearna f-ja): DuC = (0.049*angle + 3.1)/100 - nije u %
	 */
	
	static uint_least16_t DuC = 0;
	static const float koef = 0.049;	
	static const float y_ofset = 3.1;									
	
	
	//zabranjeno stanje
	if( min >= max)
		return 0;

	
	/*zastita za minimalni ugao*/
	if (min < 0)
		min = 0;
	else if (min > 180)
		min = 180;

	
	/*zastita za maksimalni ugao*/
	if (max < 0)
		max = 0;
	else if (max > 180)
		max = 180;
	
	/*zastita za zeljeni ugao*/
	if (angle < 0)
		angle = 0;
	else if (angle > 180)
		angle = 180;
	
	
	
	/*opseg koji zeli korisnik*/
	if(angle > max)
		angle = max;
	if(angle < min)
		angle = min;
	
	
	DuC = ((koef*angle + y_ofset)/100 )* TOP_PWM;		//top pwm = pwm period registar
														//top pwm mnozi za koeficijentom (0-1), sto je fakticki faktor ispune
	return DuC;
	
}