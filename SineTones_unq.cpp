/*************************************
 * Universidad Nacional de Quilmes	 *
 *			 SineTones library		 *
 * 									 *
 * 									 *
 * 									 *
 * 									 *
 * 									 *
 * Nombre: SineTones_unq.h			 *
 *************************************/ 

 
 
 
#include <Arduino.h>
#include "SineTones_unq.h"
 
//-------------------------------set defines------------------------------------
// PWM output 
#define STIMPINR 	11 //OC1A
#define STIMPINL 	12 //OC1B
#define FDBKPINR 	5  //OC3A
#define FDBKPINL 	2  //OC3B
#define NOISEPINR 	10 //OC2A
#define NOISEPINL 	9  //OC2B
#define VGPIN 		A11
// DDS resolution
#define FREQ_SAMP	31180; // 16MHz/513 porque ahora el PWM está en modo 9 bits
#define RESOLUTION	(unsigned long) pow(2,32)/FREQ_SAMP;

#define LFSR_INIT	0xfeedfaceUL
#define LFSR_MASK	((unsigned long)( 	1UL<<31 | 1UL <<15 | 1UL <<2 | 1UL <<1))

//-------------------------------set variables----------------------------------


unsigned int stimFreq = 440;
unsigned int fdbkFreq = 660;

unsigned int stimDuration = 50;
unsigned int fdbkDuration = 50;
/* defines the frequency / duration (_i.e., pitch) of the 
tone (in Hz) and dutarion these vars  can be setted by 
public setter and accessed by getter*/
long int prevStim_t=0,prevFdbk_t=0;



boolean selfTimming =true;
/* condition for check millis in timer2 routine, 
 has a setter and getter
*/



// 16 bit delta
uint16_t phaseIncrementStim = 0;
uint16_t phaseIncrementFdbk = 0;
//Bools of channels on
#define PINVG A9
boolean stimRight = true ,			stimLeft	= true;
//pin11 -timer1, chnl A: right && pin12 -timer1, chnl B: left

boolean fdbkRight = true, fdbkLeft	= true, noise 		= false;
//pin5 -timer3, chnl A: right && pin2 -timer3, channel B: left

unsigned long int _t=0;
long int prevStim=0,prevFdbk=0;



boolean fdbk_flag,stim_flag;

int vg 			= 0;
int read_vg 	= 0;
int amplitude 	= 1;




//VER EL PARSE DATA
#define ANTIBOUNCE (0.5*isi)//minimum interval between responses (milliseconds)
boolean allow;
int fdbk, _i;
unsigned int stim_number,fdbk_number;
unsigned int *event_number;
char *event_name;
unsigned long *event_time;
unsigned int event_counter; 

unsigned int isi=300,n_stim=3; //Entiendo que son valores iniciales por las dudas 
int perturb_size=0;  // estos los agrego paraque no tire error
unsigned int perturb_bip=0;
unsigned int event_type=0;
#define INPUTPIN A9
char message[20];
boolean SR=false, SL=false, FR=false, FL=false;







//------------------------------------------------------------------------------

