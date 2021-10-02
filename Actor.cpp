#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;

// =================================  Actor's function implementations  =========================================




// ================================  Character's function implementations  =======================================




// =================================  Human's functions implementations  =========================================




// =================================  Penelope's functions implementations  =======================================

void Penelope::shootFlameThrower()
{
	for (int i = 1; i < 4; i++)
	{
		cout << "calling flames" << endl;
		double x = getX();
		double y = getY();
		
		switch (getDirection())
		{
		case 270:
			if (getWorld()->doesOverlap(x,y- i*SPRITE_HEIGHT)) //down
				return;
			getWorld()->addFlame(x,y-i*SPRITE_HEIGHT , 270);
			break;
		case 90:
			if (getWorld()->doesOverlap(x, y + i * SPRITE_HEIGHT))//up
				return;
			getWorld()->addFlame(x, y + i * SPRITE_HEIGHT, 90);
			break;
		case 180:
			if (getWorld()->doesOverlap(x - i * SPRITE_WIDTH, y)) //left
				return;
			getWorld()->addFlame(x - i * SPRITE_HEIGHT, y , 180);
			break;
		case 0:
			if (getWorld()->doesOverlap(x + i * SPRITE_WIDTH, y)) // right
				return;
			getWorld()->addFlame(x + i * SPRITE_HEIGHT, y, 0);
			break;
		}
	}
}

void Penelope::setDead()
{
	if(!isDead())
		Actor::setDead();
	getWorld()->decLives();

}

void Penelope::doSomething()
{
	if (isDead()) return;						// if dead return
	if (isInfected()) incrementNumInfected();	// if infected, increment infection count
	if (infectionCount() == 500) Actor::setDead();		// if infection is terminal, Penel is dead	;
	if (isDead())								// if infection kills her, play sound and return
	{
		getWorld()->playSound(SOUND_PLAYER_DIE);
		return;									// StudentObject should detect she dead and then end the level
	}
		
	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if(getDirection() != left)
				setDirection(left);
			if(!getWorld()->isBlocked(getX() - 4, getY(), this))
				moveTo(getX() - 4, getY());
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
				setDirection(right);
			if (!getWorld()->isBlocked(getX() + 4, getY(), this))
				moveTo(getX() + 4, getY());
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up)
				setDirection(up);
			if (!getWorld()->isBlocked(getX(), getY() + 4, this))
				moveTo(getX(), getY() + 4);
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
				setDirection(down);
			if (!getWorld()->isBlocked(getX(), getY() - 4, this))
				moveTo(getX(), getY() - 4);
			break;
		case KEY_PRESS_SPACE:
			if (m_numFlameThrowers <= 0)
				return;
			shootFlameThrower(); m_numFlameThrowers--;	
			getWorld()->playSound(SOUND_PLAYER_FIRE);
			break;
		case KEY_PRESS_TAB:
			if (m_numLandmines <= 0)
				return;
			getWorld()->addLandmine(getX(), getY()); m_numLandmines--;
			break;
		case KEY_PRESS_ENTER:
			if (m_numVaccines <= 0)
				return;
			cureInfection();
			m_numVaccines--;
			break;
		}
	}
}

// =================================  Citizen's functions implementations  =========================================


void Citizen::useExitIfAppropriate()
{return;}

