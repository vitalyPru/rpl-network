#include "ConfigApp.h"

#if defined(PROTOCOL_LOWPAN)

//#include "WirelessProtocols/P2P/P2P.h"
#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "WirelessProtocols/Console.h"
//#include "WirelessProtocols/NVM.h"
#include "WirelessProtocols/SymbolTime.h"
#include "Transceivers/MCHP_MAC.h"
#include "Transceivers/Transceivers.h"
#include "WirelessProtocols/MCHP_API.h"
//#include "WirelessProtocols/EEPROM.h"
#include "stdlib.h"
#include "Trickle.h"
#include "OFunction.h"

/*********************************************************************
 * Defines
 ********************************************************************/

/* Commands  */
#define		DIO_COMMAND		(0x40)
#define		DIS_COMMAND		(0x41)
#define		DAO_COMMAND		(0x42)
#define		DAO_ACK_COMMAND	(0x43)





/*********************************************************************
 * Variable definition
 ********************************************************************/


#if MY_ADDRESS_LENGTH == 8
    BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5,EUI_6,EUI_7};
#elif MY_ADDRESS_LENGTH == 7
    BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5,EUI_6};
#elif MY_ADDRESS_LENGTH == 6
    BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5};
#elif MY_ADDRESS_LENGTH == 5
    BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4};        
#elif MY_ADDRESS_LENGTH == 4
    BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3};  
#elif MY_ADDRESS_LENGTH == 3
    BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2};    
#elif MY_ADDRESS_LENGTH == 2
    BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1};    
#endif


#if defined(IEEE_802_15_4)
    WORD_VAL    myPANID = {MY_PAN_ID};    // the PAN Identifier for the device
#endif

MAC_RECEIVED_PACKET MACRxPacket;
RECEIVED_MESSAGE  rxMessage;                    // structure to store information for the received packet
BYTE TxBuffer[TX_BUFFER_SIZE];

CONNECTION_ENTRY    ConnectionTable[CONNECTION_SIZE];        // The peer device records for P2P connections
#if defined(ENABLE_SECURITY)
    DWORD_VAL IncomingFrameCounter[CONNECTION_SIZE];  // If authentication is used, IncomingFrameCounter can prevent replay attack
#endif

BYTE            currentChannel = 0;             // current operating channel for the device
BYTE            TxData;

volatile LOWPAN_STATUS   LowPanStatus;
WORD_VAL		MyRank;
BYTE 			PreferredParent[8];


/*********************************************************************
 * Function prototype definition
 ********************************************************************/


#if defined(IEEE_802_15_4)
    BOOL SendPacket(INPUT BOOL Broadcast, INPUT WORD_VAL DestinationPANID, 
                    INPUT BYTE *DestinationAddress, INPUT BOOL isCommand, 
                    INPUT BOOL SecurityEnabled);
#else
    BOOL SendPacket(INPUT BOOL Broadcast,  
                    INPUT BYTE *DestinationAddress, INPUT BOOL isCommand, 
                    INPUT BOOL SecurityEnabled);
#endif                    

void 	LowPanTasks(void);
BOOL    isSameAddress(INPUT BYTE *Address1, INPUT BYTE *Address2);

/*********************************************************************
 * BOOL    isSameAddress(BYTE *Address1, BYTE *Address2)
 *
 * Overview:        This function compares two long addresses and returns
 *                  the boolean to indicate if they are the same
 *
 * PreCondition:    
 *
 * Input:  
 *          Address1    - Pointer to the first long address to be compared
 *          Address2    - Pointer to the second long address to be compared
 *                  
 * Output: 
 *          If the two address are the same
 *
 * Side Effects:    
 *
 ********************************************************************/
BOOL    isSameAddress(INPUT BYTE *Address1, INPUT BYTE *Address2)
{
    BYTE i;
    
    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
    {
        if( Address1[i] != Address2[i] )
        {
            return FALSE;
        }
    }
    return TRUE;
}