//-----------------Lookup table for waveform generation-------------------------
static const uint8_t	sineTable[] PROGMEM ={
0x80,0x83,0x86,0x89,0x8c,0x8f,0x92,0x95,0x98,0x9c,0x9f,0xa2,0xa5,0xa8,0xab,0xae,
0xb0,0xb3,0xb6,0xb9,0xbc,0xbf,0xc1,0xc4,0xc7,0xc9,0xcc,0xce,0xd1,0xd3,0xd5,0xd8,
0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xed,0xef,0xf0,0xf2,0xf3,0xf5,
0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfc,0xfd,0xfe,0xfe,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfe,0xfd,0xfc,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,
0xf6,0xf5,0xf3,0xf2,0xf0,0xef,0xed,0xec,0xea,0xe8,0xe6,0xe4,0xe2,0xe0,0xde,0xdc,
0xda,0xd8,0xd5,0xd3,0xd1,0xce,0xcc,0xc9,0xc7,0xc4,0xc1,0xbf,0xbc,0xb9,0xb6,0xb3,
0xb0,0xae,0xab,0xa8,0xa5,0xa2,0x9f,0x9c,0x98,0x95,0x92,0x8f,0x8c,0x89,0x86,0x83,
0x80,0x7c,0x79,0x76,0x73,0x70,0x6d,0x6a,0x67,0x63,0x60,0x5d,0x5a,0x57,0x54,0x51,
0x4f,0x4c,0x49,0x46,0x43,0x40,0x3e,0x3b,0x38,0x36,0x33,0x31,0x2e,0x2c,0x2a,0x27,
0x25,0x23,0x21,0x1f,0x1d,0x1b,0x19,0x17,0x15,0x13,0x12,0x10,0x0f,0x0d,0x0c,0x0a,
0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x03,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,
0x09,0x0a,0x0c,0x0d,0x0f,0x10,0x12,0x13,0x15,0x17,0x19,0x1b,0x1d,0x1f,0x21,0x23,
0x25,0x27,0x2a,0x2c,0x2e,0x31,0x33,0x36,0x38,0x3b,0x3e,0x40,0x43,0x46,0x49,0x4c,
0x4f,0x51,0x54,0x57,0x5a,0x5d,0x60,0x63,0x67,0x6a,0x6d,0x70,0x73,0x76,0x79,0x7c
};
//------------------------------------------------------------------------------

//----------------------------- SETTERS GETTERS defines-------------------------

//SOUND FREQUENCIES
/* This function sets the frequency for stimulus
or feedback, to newFreq 
if tipo is 's', sets stimulus frequency
if tipo is 'f', sets feedback frequency*/
void setSoundFreq(unsigned int newFreq, char tipo){
	if (tipo == 's')
		stimFreq = newFreq;
	if (tipo == 'f')
		fdbkFreq = newFreq;
}

unsigned int getSoundFreq(char tipo){
	if (tipo == 's')
		return stimFreq;
	if (tipo == 'f')
		return fdbkFreq;
}

//SOUND DURATIONS
/* This function sets the duration for stimulus
or feedback, to newDur 
if tipo is 's', sets stimulus duration
if tipo is 'f', sets feedback duration*/
void setSoundDur(unsigned int newDur, char tipo){
	if (tipo == 's')
		stimDuration = newDur;
	if (tipo == 'f')
		fdbkDuration = newDur;
}

unsigned int getSoundDur(char tipo){
	if (tipo == 's')
		return stimDuration;
	if (tipo == 'f')
		return fdbkDuration;
}


void setSelfTimming(boolean newVal){
	selfTimming = newVal;
}

boolean getSelfTimming(void){
	return selfTimming;
}

boolean getFlag(char tipo){
	if (tipo == 's')
		return stim_flag;
	if (tipo == 'f')
		return fdbk_flag;
}

void setAllow(boolean b){
	allow = b;
}

boolean getAllow(void){
	return allow;
}

void set_t(unsigned long int time){
	_t = time;
}

//----------------------Timers Interrupts Routines (ISRs)-----------------------

// TIMER1 overflow freq 31.25KHz.
ISR(TIMER1_OVF_vect){
	static uint8_t index1 = 0;
	static uint16_t phaseAccum1atorStim = 0;
	uint16_t tableValue;

	// Update accumulator
	phaseAccum1atorStim += phaseIncrementStim;

	index1 		= phaseAccum1atorStim >> 8;
	tableValue 	= pgm_read_byte( &sineTable[index1] );

	if(stimRight&&stim_flag){
		OCR1A = tableValue;
	}  else  {
		OCR1A = vg;  }
	if(stimRight&&stim_flag){
		OCR1B = tableValue;
	}  else  {
		OCR1B = vg;}

}

// TIMER2 overflow at 32.5kHz.
ISR(TIMER2_OVF_vect){//pseudorand noise
	OCR2A = 127;
	if(noise==true)
		OCR2B = _generateNoise(amplitude,vg);
	else
		OCR2B = vg;
	_checkStims(); //check the stimdurations and timming
	//adding comments bellow in it's definition...
}


