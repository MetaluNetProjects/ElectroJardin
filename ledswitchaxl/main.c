/*********************************************************************
 *               switch example for Versa1.0
 *	Switch capture on connectors K1, K2, K3 and K4. 
 *********************************************************************/

#define BOARD Versa1

#include <fruit.h>
#include <switch.h>
#include <analog.h>
#include <ADXL345.h>
#include <i2c_master.h>

ADXL345 adxl1;
ADXL345 adxl2;

t_delay mainDelay;

void setup(void) {	
//----------- Setup ----------------
	fruitInit();
			
	pinModeDigitalOut(LED); 	// set the LED pin mode to digital out
	digitalClear(LED);		// clear the LED
	delayStart(mainDelay, 5000); 	// init the mainDelay to 5 ms

//----------- Switch setup ----------------
	switchInit();		// init switch module
	switchSelect(0,SWITCH1);
	switchSelect(1,SWITCH2);
	switchSelect(2,SWITCH3);
	switchSelect(3,SWITCH4);
	switchSelect(4,SWITCH5);
	switchSelect(5,SWITCH6);

	INTCON2bits.RBPU = 0;
//----------- Switch setup ----------------
	pinModeDigitalOut(LED1);
	pinModeDigitalOut(LED2);
	pinModeDigitalOut(LED3);
	pinModeDigitalOut(LED4);
	pinModeDigitalOut(LED5);
	pinModeDigitalOut(LED6);
//----------- Analog setup ----------------
	analogInit();		// init analog module
	analogSelect(0,POT1);	// assign connector K1 to analog channel 0
	analogSelect(1,POT2);

//----------- setup I2C master ----------------
	i2cm_init(I2C_MASTER, I2C_SLEW_ON, FOSC/400000/4-1);
	
//----------- setup ADXL345 ----------------
	ADXL345Init(&adxl1, 0); // 1st ADXL345's SDO pin is high voltage level
	ADXL345Init(&adxl2, 1); // 2nd ADXL345's SDO pin is low voltage level
}

byte cycle = 0;

void loop() {
// ---------- Main loop ------------
	fraiseService();	// listen to Fraise events
	switchService();	// switch management routine
	fraiseService();	// listen to Fraise events
	analogService();	// switch management routine
	fraiseService();	// listen to Fraise events
	ADXL345Service(&adxl1);
	fraiseService();	// listen to Fraise events
	ADXL345Service(&adxl2);
	fraiseService();	// listen to Fraise events

	if(delayFinished(mainDelay)) // when mainDelay triggers :
	{
		delayStart(mainDelay, 5000); 	// reinit the mainDelay to 5 ms
		if(cycle == 0) { if(!switchSend()) analogSend(); } // send switch/analog channels that changed
		else if(cycle == 1) ADXL345Send(&adxl1, 1);
		else if(cycle == 2) ADXL345Send(&adxl2, 2);
		
		cycle++;
		if(cycle == 3) cycle = 0;
	}
}

// Receiving

void fraiseReceiveChar() // receive textfraiseReceive
{
	unsigned char c;
	
	c=fraiseGetChar();
	if(c=='L'){		//switch LED on/off 
		c=fraiseGetChar();
		digitalWrite(LED, c!='0');		
	}
	else if(c=='E') { 	// echo text (send it back to host)
		printf("C");
		c = fraiseGetLen(); 			// get length of current packet
		while(c--) printf("%c",fraiseGetChar());// send each received byte
		putchar('\n');				// end of line
	}	
}

void fraiseReceive() // receive raw
{
	unsigned char c;
	
	c=fraiseGetChar();
	if(c==10) {
		if(fraiseGetLen()>1) {
			c = fraiseGetChar();
			digitalWrite(LED1, (c&1) != 0);
			digitalWrite(LED2, (c&2) != 0);
			digitalWrite(LED3, (c&4) != 0);
			digitalWrite(LED4, (c&8) != 0);
			digitalWrite(LED5, (c&16) != 0);
			digitalWrite(LED6, (c&32) != 0);
		}
			
	}	

}
