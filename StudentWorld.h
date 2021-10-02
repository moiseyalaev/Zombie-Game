#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

//class Actor;
//class Penlope;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	virtual ~StudentWorld();
	bool isBlocked(double x, double y, Actor* a);
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void addZombie(double x, double y);
	double distance(double dx, double dy, Actor* a1, Actor* a2);
	double closestZombieDist(double dx, double dy, Actor* a);
	double* newCoords(double db[2], Direction checkDir, int pixelsToMove);
	std::string readInLevel();
	bool doesOverlap(Actor* a1, Actor* a2);
	bool doesOverlap(double x, double y);
	void addLandmine(double x, double y);
	void addFlame(double x, double y, int dir); 
	void addPit(double x, double y);
	void updateGameStatText();
	Penelope* getPenelope()			{ return m_player; }
	void recordCitizenGone()		{ m_numCitizens--; }
	int numCitizens() const			{ return m_numCitizens; }
	void levelCompleted()			{ m_levelCompleted = true; }
		  
	//double determineMovement(Actor* a);
//	void activateOnAptActors(Actor* a);// For each actor overlapping a, activate a if appropriate.
	
	


private:
	std::vector<Actor*> m_actors; // vector of Actor pointers
	Penelope* m_player;
	int m_numCitizens;
	bool m_levelCompleted, m_destructorCalled;
};

#endif 