void Citizen::doSomething()
{
	if (m_numTicks == 10000)
		m_numTicks = 0;

	if (isDead())
		return;

	if (isInfected())
		incrementNumInfected();	// if infected, increment infection count

	if (infectionCount() == 500)
	{
		setDead();		// if infection is terminal, Citizen is dead
		getWorld()->playSound(SOUND_ZOMBIE_BORN);
		getWorld()->addZombie(getX(), getY());
		return;
	}

	if (m_numTicks % 2 == 0)
	{
		m_numTicks++;
		return;
	}
	m_numTicks++;

	double dist_p = getWorld()->distance(0, 0, this, getWorld()->getPenelope()); // distance between Ciizen and Penelope
	double dist_z = getWorld()->closestZombieDist(0, 0, this);

	double coord1[2] = { getX(), getY() }; // citezen's coods
	double coord2[2] = { getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY() }; //penelope's coods

	if (dist_p < dist_z && dist_p <= 80) // #6
	{
		if (coord1[0] == coord2[0]) // same x coord
		{
			if (coord1[1] < coord2[1]) // citizen is below penele
			{
				double* temp = getWorld()->newCoords(coord1, up, 2);
				if (!getWorld()->isBlocked(temp[0], temp[1], this))// look ahead in the upward dir to see if its safe to go to
				{
					setDirection(up);
					moveTo(temp[0], temp[1]);
				}
			}
			else
			{
				double* temp = getWorld()->newCoords(coord1, down, 2);
				if (!getWorld()->isBlocked(temp[0], temp[1], this)) // look ahead in the doownward dir to see if its safe to go to
				{
					setDirection(down);
					moveTo(temp[0], temp[1]);
				}
			}
		}
		else if (coord1[1] == coord2[1]) // same y coord
		{
			if (coord1[0] < coord2[0]) // citizen is left of penel
			{
				double* temp = getWorld()->newCoords(coord1, right, 2);
				if (!getWorld()->isBlocked(temp[0], temp[1], this))
				{
					setDirection(right);
					moveTo(temp[0], temp[1]);
				}
			}
			else		// citizen is right of penel
			{
				double* temp = getWorld()->newCoords(coord1, left, 2);
				if (!getWorld()->isBlocked(temp[0], temp[1], this))
				{
					setDirection(left);
					moveTo(temp[0], temp[1]);
				}
			}
		}
		else // not in row or col
		{
			bool didFirstPart = false;
			if (randInt(1, 2) == 1) // then we will movw in the better of the two horizontal directions
			{
				if (coord1[0] < coord2[0]) //citizen is to the left of Penel
				{
					double* temp = getWorld()->newCoords(coord1, right, 2);
					if (!getWorld()->isBlocked(temp[0], temp[1], this))// look ahead in the rightward dir to see if its safe to go to
					{
						setDirection(right);			// must be safe to go, so go in that dir
						moveTo(temp[0], temp[1]);
					}
					else
						didFirstPart = true;
				}
				else
				{
					double* temp = getWorld()->newCoords(coord1, left, 2);
					if (!getWorld()->isBlocked(temp[0], temp[1], this))// look ahead in the leftward dir to see if its safe to go to
					{
						setDirection(left);				// must be safe to go, so go in that dir
						moveTo(temp[0], temp[1]);
					}
					else
						didFirstPart = true;
				}
			}
			else if (!didFirstPart)
			{
				if (coord1[1] < coord2[1]) //citizen is below Penel
				{
					double* temp = getWorld()->newCoords(coord1, up, 2);
					if (!getWorld()->isBlocked(temp[0], temp[1], this))// look ahead in the upward dir to see if its safe to go to
					{
						setDirection(up);				// must be safe to go, so go in that dir
						moveTo(temp[0], temp[1]);
					}
				}
				else
				{
					double* temp = getWorld()->newCoords(coord1, down, 2);
					if (!getWorld()->isBlocked(temp[0], temp[1], this)) // look ahead in the downward dir to see if its safe to go to
					{
						setDirection(left);				// must be safe to go, so go in that dir
						moveTo(temp[0], temp[1]);
					}
				}
			}
		}
	}
	else if (dist_z <= 80) // #7
	{
		double y1, x1; // coords of citizen
		bool dirs[4] = {}; //arr of the directions
		double distToZ[4] = {}; //distances to Zombie

		for (int i = 0; i != 4; i++)
		{
			x1 = getX();
			y1 = getY();

			switch (i * 90) // test the different directions you can go
			{
			case(0):
				x1 += 2;
				break;
			case(90):
				y1 += 2;
				break;
			case(180):
				x1 -= 2;
				break;
			case(270):
				y1 -= 2;
				break;
			}

			if (!getWorld()->isBlocked(x1, y1, this)) //if this direction is blocked, continue
			{
				if (getWorld()->closestZombieDist(x1, y1, this) > dist_z) // if the distance in this dir gets it further away from a zombie
				{
					dirs[i] = true;												// note that it is true in the directions array
					distToZ[i] = getWorld()->closestZombieDist(x1, y1, this);	// and insert its distance into distanceToZombie array
				}
			}
		}
		// find the max and its postion in the arrays
		double max = 0;
		int posOfMax = -1;
		for (int j = 0; j != 4; j++)
		{
			if (dirs[j] && distToZ[j] > max) { //check to update max
				max = distToZ[j]; //update max
				posOfMax = j;
			}
		}
		if (posOfMax == -1) // if no max found, return
			return;

		x1 = getX(); // the xdir we go in now
		y1 = getY(); // the ydir we go in now
		switch (posOfMax * 90)
		{
		case(0):
			x1 += 2;
			break;
		case(90):
			y1 += 2;
			break;
		case(180):
			x1 -= 2;
			break;
		case(270):
			y1 -= 2;
			break;
		}
		// move in the resultant dir
		setDirection(posOfMax * 90);
		moveTo(x1, y1);
	}

}

void Citizen::setDead()
{
	Actor::setDead();
	getWorld()->increaseScore(-1000);
	getWorld()->recordCitizenGone();
}


