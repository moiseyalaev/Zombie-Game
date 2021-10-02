#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>


class StudentWorld;

// =================================  Actor's Decleration  =========================================

class Actor : public GraphObject
{
	public:
		Actor(int imageID, double startX, double startY, Direction startDirection, int depth, StudentWorld* world,
			bool trigM, bool trigV, bool threatenC, bool trigC, bool blocksM, bool blocksF, bool canForB)
			: GraphObject(imageID, startX, startY, startDirection, depth),m_world(world), m_isDead(false),
			m_triggersLandmines(trigM), m_triggersVomit(trigV), m_threatensCitizens(threatenC), m_triggersCitizens(trigC),
			m_blocksMovement(blocksM), m_blocksFlame(blocksF), m_canFallorBurn(canForB){}
		virtual ~Actor() {}
		virtual void doSomething()							= 0;
		virtual void activateIfApt(Actor* a)				= 0;
		//virtual void beVomitedOnIfApt()					= 0;
		//virtual void pickUpGoodieIfApt(Goodie* g)			= 0;
		virtual bool blocksMovement() const					{ return m_blocksMovement; }
		virtual bool blocksFlame() const					{ return m_blocksFlame; }
		virtual bool canFallorBurn() const					{ return m_canFallorBurn; }
				// Identifiers
		virtual bool triggersMines() const					{ return m_triggersLandmines; }
		virtual bool triggersVomit() const					{ return m_triggersVomit; }
		virtual bool threatensCitizens() const				{ return m_threatensCitizens; }
		virtual bool triggersCitizens() const				{ return m_triggersCitizens; }
			// Accessors and Mutators
		StudentWorld* getWorld() const						{ return m_world; }
		bool isDead() const									{ return m_isDead; }
		virtual void setDead()								{ m_isDead = true; }

	private:
		StudentWorld* m_world;
		bool m_isDead, m_triggersLandmines, m_triggersVomit, m_threatensCitizens, m_triggersCitizens,
			 m_blocksMovement, m_blocksFlame, m_canFallorBurn;
};

// ================================================================================================
// =================================  Character's Decleration  ====================================
// ================================================================================================

class Character : public Actor
{
	public:
		Character(int imageID, double startX, double startY, StudentWorld* world, bool trigV, bool threatenC, bool trigC)
			: Actor(imageID, startX, startY, right, 0, world, true, trigV, threatenC, trigC, true, false, true)  {}
		virtual ~Character() {}
		virtual void activateIfApt(Actor* a) {}	
};

// =================================  Human's Delceration  =======================================
// ================================================================================================

class Human : public Character
{
	public:
		Human(int imageID, double startX, double startY, StudentWorld* world, bool trigC)
			: Character(imageID, startX, startY, world,/**/ true, false, trigC), m_numInfected(0), m_isInfected(false) {}
		virtual ~Human() {}
		bool isInfected() const						{ return m_isInfected; }
		void setInfected()							{ m_isInfected = true; }
		void incrementNumInfected()					{ m_numInfected++; }
		int infectionCount() const					{ return m_numInfected;  }
		void cureInfection()						{ m_numInfected = 0; m_isInfected = false; }
		int getNumInfected() const					{ return m_numInfected; }

	private:
		int m_numInfected;
		bool m_isInfected;
		
};

// =================================  Penelope's Decleration  ======================================

class Penelope : public Human
{
	public:
		Penelope(double startX, double startY, StudentWorld* world)
			: Human(IID_PLAYER, startX, startY, world, true),
			  m_numLandmines(0), m_numFlameThrowers(0), m_numVaccines(0) {}
		virtual ~Penelope() {}
		virtual void doSomething();
		void shootFlameThrower();
			// Accessors
		int getNumLandmines() const		{ return m_numLandmines; }
		int getNumFlames() const		{ return m_numFlameThrowers; }
		int getNumVaccines() const		{ return m_numVaccines; }
		void changeNumVaccines(int x)	{ m_numVaccines += x; }
		void changeNumLandmines(int x)	{ m_numLandmines += x; }
		void changeNumFlames(int x)		{ m_numFlameThrowers += x; }
		virtual bool triggersCitizens() const { return true; }
		virtual void setDead();
		
		
	private:
		int m_numLandmines, m_numFlameThrowers, m_numVaccines;
		
};

//=================================  Citizen's Decleration  ======================================

class Citizen : public Human
{
	public:
		Citizen(double startX, double startY, StudentWorld* world)
			: Human(IID_CITIZEN, startX, startY, world,/**/ false) {}
		virtual ~Citizen() {}
		virtual void setDead();
		virtual void doSomething();
		virtual void useExitIfAppropriate();

	private:
		int m_numTicks;
};

//=================================  Zombie's Decleration  ========================================
// ================================================================================================

//=====================================  Zombie's Decleration  ========================================
//======================================================================================================

class Zombie : public Character
{
	public:
		Zombie(double startX, double startY, StudentWorld* world)
			: Character(IID_ZOMBIE, startX, startY, world, false, true, true) {}
		virtual ~Zombie() {}

};

//=================================  DumbZombie's Decleration  ========================================

