#include "SystemProfile.h"


#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "WirelessProtocols/SymbolTime.h"
#include "Trickle.h"

MIWI_TICK 			TrickleStartTime;
TRICKLE_FACTOR		TrickleFactor;

MIWI_TICK			DISTimer;
MIWI_TICK 			TrickleInterval;
MIWI_TICK 			TricklePoint;
TRICKLE_COUNTER 	TrickleCounter;

MIWI_TICK 			TrickleMinInterval;
TRICKLE_COUNTER		TrickleRedundancy;
TRICKLE_FACTOR		TrickleMaxFactor;

union {
	BYTE Val;
	struct {
		BYTE	Running:1 ; 			// Trickle Timer is activated
		BYTE	PointPassed:1;		// Trickle timer point already passed
		BYTE	DISmode:1;
	} bits;
} 	TrickleStatus;

void TrickleStartInterval();


void TrickleTask( void ) {

  MIWI_TICK t;
  if ( TrickleStatus.bits.Running ) { 
    t = MiWi_TickGet();
	if ( t.Val >= TricklePoint.Val ) {
		if ( TrickleCounter < TrickleRedundancy ) {
			if ( TrickleStatus.bits.PointPassed == 0 ) {
				TrickleCB_Send( TrickleStatus.bits.DISmode  );
				TrickleStatus.bits.PointPassed = 1;
			}
		}	
	}

	if ( t.Val >= TrickleInterval.Val ) {
		if ( TrickleStatus.bits.DISmode ) {
			TrickleIncCounter();
		} else  {
			if ( TrickleFactor <= TrickleMaxFactor ) {
				TrickleFactor++;
			}
		}
		TrickleStartInterval();
	} 
  }
}


void TrickleStart(  BOOL disMode ) {

	TrickleStatus.Val = 0;
	if ( disMode ) {
		TrickleMinInterval.Val = DIS_TIMER_TIME;
		TrickleRedundancy   = DIS_TIMER_REPEAT;
	    TrickleStatus.bits.DISmode = TRUE;
		
	} else {	
		TrickleMinInterval.Val = TRICKLE_MIN_INTERVAL;
		TrickleMaxFactor   = TRICKLE_MAX_FACTOR;
		TrickleRedundancy   = TRICKLE_REDUNDANCY_CONST;
	}

	TrickleStatus.bits.Running = 1;
	TrickleFactor   = 0;	
	TrickleStartInterval();
}

void TrickleStop() {
	TrickleStatus.bits.Running = 0;
}

void TrickleStartInterval(  ) {
	TrickleStatus.bits.PointPassed = 0;
	TrickleStartTime = MiWi_TickGet();
	TrickleInterval.Val =(TrickleMinInterval.Val << TrickleFactor) ;
	if ( !TrickleStatus.bits.DISmode ) TrickleCounter = 0;
	TricklePoint.Val = rand(); 
	TricklePoint.Val /= 2; 
    TricklePoint.Val *=  TrickleInterval.Val;
	TricklePoint.Val /= RAND_MAX;
	TricklePoint.Val += (TrickleStartTime.Val + TrickleInterval.Val/2);
 	TrickleInterval.Val += TrickleStartTime.Val;
}

void TrickleIncCounter() {
	TrickleCounter++;
}
