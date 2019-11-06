
//Un timer controla dos pines de salida
//Estímulo: seno -pin 11 (OC1A)- Van Vugt script adaptation. pin 12: OC1B 
//Feedback: un timer controla dos pines:  seno -pin 5 (OC3A) y pin2 (OC3B) - Van Vugt script adaptation. pin 3: OC3C, pin 2: OC3B 
//trial eterno (sin Matlab)
//Manda estímulo al pin 11 y al pin 12 (timer 1)
//y feedback al pin 5 y al pin 2 (timer 3)
#include <SineTones_unq.h>


#define INPUTPIN A9
unsigned long int t=0;
int  i;



void setup() {

  Serial.begin(9600); //USB communication with computer
  pinMode(INPUTPIN,INPUT);
  
  cli();
  
  initAllToneTimers();
  setSelfTimming(false);
 
  setAllow(false);

  sei();
}



void loop() {

	if(getAllow() == false){
    //just in case, clear incoming buffer once read
    Serial.flush();
		get_parameters();
		
		setAllow(true);

    allocsAndInit();
	}

	else{
    t = millis();
    set_t(t);
    //turn on noise
    //SoundSwitch('n',1,true,true);
    
		//send stimulus
    stimFunc();


		//read response
    respFunc();

		//end trial
		//allow one more period (without stimulus)
    estoFunc();
		

	}
  
}
