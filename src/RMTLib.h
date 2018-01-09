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
		
		void begin();
		void send();
		
#if SEND_NEC
		void sendNEC(unsigned long data);
#endif
		//void decodeNEC();

#if SEND_SAMSUNG		
		void sendSAMSUNG(unsigned long data);
#endif
		//void decodeSAMSUMG();
	
#if SEND_RC5
		void sendRC5(unsigned long data);
#endif
		//void decodeRC5();
		
	private:
		//

};

#endif