#include "main.h"
#include "send_packet.h"

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif

extern HNet::_ServerNetwork NetObj;
extern _Ship g_ShipList[];
unsigned int numShips;

void SendPacketProcess_NewAccept(const int SessionID)
{	
	numShips += 1;

	if (numShips > 3)
		SendPacketProcess_NoAccept(SessionID);
	else
	{
		// Add into ship ilst.
		g_ShipList[SessionID].connected = true;
		// Send welcome message.
		SendPacketProcess_NewAccept_SendWelcomeMessage(SessionID);
	}		
}

void SendPacketProcess_NewAccept_SendWelcomeMessage( const int SessionID )
{
    // Send Welcome packet with spaceship ID.
    struct HNet::_PacketMessage Packet;       // Create the _PacketMessage structure object.
    struct PKT_S2C_WelcomeMessage PacketData;
    int PacketID = PACKET_ID_S2C_WELCOMEMESSAGE;
    PacketData.ShipID = SessionID;

    Packet << PacketID;
    Packet << PacketData;
    NetObj.SendPacket( SessionID, Packet );
#ifdef _DEBUG
    log( "\nSend [PACKET_ID_S2C_WELCOMEMESSAGE] ID:%d", PacketData.ShipID );
#endif
}

void SendPacketProcess_NoAccept(const int SessionID)
{
	SendPacketProcess_EnemyShipDisconnect(SessionID);
	
		struct HNet::_PacketMessage Packet;
		struct PKT_S2C_FullMessage PacketData;
		int PacketID = PACKET_ID_S2C_FullMessage;

		PacketData.isFull = true;

		Packet << PacketID;
		Packet << PacketData;

		NetObj.SendPacket(SessionID, Packet);
}

void SendPacketProcess_EnemyShipDisconnect( const int SessionID )
{
    g_ShipList[SessionID].connected = false;
	numShips -= 1;
    struct HNet::_PacketMessage Packet;
    int PacketID = PACKET_ID_S2C_DISCONNECTENEMYSHIP;
    Packet << PacketID;
    Packet << SessionID;

    int Index;
    for( Index = 1; Index <= MAX_CLIENT_CONNECTION; ++Index )
    {
        if( true == g_ShipList[Index].connected )
        {
            if( SessionID == Index ) continue;

            NetObj.SendPacket( Index, Packet );
#ifdef _DEBUG
            log( "\n Disconnect packet sent to ClientID:%d", Index );
#endif
        }
    }
}

void SendPacketProcess_AsteroidMovement( _Asteroid *asteroid )
{
    struct HNet::_PacketMessage Packet;
    int PacketID = PACKET_ID_S2C_ASTEROIDMOVEMENT;

    struct PKT_S2C_AsteroidMovement PacketData;
    PacketData.AsteroidID = asteroid->get_ID();
    PacketData.server_x = asteroid->get_x();
    PacketData.server_y = asteroid->get_y();
    PacketData.velocity_x = asteroid->get_velocity_x();
    PacketData.velocity_y = asteroid->get_velocity_y();
    PacketData.angular_velocity = asteroid->get_angular_velocity();

    Packet << PacketID;
    Packet << PacketData;
    NetObj.SendPacketToAll( Packet );
}

void SendPacketProcess_AsteroidCollided(_Asteroid *asteroid)
{
	struct HNet::_PacketMessage Packet;
	int PacketID = PACKET_ID_S2C_ASTEROIDCOLLIDED;
	struct PKT_S2C_AsteroidCollided PacketData;
	PacketData.AsteroidID = asteroid->get_ID();
	PacketData.server_x = asteroid->get_x();
	PacketData.server_y = asteroid->get_y();
	PacketData.velocity_x = asteroid->get_velocity_x();
	PacketData.velocity_y = asteroid->get_velocity_y();
	PacketData.angular_velocity = asteroid->get_angular_velocity();
	Packet << PacketID;
	Packet << PacketData;
	NetObj.SendPacketToAll(Packet);
}