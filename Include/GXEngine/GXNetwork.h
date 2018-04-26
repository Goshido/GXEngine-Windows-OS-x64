// verison 1.8

#ifndef GX_NETWORK
#define GX_NETWORK


#include <GXCommon/GXLogger.h>
#include <GXCommon/GXThread.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <WinSock2.h>
#include <GXCommon/GXRestoreWarnings.h>


#define GX_MAX_NETWORK_CLIENTS	64u
#define GX_SOCKET_BUFFER_SIZE	4194304u	// 4 Mb


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

	private:
		GXNetConnectionTCP ( const GXNetConnectionTCP &other ) = delete;
		GXNetConnectionTCP& operator = ( const GXNetConnectionTCP &other ) = delete;
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

		GXVoid SetOnNewTCPConnection ( PFNGXONSERVERNEWTCPCONNECTIONPROC callback );
		GXVoid SetOnDisconnectFunc ( PFNGXONSERVERDISCONNECTPROC callback );

		GXVoid SetOnMessageFuncTCP ( PFNGXONSERVERPMESSAGETCPPROC callback );
		GXVoid SetOnMessageFuncUDP ( PFNGXONSERVERPMESSAGEUDPPROC callback );

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

class GXNetClient
{
	private:
		sockaddr_in							serverAddressUDP;

		static SOCKET						socketTCP;
		static SOCKET						socketUDP;

		static GXThread*					threadTCP;
		static GXThread*					threadUDP;

		static PFNGXONCLIENTMESSAGEPROC		OnMessageTCP;
		static PFNGXONCLIENTMESSAGEPROC		OnMessageUDP;

		static GXUByte						bufferTCP[ GX_SOCKET_BUFFER_SIZE ];
		static GXUByte						bufferUDP[ GX_SOCKET_BUFFER_SIZE ];

		static GXNetClient*					instance;

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

		GXVoid SetOnMessageTCPFunc ( PFNGXONCLIENTMESSAGEPROC onMessageFunc );
		GXVoid SetOnMessageUDPFunc ( PFNGXONCLIENTMESSAGEPROC onMessageFunc );

	private:
		GXNetClient ();

		static GXUPointer GXTHREADCALL ReceiveTCP ( GXVoid* arg, GXThread &thread );
		static GXUPointer GXTHREADCALL ReceiveUDP ( GXVoid* arg, GXThread &thread );

		GXNetClient ( const GXNetClient &other ) = delete;
		GXNetClient& operator = ( const GXNetClient &other ) = delete;
};


#endif // GX_NETWORK
