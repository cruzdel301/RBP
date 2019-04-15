/* Copyright 2017, Danilo Zecchin.
 * Copyright 2018, Eric Pernia.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Date: 2011-11-17
 */

/* 
HC-SR04 connections:

   +--------------------------------------+
   |     *****     +----- +     *****     |  
   |   *+++++++*   +------+   *+++++++*   |   
   |  *+++++++++*   HCSR04   *+++++++++*  | 
   |   *+++++++*              *+++++++*   | 
   |     *****    o  o  o  o    *****     |
   +--------------+--+--+--+--------------+
                  |  |  |  |
                  V  T  E  G 
                  C  r  c  N
                  C  i  h  D
                     g  o

     EDU-CIAA-NXP +5V <--> Vcc HCSR04
   EDU-CIAA-NXP GPIO0 <--> Trig HCSR04
   EDU-CIAA-NXP GPIO2 <--> Echo HCSR04
     EDU-CIAA-NXP GND <--> GND HCSR04
 */

/*==================[inclusions]=============================================*/


//headers de las versiones para iterar
#include "dst_mm.h"
#include "dst_mm_xdef.h"
#include "dst_mm_exc.h"
#include "dst_cm.h"

//header sAPI
#include "sapi.h"
//header de las funciones para emplear las funciones de sapi al nivel del aplicativo RB
#include "sapi_ultrasonic_hcsr04.h"
#include "sapi_datatypes.h"
//header del test de aceptacion
#include "atest.h"
/*==================[macros and definitions]=================================*/
#define UART_DEBUG UART_USB

//CONSOLE_PRINT_ENABLE
DEBUG_PRINT_ENABLE
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void format( float valor, char *dst, uint8_t pos );

/*==================[internal data definition]===============================*/
char buffout[64];
uint32_t testDistance, another;
float Checkpoint;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void)
{

	uint32_t distanceInInchs, distanceInCms, acceptedDistance;
	/* ------------- INICIALIZACIONES ------------- */

	/* Inicializar la placa */
	boardConfig();

	/* Inicializar UART_USB como salida de consola */
	//consolePrintConfigUart( UART_USB, 115200 );
	debugPrintConfigUart( UART_DEBUG, 115200 );
	/* Inicializar el sensor ultrasonico #0 */
	ultrasonicSensorConfig(ULTRASONIC_SENSOR_0, ULTRASONIC_SENSOR_ENABLE);

	/* Configurar PWM */
	pwmConfig( 0, PWM_ENABLE );
	pwmConfig( PWM7, PWM_ENABLE_OUTPUT );
	bool_t grabarTest,  testCon, Tflag;
	bool_t tomarCheckpoint;
	bool_t correrVersiones;

	bool_t version1pass, version2pass, version3pass, version4pass;

	/* Delay inicial */
	delay(100);
	/* ------------- REPETIR POR SIEMPRE ------------- */
	while(1)
	{

		//tecla1 toma la distancia aceptada
		grabarTest = !gpioRead(TEC1);

		if(grabarTest){
			acceptedDistance = ultrasonicSensorGetDistance(ULTRASONIC_SENSOR_0, MM);

			debugPrintString("Test grabado ");
			debugPrintInt(acceptedDistance);
			debugPrintString("mm");
			debugPrintEnter();
			debugPrintEnter();

		}
		//tecla2 solicita el checkpoint para comparar
		tomarCheckpoint = !gpioRead(TEC2);

		if(tomarCheckpoint){
			ultrasonicGetCheckpoint(ULTRASONIC_SENSOR_0, &Checkpoint);
			debugPrintString("EL Checkpoint es  ");
			format( Checkpoint, buffout, 0 );
			debugPrintString( buffout );
			debugPrintEnter();


		}
		//tecla3 corre las iteraciones
		correrVersiones = !gpioRead(TEC3);

		testCon = !gpioRead(TEC4);

		if(testCon){
			float capt;
			testDistance =	distanceMM(Checkpoint);
			another = testDistance;
			Tflag = evalTest(another, testDistance);


			if(Tflag){

				debugPrintString("funciona");
				debugPrintEnter();

			}else  {

				debugPrintString("funcionaPara diferentes");
				debugPrintEnter();

			}

			debugPrintInt(testDistance);
			debugPrintString("mm");
			debugPrintEnter();

		}

		if(correrVersiones){

			//llamar a version1 con Checkpoint
			testDistance =	distanceMM(Checkpoint);

			//correr el test de aceptacion
			version1pass = evalTest(acceptedDistance, testDistance);


			if(version1pass){
				/*salida correcta*/

				debugPrintString("Salida correcta de medida ");
				debugPrintInt(testDistance);
				debugPrintString("mm ");
				debugPrintString("contra el parametro");
				debugPrintInt(acceptedDistance);
				debugPrintString("mm ");
				debugPrintEnter();

			}
			else{
				//llamar a version2 con Checkpoint
				testDistance =	distanceMMexc(Checkpoint);
				//correr el test de aceptacion
				version2pass = evalTest(acceptedDistance, testDistance);



				if(version2pass){

					/*salida correcta*/
					debugPrintString("Salida correcta de medida ");
					debugPrintInt(testDistance);
					debugPrintString("mm ");
					debugPrintString("contra el parametro");
					debugPrintInt(acceptedDistance);
					debugPrintString("mm ");
					debugPrintEnter();


				}else{

					//llamar a version3 con Checkpoint
					testDistance =	distanceMMxdef(Checkpoint);
					//correr el test de aceptacion
					version3pass = evalTest(acceptedDistance, testDistance);


					if(version3pass){

						/*salida correcta*/
						debugPrintString("Salida correcta de medida ");
						debugPrintInt(testDistance);
						debugPrintString("mm ");
						debugPrintString("contra el parametro");
						debugPrintInt(acceptedDistance);
						debugPrintString("mm ");
						debugPrintEnter();


					}else{
						//llamar a version4 con Checkpoint
						testDistance =	distanceCM(Checkpoint);
						//correr el test de aceptacion
						version4pass = evalTest(acceptedDistance, testDistance);



						if(version4pass){

							/*salida correcta*/
							debugPrintString("Salida correcta de medida ");
							debugPrintInt(testDistance);
							debugPrintString("mm ");
							debugPrintString("contra el parametro");
							debugPrintInt(acceptedDistance);
							debugPrintString("mm ");
							debugPrintEnter();

						}else{

							/*FALLA TOTAL DE LA MEDICION*/
							debugPrintString("FALLA TOTAL DE LA MEDICION VS. PARAMETRO ");
						}






					}




				}




			}




		}

		delay(50);



	}

	/* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
	  por ningun S.O. */
	return 0 ;
}



static void format( float valor, char *dst, uint8_t pos ){
	uint16_t val;
	val = 10 * valor;
	val = val % 1000;
	dst[pos] = (val / 100) + '0';
	pos++;
	dst[pos] = (val % 100) / 10 + '0';
	pos++;
	dst[pos] = '.';
	pos++;
	dst[pos] = (val % 10)  + '0';
	pos++;
	dst[pos] = '\0';
}



/*==================[end of file]============================================*/
