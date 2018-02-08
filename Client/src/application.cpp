#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "hge.h"

#include "globals.h"
#include "net\process_packet.h"
#include "net\send_packet.h"
#include "Application.h"
#include "movables\ship.h"
#include "movables\asteroid.h"
#include "movables\missile.h"

#include <stdlib.h>
#include <time.h>

#ifdef _DEBUG
#include <io.h>
#endif

#ifdef _DEBUG
void log(char *szFormat, ...)
{
	char Buff[1024];
	char Msg[1024];
	va_list arg;

	va_start(arg, szFormat);
	_vsnprintf_s(Msg, 1024, szFormat, arg);
	va_end(arg);

	sprintf_s(Buff, 1024, "%s", Msg);
	_write(1, Buff, strlen(Buff));
}
#endif

#define ABS(n) ( (n < 0) ? (-n) : (n) )

/**
* Constuctor
*
* Creates an instance of the graphics engine and network engine
*/
Application::Application() :
	hge_(hgeCreate(HGE_VERSION))
{
	SetGameState(GAMESTATE_NONE);

}

/**
* Destructor
*
* Does nothing in particular apart from calling Shutdown
*/
Application::~Application() throw()
{
	Shutdown();
}

/**
* Initialises the graphics system
* It should also initialise the network system
*/
bool Application::Init()
{
	hge_->System_SetState(HGE_FRAMEFUNC, Application::Loop);
	hge_->System_SetState(HGE_WINDOWED, true);
	hge_->System_SetState(HGE_USESOUND, false);
	hge_->System_SetState(HGE_TITLE, "SpaceShooter");
	hge_->System_SetState(HGE_LOGFILE, "SpaceShooter.log");
	hge_->System_SetState(HGE_DONTSUSPEND, true);

	if (false == hge_->System_Initiate())
	{
		return false;
	}

	srand((unsigned int)time(NULL));

	// Initialize and prepare the game data & systems.
	// Initialize my own spaceship.
	int shiptype = (rand() % 4) + 1;
	float startx = (float)((rand() % 600) + 100);
	float starty = (float)((rand() % 400) + 100);
	float startw = 0.0f;
	myship_ = new Ship(shiptype, "MyShip", startx, starty, startw);

	// Boom
	boom_tex_ = hge_->Texture_Load("boom.jpg");
	hge_->Release();

	// Missile
	mymissile = NULL;
	keydown_enter = false;

	// Full msg
	isfull_ = false;

	// Initialize asteroids.
	//asteroids_.push_back( new Asteroid( "asteroid.png", 100, 100, 1 ) );
	//asteroids_.push_back( new Asteroid( "asteroid.png", 700, 500, 1.5 ) );

	// Initialize the network with Network Library.
	if (!(Net::InitNetwork()))
	{
		return false;
	}
	SetGameState(GAMESTATE_INITIALIZING);

	return true;
}

