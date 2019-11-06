/*************************************
 * Universidad Nacional de Quilmes	 *
 *       SineTones library           *
 * 									 *
 * 									 *
 * 									 *
 * 									 *
 * 									 *
 * Nombre: SineTones_unq.h			 *
 *************************************/ 
//TODO: AVERIGUAR SOBRE LICENCIAS DE ESTAS COSAS
#ifndef _SINETONES_UNQ_H

	#define _SINETONES_UNQ_H

//------------------------------------------------------------------------------
//----------------- -----Prototipos de funciones Publicas ----------------------
//------------------------------------------------------------------------------

	//Setters Getters--------------
	void 			setSoundFreq(unsigned int , char);
	unsigned int 	getSoundFreq(char);

	void 			setSoundDur(unsigned int , char);
	unsigned int 	getSoundDur(char);

	void setSelfTimming(boolean);
	boolean getSelfTimming(void);
	
	void setAllow(boolean);
	boolean getAllow(void);

	void set_t(unsigned long int);
	//end Setters Getters----------




//Tone Specific -------------------
	void initAllToneTimers(void);
	void SoundSwitch(char, boolean, boolean);
//Posible deprecated
	void tickOn(char, boolean, boolean);
	void tickOff(char);
//end Tone Specific ---------------


//Paula's utils -------------------
	int readVirtualGround(void);
	void serial_print_string(char);
	void parse_data(char);
	void get_parameters();
	void save_data(char, unsigned int, unsigned long);
//end Paula's utils ---------------

//Esto es para hacerlo funcional, pero la verdad hay que charlarlo
	void allocsAndInit(void);
	void stimFunc(void);
	void respFunc(void);
	void estoFunc(void);


//------------------------------------------------------------------------------
//-----------------------Prototipos de funciones Privadas ----------------------
//------------------------------------------------------------------------------

	uint16_t _setFrequency(uint16_t);
	unsigned int _generateNoise(int, int);
	void _checkStims(void);




#endif

/*------------------------------------------------------------*-
---- END OF FILE --------------------------------------------
-*------------------------------------------------------------*/
