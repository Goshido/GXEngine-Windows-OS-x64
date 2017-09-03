//verison 1.7

#ifndef GX_NETWORK
#define GX_NETWORK


#include <WinSock2.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXThread.h>


#define GX_MAX_NETWORK_CLIENTS	64
#define GX_SOCKET_BUFFER_SIZE	4194304		//4 Mb


typedef GXVoid ( GXCALL* PFNGXONSERVERPMESSAGETCPPROC ) ( GXUInt clientID, GXVoid* data, GXUInt size );
typedef GXVoid ( GXCALL* PFNGXONSERVERPMESSAGEUDPPROC ) ( const sockaddr_in &fromClient, GXVoid* data, GXUInt size );
typedef GXVoid ( GXCALL* PFNGXONSERVERNEWTCPCONNECTIONPROC ) ( GXUInt clientID );
typedef GXVoid ( GXCALL* PFNGXONSERVERDISCONNECTPROC ) ( GXUInt clientID );
typedef GXVoid ( GXCALL* PFNGXONCLIENTMESSAGEPROC ) ( const GXVoid* data, GXUInt size );


class GXNetConnectionTCP
{
	private:
		SOCKET			socket;
		GXThread*		thread;

	public:
		GXNetConnectionTCP ();
		~GXNetConnectionTCP ();

		GXBool IsFree ();
		SOCKET GetSocket ();
		GXThread* GetThread ();

		GXVoid Init ( SOCKET socketObject, GXThread* threadObject );
		GXVoid Destroy ();
};

class GXNetServer
{
	private:
		GXThread*									threadTCP;
		GXThread*									threadUDP;

		static SOCKET								listenerTCP;
		static SOCKET								listenerUDP;

		static GXUInt								numClientsTCP;

		static GXNetConnectionTCP					clientsTCP[ GX_MAX_NETWORK_CLIENTS ];

		static PFNGXONSERVERNEWTCPCONNECTIONPROC	OnNewConnectionTCP;
		static PFNGXONSERVERDISCONNECTPROC			OnDisconnect;

		static PFNGXONSERVERPMESSAGETCPPROC			OnMessageTCP;
		static PFNGXONSERVERPMESSAGEUDPPROC			OnMessageUDP;

		static GXUByte								bufferTCP[ GX_SOCKET_BUFFER_SIZE ];
		static GXUByte								bufferUDP[ GX_SOCKET_BUFFER_SIZE ];

		static GXNetServer*							instance;

	public:
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

		GXVoid SetOnNewTCPConnection ( PFNGXONSERVERNEWTCPCONNECTIONPROC callback );
		GXVoid SetOnDisconnectFunc ( PFNGXONSERVERDISCONNECTPROC callback );

		GXVoid SetOnMessageFuncTCP ( PFNGXONSERVERPMESSAGETCPPROC callback );
		GXVoid SetOnMessageFuncUDP ( PFNGXONSERVERPMESSAGEUDPPROC callback );

		static GXNetServer& GXCALL GetInstance ();

	private:
		GXNetServer ();

		static GXUPointer GXTHREADCALL ListenTCP ( GXVoid* arg, GXThread &thread );
		
		static GXUPointer GXTHREADCALL ServeClientTCP ( GXVoid* arg, GXThread &thread );
		static GXUPointer GXTHREADCALL ServeClientUDP ( GXVoid* arg, GXThread &thread );

		static GXInt GetFreeClientTCP ();
		static GXInt FindClientTCP ( SOCKET socket );
		static GXBool GetClientIP ( sockaddr_in &address, GXUInt clientID );
};

class GXNetClient
{
	private:
		static SOCKET						socketTCP;
		static SOCKET						socketUDP;

		static GXThread*					threadTCP;
		static GXThread*					threadUDP;

		sockaddr_in							serverAddressUDP;

		static PFNGXONCLIENTMESSAGEPROC		OnMessageTCP;
		static PFNGXONCLIENTMESSAGEPROC		OnMessageUDP;

		static GXUByte						bufferTCP[ GX_SOCKET_BUFFER_SIZE ];
		static GXUByte						bufferUDP[ GX_SOCKET_BUFFER_SIZE ];

		static GXNetClient*					instance;

	public:
		~GXNetClient ();

		GXBool ConnectTCP ( const GXChar* url, GXUShort port );
		GXBool DeployUDP ( const GXChar* url, GXUShort port );

		GXBool SendTCP ( GXVoid* data, GXInt size );
		GXBool SendUDP ( GXVoid* data, GXInt size );

		GXBool DisconnectTCP ();
		GXBool DestroyUDP ();

		GXBool IsConnectedTCP ();
		GXBool IsDeployedUDP ();

		GXVoid SetOnMessageTCPFunc ( PFNGXONCLIENTMESSAGEPROC onMessageFunc );
		GXVoid SetOnMessageUDPFunc ( PFNGXONCLIENTMESSAGEPROC onMessageFunc );

		static GXNetClient& GXCALL GetInstance ();

	private:
		GXNetClient ();

		static GXUPointer GXTHREADCALL ReceiveTCP ( GXVoid* arg, GXThread &thread );
		static GXUPointer GXTHREADCALL ReceiveUDP ( GXVoid* arg, GXThread &thread );
};


#endif //GX_NETWORK