// TIMER3 overflow 62.5KHz.
ISR(TIMER3_OVF_vect){
	static uint8_t index3 = 0;
	static uint16_t phaseAccumulatorFdbk = 0;
	uint16_t tableValue;

	// Update accumulator
	phaseAccumulatorFdbk += phaseIncrementFdbk;
	
	index3 		= phaseAccumulatorFdbk >> 8;
	tableValue 	= pgm_read_byte( &sineTable[index3] );

	if(fdbkRight && fdbk_flag){ 
		OCR3A = tableValue;
	}  else  {
	OCR3A = vg;  }

	if(fdbkLeft && fdbk_flag){
		OCR3B = tableValue;
	}  else  {
		OCR3B = vg;  }
}

//------------------------------------------------------------------------------
//---------------------Private Functions declarations --------------------------
//------------------------------------------------------------------------------

/* Translates the desired output frequency to a phase
 increment to be used with the phase accumulator.*/
uint16_t _setFrequency( uint16_t frequency ){
uint32_t phaseIncr32 =	frequency*RESOLUTION;
	return (phaseIncr32 >> 16);
}

unsigned int _generateNoise(int amplitude, int vg_value){
	uint8_t aux=0;
	static unsigned long int lfsr = LFSR_INIT;	// See https://en.wikipedia.org/wiki/Linear_feedback_shift_register#Galois_LFSRs
	// 32 bit init, nonzero
	// If the output bit is 1, apply toggle mask. The value has 1 at bits corresponding to taps, 0 elsewhere.

	if(lfsr & 1) { 
		lfsr =	(lfsr >>1) ^ LFSR_MASK; 
		aux = vg_value+amplitude;
	}
	else{
		lfsr >>= 1;
		aux = vg_value-amplitude;
	}
	return(aux);
}


/*Esta funcion checkea  si tiene que apagar algún flag.
el tema es que creo que es subóptimo checkear con timer 2,
esto es porque este timer esta trabajando a 62kHz mientas
las interrupciones donde estos flags participan son a 31kHz
La solucion correcta sería mirar el millis en el loop del 
script principal. por eso se creó el 'selfTimming' con su 
setter y getter. si se pasa a false, esta funcion checkea
la condicion, pero no el tiempo. también se puede pensar en bajar
la frecuencia de la interrupcion de timer2. o, realizar el check
de stimulus en otro timer completamente distinto. o en el timer
en el que se va a usar el flag. Para analizar y pensar
*/
void _checkStims(void){
	if (selfTimming==true)
		_t =millis();
	if (_t-prevStim>stimDuration)
		stim_flag = false;
	if (_t-prevFdbk>stimDuration)
		fdbk_flag = false;
}
//----------------------------Private End --------------------------------------





//------------------------------------------------------------------------------
//----------------------------Public Function declarations ---------------------
//------------------------------------------------------------------------------


//initialize all timers at the same time, (1,2 and 3)
//with their respectives pins
void initAllToneTimers(void){
	// Set pwm pins as output
	pinMode(STIMPINR,	OUTPUT); //Stimulus Right
	pinMode(STIMPINL,	OUTPUT); //Stimulus Left
	pinMode(FDBKPINR,	OUTPUT); //Feedback Right
	pinMode(FDBKPINL,	OUTPUT); //Feedback Left
	pinMode(NOISEPINR, 	OUTPUT); //Noise Right
	pinMode(NOISEPINL, 	OUTPUT); //Noise left

	// TIMER1
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
	TCCR1B = _BV(CS10)	 | _BV(WGM12);
	// Fast PWM 9-bits non inverted, CTC, TOF1 on TOP (OCR1A)

	// TIMER3
	TCCR3A = _BV(COM3A1) | _BV(COM3B1) | _BV(WGM31);
	TCCR3B = _BV(CS30)	 | _BV(WGM32);
	// Fast PWM 9bits non inverted, CTC, TOF3 on TOP (OCR3A)

	// TIMER2
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
	TCCR2B = _BV(CS20) | _BV(WGM22);
	// Fast PWM 8bits non inverted, CTC, TOF2 on TOP (OCR2A)
}



