#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "send_packet.h"

#ifdef _DEBUG
void log( char *szFormat, ... );
#endif

extern HNet::_ClientNetwork NetObj;
// Global variables for send_packet.cpp
namespace Net
{
    _Timer net_timer;
    float timer_net_movement_update = 0;
}

namespace Net
{
    //-------------------------------------------------------------------------
    void send_packet_enter_game( Ship * myship )
    {
        struct PKT_C2S_EnterGame PacketData;
        PacketData.ShipID = myship->GetShipID();
        PacketData.ShipType = myship->GetShipType();
        PacketData.x = myship->get_x();
        PacketData.y = myship->get_y();
        PacketData.w = myship->get_w();
        PacketData.velocity_x = myship->get_velocity_x();
        PacketData.velocity_y = myship->get_velocity_y();
        PacketData.angular_velocity = myship->get_angular_velocity();
		PacketData.Hp = myship->GetShipHp();

        struct HNet::_PacketMessage Packet;
        int PacketID = PACKET_ID_C2S_ENTERGAME;
        Packet << PacketID;
        Packet << PacketData;
#ifdef _DEBUG
        log( "\nSend: PACKET_ID_C2S_ENTERGAME, ID:%d, Type:%d, x:%0.0f, y:%0.0f, w:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
             PacketData.ShipID, PacketData.ShipType, PacketData.x, PacketData.y, PacketData.w,
             PacketData.velocity_x, PacketData.velocity_y, PacketData.angular_velocity );
#endif
        NetObj.SendPacket( Packet );
    }

    //-------------------------------------------------------------------------
    void send_packet_myship_movement( Ship *myship )
    {
        timer_net_movement_update += net_timer.GetTimer_msec();
        if( timer_net_movement_update > 500.0 )
        {
            timer_net_movement_update = 0;

            struct PKT_C2S_Movement PacketData;
            PacketData.ShipID = myship->GetShipID();
            PacketData.x = myship->get_x();
            PacketData.y = myship->get_y();
            PacketData.w = myship->get_w();
            //PacketData.velocity_x = myship->get_velocity_x();
            //PacketData.velocity_y = myship->get_velocity_y();
			PacketData.velocity_x = myship->get_server_velocity_x();
			PacketData.velocity_y = myship->get_server_velocity_y();
            PacketData.angular_velocity = myship->get_angular_velocity();
			PacketData.Hp = myship->GetShipHp();

            struct HNet::_PacketMessage Packet;
            int PacketID = PACKET_ID_C2S_MOVEMENT;
            Packet << PacketID;
            Packet << PacketData;
#ifdef _DEBUG
            log( "\nSend: PACKET_ID_C2S_MOVEMENT, ID:%d, x:%0.0f, y:%0.0f, w:%0.2f, av:%0.2f",
                 PacketData.ShipID, PacketData.x, PacketData.y, PacketData.w,
                 PacketData.velocity_x, PacketData.velocity_y, PacketData.angular_velocity);
#endif
            NetObj.SendPacket( Packet );
        }
    }

    //-------------------------------------------------------------------------
    void send_packet_collided( Ship *myship )
    {
        struct PKT_C2S_Collided PacketData;
        PacketData.ShipID = myship->GetShipID();
        PacketData.x = myship->get_x();
        PacketData.y = myship->get_y();
        PacketData.w = myship->get_w();
        PacketData.velocity_x = myship->get_server_velocity_x();
        PacketData.velocity_y = myship->get_server_velocity_y();
        PacketData.angular_velocity = myship->get_angular_velocity();
		PacketData.Hp = myship->GetShipHp();

        struct HNet::_PacketMessage Packet;
        int PacketID = PACKET_ID_C2S_COLLIDED;
        Packet << PacketID;
        Packet << PacketData;
        NetObj.SendPacket( Packet );
    }

	//-------------------------------------------------------------------------
	void send_packet_asteroid_collided(Asteroid * asteroid)
	{
		struct PKT_C2S_AsteroidCollided PacketData;
		PacketData.AsteroidID = asteroid->get_id();
		PacketData.x = asteroid->get_x();
		PacketData.y = asteroid->get_y();
		PacketData.velocity_x = asteroid->get_velocity_x();
		PacketData.velocity_y = asteroid->get_velocity_y();
		PacketData.server_velocity_x = asteroid->get_server_velocity_x();
		PacketData.server_velocity_y = asteroid->get_server_velocity_y();
		PacketData.angular_velocity = asteroid->get_angular_velocity();
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_ASTEROIDCOLLIDED;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}

	void send_packet_new_missile(Missile * newMissile)
	{
		struct PKT_C2S_NewMissile PacketData;
		PacketData.OwnerShipID = newMissile->get_ownerid();
		PacketData.x = newMissile->get_x();
		PacketData.y = newMissile->get_y();
		PacketData.w = newMissile->get_w();
		PacketData.velocity_x = newMissile->get_server_velocity_x();
		PacketData.velocity_y = newMissile->get_server_velocity_y();
		PacketData.angular_velocity = newMissile->get_angular_velocity();
	
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_NEWMISSILE;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
}
