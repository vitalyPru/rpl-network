#include "WirelessProtocols/Console.h"
#include "ConfigApp.h"
#include "HardwareProfile.h"
#include "WirelessProtocols/MCHP_API.h"
#include "WirelessProtocols/LCDBlocking.h"

#include "WirelessProtocols/SymbolTime.h"



//MACINIT_PARAM initParameter;
BYTE myChannel = 24;

#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
    BYTE i;
    BYTE TxSynCount = 0;
    BYTE TxSynCount2 = 0;
    BYTE TxNum = 0;
    BYTE RxNum = 0;
    
    /*******************************************************************/
    // Initialize the system
    /*******************************************************************/
    BoardInit();      
    ConsoleInit(); 
    


    /*******************************************************************/
    // Following block display demo information on LCD of Explore 16 or 
    // PIC18 Explorer demo board.
    /*******************************************************************/

    LCDDisplay((char *)"LowPAN demo", 0, TRUE);

    Printf("\r\nLowPAN demo:");
    
    Printf("\r\n     RF Transceiver: MRF24J40");
    LED_1 = 0;
    LED_2 = 0;


    /*******************************************************************/
    // Initialize Microchip proprietary protocol. Which protocol to use
    // depends on the configuration in ConfigApp.h
    /*******************************************************************/
    /*******************************************************************/
    // Function MiApp_ProtocolInit initialize the protocol stack. The
    // only input parameter indicates if previous network configuration
    // should be restored. In this simple example, we assume that the 
    // network starts from scratch.
    /*******************************************************************/
    MiApp_ProtocolInit(FALSE); 


    // Set default channel
    if( MiApp_SetChannel(myChannel) == FALSE )
    {
        Printf("\r\nSelection of channel ");
        PrintDec(myChannel);
        Printf(" is not supported in current condition.\r\n");
        #if defined(__18CXX)
            return;
        #else
            return 0;
        #endif
    }
    
    /*******************************************************************/
    // Function MiApp_ConnectionMode defines the connection mode. The
    // possible connection modes are:
    //  ENABLE_ALL_CONN:    Enable all kinds of connection
    //  ENABLE_PREV_CONN:   Only allow connection already exists in 
    //                      connection table
    //  ENABL_ACTIVE_SCAN_RSP:  Allow response to Active scan
    //  DISABLE_ALL_CONN:   Disable all connections. 
    /*******************************************************************/
//    MiApp_ConnectionMode(ENABLE_ALL_CONN);
    
    /*******************************************************************/
    // Display current opertion on LCD of demo board, if applicable
    /*******************************************************************/
//    LCDDisplay((char *)"Connecting Peer  on Channel %d ", myChannel, TRUE);

    /*******************************************************************/
    // Function MiApp_EstablishConnection try to establish a new 
    // connection with peer device. 
    // The first parameter is the index to the active scan result, 
    //      which is acquired by discovery process (active scan). If 
    //      the value of the index is 0xFF, try to establish a 
    //      connection with any peer.
    // The second parameter is the mode to establish connection, 
    //      either direct or indirect. Direct mode means connection 
    //      within the radio range; indirect mode means connection 
    //      may or may not in the radio range. 
    /*******************************************************************/
//    i = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
    
    
    /*******************************************************************/
    // Display current opertion on LCD of demo board, if applicable
    /*******************************************************************/