// Lectura de la tierra virtual desde un pin analógico
int readVirtualGround(void){
	vg = analogRead(PINVG);
	return vg;
}



void SoundSwitch(char tipo, boolean left, boolean right){
/* This function enables the timerOverflowInterrupt in the time mask depending on tipo: 
if tipo is 's', selects Timer 1 (stimulus)
if tipo is 'f' selects Timer 3 (feedback)
if tipo is 'n' selects Timer 2 (Noise)
the left and right bools as true or false selects the channels where the signals will come out
for 's' and 'f', freq is the frequency of the beep.
for 'n', freq is the amplitud in GenerateNoise, meaning the volume of the white noise.
*/
	switch(tipo){
		case 's':
			//phaseAccumulatorStim = 0; Lo comento porque es propio 
										//de isr ahora, así que no se puede 
										//modificar desde afuera
			phaseIncrementStim = _setFrequency(stimFreq);
			stimRight = right;
			stimLeft = left;
			stim_flag =true;
			break;
		case 'f':
			//phaseAccumulatorFdbk = 0;
			phaseIncrementFdbk = _setFrequency(fdbkFreq);
			fdbkRight = right;
			fdbkLeft = left;
			fdbk_flag =true;
			break;
		case 'n':
			if(right && left ==1)
				noise = true;
			else noise = false;
			break;
	}
}




//---------------------------------------------------------------------
//print a line avoiding "carriage return" of Serial.println()
void serial_print_string(char *string) {
  Serial.print(string);
  Serial.print("\n");
}


//---------------------------------------------------------------------
//parse data from serial input
//input data format: eg "I500;N30;P-10;B15;E5;X"
/*aca hay varias variables que se tocan en el parse, para que ande ahora,
las voy a declarar dentro de la libreria, y serian 'globales' pero eso no
esta del todo bien*/

void parse_data(char *line) {
	char field[10];
	int n,data;
	//scan input until next ';' (field separator)
	while (sscanf(line,"%[^;]%n",field,&n) == 1) {
		data = atoi(field+1);
		//parse data according to field header
		switch (field[0]) {
			case 'I':
				isi = data;
				break;
			case 'N':
				n_stim = data;
				break;
			case 'P':
				perturb_size = data;
				break;
			case 'B':
				perturb_bip = data;
				break;
			case 'E':
				event_type = data;
				break;
			case 'S':
				switch (field[1]){
					case 'R':
						SR = true;
						SL = false;
						break;
					case 'L':
						SR = false;
						SL = true;
						break;
					case 'B':
						SR = true;
						SL = true;
						break;
					case 'N':
						SR = false;
						SL = false;
					break;
				}
				break;
			case 'F':
				switch (field[1]){
					case 'R':
						FR = true;
						FL = false;
						break;
					case 'L':
						FR = false;
						FL = true;
						break;
					case 'B':
						FR = true;
						FL = true;
						break;
					case 'N':
						FR = false;
						FL = false;
						break;
				}
				break;
			default:
				break;
		}
		line += n+1;
//		if (*line != ';')
//			break;
//		while (*line == ';')
//			line++;
	}
}


//---------------------------------------------------------------------

void get_parameters() {
  char line[45];
	char _i,aux='0';
	_i = 0;

	//directly read next available character from buffer
	//if flow ever gets here, then next available character should be 'I'
	aux = Serial.read();

	//read buffer until getting an X (end of message)
	while (aux != 'X') {
		//keep reading if input buffer is empty
		while (Serial.available() < 1) {}
		line[_i] = aux;
		_i++;
		aux = Serial.read();
	}
	line[_i] = '\0';					//terminate the string

	//just in case, clear incoming buffer once read
	//Serial.flush();
	//parse input chain into parameters
	parse_data(line);
}

