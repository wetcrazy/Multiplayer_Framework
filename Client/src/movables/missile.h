#ifndef _MISSILE_H_
#define _MISSILE_H_

#include "hge.h"
#include "hgerect.h"
#include "movables.h"

class hgeSprite;

class Missile : public Movables
{
    public:
    	HTEXTURE tex_; //!< Handle to the sprite's texture
    	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship

    	int ownerid_;

    public: // Constructors and Destructor.
        Missile( char* filename, float x, float y, float w, int shipid );
    	~Missile();

    public: // Getters and Setters.
        void set_ownerid( int ownerid ) { ownerid_ = ownerid; }
    	int get_ownerid() { return ownerid_; }

    public: // Interface functions.
    	void Render();
};

#endif