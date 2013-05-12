// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/WebSocket.h"
#include "TCPIP Stack/Helpers.h"


int CreateHandShake(TCP_SOCKET MySocket,unsigned char* WebSocketKey);
int UnMaskFrame(WebSocketFrame *Frame);
void CreateFrame(WebSocketFrame * OutFrame, BYTE* Data, int Length, int Opcode);

BYTE ServerReply [] =
"HTTP/1.1 101 Switching Protocols\r\n"
"Upgrade: websocket\r\n"
"Connection: Upgrade\r\n"
"Sec-WebSocket-Accept: ";
BYTE WebSocketGuid [] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
/*
 * Requires that TCPIsPutReady() is called first
 */
int CreateHandShake(TCP_SOCKET MySocket,unsigned char* WebSocketKey){

    static BYTE MyDebug[30];
    int b;
    static HASH_SUM ReplyHash;
    static BYTE Sha1Result[20];
    static BYTE ResultBase64[40];
    static int ResultStringLength;
    
    for(b=0;b<WebSocketKeyLength;b++){
        MyDebug[b] = WebSocketKey[b];
    }

    TCPPutArray(MySocket,ServerReply,sizeof ServerReply - 1);
    
    SHA1Initialize(&ReplyHash);
    HashAddData(&ReplyHash,(BYTE*)WebSocketKey,(WORD)WebSocketKeyLength);
    HashAddData(&ReplyHash,(BYTE*)WebSocketGuid,(WORD)sizeof WebSocketGuid - 1);
    SHA1Calculate(&ReplyHash,Sha1Result);
    ResultStringLength = Base64Encode(Sha1Result,20,ResultBase64,40);
    TCPPutArray(MySocket,ResultBase64,ResultStringLength);
    TCPPut(MySocket,0xD);//\r
    TCPPut(MySocket,0xA);//\n
    TCPPut(MySocket,0xD);//\r
    TCPPut(MySocket,0xA);//\n
    return 0;
}

int UnMaskFrame(WebSocketFrame *Frame) {
    int i;
    int DataOffset = 4;
    int DataLength = 0;
    int MaskOffset = 0;
    unsigned char Mask[4];
    
    if (!Frame->Mask || !Frame->FIN) {
        return 0;//already opened, or incorrect, should be masked, plus we don't accept fragmented frames at this point
    }
    if (Frame->PayloadLen == 126){
        DataOffset += 2;
        MaskOffset += 2;
        DataLength = (Frame->data[0] << 8) + Frame->data[1];
    } else if(Frame->PayloadLen == 127 ){
        return 0;//not supported
    } else    {
        DataLength = Frame->PayloadLen;
    }
    if(DataLength > MaxFrameLength)
        return 0; //too long
    
    memcpy(Mask, &Frame->data[MaskOffset], 4);
    for (i = 0; i < DataLength; i++)
        Frame->data[i] = Frame->data[i + DataOffset] ^ Mask[i % 4];
    return DataLength;
}

void CreateFrame(WebSocketFrame * OutFrame, BYTE* Data, int Length, int Opcode) {
    
    int a;
    int DataOffset = 0;

    OutFrame->FIN = 1;
    OutFrame->Mask = 0;
    OutFrame->Opcode = Opcode;
    OutFrame->RSV = 0;
    
    if (Length < 126)
        OutFrame->PayloadLen = Length;
    else {
        DataOffset = 2;
        OutFrame->PayloadLen = 126; //Flag that we use the 16 bit extended length
        OutFrame->data[0] = (Length >> 8)&0xFF;
        OutFrame->data[1] = Length & 0xFF;
    }
    if (Length > MaxFrameLength)
        Length = MaxFrameLength;
    for (a = 0; a < Length; a++) {
        OutFrame->data[a + DataOffset] = Data[a];
    }
    return;
}