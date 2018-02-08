#ifndef _PROCESSPACKET_H
#define _PROCESSPACKET_H
#pragma once

#include "ClientNetwork.h"

// Function declarerations.
namespace Net {
    bool InitNetwork( void );
    void ProcessPacket( Application *thisapp );

    void WelcomeMessage( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void NewEnemyShip( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void NewAstreroid( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void DisconnectEnemyShip( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void ShipMovement( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void ShipCollided( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void AsteroidMovement( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
	void AsteroidCollided(Application *thisapp, struct HNet::_ProcessSession *ToProsessSession);
	void NewMissile(Application *thisapp, struct HNet::_ProcessSession *ToProsessSession);
	void FullMessage(Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin);
}



#endif