/*
    if( i != 0xFF )
    {
        LCDDisplay((char *)" Connected Peer  on Channel %d", myChannel, TRUE);
    }
    else
    {
*/
        /*******************************************************************/
        // If no network can be found and join, we need to start a new 
        // network by calling function MiApp_StartConnection
        //
        // The first parameter is the mode of start connection. There are 
        // two valid connection modes:
        //   - START_CONN_DIRECT        start the connection on current 
        //                              channel
        //   - START_CONN_ENERGY_SCN    perform an energy scan first, 
        //                              before starting the connection on 
        //                              the channel with least noise
        //   - START_CONN_CS_SCN        perform a carrier sense scan 
        //                              first, before starting the 
        //                              connection on the channel with 
        //                              least carrier sense noise. Not
        //                              supported for current radios
        //
        // The second parameter is the scan duration, which has the same 
        //     definition in Energy Scan. 10 is roughly 1 second. 9 is a 
        //     half second and 11 is 2 seconds. Maximum scan duration is 
        //     14, or roughly 16 seconds.
        //
        // The third parameter is the channel map. Bit 0 of the 
        //     double word parameter represents channel 0. For the 2.4GHz 
        //     frequency band, all possible channels are channel 11 to 
        //     channel 26. As the result, the bit map is 0x07FFF800. Stack 
        //     will filter out all invalid channels, so the application 
        //     only needs to pay attention to the channels that are not 
        //     preferred.
        /*******************************************************************/
/*
        MiApp_StartConnection(START_CONN_DIRECT, 10, 0);
    }
*/
    /*******************************************************************/
    // Function DumpConnection is used to print out the content of the
    //  Connection Entry on the hyperterminal. It may be useful in 
    //  the debugging phase.
    // The only parameter of this function is the index of the  
    //  Connection Entry. The value of 0xFF means to print out all
    //  valid Connection Entry; otherwise, the Connection Entry
    //  of the input index will be printed out.
    /*******************************************************************/