/**
* Update cycle
*
* Checks for keypresses:
*   - Esc - Quits the game
*   - Left - Rotates ship left
*   - Right - Rotates ship right
*   - Up - Accelerates the ship
*   - Down - Deccelerates the ship
*
* Also calls Update() on all the ships in the universe
*/
bool Application::Update()
{
	float timedelta = hge_->Timer_GetDelta();

	// Process the packet received from server.
	Net::ProcessPacket(this);

	if (GAMESTATE_INPLAY == GetGameState())
	{

		// Check key inputs and process the movements of spaceship.
		if (hge_->Input_GetKeyState(HGEK_ESCAPE))
			return true;

		myship_->set_angular_velocity(0.0f);
		if (hge_->Input_GetKeyState(HGEK_LEFT))
		{
			myship_->set_angular_velocity(myship_->get_angular_velocity() - DEFAULT_ANGULAR_VELOCITY);
		}
		if (hge_->Input_GetKeyState(HGEK_RIGHT))
		{
			myship_->set_angular_velocity(myship_->get_angular_velocity() + DEFAULT_ANGULAR_VELOCITY);
		}
		if (hge_->Input_GetKeyState(HGEK_UP))
		{
			myship_->Accelerate(DEFAULT_ACCELERATION, timedelta);
		}
		if (hge_->Input_GetKeyState(HGEK_DOWN))
		{
			myship_->Accelerate(-DEFAULT_ACCELERATION, timedelta);
		}
		if (hge_->Input_GetKeyState(HGEK_0))
		{
			myship_->stop_moving();
		}

		// Shoot missile
		if (hge_->Input_GetKeyState(HGEK_SPACE))
		{
			if (!keydown_enter)
			{
				CreateMissile(myship_->get_x(), myship_->get_y(), myship_->get_w(), myship_->GetShipID());
				keydown_enter = true;
			}
		}
		else
		{
			if (keydown_enter)
				keydown_enter = false;
		}

		// Update my space ship.
		bool AmIMoving = false;
		if (true == (AmIMoving = myship_->Update(timedelta, myship_->sprite_->GetWidth(), myship_->sprite_->GetHeight())))
		{
			for (auto itr_asteroid : asteroids_)
			{
				if (true == CheckCollision(myship_, itr_asteroid, timedelta))
				{
					myship_->set_collided_with_me(true);
				}
			}
		}

		// Update asteroids.
		for (AsteroidList::iterator itr_asteroid = asteroids_.begin(); itr_asteroid != asteroids_.end(); ++itr_asteroid)
		{
			if (true == (*itr_asteroid)->Update(timedelta, (*itr_asteroid)->sprite_->GetWidth(), (*itr_asteroid)->sprite_->GetHeight()))
			{
				// Collision check with other asteroids.
				for (AsteroidList::iterator next_asteroid = (itr_asteroid + 1); next_asteroid != asteroids_.end(); ++next_asteroid)
				{
					CheckCollision((*itr_asteroid), (*next_asteroid), timedelta);
				}

				// Collision check with my ship.
				if (false == AmIMoving)
				{
					if (false == myship_->get_collided_with_me())
						CheckCollision((*itr_asteroid), myship_, timedelta);
					else
						myship_->set_collided_with_me(false);
				}
			}
		}

		// Update enemy ships.
		for (ShipList::iterator itr_enemyship = enemyships_.begin(); itr_enemyship != enemyships_.end(); ++itr_enemyship)
		{
			(*itr_enemyship)->Update(timedelta, (*itr_enemyship)->sprite_->GetWidth(), (*itr_enemyship)->sprite_->GetHeight());
		}

		// Update my missiles
		if (mymissile)
		{
			if (true == mymissile->Update(timedelta, mymissile->sprite_->GetWidth(), mymissile->sprite_->GetHeight()))
			{
				bool isdelete = false;
				// Collision check with asteroids
				for (auto itr_asteroid : asteroids_)
				{
					if (HasCollided(mymissile, itr_asteroid))
					{
						//Net::send_packet_delete_missile(mymissile);				
						isdelete = true;
						break;
					}
				}

				// Collision check with enemyships
				for (auto itr_enemyship : enemyships_)
				{
					if (HasCollided(mymissile, itr_enemyship))
					{
						//Net::send_packet_delete_missile(mymissile);
						isdelete = true;
						break;
					}
				}
				if (isdelete)
				{
					delete mymissile;
					mymissile = NULL;
				}
			}
		}

		// Update enemy missiles
		for (auto itr_missile : enemymissiles_)
		{
			itr_missile->Update(timedelta, itr_missile->sprite_->GetWidth(),
				itr_missile->sprite_->GetHeight());
		}

		// Check for any explosion and place it into the 
	/*	for (auto *itr_boom : boom_list_)
		{
			if (itr_boom != NULL)
				continue;

		}*/

		// Now send the update to server. But not every frame.
		if (!myship_->compare_xyw()) Net::send_packet_myship_movement(myship_);
	}

	return false;
}