BOOL MiApp_ProtocolInit(BOOL bNetworkFreezer)
{
    
    MACINIT_PARAM initValue;
            
    //clear all status bits
    LowPanStatus.Val = 0;

	#ifdef  NWK_ROLE_COORDINATOR
	LowPanStatus.bits.RootNode = 1;
	#endif

	OFunctionInit( LowPanStatus.bits.RootNode );
	MyRank.Val = CalcRank( );

//    for(i = 0; i < CONNECTION_SIZE; i++)
//    {
//        ConnectionTable[i].status.Val = 0;
//    }
 
    InitSymbolTimer();
    
//    TxData = 0;

    #ifdef ENABLE_INDIRECT_MESSAGE
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            indirectMessages[i].flags.Val = 0;
        }
    #endif   
        
    #if defined(ENABLE_SECURITY)
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            IncomingFrameCounter[i].Val = 0;
        }
    #endif
        
    initValue.PAddress = myLongAddress;
    initValue.actionFlags.bits.CCAEnable = 1;
    initValue.actionFlags.bits.PAddrLength = MY_ADDRESS_LENGTH;
 //   initValue.actionFlags.bits.NetworkFreezer = bNetworkFreezer;
    initValue.actionFlags.bits.RepeaterMode = 0;

    MiMAC_Init(initValue);
    
    #if defined(IEEE_802_15_4)
        {
            WORD tmp = 0xFFFF;
            MiMAC_SetAltAddress((BYTE *)&tmp, (BYTE *)&myPANID.Val);
        }
    #endif
    
    MiApp_SetChannel(currentChannel);

    #if defined(ENABLE_TIME_SYNC)
        #if defined(ENABLE_SLEEP)
            WakeupTimes.Val = 0;
            CounterValue.Val = 61535;   // (0xFFFF - 4000) one second
        #elif defined(ENABLE_INDIRECT_MESSAGE)
            TimeSlotTick.Val = ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) / TIME_SYNC_SLOTS;
        #endif
    #endif

//    P2PCapacityInfo = 0;
//    #if !defined(ENABLE_SLEEP)
//        P2PCapacityInfo |= 0x01;
//    #endif
//    #if defined(ENABLE_SECURITY)
//        P2PCapacityInfo |= 0x08;
//    #endif
//    P2PCapacityInfo |= (ConnMode << 4);
//	TrickleInit();
	if ( LowPanStatus.bits.RootNode ) TrickleStart( FALSE );
	else TrickleStart( TRUE ) ;

    RFIE = 1;
    return TRUE;
}


BOOL MiApp_SetChannel(BYTE channel)
{
    if( MiMAC_SetChannel(channel, 0) )
    {
        currentChannel = channel;
        #if defined(ENABLE_NETWORK_FREEZER)
            nvmPutCurrentChannel(&currentChannel);
        #endif
        return TRUE;
    }
    return FALSE;
}


BOOL MiApp_BroadcastPacket( INPUT BOOL SecEn )                                                     
{
    #ifdef ENABLE_INDIRECT_MESSAGE
        BYTE i;
        
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            if( ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.RXOnWhenIdle == 0 )
            {
                #if defined(IEEE_802_15_4)
                    IndirectPacket(TRUE, myPANID, NULL, FALSE, SecEn);
                #else
                    IndirectPacket(TRUE, NULL, FALSE, SecEn);
                #endif
                break;
            }     
        }
    #endif
    
    #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEPING)
        if( P2PStatus.bits.Sleeping )
        {
            P2PStatus.bits.Enhanced_DR_SecEn = SecEn;
            return TRUE;
        }    
    #endif
    
    #if defined(IEEE_802_15_4)
        return SendPacket(TRUE, myPANID, NULL, FALSE, SecEn);
    #else
        return SendPacket(TRUE, NULL, FALSE, SecEn);
    #endif
}


