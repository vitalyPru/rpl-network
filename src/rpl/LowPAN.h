#ifndef __LOWPAN_H_
#define __LOWPAN_H_

#define INPUT
#define OUTPUT
#define IOPUT
#include "ConfigApp.h"

#if defined(PROTOCOL_LOWPAN)
/************************ HEADERS **********************************/
#include "ConfigLOWPAN.h"
#include "WirelessProtocols/SymbolTime.h"
#include "Transceivers/Transceivers.h"
#include "Transceivers/MCHP_MAC.h"

#include "Trickle.h"
/************************ DEFINITIONS ******************************/

#define PAYLOAD_START                           0

#define UNDEFINED_RANK				(0xFFFF)

/* Payload bytes positions */
#define  	COMMAND_POS		0   		//  Command Identifier

/* DIO */
#define		DIO_RANK_POS			1  			// Rank
#define		DIO_METRIC_POS			3  			// Selected metric position
#define		DIO_TRICKLEMIN_POS		5			// Trickle timer Min Interval Value
#define		DIO_TRICKLEMAX_POS		6			// Trickle timer Max Interval Doubling
#define		DIO_TRICKLERED_POS		7			// Trickle timer Redundancy Constant Value


#define STATUS_SUCCESS                          0x00
//#define STATUS_EXISTS                           0x01
//#define STATUS_ACTIVE_SCAN                      0x02
//#define STATUS_ENTRY_NOT_EXIST                  0xF0
//#define STATUS_NOT_ENOUGH_SPACE                 0xF1
//#define STATUS_NOT_SAME_PAN                     0xF2
//#define STATUS_NOT_PERMITTED                    0xF3

//#define CMD_P2P_CONNECTION_REQUEST              0x81
//#define CMD_P2P_CONNECTION_REMOVAL_REQUEST      0x82
//#define CMD_DATA_REQUEST                        0x83
//#define CMD_CHANNEL_HOPPING                     0x84
//#define CMD_TIME_SYNCHRONIZATION_REQUEST        0x85
//#define CMD_TIME_SYNCHRONIZATION_NOTIFICATION   0x86
//#define CMD_P2P_ACTIVE_SCAN_REQUEST             0x87

//#define CMD_TIME_SYNC_DATA_PACKET               0x8A
//#define CMD_TIME_SYNC_COMMAND_PACKET            0x8B

//#define CMD_P2P_CONNECTION_RESPONSE             0x91
//#define CMD_P2P_CONNECTION_REMOVAL_RESPONSE     0x92
//#define CMD_P2P_ACTIVE_SCAN_RESPONSE            0x97

//#define CMD_MAC_DATA_REQUEST                    0x04

//#if defined(ENABLE_ED_SCAN) && defined(ENABLE_FREQUENCY_AGILITY)
//    #define FREQUENCY_AGILITY_STARTER
//#endif

//#define MICRO_SECOND_PER_COUNTER_TICK   (1000000ul / ( COUNTER_CRYSTAL_FREQ / 8 ))

/************************ DATA TYPE *******************************/
 
/******************************************************************
 * Overview: The capacity information for a MiWi(TM) P2P device. It is the 
 * definition of the first byte of PeerInfo defined in 
 * P2P_CONNECTION_ENTRY. The highest bit also be used to indicate 
 * if the P2P connection entry is a valid entry
 *****************************************************************/
/*
typedef union 
{
    BYTE    Val;                            // the value of the P2P capacitity 
    struct _P2P_CAPACITY_BITS
    {
        BYTE    RXOnWhileIdel       : 1;    // if device turns on radio when idle
        BYTE    DataRequestNeeded   : 1;    // if data request is required when device turns off radio when 
                                            // idle. It is used to decide if an indirect message is necessary
                                            // to be stored.
        BYTE    TimeSynchronization : 1;    // reserved bit for future development
        BYTE    SecurityCapacity    : 1;    // if the device is capable of handling encrypted information
        BYTE    ConnectionMode      : 2;
        BYTE    filler              : 1;
        BYTE    isValid             : 1;    // use this bit to indicate that this entry is a valid entry
    } bits;
} P2P_CAPACITY;         
*/

/******************************************************************
 * Overview: The structure to store indirect messages for devices turn off 
 * radio when idle
 *****************************************************************/
/*
typedef struct 
{
    MIWI_TICK       TickStart;      // start time of the indirect message. Used for checking 
                                // indirect message time out
    #if defined(IEEE_802_15_4)                                
        WORD_VAL    DestPANID;      // the PAN identifier for the destination node
    #endif
    union 
    {
        BYTE        DestLongAddress[MY_ADDRESS_LENGTH];             // unicast destination long address
        BYTE        DestIndex[CONNECTION_SIZE];                     // broadcast index of the P2P Connection Entries 
                                                                    // for destination RFD devices
    } DestAddress;  // destination address for the indirect message. Can either for unicast or broadcast
    union 
    {
        BYTE    Val;                        // value for the flags
        struct 
        {
            BYTE    isValid         : 1;    // if this indirect message is valid
            BYTE    isBroadcast     : 1;    // if this indirect message is for broadcasting
            BYTE    isCommand       : 1;    // if this indirect message a command
            BYTE    isSecured       : 1;    // if this indirect message requires encryption
        } bits;                             // bit map of the flags
    } flags;                                // flags for indirect message
    BYTE        PayLoadSize;                // the indirect message pay load size
    #if (TX_BUFFER_SIZE) > 110
        BYTE    PayLoad[110];
    #else
        BYTE    PayLoad[TX_BUFFER_SIZE];    // the indirect message pay load
    #endif
} INDIRECT_MESSAGE; 
*/

/******************************************************************
 * Overview: structure to indicate the status of P2P stack
 *****************************************************************/
/*
typedef union
{
    BYTE Val;                               // The value of the P2P status flags
    struct 
    {
        BYTE Sleeping               :1;     // indicate if the device in sleeping state
        BYTE SaveConnection         :1;     // indicate if it is time for Network Freezer module to
                                            // save valid connections. Important because there may be
                                            // more than one response coming back and NVM operation is 
                                            // blocking
        BYTE DataRequesting         :1;     // indicate that device is in the process of data request
                                            // from its parent. Only effective if device enables sleeping
        BYTE RxHasUserData          :1;     // indicate if the received frame needs processing from
                                            // the application layer   
        BYTE SearchConnection       :1;     // indicate if the stack is currently in the process of 
                                            // looking for new connection
        BYTE Resync                 :1;     // indicate if the stack is currently in the process of 
                                            // resynchronizing connection with the peer device
        BYTE Enhanced_DR_SecEn      :1;
    }bits;                                  // bit map of the P2P status
} P2P_STATUS;                               
*/ 

typedef union
{
    BYTE Val;                               // The value of the P2P status flags
    struct 
    {
        BYTE RootNode                 :1;     // indicate if the stack is currently in the process of 
                                            // resynchronizing connection with the peer device
    }bits;                                  // bit map of the P2P status
} LOWPAN_STATUS;                               



/************************ EXTERNAL VARIABLES **********************/
extern BYTE TxBuffer[];
extern BYTE TxData;
extern volatile LOWPAN_STATUS LowPanStatus;
#if defined(IEEE_802_15_4)
    extern WORD_VAL myPANID;
#endif
extern WORD_VAL CounterVal;

/************************ FUNCTION PROTOTYPES **********************/
//void    DumpConnection(INPUT BYTE index);
BOOL    isSameAddress(INPUT BYTE *Address1, INPUT BYTE *Address2);

#endif

#endif