// =====================================  Zombie's functions implementations  ========================================



// ====================================  DumbZombie's functions implementations  =====================================

	void DumbZombie::setDead()
	{
		//make zombie dead, increase score
		Actor::setDead();
		getWorld()->increaseScore(1000);
	}

// ============================--=====  SmartZombie's functions implementations  =====================================

	void SmartZombie::setDead()
	{
		//make zombie dead, increase score
		Actor::setDead();
		getWorld()->increaseScore(2000);
	}

// =================================  Activatable's functions implementations  =====================================



// ========================================  Exit's functions implementations  =====================================


void Exit::activateIfApt(Actor* a)
{
	if (a->triggersVomit()) // isHuman
	{
		if (!a->triggersCitizens()) //isCitizen
		{
			getWorld()->increaseScore(1500); // offset the setDead function
			static_cast<Citizen*>(a)->setDead(); //static cast for correct use of setDead()
			getWorld()->playSound(SOUND_CITIZEN_SAVED);
		}
		else // isPenelope
		{
			// update that level completed 
			if (getWorld()->numCitizens() == 0)
				getWorld()->levelCompleted();
		}

	}
}

//======================================  Goodie's functions implementations  ======================================

void Goodie::setDead()
{
	Actor::setDead();
	getWorld()->increaseScore(50);
}

//===================================  VaccineGoodie's functions implementations  =================================

void VaccineGoodie::activateIfApt(Actor *a)
{
	// activate goodie by setting it to dead, then increasing its count, then making a noise
	Goodie::setDead();
	getWorld()->getPenelope()->changeNumVaccines(1);
	getWorld()->playSound(SOUND_GOT_GOODIE);
} 

//===================================  GasCanGoodie's functions implementations  ==================================
 
void GasCanGoodie::activateIfApt(Actor *a)
{
	// activate goodie by setting it to dead, then increasing its count, then making a noise
	Goodie::setDead();
	getWorld()->getPenelope()->changeNumFlames(5);
	getWorld()->playSound(SOUND_GOT_GOODIE);
} 
//===================================  LandmineGoodie's functions implementations  =================================

void LandmineGoodie::activateIfApt(Actor *a)
{
	// activate goodie by setting it to dead, then increasing its count, then making a noise
	Goodie::setDead();
	getWorld()->getPenelope()->changeNumLandmines(2);
	getWorld()->playSound(SOUND_GOT_GOODIE);
} 


// ====================================  Vomit's functions implementations  =========================================

void Vomit::doSomething()
{
	return;
}

// ====================================  Flame's functions implementations  =========================================

void Flame::doSomething()
{
	//depending on ticks, setDead the Flame
	if (isDead())
		return;
	if (m_numTicks == 2)
		Actor::setDead();

	m_numTicks++;
}

void Flame::activateIfApt(Actor* a) 
{
	//activate Flame on objs that can fall or burn and set them to Dead
	if (a->isDead())
		return;
	if (a->canFallorBurn())
		a->setDead();
}

// ====================================  Landmine's functions implementations  =======================================

void Landmine::doSomething()
{
	//activate landmine after 30 ticks
	if (isDead())
		return;
	if (m_numOfTicks == 30)
		m_isActive = true;

	m_numOfTicks++;
}

void Landmine::activateIfApt(Actor *a)
{
	if (m_isActive && a->triggersMines() && a->blocksMovement())
		setDead();
	else if (!a->blocksMovement() && a->triggersMines())
		setDead();
	else
	{
		Actor::setDead();
		
		double x = getX();
		double y = getY();

				//add the ssquare grid of flames
		getWorld()->addFlame(x, y, up);
		getWorld()->addFlame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT, up); 
		getWorld()->addFlame(x + SPRITE_WIDTH, y - SPRITE_HEIGHT, up); 
		getWorld()->addFlame(x - SPRITE_WIDTH, y - SPRITE_HEIGHT, up);  
		getWorld()->addFlame(x - SPRITE_WIDTH, y + SPRITE_HEIGHT, up); 
		getWorld()->addFlame(x + SPRITE_WIDTH, y, up); 
		getWorld()->addFlame(x - SPRITE_WIDTH, y, up); 
		getWorld()->addFlame(x, y + SPRITE_HEIGHT, up); 
		getWorld()->addFlame(x, y - SPRITE_HEIGHT, up); 

		getWorld()->addPit(x, y); 
		getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
	}
		
} 



// ====================================  Pit's functions implementations  ===========================================

void Pit::activateIfApt(Actor* a)
{
	//if this ojb can fall or burn, set it to Dead
	if (a->isDead())
		return;
	if (a->canFallorBurn())
		a->setDead();
}

// ====================================  Wall's functions implementations  =========================================