#if defined(IEEE_802_15_4)
    BOOL SendPacket(INPUT BOOL Broadcast, 
                    INPUT WORD_VAL DestinationPANID, 
                    INPUT BYTE *DestinationAddress, 
                    INPUT BOOL isCommand, 
                    INPUT BOOL SecurityEnabled)
#else
    BOOL SendPacket(INPUT BOOL Broadcast, 
                    INPUT BYTE *DestinationAddress, 
                    INPUT BOOL isCommand, 
                    INPUT BOOL SecurityEnabled)
#endif                                        
{ 
    MAC_TRANS_PARAM tParam;
    BOOL status;

    tParam.flags.Val = 0;
    tParam.flags.bits.packetType = (isCommand) ? PACKET_TYPE_COMMAND : PACKET_TYPE_DATA;
    tParam.flags.bits.ackReq = (Broadcast) ? 0 : 1;
    tParam.flags.bits.broadcast = Broadcast;
    tParam.flags.bits.secEn = SecurityEnabled;
    #if defined(IEEE_802_15_4)
        tParam.altSrcAddr = 0;
        tParam.altDestAddr = (Broadcast) ? TRUE : FALSE;
    #endif
    
    #if defined(INFER_DEST_ADDRESS)
        tParam.flags.bits.destPrsnt = 0;
    #else
        tParam.flags.bits.destPrsnt = (Broadcast) ? 0:1;
    #endif
    
    #if defined(SOURCE_ADDRESS_ABSENT)
        if( tParam.flags.bits.packetType == PACKET_TYPE_COMMAND )
        {
            tParam.flags.bits.sourcePrsnt = 1;
        }
        else
        {
            tParam.flags.bits.sourcePrsnt = 0;
        }
    #else
        tParam.flags.bits.sourcePrsnt = 1;
    #endif
    
    tParam.DestAddress = DestinationAddress;

    #if defined(IEEE_802_15_4)
        tParam.DestPANID.Val = DestinationPANID.Val;
    #endif

    status = MiMAC_SendPacket(tParam, TxBuffer, TxData); 

    TxData = 0;
    
    return status;
}


BOOL SendCommand ( BYTE command) {
    MAC_TRANS_PARAM tParam;
    BOOL status;
	WORD_VAL metric;

    tParam.flags.Val = 0;
    tParam.flags.bits.packetType = PACKET_TYPE_COMMAND;
    tParam.flags.bits.ackReq = 0;
    tParam.flags.bits.broadcast = 1;
    tParam.flags.bits.secEn = 0;
    #if defined(IEEE_802_15_4)
        tParam.altSrcAddr = 0;
        tParam.altDestAddr = 0;
    #endif
    
    #if defined(INFER_DEST_ADDRESS)
        tParam.flags.bits.destPrsnt = 0;
    #else
        tParam.flags.bits.destPrsnt = 0;
    #endif
    
    tParam.flags.bits.sourcePrsnt = 1;
    
//    tParam.DestAddress = DestinationAddress;

    //#if defined(IEEE_802_15_4)
        //tParam.DestPANID.Val = DestinationPANID.Val;
    //#endif
    MiApp_FlushTx();
	switch ( command ) {
       case DIO_COMMAND:
	/***************************************************************************
	*  DIO PayLoad Format:
	*  0  	(COMMAND_POS)   		-  Command Identifier
	*  1-2	(DIO_RANK_POS)  		-  Rank
	*  3-4	(DIO_METRICT_POS)  		-  Selected Metric
	*  5    (DIO_TRICKLEMIN_POS)	-  Trickle timer Min Interval Value
	*  6    (DIO_TRICKLEMAX_POS)	-  Trickle timer Max Interval Doubling
	*  7-8  (DIO_TRICKLERED_POS)	-  Trickle timer Redundancy Constant Value
	****************************************************************************/
    		MiApp_WriteData( DIO_COMMAND );
			MiApp_WriteData( MyRank.byte.HB  );
			MiApp_WriteData( MyRank.byte.LB  );
			metric.Val = getMyMetric();
			MiApp_WriteData( metric.byte.HB  );
			MiApp_WriteData( metric.byte.LB  );
			MiApp_WriteData(3);
			MiApp_WriteData(TrickleMaxFactor);
			MiApp_WriteData(TrickleRedundancy);
			break;	
		case  DIS_COMMAND:
    		MiApp_WriteData( DIS_COMMAND );
			break;
	}
    status = MiMAC_SendPacket(tParam, TxBuffer, TxData); 

    TxData = 0;
   
    return status;	
}

