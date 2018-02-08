///////////////////////////////////////////////////////////////////////////////
// Simple message broadcasting server for SpaceShooter!                      //
///////////////////////////////////////////////////////////////////////////////
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "main.h"
#include "ServerNetwork.h"
#include "send_packet.h"
#include "process_packet.h"
#include "Ship.h"
#include "asteroid.h"
#include "timer\timer.h"

#include <vector>

#ifdef _DEBUG
#include <io.h>
#endif

using namespace std;

HNet::_ServerNetwork NetObj;
struct _Ship g_ShipList[MAX_CLIENT_CONNECTION + 1]; // Ship list. Array Index number is same as SessionIndex number from Network Library.
std::vector <_Asteroid *> g_AsteroidList; // Asteroid list.

_Timer g_LoopTimer;


#ifdef _DEBUG
void log( char *szFormat, ... )
{
    char Buff[1024];
    char Msg[1024];
    va_list arg;

    va_start( arg, szFormat );
    _vsnprintf_s( Msg, 1024, szFormat, arg );
    va_end( arg );

    sprintf_s( Buff, 1024, "%s", Msg );
    _write( 1, Buff, strlen( Buff ) );
}
#endif

bool init_game_server( void )
{ // Initialize whatever you need.

    // Initialize two asteroid. This is for simple example.
    g_AsteroidList.push_back( new _Asteroid( 1, 100, 100, 1 ) );
    g_AsteroidList.push_back( new _Asteroid( 2, 700, 500, 1 ) );

    return true;
}

int main( void )
{
    if( false == init_game_server() )
    {
        return 0;
    }

    if( false == NetObj.InitNet( HNet::APPTYPE_SERVER, HNet::PROTOCOL_TCP, 3456 ) )
    {
#ifdef _DEBUG
        log( "%s", NetObj.GetErrorMessage() );
#endif
        return 0;
    }
#ifdef _DEBUG
    log( "%s", "\n Server network initialized!" );
    log( "%s", "\n Network thread started! Ready to accept & recevie the message." );
#endif
    Sleep( 1000 ); // Wait for a while to make sure everything is ok.

    g_LoopTimer.GetTimer_sec(); // To initialize the timer.
    struct HNet::_ProcessSession *ToProcessSessoin;
    while( 1 )
    {
        while( nullptr != ( ToProcessSessoin = NetObj.GetProcessList()->GetFirstSession() ) )
        {
            switch( ToProcessSessoin->SessionState )
            {
                case HNet::SESSION_STATE_NEWCONNECTION:
                { // New connection request arrived.
#ifdef _DEBUG
                    log( "\n New connection connected: Index:%d. Total Connection now:%d", ToProcessSessoin->SessionIndex, NetObj.GetConnectedCount() );
#endif
                    SendPacketProcess_NewAccept( ToProcessSessoin->SessionIndex );
                }
                break;

                case HNet::SESSION_STATE_CLOSEREDY:
                { // Connection closed arrived or communication error.
#ifdef _DEBUG
                    log( "\n Received: Index %d wants to close or already closed.\n Total Connection now:%d",
                            ToProcessSessoin->SessionIndex, NetObj.GetConnectedCount() );
#endif
                    SendPacketProcess_EnemyShipDisconnect( ToProcessSessoin->SessionIndex );
                }
                break;

                case HNet::SESSION_STATE_READPACKET:
                { // Any packet data recevied.
                    ReceviedPacketProcess( ToProcessSessoin );
                }
                break;
            }

            NetObj.GetProcessList()->DeleteFirstSession();
        }

        // You can add your own server codes here. If there is any regular work.

        // Update the asteroid movement.
        float asteroid_prev_x, asteroid_prev_y;
        float time_delta = g_LoopTimer.GetTimer_sec();
        for( auto asteroid : g_AsteroidList )
        {
            asteroid_prev_x = asteroid->get_x();
            asteroid_prev_y = asteroid->get_y();
            asteroid->Update( time_delta, 30, 30 );
            if( ( asteroid_prev_x != asteroid->get_x() ) || ( asteroid_prev_y != asteroid->get_y() ) )
            {
                SendPacketProcess_AsteroidMovement( asteroid );
            }
        }

        Sleep( 100 ); // You can check a timer to nomalize the looping spped (FPS).
    }

    return 0;
}