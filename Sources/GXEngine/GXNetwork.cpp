// version 1.12

#include <GXEngine/GXNetwork.h>
#include <GXCommon/GXMemory.h>


static GXBool       gx_net_wsaLibraryServer = GX_FALSE;
static GXBool       gx_net_wsaLibraryClient = GX_FALSE;


enum class eGXNetModule : GXUByte
{
    Server,
    Client
};

struct GXClientInfo
{
    GXUInt      _id;
    SOCKET      _socket;
};

//------------------------------------------------------------------------------------------------

GXBool GXCALL GXNetWSAInit ( eGXNetModule who )
{
    if ( !gx_net_wsaLibraryServer && !gx_net_wsaLibraryClient )
    {
        WSADATA buff;

        GXBool perfect = WSAStartup ( 0x0202, (WSADATA*)&buff ) ? GX_FALSE : GX_TRUE;

        if ( !perfect )
        {
            GXLogA ( "GXNetWSAInit::Error - Ошибка WSAStartup\n" );
            return GX_FALSE;
        }
    }

    switch ( who )
    {
        case eGXNetModule::Server:
            gx_net_wsaLibraryServer = GX_TRUE;
        break;

        case eGXNetModule::Client:
            gx_net_wsaLibraryClient = GX_TRUE;
        break;
    }

    return GX_TRUE;
}

GXBool GXCALL GXNetWSADestroy ( eGXNetModule who )
{    
    GXBool needDestroy = GX_FALSE;

    switch ( who )
    {
        case eGXNetModule::Server:
        {
            if ( gx_net_wsaLibraryServer && !gx_net_wsaLibraryClient )
            {
                needDestroy = GX_TRUE;
                gx_net_wsaLibraryServer = GX_FALSE;
            }
        }
        break;

        case eGXNetModule::Client:
        {
            if ( !gx_net_wsaLibraryServer && gx_net_wsaLibraryClient )
            {
                needDestroy = GX_TRUE;
                gx_net_wsaLibraryClient = GX_FALSE;
            }
        }
        break;
    }

    switch ( who )
    {
        case eGXNetModule::Server:
            gx_net_wsaLibraryServer = GX_FALSE;
        break;

        case eGXNetModule::Client:
            gx_net_wsaLibraryClient = GX_FALSE;
        break;
    }


    if ( !needDestroy || WSACleanup () == 0 ) return GX_TRUE;

    GXLogA ( "GXNetWSADestroy::Error - Ошибка WSACleanup\n" );

    switch ( who )
    {
        case eGXNetModule::Server:
            gx_net_wsaLibraryServer = GX_TRUE;
        break;

        case eGXNetModule::Client:
            gx_net_wsaLibraryClient = GX_TRUE;
        break;
    }
    
    return GX_FALSE;
}

//------------------------------------------------------------------------------------------------------

GXNetConnectionTCP::GXNetConnectionTCP ():
    _socket ( INVALID_SOCKET ),
    _thread ( nullptr )
{
    // NOTHING
}

GXNetConnectionTCP::~GXNetConnectionTCP ()
{
    Destroy ();
}

GXBool GXNetConnectionTCP::IsFree ()
{
    return _socket == INVALID_SOCKET ? GX_TRUE : GX_FALSE;
}

SOCKET GXNetConnectionTCP::GetSocket ()
{
    return _socket;
}

GXThread* GXNetConnectionTCP::GetThread ()
{
    return _thread;
}

GXVoid GXNetConnectionTCP::Init ( SOCKET socketObject, GXThread* threadObject )
{
    GXSafeDelete ( _thread );
    _thread = threadObject;
    _socket = socketObject;
}

GXVoid GXNetConnectionTCP::Destroy ()
{
    _socket = INVALID_SOCKET;
}

//------------------------------------------------------------------------------------------------------

SOCKET                                  GXNetServer::_listenerTCP = INVALID_SOCKET;
SOCKET                                  GXNetServer::_listenerUDP = INVALID_SOCKET;

GXUInt                                  GXNetServer::_numClientsTCP = 0u;

GXNetConnectionTCP                      GXNetServer::_clientsTCP[ GX_NETWORK_MAX_NETWORK_CLIENTS ];

GXNetworkOnNewTCPConectionHandler       GXNetServer::_onNewConnectionTCP = nullptr;
GXNetworkOnServerDisconnectedHandler    GXNetServer::_onDisconnect = nullptr;

