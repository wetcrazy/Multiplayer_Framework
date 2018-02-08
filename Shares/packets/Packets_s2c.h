#ifndef _PACKETS_S2C_H
#define _PACKETS_S2C_H
#pragma once


enum _PACKET_ID_SERVER_TO_CLIENT {
    PACKET_ID_S2C_EMPTY = 0,            // Start of the list. Do not remove this.

    PACKET_ID_S2C_WELCOMEMESSAGE,
    PACKET_ID_S2C_ENTERGAMEOK,
    PACKET_ID_S2C_ENEMYSHIP,
    PACKET_ID_S2C_NEWENEMYSHIP,
    PACKET_ID_S2C_NEWASTEROID,
    PACKET_ID_S2C_DISCONNECTENEMYSHIP,
    PACKET_ID_S2C_MOVEMENT,
    PACKET_ID_S2C_COLLIDED,
    PACKET_ID_S2C_ASTEROIDMOVEMENT,
	PACKET_ID_S2C_ASTEROIDCOLLIDED,
	PACKET_ID_S2C_NEWMISSILE,
	PACKET_ID_S2C_NEWEXPLOSION,
	PACKET_ID_S2C_FullMessage,

    PACKET_ID_S2C_END
};


/**
* This is the list of data structures for each packet.
* ** IMPORTANT ** All the structures have to be fixed size.
*/
#pragma pack(push, 1) // Pack the memory alignment. 
struct PKT_S2C_WelcomeMessage {
    int ShipID; // ShipID is same as SessionID in the server.
};

struct PKT_S2C_EnemyShip {
    int ShipID;
    int ShipType;
	int Hp;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_NewEnemyShip {
    int ShipID;
    int ShipType;
	int Hp;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_NewAsteroid {
    int AsteroidID;
    float x;
    float y;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_EnemyShipDisconnect {
    int ShipID;
};

struct PKT_S2C_Movement {
    int ShipID;
	int Hp;
    float server_x;
    float server_y;
    float server_w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_Collided {
    int ShipID;
	int Hp;
    float server_x;
    float server_y;
    float server_w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_AsteroidMovement {
    int AsteroidID;
    float server_x;
    float server_y;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_AsteroidCollided {
	int AsteroidID;
	float server_x;
	float server_y;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

struct PKT_S2C_NewMissile {
	int OwnerShipID;
	float x;
	float y;
	float w;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

struct PKT_S2C_NewExplosion
{	
	float x;
	float y;
};

struct PKT_S2C_FullMessage
{
	bool isFull;
};

#pragma pack(pop) // Return back to default memory alignment.


#endif