/**
* Render Cycle
*
* Clear the screen and render all the ships
*/
void Application::Render()
{
	hge_->Gfx_BeginScene();
	hge_->Gfx_Clear(0);

	// Render Text
	if (isfull_)
	{
		paint.reset(new hgeFont("font1.fnt"));
		paint->printf(5, 5, HGETEXT_LEFT, "FULL");
		paint.release();
		hge_->Gfx_EndScene();		
		return;
	}

	// Render my space ship.
	myship_->Render();

	// Render enemy ships.
	for (auto enemyship : enemyships_) enemyship->Render();

	// Render asteroids.
	for (auto asteroid : asteroids_) asteroid->Render();

	// Render Missiles
	if (mymissile) mymissile->Render();

	// Render Enemy Missiles
	for (auto enemymissile : enemymissiles_) enemymissile->Render();

	// Render Boom
	/*for (auto *itr_boom : boom_list_)
	{
		if (itr_boom == NULL)
			continue;

		itr_boom->boom_sprite_.reset(new hgeSprite(boom_tex_, itr_boom->x, itr_boom->y, 40, 20));
		itr_boom->boom_sprite_->SetHotSpot(20, 10);
	}	*/	

	hge_->Gfx_EndScene();
}

Ship * Application::FindEnemyShip(int ShipID)
{
	for (auto enemyship : enemyships_)
	{
		if (ShipID == enemyship->GetShipID())
		{
			return enemyship;
		}
	}

	return nullptr;
}

void Application::CreateMissile(float x, float y, float w, int id)
{
	if (mymissile)
	{// delete existing missile
		delete mymissile;
		mymissile = 0;
	}

	// add a new missile based on the following parameter coordinates
	mymissile = new Missile("missile.png", x, y, w, id);

	// Send new missile info to server
	Net::send_packet_new_missile(mymissile);
}


/**
* Main game loop
*
* Processes user input events
* Supposed to process network events
* Renders the ships
*
* This is a static function that is called by the graphics
* engine every frame, hence the need to loop through the
* global namespace to find itself.
*/
bool Application::Loop()
{
	Global::application->Render();
	return Global::application->Update();
}

/**
* Shuts down the graphics and network system
*/
void Application::Shutdown()
{
	hge_->System_Shutdown();
	hge_->Release();
}

/**
* Kick starts the everything, called from main.
*/
void Application::Start()
{
	if (Init())
	{
		hge_->System_Start();
	}
}

template <typename T1, typename T2>
bool Application::HasCollided(T1 &object, T2 &movable)
{
	hgeRect object_collidebox;
	hgeRect movable_Collidebox;

	object->sprite_->GetBoundingBox(object->get_x(), object->get_y(), &object_collidebox);
	movable->sprite_->GetBoundingBox(movable->get_x(), movable->get_y(), &movable_Collidebox);
	return object_collidebox.Intersect(&movable_Collidebox);
}

template <typename Mov, typename Tgt>
bool Application::CheckCollision(Mov &moving_object, Tgt &other, float timedelta)
{
	if (HasCollided(moving_object, other))
	{
		other->set_velocity_x(moving_object->get_velocity_x());
		other->set_velocity_y(moving_object->get_velocity_y());
		other->set_server_velocity_x(moving_object->get_server_velocity_x());
		other->set_server_velocity_y(moving_object->get_server_velocity_y());
		other->Update(timedelta, other->sprite_->GetWidth(), other->sprite_->GetHeight());

		moving_object->set_velocity_x(moving_object->get_velocity_x());
		moving_object->set_velocity_y(-moving_object->get_velocity_y());
		moving_object->set_server_velocity_x(-moving_object->get_server_velocity_x());
		moving_object->set_server_velocity_y(-moving_object->get_server_velocity_y());
		moving_object->restore_xy();
		moving_object->Update(timedelta, moving_object->sprite_->GetWidth(), moving_object->sprite_->GetHeight());

		if ((void *)moving_object == (void *)myship_)
		{ // If I collided with others, need to send the message to the server.
			Net::send_packet_collided((Ship *)moving_object);
		}
		else if (MOVABLE_OBJECT_TYPE_ASTEROID == moving_object->get_object_type())
		{
			Net::send_packet_asteroid_collided((Asteroid *)moving_object);
		}

		if (MOVABLE_OBJECT_TYPE_ASTEROID == other->get_object_type())
		{
			Net::send_packet_asteroid_collided((Asteroid *)other);
		}
		return true;
	}

	return false;
}