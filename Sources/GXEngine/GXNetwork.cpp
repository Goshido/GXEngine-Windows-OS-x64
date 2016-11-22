//version 1.5

#include <GXEngine/GXNetwork.h>
#include <GXCommon/GXCommon.h>


GXBool		gx_net_wsaLibraryServer = GX_FALSE;
GXBool		gx_net_wsaLibraryClient = GX_FALSE;


enum GXNetModule
{
	GX_SERVER,
	GX_CLIENT
};

struct GXClientInfo
{
	GXUInt	id;
	SOCKET	socket;
};

//------------------------------------------------------------------------------------------------

GXBool GXCALL GXNetWSAInit ( GXNetModule who )
{
	if ( !gx_net_wsaLibraryServer && !gx_net_wsaLibraryClient )
	{
		WSADATA buff;

		GXBool perfect = WSAStartup ( 0x0202, (WSADATA*)&buff ) ? GX_FALSE : GX_TRUE;
		if ( !perfect )
		{
			GXLogW ( L"GXNetWSAInit::Error - Ошибка WSAStartup\n" );
			return GX_FALSE;
		}
	}

	switch ( who )
	{
		case GX_SERVER:
			gx_net_wsaLibraryServer = GX_TRUE;
		break;

		case GX_CLIENT:
			gx_net_wsaLibraryClient = GX_TRUE;
		break;
	}

	return GX_TRUE;
}

GXBool GXCALL GXNetWSADestroy ( GXNetModule who )
{	
	GXBool needDestroy = GX_FALSE;

	switch ( who )
	{
		case GX_SERVER:
			if ( gx_net_wsaLibraryServer && !gx_net_wsaLibraryClient )
			{
				needDestroy = GX_TRUE;
				gx_net_wsaLibraryServer = GX_FALSE;
			}
		break;

		case GX_CLIENT:
			if ( !gx_net_wsaLibraryServer && gx_net_wsaLibraryClient )
			{
				needDestroy = GX_TRUE;
				gx_net_wsaLibraryClient = GX_FALSE;
			}
		break;
	}

	switch ( who )
	{
		case GX_SERVER:
			gx_net_wsaLibraryServer = GX_FALSE;
		break;

		case GX_CLIENT:
			gx_net_wsaLibraryClient = GX_FALSE;
		break;
	}


	if ( !needDestroy ) return GX_TRUE;

	GXBool perfect = WSACleanup () ? GX_FALSE : GX_TRUE;
	if ( !perfect )
	{
		GXLogW ( L"GXNetWSADestroy::Error - Ошибка WSACleanup\n" );

		switch ( who )
		{
			case GX_SERVER:
				gx_net_wsaLibraryServer = GX_TRUE;
			break;

			case GX_CLIENT:
				gx_net_wsaLibraryClient = GX_TRUE;
			break;
		}
	}
	
	return perfect;
}

//------------------------------------------------------------------------------------------------------

GXNetConnectionTCP::GXNetConnectionTCP ()
{
	socket = INVALID_SOCKET;
	thread = 0;
}

GXNetConnectionTCP::~GXNetConnectionTCP ()
{
	Destroy ();
}

GXBool GXNetConnectionTCP::IsFree ()
{
	return socket == INVALID_SOCKET ? GX_TRUE : GX_FALSE;
}

SOCKET GXNetConnectionTCP::GetSocket ()
{
	return socket;
}

GXThread* GXNetConnectionTCP::GetThread ()
{
	return thread;
}

GXVoid GXNetConnectionTCP::Init ( SOCKET socket, GXThread* thread )
{
	if ( this->thread ) delete thread;
	this->thread = thread;

	this->socket = socket;
}

GXVoid GXNetConnectionTCP::Destroy ()
{
	socket = INVALID_SOCKET;
}

//------------------------------------------------------------------------------------------------------

SOCKET								GXNetServer::listenerTCP = INVALID_SOCKET;
SOCKET								GXNetServer::listenerUDP = INVALID_SOCKET;

GXUInt								GXNetServer::numClientsTCP = 0;

GXNetConnectionTCP					GXNetServer::clientsTCP[ GX_MAX_NETWORK_CLIENTS ];

PFNGXONSERVERNEWTCPCONNECTIONPROC	GXNetServer::OnNewConnectionTCP = 0;
PFNGXONSERVERDISCONNECTPROC			GXNetServer::OnDisconnect = 0;