//---------------------------------------------------------------------

/*idem con el parse*/


void save_data(char ename, unsigned int enumber, unsigned long etime){
      //store event data
      event_name[event_counter] = ename;
      event_number[event_counter] = enumber;
      event_time[event_counter] = etime;
      event_counter++;

      switch(ename){
        case 'S':
          stim_number++;
          break;

        case 'F':
          fdbk_number++;
          break;
      }    
}



void stimFunc(void){
if ((_t-prevStim_t)> isi && stim_flag==false) { //enciende el sonido
	SoundSwitch('s', SL, SR);
	prevStim_t=_t;
	stim_flag=true;
	save_data('S', stim_number, _t);
	}
}

void respFunc(void){
	if ((_t - prevFdbk_t) > ANTIBOUNCE && getFlag('f')==false) {
		fdbk = digitalRead(INPUTPIN);
		if (fdbk == HIGH){
			SoundSwitch('f', FL, FR);
			prevFdbk_t=_t;
			fdbk_flag=true;
			save_data('F', fdbk_number, _t);
		}
	}
}

void estoFunc(void){
	if (stim_number > n_stim && (_t - prevStim_t) >= isi) {
		for (_i=0; _i<event_counter; _i++) {
			sprintf(message,"%c %d %ld",event_name[_i],event_number[_i],event_time[_i]);
			serial_print_string(message);
		}
		Serial.println("E");	//send END message
		allow = false;

		//turn off noise 
		SoundSwitch('s',false,false);
		SoundSwitch('f',false,false);
		SoundSwitch('n',false,false);

		free(event_name);
		free(event_number);
		free(event_time);	
	}
}


void allocsAndInit(void){
	stim_number = 1;
	fdbk_number = 1;
	event_counter = 0;

	event_name = (char*) calloc(3*n_stim,sizeof(char));
	event_number = (unsigned int*) calloc(3*n_stim,sizeof(unsigned int));
	event_time = (unsigned long*) calloc(3*n_stim,sizeof(unsigned long));

}















// function to turn tick on
void tickOn(char tipo,uint16_t freq, boolean left, boolean right){
//PROBABLY DEPRECATED

/* This function enables the timerOverflowInterrupt in the time Mask 
Depending on tipo, 
if tipo is 's', selects Timer1 (stimulus)
if tipo is 'f', selects Timer3 (feedback)
if tipo is 'n', selects Timer2 (Noise)
The left and right bools as true or false selects 
the channels where the signals will come out*/
	switch(tipo){
		case 's':
//			phaseAccumulatorStim = 0;
			phaseIncrementStim = _setFrequency(freq);
			stimRight = right;
			stimLeft = left;
			break;
		case 'f':
//			phaseAccumulatorFdbk = 0;
			phaseIncrementFdbk = _setFrequency(freq);
			fdbkRight = right;
			fdbkLeft = left;
			break;
		case 'n':
			if(right && left ==1)
				noise = true;
			else noise = false;
			break;
	}
}

// function to turn tick off
void tickOff(char tipo) {
/* Disable the timerOverflowInterrupt based on 'tipo'*/
// THIS MIGHT BE DEPRECATED!
		if (tipo=='s'){//Stimulus 
		TIMSK1 &= ~_BV(TOIE1);
		OCR1A	 = 128;} //dejo el comp en la mitad, para tener 2.5v sirve?
	if (tipo=='f'){
		TIMSK3 &= ~_BV(TOIE3);
		OCR3A	 = 128;} //dejo el comp en la mitad, para tener 2.5v sirve?
	if (tipo=='n'){
		TIMSK2 &= ~_BV(TOIE2);
		OCR2A	 = 128;} //dejo el comp en la mitad, para tener 2.5v sirve?
}

//------------------------------------------------------------------------------

/*******************************************************************************
* This is the end of library, made in Universidad nacional de quilmes					*
* Version = 0.0.1 may 25th of 2018												 *
*******************************************************************************/