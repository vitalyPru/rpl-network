#ifndef __TRICKLE_H_
#define __TRICKLE_H_


typedef	unsigned int TRICKLE_COUNTER;
typedef	unsigned char TRICKLE_FACTOR;


extern TRICKLE_COUNTER		TrickleRedundancy;
extern TRICKLE_FACTOR		TrickleMaxFactor;



//void TrickleInit(void);
void TrickleStart( BOOL disMode);
void TrickleStop(void);
void TrickleTask( void );
void TrickleIncCounter();

extern void TrickleCB_Send(BOOL disMode);

#endif

