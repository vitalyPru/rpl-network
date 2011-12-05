#include "SystemProfile.h"


#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "OFunction.h"

typedef	unsigned char NODELIST_SIZE_TYPE;


typedef struct {
	BOOL 		Valid;
	BYTE 		Address[8];
	WORD		PathCost;
	WORD		Rank;
} NODE_RECORD;

NODE_RECORD		NeighborList[NEIGHBOR_LIST_SIZE];
WORD			CurrentPathCost;

#define 	LQL_THR1		(0x80)
#define 	LQL_THR2		(0x40)
#define 	LQL_THR3		(0x20)

#define     BEST_NODE 					0
#define 	NODE_NOT_FOUND				(0xFF)
#define 	MAX_PATH_COST				(0xFFFF)
#define     MIN_PATH_COST				(0x00)
#define 	PARENT_SWITCH_THRESHOLD		(2)

void OFunctionInit( BOOL root ) {
	BYTE i;
	if ( root ) {
		CurrentPathCost = MIN_PATH_COST;
	} else {
		CurrentPathCost = MAX_PATH_COST;
	}
	for ( i = 0; i<NEIGHBOR_LIST_SIZE; i++ ) NeighborList[i].Valid = FALSE;

}

WORD PathCostCalc( MAC_RECEIVED_PACKET MACRxPacket ) {
	WORD LQL = 0;
	if (  MACRxPacket.RSSIValue > LQL_THR1 ) LQL=3;
	else if ( MACRxPacket.RSSIValue > LQL_THR2 ) LQL = 2;
    else if ( MACRxPacket.RSSIValue > LQL_THR3 ) LQL = 1;
	else LQL = 0;
 	return  (LQL ) ? LQL + *((WORD *)( MACRxPacket.Payload + DIO_METRIC_POS ))
			: MAX_PATH_COST;
}

void UpdateObjFun(	BYTE *sourceAddress, // NodeAddress
 				WORD  pathCost, WORD rank) 
{
	
	NODELIST_SIZE_TYPE 	i,j;
	BOOL 				trigParentSel = FALSE;
	NODELIST_SIZE_TYPE 	found = NODE_NOT_FOUND;

// Look if we already have record for this node
	for ( i = 0; i<NEIGHBOR_LIST_SIZE; i++ ) {
		if ( NeighborList[i].Valid && isSameAddress( NeighborList[i].Address, sourceAddress )) {
			found = i;
			if ( pathCost != NeighborList[i].PathCost ) trigParentSel = TRUE;
			break;
		}
	} 

	if ( trigParentSel ) {

      if ( found != NODE_NOT_FOUND ) {
		// temporary remove the node
		for ( j=found; j<NEIGHBOR_LIST_SIZE-1; j++) {
				NeighborList[j] = NeighborList[j+1];
		}
		NeighborList[NEIGHBOR_LIST_SIZE-1].Valid = FALSE;
	  }
 
	  for ( i = 0; i<NEIGHBOR_LIST_SIZE; i++ ) {
		if ( NeighborList[i].Valid ) {
			if ( pathCost < NeighborList[i].PathCost) {
				//	Insert new node
				for( j = NEIGHBOR_LIST_SIZE-1; j > i ; j--  ) {
					NeighborList[j] = NeighborList[j-1];					
				}
				//	Add new node
				NeighborList[i].Valid = TRUE;
				NeighborList[i].PathCost = pathCost;						
				NeighborList[i].Rank = rank;						
				memcpy( NeighborList[i].Address, sourceAddress, 8);
				if ( found == NODE_NOT_FOUND ) trigParentSel = TRUE;
				break;
			}
		} else {
			//	Add new node
			NeighborList[i].Valid = TRUE;
			NeighborList[i].PathCost = pathCost;
			NeighborList[i].Rank = rank;												
			memcpy( NeighborList[i].Address, sourceAddress, 8);
			if ( found == NODE_NOT_FOUND ) trigParentSel = TRUE;
			break;
		}
	  }

	  /// 
	  BYTE *bestPath = NeighborList[BEST_NODE].Address;
	  WORD bestPathCost = (( NeighborList[BEST_NODE].PathCost == MAX_PATH_COST) ||
            ( ! NeighborList[BEST_NODE].Valid )) ? MAX_PATH_COST  :  NeighborList[BEST_NODE].PathCost  ;

	  if (  !((CurrentPathCost > bestPathCost) &&  
			   ((CurrentPathCost - bestPathCost) < PARENT_SWITCH_THRESHOLD)) ||
			  (bestPathCost == MAX_PATH_COST)) {
					CurrentPathCost = bestPathCost;
					OFCbk_SetNewParent(bestPath);
	  }
	}
}



WORD CalcRank( void  ) {
	return ( CurrentPathCost > (MAX_PATH_COST - 0x100) ) ? UNDEFINED_RANK :  CurrentPathCost + 0x100; 

}

WORD getMyMetric( void ) {
		return CurrentPathCost;
}

