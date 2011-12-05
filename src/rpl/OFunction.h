#ifndef __OFUNCTION_H_
#define __OFUNCTION_H_



void OFunctionInit( BOOL root );
void UpdateObjFun(	BYTE *sourceAddress, // NodeAddress
 				WORD  metric, WORD rank) ;

WORD PathCostCalc( MAC_RECEIVED_PACKET MACRxPacket );
WORD CalcRank( void );
void OFCbk_SetNewParent( BYTE *);
WORD getMyMetric( void );


#endif
