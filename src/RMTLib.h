/*
 * Protocols:
 *  - Pulse Distance Modulation (NEC)
 *  - Manchester Encoding (RC5)
 *
 */
 
#ifndef	RMTLib_H
#define	RMTLib_H

#define SEND_NEC		1
#define SEND_SAMSUNG	1
#define SEND_RC5		1

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

class RMTLib {
	public:
		RMTLib();
		
		gpio_num_t tx_pin;
		
		void begin();
		void send();
		void setTxPin(unsigned short pin);
		
#if SEND_NEC
		//void initNEC(unsigned short pin);
		void sendNEC(unsigned long data);
#endif
		//void decodeNEC();

#if SEND_SAMSUNG		
		//void initSAMSUNG(unsigned short pin);
		void sendSAMSUNG(unsigned long data);
#endif
		//void decodeSAMSUMG();
	
#if SEND_RC5
		//void initRC5(unsigned short pin);
		void sendRC5(unsigned long data);
#endif
		//void decodeRC5();
		
	private:
		//

};

#endif