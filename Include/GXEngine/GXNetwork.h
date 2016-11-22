//verison 1.5

#ifndef GX_NETWORK
#define GX_NETWORK


#include <WinSock2.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXThread.h>


#define GX_MAX_NETWORK_CLIENTS	64
#define GX_SOCKET_BUFFER_SIZE	1024 * 1024 * 4		//64 кЅайта


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

		GXVoid Init ( SOCKET socket, GXThread* thread );
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

	public:
		GXNetServer ();
		virtual ~GXNetServer ();

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
		static GXDword GXTHREADCALL ListenTCP ( GXVoid* arg );
		
		static GXDword GXTHREADCALL ServeClientTCP ( GXVoid* arg );
		static GXDword GXTHREADCALL ServeClientUDP ( GXVoid* arg );

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

	public:
		GXNetClient ();
		virtual ~GXNetClient ();

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
		static GXDword GXTHREADCALL ReceiveTCP ( GXVoid* arg );
		static GXDword GXTHREADCALL ReceiveUDP ( GXVoid* arg );
};


#endif //GX_NETWORK