void TrickleCB_Send(BOOL disMode) {
	if ( !disMode ) {
		SendCommand(DIO_COMMAND);
	} else {
		SendCommand(DIS_COMMAND);
	}
}


BOOL MiApp_MessageAvailable(void)
{ 
    LowPanTasks(); 
    
    return FALSE;
}


BOOL IsRankDefined () {
	if ( MyRank.Val == UNDEFINED_RANK ) return (FALSE);
	return (TRUE);
}

void OFCbk_SetNewParent( BYTE * bestPath) {
	memcpy(  PreferredParent, bestPath, 8);	
	MyRank.Val = CalcRank( ); 
	if ( IsRankDefined() ) TrickleStart( FALSE );
	else TrickleStop();
}


void LowPanTasks(void) {

    if( MiMAC_ReceivedPacket() )
    { 
/*
        rxMessage.flags.Val = 0;
        rxMessage.flags.bits.broadcast = MACRxPacket.flags.bits.broadcast;
        rxMessage.flags.bits.secEn = MACRxPacket.flags.bits.secEn;
        rxMessage.flags.bits.command = (MACRxPacket.flags.bits.packetType == PACKET_TYPE_COMMAND) ? 1:0;
        rxMessage.flags.bits.srcPrsnt = MACRxPacket.flags.bits.sourcePrsnt;
        if( MACRxPacket.flags.bits.sourcePrsnt )
        {
            rxMessage.SourceAddress = MACRxPacket.SourceAddress;
        }
        #if defined(IEEE_802_15_4) && !defined(TARGET_SMALL)
            rxMessage.SourcePANID.Val = MACRxPacket.SourcePANID.Val;
        #endif

        rxMessage.PayloadSize = MACRxPacket.PayloadLen;
        rxMessage.Payload = MACRxPacket.Payload;
      
        #ifndef TARGET_SMALL
            rxMessage.PacketLQI = MACRxPacket.LQIValue;
            rxMessage.PacketRSSI = MACRxPacket.RSSIValue;
        #endif
*/
        if( MACRxPacket.flags.bits.packetType == PACKET_TYPE_COMMAND )
        {
		  if ( MACRxPacket.SourcePANID.Val == MY_PAN_ID ) {
            // if comes here, we know it is a command frame
            switch( MACRxPacket.Payload[COMMAND_POS] )
            {
				case DIS_COMMAND :
					if ( IsRankDefined()) TrickleStart(FALSE);	
					break;
				case DIO_COMMAND :
				  if ( ! LowPanStatus.bits.RootNode ) {
					 UpdateObjFun(
						        MACRxPacket.SourceAddress, // NodeAddress
								PathCostCalc( MACRxPacket ),
								*((WORD *)( MACRxPacket.Payload + DIO_RANK_POS ))); 						 

//					update TrickleParameters
				    TrickleRedundancy = *((TRICKLE_COUNTER *)( MACRxPacket.Payload + DIO_TRICKLERED_POS ));
					TrickleMaxFactor = *((TRICKLE_FACTOR *)( MACRxPacket.Payload + DIO_TRICKLEMAX_POS ));
				  }
				  break;
				default:
					break;
			}
		  }
		} else if( MACRxPacket.flags.bits.packetType == PACKET_TYPE_DATA ){
			
		}
	}
    /* Scan Time */
	

	/* Trickle Task */
	TrickleTask();
}



#endif