class DumbZombie : public Zombie
{
	public:
		DumbZombie(double startX, double startY, StudentWorld* world)
			: Zombie(startX, startY, world) {}
		virtual ~DumbZombie() {}
		virtual void setDead();
		virtual void doSomething() {} //for now
		virtual void dieByFallOrBurnIfAppropriate() {} // for now
};

//=================================  SmartZombie's Decleration  ========================================

class SmartZombie : public Zombie
{
	public:
		SmartZombie(double startX, double startY, StudentWorld* world)
			: Zombie(startX, startY, world) {}
		virtual ~SmartZombie() {}
		virtual void setDead();
		virtual void doSomething() {} // for now
};

// ================================================================================================
// =================================  Activatable's Decleration  =====================================
// ================================================================================================

class Activatable : public Actor
{
	public:
		Activatable(int imageID, double startX, double startY, Direction startDirection, int depth, StudentWorld* world,bool trigM, bool trigC, bool blocksF, bool ForB)
			: Actor(imageID, startX, startY, startDirection, depth, world,/**/ trigM, false, false, trigC, false, blocksF , ForB) {}
		virtual ~Activatable() {}
};

// =====================================  Pit's Decleration  ==============================================

class Pit : public Activatable
{
	public:
		Pit(double startX, double startY, StudentWorld* world)
			: Activatable(IID_PIT, startX, startY, right, 0, world, false, true, false, false) {}
		virtual ~Pit() {}
		virtual void doSomething() {}
		virtual void activateIfApt(Actor* a); 

};

// =======================================  Flame's Decleration  ===========================================

class Flame : public Activatable
{
	public:
		Flame(double startX, double startY, int dir, StudentWorld* world)
			: Activatable(IID_FLAME, startX, startY, dir, 0, world, true, true, false, false), m_numTicks(0) {}
		virtual ~Flame() {}
		virtual void doSomething();
		virtual void activateIfApt(Actor* a);
	
	private:
		int m_numTicks;
};

// =======================================  Vomit's Decleration  ===========================================

class Vomit : public Activatable
{
	public:
		Vomit(double startX, double startY, StudentWorld* world)
			: Activatable(IID_VOMIT, startX, startY, right /*? ? ? ?*/ , 0, world,false, true, false, false) {}
		virtual ~Vomit() {}
		virtual void doSomething();
		virtual void activateIfAppropriate(Actor* a) {} //for now

};

// =====================================  Landmine's Decleration  ==========================================

class Landmine : public Activatable
{
	public:
		Landmine(double startX, double startY, StudentWorld* world)
			: Activatable(IID_LANDMINE, startX, startY, right, 1, world,false, true, false, true),
			  m_numOfTicks(0), m_isActive(false) {}
		virtual ~Landmine() {}
		virtual void doSomething();
		virtual void activateIfApt(Actor* a);
		virtual void dieByFallOrBurnIfAppropriate() {};

	private:
		int m_numOfTicks;
		bool m_isActive;
};

// ========================================  Exit's Decleration  ===========================================

class Exit : public Activatable
{
	public:
		Exit(double startX, double startY, StudentWorld* world)
			: Activatable(IID_EXIT, startX, startY, right, 1, world,false, true, true, false) {}
		virtual ~Exit() {};
		virtual void doSomething() {}
		virtual void activateIfApt(Actor* a);

};

// ===================================  Goodie's Decleration  =======================================
// ==================================================================================================

class Goodie : public Activatable
{
	public:
		Goodie::Goodie(int imageID, double startX, double startY, StudentWorld* world)
			: Activatable(imageID, startX, startY, right, 1, world,false, false, false, true) {}
		virtual ~Goodie() {}
		virtual void setDead();
		virtual void activateIfApt(Actor* a) {}

};

// ===================================  VaccineGoodie's Decleration  =======================================

class VaccineGoodie : public Goodie
{
	public:
		VaccineGoodie::VaccineGoodie(double startX, double startY, StudentWorld* world)
			: Goodie(IID_VACCINE_GOODIE, startX, startY, world) {}
		virtual ~VaccineGoodie() {}
		virtual void doSomething() {}
		virtual void activateIfApt(Actor* a);

};

// ===================================  GasCanGoodie's Decleration  =======================================

class GasCanGoodie : public Goodie
{
	public:
		GasCanGoodie::GasCanGoodie(double startX, double startY, StudentWorld* world)
			: Goodie(IID_GAS_CAN_GOODIE, startX, startY, world) {}
		virtual ~GasCanGoodie() {}
		virtual void doSomething() {}
		virtual void activateIfApt(Actor* a);

};

// ===================================  LandmineGoodie's Decleration  =====================================

class LandmineGoodie : public Goodie
{
	public:
		LandmineGoodie::LandmineGoodie(double startX, double startY, StudentWorld* world)
			: Goodie(IID_LANDMINE_GOODIE, startX, startY, world) {}
		virtual ~LandmineGoodie() {}
		virtual void doSomething() {}
		virtual void activateIfApt(Actor* a);
	
};

// ===================================  Wall's Decleration  ===============================================

class Wall : public Actor
{
	public:
		Wall::Wall(double startX, double startY, StudentWorld* world)
			: Actor(IID_WALL, startX, startY, right, 0, world, false, false, false, false, true, true, false) {}
		virtual ~Wall() {}
		virtual void doSomething() {}
		virtual void activateIfApt(Actor* a) {};

};

#endif 

