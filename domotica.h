#ifndef domotica_h
#define domotica_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <NRF905.h>


#define BUF_LEN          32
#define CONF_LEN         10
#define NRF905_CSN       10
#define MSG_LEN			 28


#define	MASTER  1
#define	LEDSTRIP 2
#define	THERMO  3
#define	LIGHTS  4


//int addresses[4][4];

class Domotica {
	public:
		Domotica();
		void init(int);
		char** split(char*);
		bool checkNewMsg(void);
		char* getMsg(void);
		void setDebug(bool);
		void sendToNode(int, char*);
		void sendToNode(char*, char*buffer);
		
				
	private:
		int node;
		//char tx_buf[BUF_LEN];
		char rx_buf[BUF_LEN];
		char read_config_buf[CONF_LEN];
		char buffer[BUF_LEN];
		NRF905 transmitter;
		//char addresses[4][4];
		bool debug;
	

		
	
};

#endif