PFNGXONSERVERPMESSAGETCPPROC		GXNetServer::OnMessageTCP = 0;
PFNGXONSERVERPMESSAGEUDPPROC		GXNetServer::OnMessageUDP = 0;


GXNetServer::GXNetServer ()
{
	listenerTCP = listenerUDP = INVALID_SOCKET;
	numClientsTCP = 0;
	OnMessageTCP = 0;
	OnMessageUDP = 0;
	OnNewConnectionTCP = 0;
	OnDisconnect = 0;

	threadTCP = threadUDP = 0;
}

GXNetServer::~GXNetServer ()
{
	if ( !DestroyTCP () )
		GXLogW ( L"GXNetServer::~GXNetServer::Warning - DestroyTCP отработал некорректно\n" ); 

	DestroyUDP ();
}

GXBool GXNetServer::CreateTCP ( GXUShort port )
{
	DestroyTCP ();

	numClientsTCP = 0;

	if ( !GXNetWSAInit ( GX_SERVER ) )
	{
		GXLogW ( L"GXNetServer::CreateTCP::Error - Ошибка WSAStartup\n" );
		return GX_FALSE;
	}
			
	if ( ( listenerTCP = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		GXLogW ( L"GXNetServer::CreateTCP::Error - Ошибка создания сокета\n" );

		if ( listenerUDP == INVALID_SOCKET )
			GXNetWSADestroy ( GX_SERVER );

		return GX_FALSE;
	}

	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons ( port );
	local_addr.sin_addr.s_addr = 0;

	if ( bind ( listenerTCP, (sockaddr*)&local_addr, sizeof ( local_addr ) ) )
	{
		GXLogW ( L"GXNetServer::CreateTCP::Error - Ошибка bind\n" );

		shutdown ( listenerTCP, SD_BOTH );
		closesocket ( listenerTCP );
		listenerTCP = INVALID_SOCKET;

		if ( listenerUDP == INVALID_SOCKET )
			GXNetWSADestroy ( GX_SERVER );

		return GX_FALSE;
	}

	if ( listen ( listenerTCP, 0x100 ) )
	{
		GXLogW ( L"GXNetServer::CreateTCP::Error - Ошибка listen\n" );

		shutdown ( listenerTCP, SD_BOTH );
		closesocket ( listenerTCP );
		listenerTCP = INVALID_SOCKET;

		if ( listenerUDP == INVALID_SOCKET )
			GXNetWSADestroy ( GX_SERVER );

		return GX_FALSE;
	}

	threadTCP = new GXThread ( &ListenTCP, 0, GX_WORKING ); 

	return GX_TRUE;
}

GXBool GXNetServer::CreateUDP ( GXUShort port )
{
	DestroyUDP ();

	if ( !GXNetWSAInit ( GX_SERVER ) )
	{
		GXLogW ( L"GXNetServer::CreateUDP::Error - Ошибка WSAStartup\n" );
		return GX_FALSE;
	}
			
	if ( ( listenerUDP = socket ( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		GXLogW ( L"GXNetServer::CreateUDP::Error - Ошибка создания сокета\n" );

		if ( listenerTCP == INVALID_SOCKET ) 
			GXNetWSADestroy ( GX_SERVER );

		return GX_FALSE;
	}

	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons ( port );
	local_addr.sin_addr.s_addr = 0;

	if ( bind ( listenerUDP, (sockaddr*)&local_addr, sizeof ( local_addr ) ) )
	{
		GXLogW ( L"GXNetServer::CreateUDP::Error - Ошибка bind\n" );

		shutdown ( listenerUDP, SD_BOTH );
		closesocket ( listenerUDP );
		listenerUDP = INVALID_SOCKET;

		if ( listenerTCP == INVALID_SOCKET )
			GXNetWSADestroy ( GX_SERVER );

		return GX_FALSE;
	}

	threadUDP = new GXThread ( &ServeClientUDP, 0, GX_WORKING );

	return GX_TRUE;
}

GXBool GXNetServer::SendTCP ( GXUInt clientID, GXVoid* data, GXInt size )
{
	if ( numClientsTCP < 1 ) return GX_FALSE;

	if ( clientID >= GX_MAX_NETWORK_CLIENTS || clientsTCP[ clientID ].IsFree () )
		return GX_FALSE;

	SOCKET socket = clientsTCP[ clientID ].GetSocket ();
	GXInt  trans = size;
	
	while ( trans > 0 )
	{
		GXInt t = send ( socket, (const GXChar*)data + size - trans, trans, 0 );
		if ( t == SOCKET_ERROR )
			return GX_FALSE;

		trans -= t;
	}

	return GX_TRUE;
}

GXBool GXNetServer::SendUDP ( const sockaddr_in &toClient, GXVoid* data, GXInt size )
{
	GXInt trans = size;
	GXInt len = sizeof ( sockaddr_in );

	while ( trans > 0 )
	{
		GXInt t = sendto ( listenerUDP, (const GXChar*)data + size - trans, trans, 0, (const sockaddr*)&toClient, len );
		if ( t == SOCKET_ERROR )
			return GX_FALSE;

		trans -= t;
	}

	return GX_TRUE;
}

GXBool GXNetServer::BroadcastTCP ( GXVoid* data, GXInt size )
{
	GXBool	result = GX_TRUE;

	for ( GXUChar i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		if ( clientsTCP[ i ].IsFree () ) continue;

		result &= SendTCP ( i, data, size );
	}

	return result;
}

GXBool GXNetServer::BroadcastUDP ( GXVoid* data, GXInt size )
{
	GXBool	result = GX_TRUE;

	for ( GXUChar i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		sockaddr_in address;
		if ( GetClientIP ( address, i ) )
			result &= SendUDP ( address, data, size );
	}

	return result;
}

GXBool GXNetServer::DestroyTCP ()
{
	if ( listenerTCP == INVALID_SOCKET ) return GX_TRUE;

	for ( GXUInt i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		if ( clientsTCP[ i ].IsFree () ) continue;

		shutdown ( clientsTCP[ i ].GetSocket (), SD_BOTH );
		closesocket ( clientsTCP[ i ].GetSocket () );

		GXThread* thread = clientsTCP[ i ].GetThread ();
		thread->Join ();

		clientsTCP[ i ].Destroy ();
		delete thread;
	}

	shutdown ( listenerTCP, SD_BOTH );
	closesocket ( listenerTCP );
	listenerTCP = INVALID_SOCKET;

	threadTCP->Join ();
	GXSafeDelete ( threadTCP );

	if ( listenerUDP == INVALID_SOCKET )
		return GXNetWSADestroy ( GX_SERVER );
	else
		return GX_TRUE;
}

GXBool GXNetServer::DestroyUDP ()
{
	if ( listenerUDP == INVALID_SOCKET ) return GX_TRUE;

	closesocket ( listenerUDP );
	listenerUDP = INVALID_SOCKET;

	threadUDP->Join ();
	GXSafeDelete ( threadUDP );

	if ( listenerTCP == INVALID_SOCKET )
		return GXNetWSADestroy ( GX_SERVER );
	else
		return GX_TRUE;
}

GXBool GXNetServer::IsDeployedTCP ()
{
	return ( listenerTCP != INVALID_SOCKET ) ? GX_TRUE : GX_FALSE;
}

GXBool GXNetServer::IsDeployedUDP ()
{
	return ( listenerUDP != INVALID_SOCKET ) ? GX_TRUE : GX_FALSE;
}

GXVoid GXNetServer::SetOnNewTCPConnection ( PFNGXONSERVERNEWTCPCONNECTIONPROC callback )
{
	OnNewConnectionTCP = callback;
}

GXVoid GXNetServer::SetOnDisconnectFunc ( PFNGXONSERVERDISCONNECTPROC callback )
{
	OnDisconnect = callback;
}

GXVoid GXNetServer::SetOnMessageFuncTCP ( PFNGXONSERVERPMESSAGETCPPROC callback )
{
	OnMessageTCP = callback;
}

GXVoid GXNetServer::SetOnMessageFuncUDP ( PFNGXONSERVERPMESSAGEUDPPROC callback )
{
	OnMessageUDP = callback;
}

GXDword GXTHREADCALL GXNetServer::ListenTCP ( GXVoid* arg )
{
	SOCKET client_socket;
	sockaddr_in client_addr;

	GXInt client_addr_size = sizeof ( client_addr );

	while ( ( client_socket = accept ( listenerTCP, (sockaddr*)&client_addr, &client_addr_size ) ) != INVALID_SOCKET )
	{
		HOSTENT* hst;
		hst = gethostbyaddr ( (GXChar*)&client_addr.sin_addr.s_addr, 4, AF_INET );
		GXLogW ( L"GXNetServer::ListenTCP::Info - Получена заявка на подключение. Пытаюсь зарегистрировать нового клиента...\n" );

		if ( numClientsTCP > GX_MAX_NETWORK_CLIENTS )
			GXLogW ( L"GXNetServer::ListenTCP::Warning - Превышено количество одновременных подключений\n" );
		else
		{
			GXInt id = GetFreeClientTCP ();
			if ( id == -1 ) continue;

			GXClientInfo* info = new GXClientInfo ();
			info->id = id;
			info->socket = client_socket;

			GXThread* newClientThread = new GXThread ( &ServeClientTCP, info, GX_WORKING );
			clientsTCP[ id ].Init ( client_socket, newClientThread );
		}
	}

	shutdown ( listenerTCP, SD_BOTH );
	closesocket ( listenerTCP );
	listenerTCP = INVALID_SOCKET;

	if ( listenerUDP == INVALID_SOCKET )
		GXNetWSADestroy ( GX_SERVER );

	return 0;
}

GXChar gx_networkServerBufferTCP[ GX_SOCKET_BUFFER_SIZE ] = { 0 };
GXDword GXTHREADCALL GXNetServer::ServeClientTCP ( GXVoid* arg )
{
	GXLogW ( L"GXNetServer::ServeClientTCP::Info - Клиент зарегистрирован\n" );

	const GXClientInfo* info = (const GXClientInfo*)arg;

	numClientsTCP++;

	BOOL disable = FALSE;
	setsockopt ( info->socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&disable, sizeof ( BOOL ) );

	if ( OnNewConnectionTCP )
		OnNewConnectionTCP ( info->id );

	GXInt size;
	while ( ( size = recv ( info->socket, gx_networkServerBufferTCP, GX_SOCKET_BUFFER_SIZE, 0 ) ) && size != SOCKET_ERROR && size != 0 )
	{
		if ( OnMessageTCP ) OnMessageTCP ( info->id, gx_networkServerBufferTCP, size );
	}

	shutdown ( info->socket, SD_BOTH );
	closesocket ( info->socket );

	GXLogW ( L"GXNetServer::ServeClientTCP::Info - Клиент отключился\n" );

	numClientsTCP--;
	clientsTCP[ info->id ].Destroy ();
	if ( OnDisconnect )
		OnDisconnect ( info->id );
	delete info;

	return 0;
}

GXChar gx_networkServerBufferUDP[ GX_SOCKET_BUFFER_SIZE ] = { 0 };
DWORD WINAPI GXNetServer::ServeClientUDP ( LPVOID arg )
{
	sockaddr_in client_addr;
	GXInt client_addr_size = sizeof ( client_addr );
	GXInt size = 0;

	while ( ( size = recvfrom ( listenerUDP, gx_networkServerBufferUDP, GX_SOCKET_BUFFER_SIZE, 0, (sockaddr*)&client_addr, &client_addr_size ) ) != SOCKET_ERROR && size != 0 )
	{
		if ( OnMessageUDP )
			OnMessageUDP ( client_addr, gx_networkServerBufferUDP, size );
	}

	closesocket ( listenerUDP );
	listenerUDP = INVALID_SOCKET;

	if ( listenerTCP == INVALID_SOCKET )
		GXNetWSADestroy ( GX_SERVER );

	return 0;
}

GXInt GXNetServer::GetFreeClientTCP ()
{
	for ( GXInt i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		if ( clientsTCP[ i ].IsFree () )
			return i;
	}

	return -1;
}

GXInt GXNetServer::FindClientTCP ( SOCKET socket )
{
	for ( GXInt i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		if ( !clientsTCP[ i ].IsFree () && clientsTCP[ i ].GetSocket () == socket )
			return i;
	}

	return -1;
}

GXBool GXNetServer::GetClientIP ( sockaddr_in &address, GXUInt clientID )
{
	if ( clientsTCP[ clientID ].IsFree () ) return GX_FALSE;

	GXInt size = sizeof ( sockaddr_in );

	if ( getpeername ( clientsTCP[ clientID ].GetSocket (), (sockaddr*)&address, &size ) == 0 ) 
		return GX_TRUE;

	return GX_FALSE;
}

//----------------------------------------------------------------------------------------------

SOCKET						GXNetClient::socketTCP = INVALID_SOCKET;
SOCKET						GXNetClient::socketUDP = INVALID_SOCKET;

GXThread*					GXNetClient::threadTCP = 0;
GXThread*					GXNetClient::threadUDP = 0;

PFNGXONCLIENTMESSAGEPROC	GXNetClient::OnMessageTCP = 0;
PFNGXONCLIENTMESSAGEPROC	GXNetClient::OnMessageUDP = 0;


GXNetClient::GXNetClient ()
{
	socketTCP = socketUDP = INVALID_SOCKET;
	threadTCP = threadUDP = 0;
	OnMessageTCP = OnMessageUDP = 0;
}

GXNetClient::~GXNetClient ()
{
	if ( !DisconnectTCP () )
		GXLogW ( L"GXNetClient::~GXNetClient::Warning - DisconnectTCP завершился некорректно\n" );

	DestroyUDP ();
}

GXBool GXNetClient::ConnectTCP ( const GXChar* url, GXUShort port )
{
	DisconnectTCP ();

	if ( !GXNetWSAInit ( GX_CLIENT ) )
	{
		GXLogW ( L"GXNetClient::ConnectTCP::Error - Ошибка WSAStart\n" );
		return GX_FALSE;
	}

	if ( ( socketTCP = socket ( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET  )
	{
		GXLogW ( L"GXNetClient::ConnectTCP::Error - Ошибка Socket\n" );
		return GX_FALSE;
	}

	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons ( port );
	HOSTENT* hst;

	if ( inet_addr ( url ) != INADDR_NONE )
		dest_addr.sin_addr.s_addr = inet_addr ( url );
	else
		if ( hst = gethostbyname ( url ) )
			( (GXULong*)&dest_addr.sin_addr )[ 0 ] = ( (GXULong**)hst->h_addr_list )[ 0 ][ 0 ];
		else
		{
			GXLogW ( L"GXNetClient::ConnectTCP::Error - Неверный адрес\n" );
			shutdown ( socketTCP, SD_BOTH );
			closesocket ( socketTCP );
			socketTCP = INVALID_SOCKET;
			GXNetWSADestroy ( GX_CLIENT );
			return GX_FALSE;
		}

	if ( connect ( socketTCP, (sockaddr*)&dest_addr, sizeof ( dest_addr ) ) == SOCKET_ERROR )
	{
		GXLogW ( L"GXNetClient::ConnectTCP::Error - Невозможно установить соединение\n" );
		
		shutdown ( socketTCP, SD_BOTH );
		closesocket ( socketTCP );
		socketTCP = INVALID_SOCKET;

		if ( socketUDP == INVALID_SOCKET )
			GXNetWSADestroy ( GX_CLIENT );

		return GX_FALSE;
	}

	threadTCP = new GXThread ( &ReceiveTCP, 0, GX_WORKING );
	
	return GX_TRUE;
}

GXBool GXNetClient::DeployUDP ( const GXChar* url, GXUShort port )
{
	DestroyUDP ();

	if ( !GXNetWSAInit ( GX_CLIENT ) )
	{
		GXLogW ( L"GXNetClient::DeployUDP::Error - Ошибка WSAStart\n" );
		return 1;
	}

	if ( ( socketUDP = socket ( AF_INET, SOCK_DGRAM, 0 ) ) == INVALID_SOCKET  )
	{
		GXLogW ( L"GXNetClient::DeployUDP::Error - Ошибка Socket\n" );
		return 1;
	}

	serverAddressUDP.sin_family = AF_INET;
	serverAddressUDP.sin_port = htons ( port );
	HOSTENT* hst;

	if ( inet_addr ( url ) != INADDR_NONE )
		serverAddressUDP.sin_addr.s_addr = inet_addr ( url );
	else
		if ( hst = gethostbyname ( url ) )
			( (GXULong*)&serverAddressUDP.sin_addr )[ 0 ] = ( (GXULong**)hst->h_addr_list )[ 0 ][ 0 ];
		else
		{
			GXLogW ( L"GXNetClient::ConnectTCP::Error - Неверный адрес\n" );

			closesocket ( socketUDP );
			socketUDP = INVALID_SOCKET;

			if ( socketTCP == INVALID_SOCKET )
				GXNetWSADestroy ( GX_CLIENT );

			return GX_FALSE;
		}

	sockaddr_in self_addr;
	self_addr.sin_family = AF_INET;
	self_addr.sin_port = ( htons ( 0 ) );
	self_addr.sin_addr.s_addr = INADDR_ANY;
	if ( bind ( socketUDP, (sockaddr*)&self_addr, sizeof ( self_addr ) ) == SOCKET_ERROR )
	{
		GXLogW ( L"GXNetClient::DeployUDP::Error - Ошибка bind\n" );
		closesocket ( socketUDP );

		if ( socketTCP == INVALID_SOCKET )
			GXNetWSADestroy ( GX_CLIENT );

		return 1;
	}

	threadUDP = new GXThread ( &ReceiveUDP, 0, GX_WORKING );
	
	return GX_TRUE;
}

GXBool GXNetClient::SendTCP ( GXVoid* data, GXInt size )
{
	if ( socketTCP == INVALID_SOCKET ) return GX_FALSE;

	GXInt  trans = size;
	
	while ( trans > 0 )
	{
		GXInt t = send ( socketTCP, (const GXChar*)data + size - trans, trans, 0 );
		if ( t == SOCKET_ERROR )
			return GX_FALSE;

		trans -= t;
	}

	return GX_TRUE;
}

GXBool GXNetClient::SendUDP ( GXVoid* data, GXInt size )
{
	if ( socketUDP == INVALID_SOCKET ) return GX_FALSE;

	GXInt trans = size;
	GXInt len = sizeof ( sockaddr_in );

	while ( trans > 0 )
	{
		GXInt t = sendto ( socketUDP, (const GXChar*)data + size - trans, trans, 0, (const sockaddr*)&serverAddressUDP, len );
		if ( t == SOCKET_ERROR )
			return GX_FALSE;

		trans -= t;
	}

	return GX_TRUE;
}

GXBool GXNetClient::DisconnectTCP ()
{
	if ( socketTCP == INVALID_SOCKET ) return GX_TRUE;

	shutdown ( socketTCP, SD_BOTH );
	closesocket ( socketTCP );
	socketTCP = INVALID_SOCKET;

	threadTCP->Join ();
	GXSafeDelete ( threadTCP );

	return GXNetWSADestroy ( GX_CLIENT );
}

GXBool GXNetClient::DestroyUDP ()
{
	if ( socketUDP == INVALID_SOCKET ) return GX_TRUE;

	closesocket ( socketUDP );
	socketUDP = INVALID_SOCKET;

	threadUDP->Join ();
	GXSafeDelete ( threadUDP );

	if ( socketTCP == INVALID_SOCKET )
		return GXNetWSADestroy ( GX_CLIENT );
	else
		return GX_TRUE;
}

GXBool GXNetClient::IsConnectedTCP ()
{
	return ( socketTCP != INVALID_SOCKET ) ? GX_TRUE : GX_FALSE;
}

GXBool GXNetClient::IsDeployedUDP ()
{
	return ( socketUDP != INVALID_SOCKET ) ? GX_TRUE : GX_FALSE;
}

GXVoid GXNetClient::SetOnMessageTCPFunc ( PFNGXONCLIENTMESSAGEPROC onMessageFunc )
{
	OnMessageTCP = onMessageFunc;
}

GXVoid GXNetClient::SetOnMessageUDPFunc ( PFNGXONCLIENTMESSAGEPROC onMessageFunc )
{
	OnMessageUDP = onMessageFunc;
}

GXChar gx_networkClientBufferTCP[ GX_SOCKET_BUFFER_SIZE ] = { 0 };
DWORD WINAPI GXNetClient::ReceiveTCP ( LPVOID arg )
{
	GXLogW ( L"GXNetClient::ReceiveTCP::Info - Соединение успешно создано\n" );

	BOOL disable = FALSE;
	setsockopt ( socketTCP, IPPROTO_TCP, TCP_NODELAY, (const char*)&disable, sizeof ( BOOL ) );

	GXInt size;
	while ( ( size = recv ( socketTCP, gx_networkClientBufferTCP, GX_SOCKET_BUFFER_SIZE, 0 ) ) && size != SOCKET_ERROR && size != 0 )
	{
		if ( OnMessageTCP )
			OnMessageTCP ( gx_networkClientBufferTCP, size );
	}

	return 0;
}

GXChar gx_networkClientBufferUDP[ GX_SOCKET_BUFFER_SIZE ] = { 0 };
DWORD WINAPI GXNetClient::ReceiveUDP ( LPVOID arg )
{
	GXLogW ( L"GXNetClient::ReceiveUDP::Info - UDP сокет успешно поднят\n" );

	sockaddr_in client_addr;
	GXInt client_addr_size = sizeof ( client_addr );
	GXInt size = 0;

	while ( ( size = recvfrom ( socketUDP, gx_networkClientBufferUDP, GX_SOCKET_BUFFER_SIZE, 0, (sockaddr*)&client_addr, &client_addr_size ) ) != SOCKET_ERROR && size != 0 )
	{		
		if ( OnMessageUDP )
			OnMessageUDP ( gx_networkClientBufferUDP, size );
	}

	return 0;
}