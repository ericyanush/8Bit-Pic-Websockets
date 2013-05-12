//#include "GenericTypeDefs.h"
#define MaxFrameLength 248
#define ShortExtendLength 2
#define LongExtendLength 4
#define MaskLength 4
#define ShortReplyAddition 2

typedef struct {
    unsigned char Opcode : 4;
    unsigned char RSV : 3;
    unsigned char FIN : 1;
    unsigned char PayloadLen : 7;
    unsigned char Mask : 1;
    BYTE data[MaxFrameLength + 6];//allow room for additional
} WebSocketFrame;

typedef enum {
    ContinuationFrame, TextFrame, BinaryFrame, ConnectionClose = 0x08, WSPing, WSPong
} OPCODES;

extern int CreateHandShake(TCP_SOCKET MySocket,unsigned char* WebSocketKey, short KeyLength);
extern int UnMaskFrame(WebSocketFrame *Frame);
extern void CreateFrame(WebSocketFrame *OutFrame, BYTE* Data, int Length, int Opcode);
extern int WebSocketEventRequest[MAX_HTTP_CONNECTIONS];//for requesting an data out event

/****************************************************************************
 Section:
 User-Implemented Callback Function Prototypes
 ***************************************************************************/

/*****************************************************************************
 Function:
 void WebSocketNewFrame(BYTE* DATA, int Len, int SocketID, int Opcode)
 
 Summary:
 Processes Incoming WebSocket Data
 
 Description:
 This function is implemented by the application developer in
 CustomHTTPApp.c.  Its purpose is to parse the data received from
 a WebSocket Frame and perform any
 application-specific tasks in response to these inputs. 
 
 If data buffer space associated
 with this connection is required, curHTTP.data may be overwritten
 here once the application is done with the values.  Any data placed
 there will be available to future callbacks for this connection,
 including HTTPExecutePost and any HTTPPrint_varname dynamic
 substitutions.
 
 Precondition:
 None
 
 Parameters:
 DATA - The data stream from the target WebSocket Frame.
 Len  - The length of the Data from the Frame.
 SocketID - the Unique ID of the current TCP Socket
 Opcode - The opcode for the type of data contained in the frame.
 
 Return Values:
 None
 
 Remarks:
 This function is only called if data is recieved over an active WebSocket.  
 This function may NOT write to the TCP buffer.
 
 This function may service multiple HTTP requests simultaneously.
 Exercise caution when using global or static variables inside this
 routine.  Use curHTTP.callbackPos or curHTTP.data for storage associated
 with individual requests.
 ***************************************************************************/
extern void WebSocketNewFrame(BYTE* DATA, int Len, int SocketID, int Opcode);

/*****************************************************************************
 Function:
 void WebSocketGetFrame(BYTE* Stream, int* Length, int* Opcode, int SocketID)
 
 Summary:
 Provides data to be sent out in a new WebSocket Frame.
 
 Description:
 This function is implemented by the application developer in
 CustomHTTPApp.c.  Its purpose is to provide data to be sent to the websocket
 client in a new frame.
 
 If data buffer space associated
 with this connection is required, curHTTP.data may be overwritten
 here once the application is done with the values.  Any data placed
 there will be available to future callbacks for this connection,
 including HTTPExecutePost and any HTTPPrint_varname dynamic
 substitutions.
 
 Precondition:
 None
 
 Parameters:
 Stream(OUT) - Pointer to the Stream where any data to be sent should be written.
 Length(OUT)  - The length of the Data written to the Stream.
 SocketID - the Unique ID of the current TCP Socket
 Opcode(OUT) - The opcode for the type of data written to the stream.
 
 Return Values:
 None
 
 Remarks:
 This function is only called if WebSocketEventRequest[SocketID] flag has been set.
 
 This function may service multiple HTTP requests simultaneously.
 Exercise caution when using global or static variables inside this
 routine.  Use curHTTP.callbackPos or curHTTP.data for storage associated
 with individual requests.
 ***************************************************************************/
extern void WebSocketGetFrame(BYTE* Stream, int* Length, int* Opcode, int SocketID);

/*****************************************************************************
 Function:
 void HTTPExecuteWebSocket(int SocketID);
 
 Summary:
 Processes IO Data for WebSocket requests.
 
 Description:
 This function is implemented by the application developer in
 CustomHTTPApp.c.  Its purpose is to handle any data recieved from
 a websocket frame and process any corresponding IO accordingly.
 It must also set the WebSocketEventRequest[SocketID] flag when/if there
 is response data to be sent back to the client.
 
 If data buffer space associated with this connection is required, 
 curHTTP.data may be overwritten here once the application is done 
 with the values.  Any data placed there will be available to future 
 callbacks for this connection, including HTTPExecutePost and any 
 HTTPPrint_varname dynamic substitutions.
 
 Precondition:
 None
 
 Parameters:
 SocketID - The unique identifier for the current TCP socket.
 
 Return Values:
 None.
 
 Remarks: 
 This function may service multiple HTTP requests simultaneously.
 Exercise caution when using global or static variables inside this
 routine.  Use curHTTP.callbackPos or curHTTP.data for storage associated
 with individual requests.
 ***************************************************************************/
extern void HTTPExecuteWebSocket(int SocketID);