GXNetworkOnMessageTCPHandler            GXNetServer::_onMessageTCP = nullptr;
GXNetworkOnMessageUDPHandler            GXNetServer::_onMessageUDP = nullptr;

GXUByte                                 GXNetServer::_bufferTCP[ GX_NETWORK_SOCKET_BUFFER_SIZE ] = { 0u };
GXUByte                                 GXNetServer::_bufferUDP[ GX_NETWORK_SOCKET_BUFFER_SIZE ] = { 0u };

GXNetServer*                            GXNetServer::_instance = nullptr;


GXNetServer& GXCALL GXNetServer::GetInstance ()
{
    if ( !_instance )
        _instance = new GXNetServer ();

    return *_instance;
}

GXNetServer::~GXNetServer ()
{
    if ( !DestroyTCP () )
        GXLogA ( "GXNetServer::~GXNetServer::Warning - DestroyTCP отработал некорректно\n" ); 

    DestroyUDP ();

    _instance = nullptr;
}

GXBool GXNetServer::CreateTCP ( GXUShort port )
{
    DestroyTCP ();

    _numClientsTCP = 0u;

    if ( !GXNetWSAInit ( eGXNetModule::Server ) )
    {
        GXLogA ( "GXNetServer::CreateTCP::Error - Ошибка WSAStartup\n" );
        return GX_FALSE;
    }
    
    _listenerTCP = socket ( AF_INET, SOCK_STREAM, 0 );

    if ( _listenerTCP == INVALID_SOCKET )
    {
        GXLogA ( "GXNetServer::CreateTCP::Error - Ошибка создания сокета\n" );

        if ( _listenerUDP == INVALID_SOCKET )
            GXNetWSADestroy ( eGXNetModule::Server );

        return GX_FALSE;
    }

    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons ( port );
    local_addr.sin_addr.s_addr = 0;

    if ( bind ( _listenerTCP, reinterpret_cast<sockaddr*> ( &local_addr ), sizeof ( local_addr ) ) )
    {
        GXLogA ( "GXNetServer::CreateTCP::Error - Ошибка bind\n" );

        shutdown ( _listenerTCP, SD_BOTH );
        closesocket ( _listenerTCP );
        _listenerTCP = INVALID_SOCKET;

        if ( _listenerUDP == INVALID_SOCKET )
            GXNetWSADestroy ( eGXNetModule::Server );

        return GX_FALSE;
    }

    if ( listen ( _listenerTCP, 0x100 ) )
    {
        GXLogA ( "GXNetServer::CreateTCP::Error - Ошибка listen\n" );

        shutdown ( _listenerTCP, SD_BOTH );
        closesocket ( _listenerTCP );
        _listenerTCP = INVALID_SOCKET;

        if ( _listenerUDP == INVALID_SOCKET )
            GXNetWSADestroy ( eGXNetModule::Server );

        return GX_FALSE;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXThread" )
    _threadTCP = new GXThread ( &GXNetServer::ListenTCP, nullptr );
    _threadTCP->Start ();

    return GX_TRUE;
}

GXBool GXNetServer::CreateUDP ( GXUShort port )
{
    DestroyUDP ();

    if ( !GXNetWSAInit ( eGXNetModule::Server ) )
    {
        GXLogA ( "GXNetServer::CreateUDP::Error - Ошибка WSAStartup\n" );
        return GX_FALSE;
    }
    
    _listenerUDP = socket ( AF_INET, SOCK_DGRAM, 0 );

    if ( _listenerUDP == INVALID_SOCKET )
    {
        GXLogA ( "GXNetServer::CreateUDP::Error - Ошибка создания сокета\n" );

        if ( _listenerTCP == INVALID_SOCKET ) 
            GXNetWSADestroy ( eGXNetModule::Server );

        return GX_FALSE;
    }

    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons ( port );
    local_addr.sin_addr.s_addr = 0;

    if ( bind ( _listenerUDP, reinterpret_cast<sockaddr*> ( &local_addr ), sizeof ( local_addr ) ) )
    {
        GXLogA ( "GXNetServer::CreateUDP::Error - Ошибка bind\n" );

        shutdown ( _listenerUDP, SD_BOTH );
        closesocket ( _listenerUDP );
        _listenerUDP = INVALID_SOCKET;

        if ( _listenerTCP == INVALID_SOCKET )
            GXNetWSADestroy ( eGXNetModule::Server );

        return GX_FALSE;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXThread" )
    _threadUDP = new GXThread ( &GXNetServer::ServeClientUDP, nullptr );
    _threadUDP->Start ();

    return GX_TRUE;
}

GXBool GXNetServer::SendTCP ( GXUInt clientID, GXVoid* data, GXInt size )
{
    if ( _numClientsTCP < 1u ) return GX_FALSE;

    if ( clientID >= GX_NETWORK_MAX_NETWORK_CLIENTS || _clientsTCP[ clientID ].IsFree () )
        return GX_FALSE;

    SOCKET socket = _clientsTCP[ clientID ].GetSocket ();
    GXInt  trans = size;
    
    while ( trans > 0 )
    {
        GXInt t = send ( socket, static_cast<const GXChar*> ( data ) + size - trans, trans, 0 );

        if ( t == SOCKET_ERROR )
            return GX_FALSE;

        trans -= t;
    }

    return GX_TRUE;
}

GXBool GXNetServer::SendUDP ( const sockaddr_in &toClient, GXVoid* data, GXInt size )
{
    GXInt trans = size;
    GXInt len = static_cast<GXInt> ( sizeof ( sockaddr_in ) );

    while ( trans > 0 )
    {
        GXInt t = sendto ( _listenerUDP, static_cast<const GXChar*> ( data ) + size - trans, trans, 0, reinterpret_cast<const sockaddr*> ( &toClient ), len );

        if ( t == SOCKET_ERROR )
            return GX_FALSE;

        trans -= t;
    }

    return GX_TRUE;
}

GXBool GXNetServer::BroadcastTCP ( GXVoid* data, GXInt size )
{
    GXBool    result = GX_TRUE;

    for ( GXUChar i = 0u; i < GX_NETWORK_MAX_NETWORK_CLIENTS; ++i )
    {
        if ( _clientsTCP[ i ].IsFree () ) continue;

        result &= SendTCP ( i, data, size );
    }

    return result;
}

GXBool GXNetServer::BroadcastUDP ( GXVoid* data, GXInt size )
{
    GXBool    result = GX_TRUE;

    for ( GXUChar i = 0u; i < GX_NETWORK_MAX_NETWORK_CLIENTS; ++i )
    {
        sockaddr_in address;

        if ( GetClientIP ( address, i ) )
            result &= SendUDP ( address, data, size );
    }

    return result;
}

GXBool GXNetServer::DestroyTCP ()
{
    if ( _listenerTCP == INVALID_SOCKET ) return GX_TRUE;

    for ( GXUInt i = 0u; i < GX_NETWORK_MAX_NETWORK_CLIENTS; ++i )
    {
        if ( _clientsTCP[ i ].IsFree () ) continue;

        shutdown ( _clientsTCP[ i ].GetSocket (), SD_BOTH );
        closesocket ( _clientsTCP[ i ].GetSocket () );

        GXThread* thread = _clientsTCP[ i ].GetThread ();
        thread->Join ();

        _clientsTCP[ i ].Destroy ();
        delete thread;
    }

    shutdown ( _listenerTCP, SD_BOTH );
    closesocket ( _listenerTCP );
    _listenerTCP = INVALID_SOCKET;

    _threadTCP->Join ();
    GXSafeDelete ( _threadTCP );

    if ( _listenerUDP == INVALID_SOCKET )
        return GXNetWSADestroy ( eGXNetModule::Server );
    else
        return GX_TRUE;
}

GXBool GXNetServer::DestroyUDP ()
{
    if ( _listenerUDP == INVALID_SOCKET ) return GX_TRUE;

    closesocket ( _listenerUDP );
    _listenerUDP = INVALID_SOCKET;

    _threadUDP->Join ();
    GXSafeDelete ( _threadUDP );

    if ( _listenerTCP == INVALID_SOCKET ) return GXNetWSADestroy ( eGXNetModule::Server );

    return GX_TRUE;
}

GXBool GXNetServer::IsDeployedTCP ()
{
    return ( _listenerTCP != INVALID_SOCKET ) ? GX_TRUE : GX_FALSE;
}

GXBool GXNetServer::IsDeployedUDP ()
{
    return ( _listenerUDP != INVALID_SOCKET ) ? GX_TRUE : GX_FALSE;
}

GXVoid GXNetServer::SetOnNewTCPConnection ( GXNetworkOnNewTCPConectionHandler callback )
{
    _onNewConnectionTCP = callback;
}

GXVoid GXNetServer::SetOnDisconnectFunc ( GXNetworkOnServerDisconnectedHandler callback )
{
    _onDisconnect = callback;
}

GXVoid GXNetServer::SetOnMessageFuncTCP ( GXNetworkOnMessageTCPHandler callback )
{
    _onMessageTCP = callback;
}

GXVoid GXNetServer::SetOnMessageFuncUDP ( GXNetworkOnMessageUDPHandler callback )
{
    _onMessageUDP = callback;
}

GXNetServer::GXNetServer ():
    _threadTCP ( nullptr ),
    _threadUDP ( nullptr )
{
    // NOTHING
}

GXUPointer GXTHREADCALL GXNetServer::ListenTCP ( GXVoid* /*arg*/, GXThread& /*thread*/ )
{
    SOCKET client_socket;
    sockaddr_in client_addr;

    GXInt client_addr_size = sizeof ( client_addr );

    while ( ( client_socket = accept ( _listenerTCP, reinterpret_cast<sockaddr*> ( &client_addr ), &client_addr_size ) ) != INVALID_SOCKET )
    {
        HOSTENT* hst;
        hst = gethostbyaddr ( reinterpret_cast<const char*> ( &client_addr.sin_addr.s_addr ), 4, AF_INET );
        GXLogA ( "GXNetServer::ListenTCP::Info - Получена заявка на подключение. Пытаюсь зарегистрировать нового клиента...\n" );

        if ( _numClientsTCP > GX_NETWORK_MAX_NETWORK_CLIENTS )
        {
            GXLogA ( "GXNetServer::ListenTCP::Warning - Превышено количество одновременных подключений\n" );
        }
        else
        {
            GXInt id = GetFreeClientTCP ();

            if ( id == -1 ) continue;

            GXClientInfo* info = new GXClientInfo ();
            info->_id = static_cast<GXUInt> ( id );
            info->_socket = client_socket;

            GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXThread" )
            GXThread* newClientThread = new GXThread ( &GXNetServer::ServeClientTCP, info );
            _clientsTCP[ id ].Init ( client_socket, newClientThread );
            newClientThread->Start ();
        }
    }

    shutdown ( _listenerTCP, SD_BOTH );
    closesocket ( _listenerTCP );
    _listenerTCP = INVALID_SOCKET;

    if ( _listenerUDP == INVALID_SOCKET )
        GXNetWSADestroy ( eGXNetModule::Server );

    return 0u;
}

GXUPointer GXTHREADCALL GXNetServer::ServeClientTCP ( GXVoid* arg, GXThread& /*thread*/ )
{
    GXLogA ( "GXNetServer::ServeClientTCP::Info - Клиент зарегистрирован\n" );

    const GXClientInfo* info = static_cast<const GXClientInfo*> ( arg );

    ++_numClientsTCP;

    BOOL disable = FALSE;
    setsockopt ( info->_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*> ( &disable ), sizeof ( BOOL ) );

    if ( _onNewConnectionTCP )
        _onNewConnectionTCP ( info->_id );

    GXInt size;
    size = recv ( info->_socket, reinterpret_cast<char*> ( _bufferTCP ), GX_NETWORK_SOCKET_BUFFER_SIZE, 0 );

    while ( size != SOCKET_ERROR && size != 0 )
    {
        if ( _onMessageTCP ) 
            _onMessageTCP ( info->_id, _bufferTCP, static_cast<GXUInt> ( size ) );

        size = recv ( info->_socket, reinterpret_cast<char*> ( _bufferTCP ), GX_NETWORK_SOCKET_BUFFER_SIZE, 0 );
    }

    shutdown ( info->_socket, SD_BOTH );
    closesocket ( info->_socket );

    GXLogA ( "GXNetServer::ServeClientTCP::Info - Клиент отключился\n" );

    --_numClientsTCP;
    _clientsTCP[ info->_id ].Destroy ();

    if ( _onDisconnect )
        _onDisconnect ( info->_id );
    delete info;

    return 0u;
}

GXUPointer GXTHREADCALL GXNetServer::ServeClientUDP ( GXVoid* /*arg*/, GXThread& /*thread*/ )
{
    sockaddr_in client_addr;
    GXInt client_addr_size = sizeof ( client_addr );

    GXInt size = 0;
    size = recvfrom ( _listenerUDP, reinterpret_cast<char*> ( _bufferUDP ), GX_NETWORK_SOCKET_BUFFER_SIZE, 0, reinterpret_cast<sockaddr*> ( &client_addr ), &client_addr_size );

    while ( size != SOCKET_ERROR && size != 0 )
    {
        if ( _onMessageUDP )
            _onMessageUDP ( client_addr, _bufferUDP, static_cast<GXUInt> ( size ) );

        size = recvfrom ( _listenerUDP, reinterpret_cast<char*> ( _bufferUDP ), GX_NETWORK_SOCKET_BUFFER_SIZE, 0, reinterpret_cast<sockaddr*> ( &client_addr ), &client_addr_size );
    }

    closesocket ( _listenerUDP );
    _listenerUDP = INVALID_SOCKET;

    if ( _listenerTCP == INVALID_SOCKET )
        GXNetWSADestroy ( eGXNetModule::Server );

    return 0u;
}

GXInt GXNetServer::GetFreeClientTCP ()
{
    for ( GXUInt i = 0u; i < GX_NETWORK_MAX_NETWORK_CLIENTS; ++i )
    {
        if ( _clientsTCP[ i ].IsFree () )
            return static_cast<GXInt> ( i );
    }

    return -1;
}

GXInt GXNetServer::FindClientTCP ( SOCKET socket )
{
    for ( GXUInt i = 0u; i < GX_NETWORK_MAX_NETWORK_CLIENTS; ++i )
    {
        if ( !_clientsTCP[ i ].IsFree () && _clientsTCP[ i ].GetSocket () == socket )
            return static_cast<GXInt> ( i );
    }

    return -1;
}

GXBool GXNetServer::GetClientIP ( sockaddr_in &address, GXUInt clientID )
{
    if ( _clientsTCP[ clientID ].IsFree () ) return GX_FALSE;

    GXInt size = sizeof ( sockaddr_in );

    if ( getpeername ( _clientsTCP[ clientID ].GetSocket (), reinterpret_cast<sockaddr*> ( &address ), &size ) == 0 ) 
        return GX_TRUE;

    return GX_FALSE;
}

//----------------------------------------------------------------------------------------------

SOCKET                              GXNetClient::_socketTCP = INVALID_SOCKET;
SOCKET                              GXNetClient::_socketUDP = INVALID_SOCKET;

GXThread*                           GXNetClient::_threadTCP = nullptr;
GXThread*                           GXNetClient::_threadUDP = nullptr;

GXNetworkOnClientMessageHandler     GXNetClient::_onMessageTCP = nullptr;
GXNetworkOnClientMessageHandler     GXNetClient::_onMessageUDP = nullptr;

GXUByte                             GXNetClient::_bufferTCP[ GX_NETWORK_SOCKET_BUFFER_SIZE ] = { 0 };
GXUByte                             GXNetClient::_bufferUDP[ GX_NETWORK_SOCKET_BUFFER_SIZE ] = { 0 };

GXNetClient*                        GXNetClient::_instance = nullptr;


GXNetClient& GXCALL GXNetClient::GetInstance ()
{
    if ( !_instance )
        _instance = new GXNetClient ();

    return *_instance;
}

GXNetClient::~GXNetClient ()
{
    if ( !DisconnectTCP () )
        GXLogA ( "GXNetClient::~GXNetClient::Warning - DisconnectTCP завершился некорректно\n" );

    DestroyUDP ();

    _instance = nullptr;
}

GXBool GXNetClient::ConnectTCP ( const GXChar* url, GXUShort port )
{
    DisconnectTCP ();

    if ( !GXNetWSAInit ( eGXNetModule::Client ) )
    {
        GXLogA ( "GXNetClient::ConnectTCP::Error - Ошибка WSAStart\n" );
        return GX_FALSE;
    }

    if ( ( _socketTCP = socket ( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET  )
    {
        GXLogA ( "GXNetClient::ConnectTCP::Error - Ошибка Socket\n" );
        return GX_FALSE;
    }

    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons ( port );
    HOSTENT* hst;

    if ( inet_addr ( url ) != INADDR_NONE )
    {
        dest_addr.sin_addr.s_addr = inet_addr ( url );
    }
    else
    {
        hst = gethostbyname ( url );

        if ( hst )
        {
            GXUInt* alpha = reinterpret_cast<GXUInt*> ( &dest_addr.sin_addr );
            GXUInt** betta = reinterpret_cast<GXUInt**>( hst->h_addr_list );
            alpha[ 0 ] = betta[ 0 ][ 0 ];
        }
        else
        {
            GXLogA ( "GXNetClient::ConnectTCP::Error - Неверный адрес\n" );
            shutdown ( _socketTCP, SD_BOTH );
            closesocket ( _socketTCP );
            _socketTCP = INVALID_SOCKET;
            GXNetWSADestroy ( eGXNetModule::Client );
            return GX_FALSE;
        }
    }

    if ( connect ( _socketTCP, reinterpret_cast<sockaddr*> ( &dest_addr ), sizeof ( dest_addr ) ) == SOCKET_ERROR )
    {
        GXLogA ( "GXNetClient::ConnectTCP::Error - Невозможно установить соединение\n" );
        
        shutdown ( _socketTCP, SD_BOTH );
        closesocket ( _socketTCP );
        _socketTCP = INVALID_SOCKET;

        if ( _socketUDP == INVALID_SOCKET )
            GXNetWSADestroy ( eGXNetModule::Client );

        return GX_FALSE;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXThread" )
    _threadTCP = new GXThread ( &GXNetClient::ReceiveTCP, nullptr );
    _threadTCP->Start ();
    
    return GX_TRUE;
}

GXBool GXNetClient::DeployUDP ( const GXChar* url, GXUShort port )
{
    DestroyUDP ();

    if ( !GXNetWSAInit ( eGXNetModule::Client ) )
    {
        GXLogA ( "GXNetClient::DeployUDP::Error - Ошибка WSAStart\n" );
        return GX_FALSE;
    }

    if ( ( _socketUDP = socket ( AF_INET, SOCK_DGRAM, 0 ) ) == INVALID_SOCKET  )
    {
        GXLogA ( "GXNetClient::DeployUDP::Error - Ошибка Socket\n" );
        return GX_FALSE;
    }

    _serverAddressUDP.sin_family = AF_INET;
    _serverAddressUDP.sin_port = htons ( port );
    HOSTENT* hst;

    if ( inet_addr ( url ) != INADDR_NONE )
        _serverAddressUDP.sin_addr.s_addr = inet_addr ( url );
    else
    {
        hst = gethostbyname ( url );

        if ( hst )
        {
            GXUInt* alpha = reinterpret_cast<GXUInt*> ( &_serverAddressUDP.sin_addr );
            GXUInt** betta = reinterpret_cast<GXUInt**> ( hst->h_addr_list );
            alpha[ 0u ] = betta[ 0u ][ 0u ];
        }
        else
        {
            GXLogA ( "GXNetClient::ConnectTCP::Error - Неверный адрес\n" );

            closesocket ( _socketUDP );
            _socketUDP = INVALID_SOCKET;

            if ( _socketTCP == INVALID_SOCKET )
                GXNetWSADestroy ( eGXNetModule::Client );

            return GX_FALSE;
        }
    }

    sockaddr_in self_addr;
    self_addr.sin_family = AF_INET;
    self_addr.sin_port = ( htons ( 0 ) );
    self_addr.sin_addr.s_addr = INADDR_ANY;

    if ( bind ( _socketUDP, reinterpret_cast<sockaddr*> ( &self_addr ), sizeof ( self_addr ) ) == SOCKET_ERROR )
    {
        GXLogA ( "GXNetClient::DeployUDP::Error - Ошибка bind\n" );
        closesocket ( _socketUDP );

        if ( _socketTCP == INVALID_SOCKET )
            GXNetWSADestroy ( eGXNetModule::Client );

        return GX_FALSE;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXThread" )
    _threadUDP = new GXThread ( &GXNetClient::ReceiveUDP, nullptr );
    _threadUDP->Start ();
    
    return GX_TRUE;
}

GXBool GXNetClient::SendTCP ( GXVoid* data, GXInt size )
{
    if ( _socketTCP == INVALID_SOCKET ) return GX_FALSE;

    GXInt  trans = size;
    
    while ( trans > 0 )
    {
        GXInt t = send ( _socketTCP, reinterpret_cast<const char*> ( data ) + size - trans, trans, 0 );

        if ( t == SOCKET_ERROR )
            return GX_FALSE;

        trans -= t;
    }

    return GX_TRUE;
}

GXBool GXNetClient::SendUDP ( GXVoid* data, GXInt size )
{
    if ( _socketUDP == INVALID_SOCKET )
        return GX_FALSE;

    GXInt trans = size;
    constexpr GXInt len = static_cast<GXInt> ( sizeof ( sockaddr_in ) );

    while ( trans > 0 )
    {
        const GXInt t = sendto ( _socketUDP, reinterpret_cast<const char*> ( data ) + size - trans, trans, 0, reinterpret_cast<const sockaddr*> ( &_serverAddressUDP ), len );

        if ( t == SOCKET_ERROR )
            return GX_FALSE;

        trans -= t;
    }

    return GX_TRUE;
}

GXBool GXNetClient::DisconnectTCP ()
{
    if ( _socketTCP == INVALID_SOCKET )
        return GX_TRUE;

    shutdown ( _socketTCP, SD_BOTH );
    closesocket ( _socketTCP );
    _socketTCP = INVALID_SOCKET;

    _threadTCP->Join ();
    GXSafeDelete ( _threadTCP );

    return GXNetWSADestroy ( eGXNetModule::Client );
}

GXBool GXNetClient::DestroyUDP ()
{
    if ( _socketUDP == INVALID_SOCKET )
        return GX_TRUE;

    closesocket ( _socketUDP );
    _socketUDP = INVALID_SOCKET;

    _threadUDP->Join ();
    GXSafeDelete ( _threadUDP );

    if ( _socketTCP == INVALID_SOCKET )
        return GXNetWSADestroy ( eGXNetModule::Client );

    return GX_TRUE;
}

GXBool GXNetClient::IsConnectedTCP ()
{
    return ( _socketTCP != INVALID_SOCKET ) ? GX_TRUE : GX_FALSE;
}

GXBool GXNetClient::IsDeployedUDP ()
{
    return ( _socketUDP != INVALID_SOCKET ) ? GX_TRUE : GX_FALSE;
}

GXVoid GXNetClient::SetOnMessageTCPFunc ( GXNetworkOnClientMessageHandler callback )
{
    _onMessageTCP = callback;
}

GXVoid GXNetClient::SetOnMessageUDPFunc ( GXNetworkOnClientMessageHandler callback )
{
    _onMessageUDP = callback;
}

GXNetClient::GXNetClient ()
{
    // NOTHING
}

GXUPointer GXTHREADCALL GXNetClient::ReceiveTCP ( GXVoid* /*arg*/, GXThread& /*thread*/ )
{
    GXLogA ( "GXNetClient::ReceiveTCP::Info - Соединение успешно создано\n" );

    constexpr BOOL disable = FALSE;
    setsockopt ( _socketTCP, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*> ( &disable ), sizeof ( BOOL ) );

    GXInt size = 0;
    size = recv ( _socketTCP, reinterpret_cast<char*> ( _bufferTCP ), GX_NETWORK_SOCKET_BUFFER_SIZE, 0 );

    while ( size != SOCKET_ERROR && size != 0 )
    {
        if ( _onMessageTCP )
            _onMessageTCP ( _bufferTCP, static_cast<GXUInt> ( size ) );

        size = recv ( _socketTCP, reinterpret_cast<char*> ( _bufferTCP ), GX_NETWORK_SOCKET_BUFFER_SIZE, 0 );
    }

    return 0u;
}

GXUPointer GXTHREADCALL GXNetClient::ReceiveUDP ( GXVoid* /*arg*/, GXThread& /*thread*/ )
{
    GXLogA ( "GXNetClient::ReceiveUDP::Info - UDP сокет успешно поднят\n" );

    sockaddr_in address;
    constexpr GXInt structSize = static_cast<GXInt> ( sizeof ( address ) );
    
    GXInt size = 0;
    size = recvfrom ( _socketUDP, reinterpret_cast<char*> ( _bufferUDP ), GX_NETWORK_SOCKET_BUFFER_SIZE, 0, reinterpret_cast<sockaddr*> ( &address ), const_cast<GXInt*> ( &structSize ) );

    while ( size != SOCKET_ERROR && size != 0 )
    {        
        if ( _onMessageUDP )
            _onMessageUDP ( _bufferUDP, static_cast<GXUInt> ( size ) );

        size = recvfrom ( _socketUDP, reinterpret_cast<char*> ( _bufferUDP ), GX_NETWORK_SOCKET_BUFFER_SIZE, 0, reinterpret_cast<sockaddr*> ( &address ), const_cast<GXInt*> ( &structSize ) );
    }

    return 0u;
}