//    DumpConnection(0xFF);


    // Turn on LED 1 to indicate connection established
    LED_1 = 1;


    /*******************************************************************/
    // Following block display demo instructions on LCD based on the
    // demo board used. 
    /*******************************************************************/
    #if defined(EXPLORER16)
        LCDDisplay((char *)"RD6: Broadcast  RD7: Unicast", 0, FALSE); 
    #elif defined(PIC18_EXPLORER)
        LCDDisplay((char *)"RB0: Broadcast  RA5: Unicast", 0, FALSE); 
    #elif defined(EIGHT_BIT_WIRELESS_BOARD)
        LCDDisplay((char *)"RB0: Broadcast  RB2: Unicast", 0, FALSE); 
    #endif

    while(1)
    {
        /*******************************************************************/
        // Function MiApp_MessageAvailable returns a boolean to indicate if 
        // a packet has been received by the transceiver. If a packet has 
        // been received, all information will be stored in the rxFrame, 
        // structure of RECEIVED_MESSAGE.
        /*******************************************************************/
        if( MiApp_MessageAvailable() )
        {
            /*******************************************************************/
            // If a packet has been received, following code prints out some of
            // the information available in rxMessage.
            /*******************************************************************/
//            if( rxMessage.flags.bits.secEn )
//            {
//                ConsolePutROMString((ROM char *)"Secured ");
//            }

//            if( rxMessage.flags.bits.broadcast )
//            {
//                ConsolePutROMString((ROM char *)"Broadcast Packet with RSSI ");
//            }
//            else
//            {
//                ConsolePutROMString((ROM char *)"Unicast Packet with RSSI ");
//            }
//            PrintChar(rxMessage.PacketRSSI);
//            if( rxMessage.flags.bits.srcPrsnt )
//            {
//                ConsolePutROMString((ROM char *)" from ");
//                if( rxMessage.flags.bits.altSrcAddr )
//                {
//                    PrintChar( rxMessage.SourceAddress[1]);
//                    PrintChar( rxMessage.SourceAddress[0]);
//                }
//                else
//                {    
//                    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
//                    {
//                        PrintChar(rxMessage.SourceAddress[MY_ADDRESS_LENGTH-1-i]);
//                    }    
//                }
//            }
//            ConsolePutROMString((ROM char *)": ");
            
//            for(i = 0; i < rxMessage.PayloadSize; i++)
//            {
//                ConsolePut(rxMessage.Payload[i]);
//            }
            
            
            // Toggle LED2 to indicate receiving a packet.
//            LED_2 ^= 1;
            
            /*******************************************************************/
            // Function MiApp_DiscardMessage is used to release the current 
            //  received packet.
            // After calling this function, the stack can start to process the
            //  next received frame 
            /*******************************************************************/        
//            MiApp_DiscardMessage();
            
            /*******************************************************************/
            // Following block update the total received and transmitted 
            // messages on the LCD of the demo board. 
            /*******************************************************************/
//            LCDTRXCount(TxNum, ++RxNum);
        }
        else
        {
            /*******************************************************************/
            // If no packet received, now we can check if we want to send out
            // any information.
            // Function ButtonPressed will return if any of the two buttons
            // has been pushed.
            /*******************************************************************/
            BYTE PressedButton = ButtonPressed();
            
            switch( PressedButton )
            {
                case 1:                 
                    /*******************************************************************/ 
                    // Button 1 pressed. We need to send out the bitmap of word "MiWi".
                    // First call MiApp_FlushTx to reset the Transmit buffer. Then fill 
                    // the buffer one byte by one byte by calling function 
                    // MiApp_WriteData
                    /*******************************************************************/
                    MiApp_FlushTx();
                    MiApp_WriteData('D');
                    MiApp_WriteData('A');
                    MiApp_WriteData('O');
                    
                    /*******************************************************************/
                    // Function MiApp_BroadcastPacket is used to broadcast a message
                    //    The only parameter is the boolean to indicate if we need to
                    //       secure the frame
                    /*******************************************************************/
                    MiApp_BroadcastPacket(FALSE);
                    
                    /*******************************************************************/
                    // Following block update the total received and transmitted 
                    // messages on the LCD of the demo board. 
                    /*******************************************************************/
                    LCDTRXCount(++TxNum, RxNum);   
                    break;
                    
//                case 2:
                    /*******************************************************************/                
                    // Button 2 pressed. We need to send out the bitmap of word "DE" 
                    // encrypted.
                    // First call function MiApp_FlushTx to reset the Transmit buffer. 
                    //  Then fill the buffer one byte by one byte by calling function 
                    //  MiApp_WriteData
                    /*******************************************************************/
//                    MiApp_FlushTx();
//                    if( (TxSynCount2 % 6 ) == 0 )
//                    {
                        // write an empty line after each bitmap
//                        MiApp_WriteData(0x0D);
//                        MiApp_WriteData(0x0A);
//                    }
//                    else
//                    {      
//                        for(i = 0; i < 11; i++)
//                        {
//                            MiApp_WriteData(DE[(TxSynCount2%6)-1][i]);
//                        }
//                    }
//                    TxSynCount2++;
                    
                    /*******************************************************************/
                    // Function MiApp_UnicastConnection is one of the functions to 
                    //  unicast a message.
                    //    The first parameter is the index of Connection Entry for 
                    //       the peer device. In this demo, since there are only two
                    //       devices involved, the peer device must be stored in the 
                    //       first Connection Entry
                    //    The second parameter is the boolean to indicate if we need to
                    //       secure the frame. If encryption is applied, the security
                    //       level and security key are defined in ConfigApp.h
                    //
                    // Another way to unicast a message is by calling function
                    //  MiApp_UnicastAddress. Instead of supplying the index of the 
                    //  Connection Entry of the peer device, this function requires the 
                    //  input parameter of destination address.
                    /*******************************************************************/
//                    if( MiApp_UnicastConnection(0, TRUE) == FALSE )
//                    {
//                        Printf("\r\nUnicast Failed\r\n");
                        /*******************************************************************/
                        // Following block display error message on LCD of demo board.
                        /*******************************************************************/
//                        LCDDisplay((char *)" Unicast Failed", 0, TRUE);
//                    }
//                    else
//                    {
//                        TxNum++;
//                    }
                    
                    /*******************************************************************/
                    // Following block update the total received and transmitted 
                    // messages on the LCD of the demo board. 
                    /*******************************************************************/
//                    LCDTRXCount(TxNum, RxNum);
//                    break;
                    
                default:
                    break;
            } 
        }
    }
}


