#include <domotica.h>

char addresses[4][4] = { {0x00,0x00,0x00,0x01},
							{0x00,0x00,0x00,0x02},
							{0x00,0x00,0x00,0x03},
							{0x00,0x00,0x00,0x04}};
							
char nrf905tx_buffer[BUF_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char nrf905rx_buffer[BUF_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
Domotica::Domotica() {
	
}

void Domotica::init(int type){	
	
	Serial.println(" ______________________________________________________");
	Serial.println("|                                                      |");
	Serial.println("| Home automation controller v0.2 (c) 2014 vermeers.ch |");
	Serial.println("|______________________________________________________|");
	Serial.println("");
	node = type;
	if(node == 2)
		transmitter = NRF905(5,4,2,10,A0,8,7);
	else
		transmitter = NRF905();
	//pinMode(NRF905_CSN,OUTPUT);
	transmitter.init();
	transmitter.write_config_address(addresses[node]);
	transmitter.read_config(read_config_buf);
	//if debug is on echo the config register
	if(debug) {
		Serial.print("[DEBUG] Config register: ");
    	for(int i=0; i<10; i++) {
        	Serial.print(read_config_buf[i],HEX);
        	Serial.print(' ');
        }
    	Serial.print("\r\n");
    	if(node == 0)
    		Serial.println("[INFO] Now accepting commands from UART");
    }
    transmitter.set_rx();
	
	
}

char** Domotica::split(char* cmd) {
	char** result;
	int length = sizeof(cmd);
	for(int i = 10; i<10; i++) {
		result[i] = (char*)malloc(11);
	}
	int arraycounter = 0;
	int pos = 0;
	for(int i = 0; i < length; i++) {
		if(cmd[i] != ';') {
			result[arraycounter][pos] = cmd[i];
		}
		else {
			pos = 0;
			arraycounter++;
		}
	} 
	return result;

}

void Domotica::setDebug(bool flag) {
	if(flag) {
		Serial.println("[INFO] Debugging enabled");
		debug = true;
	}
	else {
		Serial.println("[INFO] Debugging disabled");
		debug = false;
	}
}

bool Domotica::sendToNode(char* address, char*buffer) {
	 for(int i = 0; i<4; i++)
 	nrf905tx_buffer[i] = addresses[node][i]; //send the source address
 for(int i = 0; i<MSG_LEN; i++)
 	nrf905tx_buffer[i+4] = buffer[i];	
 if(debug) {
 Serial.print("[DEBUG] DESTINATION: ");
 	for(int i = 0; i<4; i++) {
 		Serial.print(address[i],HEX);
 		Serial.print(' ');
 	}
 	Serial.print("\n\r");
 	Serial.print("[DEBUG] Packet to be sent: ");
 	for(int i = 0; i<32; i++) {
 		Serial.print(nrf905tx_buffer[i],HEX);
 		Serial.print(' ');
 	}
 	Serial.print("\n\r");
 }
 int retries = 10;
 transmitter.TX(buffer, address);
 if(debug) Serial.println("[DEBUG] Packet transmitted");
 do {
  	transmitter.set_rx();
 	retries--;
 	delay(100);
 } while(retries > 0 && !transmitter.check_ready());
 if(!checkNewMsg()) { //timeout reached 
 	if(debug) Serial.println("[WARN] package timed out");
 	return false;
 } else {
 	transmitter.RX(nrf905rx_buffer);
 	transmitter.set_rx();
 	return true;
 }
}

bool Domotica::sendToNode(int tx_node, char* buffer) {
	sendToNode(addresses[tx_node], buffer);
 //return nrf905rx_buffer;
	
}

bool Domotica::checkNewMsg(void) {
	
	char check;
	check = transmitter.check_ready();
	if(check == 0) {
		if(debug) {
			//Serial.println("[DEBUG] no new message ready: ");
			//Serial.println(check);
		}
		return false;
	}
	else {
	 	if(debug) {
			//Serial.println("[DEBUG] new message");
			//Serial.print(check);
		}
		return true;	
	} 
	
}

char* Domotica::getMsg(void) {
	char nrf905tx_buffer[MSG_LEN] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	char ack_addr[4];
	if(checkNewMsg()) {
		transmitter.RX(nrf905rx_buffer);
		//ACK the packet
		Serial.print("ACK ADDRESS: ");
		for(int i = 0; i<4; i++) {
			ack_addr[i] = nrf905rx_buffer[i];
		ack_addr[0] = 0x00;
		ack_addr[1] = 0x00;
		ack_addr[2] = 0x00;
		ack_addr[3] = 0x01;
		Serial.print(ack_addr[i], HEX);
		}
		Serial.print("\n\r");
		
		transmitter.TX(nrf905tx_buffer, ack_addr); //first 4 bytes of rx_buffer is the source address
		transmitter.set_rx();
	}
	return nrf905rx_buffer;
}
		



