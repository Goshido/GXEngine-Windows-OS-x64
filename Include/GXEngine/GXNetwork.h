// verison 1.12

#ifndef GX_NETWORK
#define GX_NETWORK


#include <GXCommon/GXLogger.h>
#include <GXCommon/GXThread.h>

GX_DISABLE_COMMON_WARNINGS

#include <WinSock2.h>

GX_RESTORE_WARNING_STATE


#define GX_NETWORK_MAX_NETWORK_CLIENTS      64u
#define GX_NETWORK_SOCKET_BUFFER_SIZE       4194304u    // 4 Mb


typedef GXVoid ( GXCALL* GXNetworkOnMessageTCPHandler ) ( GXUInt clientID, GXVoid* data, GXUInt size );
typedef GXVoid ( GXCALL* GXNetworkOnMessageUDPHandler ) ( const sockaddr_in &fromClient, GXVoid* data, GXUInt size );
typedef GXVoid ( GXCALL* GXNetworkOnNewTCPConectionHandler ) ( GXUInt clientID );
typedef GXVoid ( GXCALL* GXNetworkOnServerDisconnectedHandler ) ( GXUInt clientID );
typedef GXVoid ( GXCALL* GXNetworkOnClientMessageHandler ) ( const GXVoid* data, GXUInt size );


class GXNetConnectionTCP final
{
    private:
        SOCKET          _socket;
        GXThread*       _thread;

    public:
        GXNetConnectionTCP ();
        ~GXNetConnectionTCP ();

        GXBool IsFree ();
        SOCKET GetSocket ();
        GXThread* GetThread ();

        GXVoid Init ( SOCKET socketObject, GXThread* threadObject );
        GXVoid Destroy ();

    private:
        GXNetConnectionTCP ( const GXNetConnectionTCP &other ) = delete;
        GXNetConnectionTCP& operator = ( const GXNetConnectionTCP &other ) = delete;
};

class GXNetServer final
{
    private:
        GXThread*                                       _threadTCP;
        GXThread*                                       _threadUDP;

        static SOCKET                                   _listenerTCP;
        static SOCKET                                   _listenerUDP;

        static GXUInt                                   _numClientsTCP;

        static GXNetConnectionTCP                       _clientsTCP[ GX_NETWORK_MAX_NETWORK_CLIENTS ];

        static GXNetworkOnNewTCPConectionHandler        _onNewConnectionTCP;
        static GXNetworkOnServerDisconnectedHandler     _onDisconnect;

        static GXNetworkOnMessageTCPHandler             _onMessageTCP;
        static GXNetworkOnMessageUDPHandler             _onMessageUDP;

        static GXUByte                                  _bufferTCP[ GX_NETWORK_SOCKET_BUFFER_SIZE ];
        static GXUByte                                  _bufferUDP[ GX_NETWORK_SOCKET_BUFFER_SIZE ];

        static GXNetServer*                             _instance;

    public:
        static GXNetServer& GXCALL GetInstance ();
        ~GXNetServer ();

        GXBool CreateTCP ( GXUShort port );
        GXBool CreateUDP ( GXUShort port );

        GXBool SendTCP ( GXUInt clientID, GXVoid* data, GXInt size );
        GXBool SendUDP ( const sockaddr_in &toClient, GXVoid* data, GXInt size );

        GXBool BroadcastTCP ( GXVoid* data, GXInt size );
        GXBool BroadcastUDP ( GXVoid* data, GXInt size );

        GXBool DestroyTCP ();
        GXBool DestroyUDP ();

        GXBool IsDeployedTCP ();
        GXBool IsDeployedUDP ();

        GXVoid SetOnNewTCPConnection ( GXNetworkOnNewTCPConectionHandler callback );
        GXVoid SetOnDisconnectFunc ( GXNetworkOnServerDisconnectedHandler callback );

        GXVoid SetOnMessageFuncTCP ( GXNetworkOnMessageTCPHandler callback );
        GXVoid SetOnMessageFuncUDP ( GXNetworkOnMessageUDPHandler callback );

    private:
        GXNetServer ();

        static GXUPointer GXTHREADCALL ListenTCP ( GXVoid* arg, GXThread &thread );
        
        static GXUPointer GXTHREADCALL ServeClientTCP ( GXVoid* arg, GXThread &thread );
        static GXUPointer GXTHREADCALL ServeClientUDP ( GXVoid* arg, GXThread &thread );

        static GXInt GetFreeClientTCP ();
        static GXInt FindClientTCP ( SOCKET socket );
        static GXBool GetClientIP ( sockaddr_in &address, GXUInt clientID );

        GXNetServer ( const GXNetServer &other ) = delete;
        GXNetServer& operator = ( const GXNetServer &other ) = delete;
};

class GXNetClient final
{
    private:
        sockaddr_in                                 _serverAddressUDP;

        static SOCKET                               _socketTCP;
        static SOCKET                               _socketUDP;

        static GXThread*                            _threadTCP;
        static GXThread*                            _threadUDP;

        static GXNetworkOnClientMessageHandler      _onMessageTCP;
        static GXNetworkOnClientMessageHandler      _onMessageUDP;

        static GXUByte                              _bufferTCP[ GX_NETWORK_SOCKET_BUFFER_SIZE ];
        static GXUByte                              _bufferUDP[ GX_NETWORK_SOCKET_BUFFER_SIZE ];

        static GXNetClient*                         _instance;

    public:
        static GXNetClient& GXCALL GetInstance ();
        ~GXNetClient ();

        GXBool ConnectTCP ( const GXChar* url, GXUShort port );
        GXBool DeployUDP ( const GXChar* url, GXUShort port );

        GXBool SendTCP ( GXVoid* data, GXInt size );
        GXBool SendUDP ( GXVoid* data, GXInt size );

        GXBool DisconnectTCP ();
        GXBool DestroyUDP ();

        GXBool IsConnectedTCP ();
        GXBool IsDeployedUDP ();

        GXVoid SetOnMessageTCPFunc ( GXNetworkOnClientMessageHandler callback );
        GXVoid SetOnMessageUDPFunc ( GXNetworkOnClientMessageHandler callback );

    private:
        GXNetClient ();

        static GXUPointer GXTHREADCALL ReceiveTCP ( GXVoid* arg, GXThread &thread );
        static GXUPointer GXTHREADCALL ReceiveUDP ( GXVoid* arg, GXThread &thread );

        GXNetClient ( const GXNetClient &other ) = delete;
        GXNetClient& operator = ( const GXNetClient &other ) = delete;
};


#endif // GX_NETWORK
