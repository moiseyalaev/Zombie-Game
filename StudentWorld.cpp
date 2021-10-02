#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <string>
#include <iostream>
#include <sstream> // for std::ostringstream
#include <iomanip>
#include <cmath> 
using namespace std;

GameWorld *createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath), m_player(nullptr), m_numCitizens(0), // inititialize
	m_levelCompleted(false), m_destructorCalled(false)
{}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init()
{	
	m_levelCompleted = m_destructorCalled = false; // resest member vars
	Level lev(assetPath());
	string strLevel = readInLevel();

	Level::LoadResult result = lev.loadLevel(strLevel);

	if (result == Level::load_fail_file_not_found)		
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;
	else if (result == Level::load_success)
	{
		for (int i = 0; i < LEVEL_WIDTH; i++) //loop for the width 
		{
			for (int j = 0; j < LEVEL_HEIGHT; j++) //loop for the height
			{
				Level::MazeEntry ge = lev.getContentsOf(i, j);
				switch (ge)
				{
				case Level::empty:
					break;
				case Level::smart_zombie:
					m_actors.push_back(new SmartZombie(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					break;
				case Level::dumb_zombie:
					m_actors.push_back(new DumbZombie(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					break;
				case Level::player:
					m_player = new Penelope(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this);
					m_actors.push_back(m_player);
					break;
				case Level::exit:
					m_actors.push_back(new Exit(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					break;
				case Level::wall:
					m_actors.push_back(new Wall(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					break;
				case Level::pit:
					m_actors.push_back(new Pit(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					break;
				case Level::landmine_goodie:
					m_actors.push_back(new LandmineGoodie(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					break;
				case Level::gas_can_goodie:
					m_actors.push_back(new GasCanGoodie(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					break;
				case Level::vaccine_goodie:
					m_actors.push_back(new VaccineGoodie(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					break;
				case Level::citizen:
					m_actors.push_back(new Citizen(i*SPRITE_WIDTH, j*SPRITE_HEIGHT, this));
					m_numCitizens++; 
					break;

				}
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
	
}

int StudentWorld::move()
{
	vector<Actor *>::iterator p1 = m_actors.begin();
	for (; p1 != m_actors.end() && !m_player->isDead(); p1++) //loop through the actor you wish to doSomething() with this tick
	{	
		if (!(*p1)->isDead()) //if the actor is dead then move on to the next actor
		{
			(*p1)->doSomething(); //call its doSomething() 
			
			vector<Actor *>::iterator p2 = m_actors.begin();
			
			for (; p2 != m_actors.end() && !m_player->isDead(); p2++) //loop through actors arry to see if doSomething() has triggered anything 
			{
				// if the curr actor is not dead and overlaps with another actor
				if (!(*p1)->blocksMovement() && (*p1) != (*p2) && !(*p1)->isDead() && !(*p2)->isDead() && doesOverlap(*p1,*p2)) 
				{
					(*p1)->activateIfApt((*p2)); //activate 

					if (m_levelCompleted) // if activation of exit, return finshed lvl
					{
						playSound(SOUND_LEVEL_FINISHED);
						return GWSTATUS_FINISHED_LEVEL;
					}
				}
			}
			
		}

	}

	if (m_player->isDead()) // prompt Penel to do something if she not dead
		return GWSTATUS_PLAYER_DIED;

	
	for (vector<Actor *>::iterator q = m_actors.begin(); q != m_actors.end(); q++)
	{
		if ((*q)->isDead()) //if the actor is dead delete what it points to and set its pointer to null
		{
			delete *q;	 // delete memory
			(*q) = nullptr; //set the ptr to null
		}
	}
	m_actors.erase(remove(m_actors.begin(), m_actors.end(), nullptr), m_actors.end()); // get rid of all elements with nullptr
	
	//update gametext and move on
	updateGameStatText(); 
	return GWSTATUS_CONTINUE_GAME;
	
}

void StudentWorld::cleanUp()
{
	//destroy everything if the destructor has not been called already
	if (m_destructorCalled)
		return;
	//delete all the elemeents in the container
	vector<Actor *>::iterator p = m_actors.begin();
	for(; p != m_actors.end(); p++)
		delete *p;

	m_actors.clear(); //empty the container

	m_destructorCalled = true;	// set the destruction bool to true
}	

void StudentWorld::addZombie(double x, double y)
{
	//addZombie after infection depending on randInt
	if (randInt(1, 10) <= 7)
		m_actors.push_back(new DumbZombie(x, y, this));
	else
		m_actors.push_back(new SmartZombie(x, y, this));
}

double* StudentWorld::newCoords(double db[2], Direction checkDir, int pixelsToMove)
{
	// essentially used for anaylizing potential moves by inserting coordinates and getting back next quardiants
	switch (checkDir)
	{
		case(0):
			db[0] += pixelsToMove;
			break;
		case(90):
			db[1] += pixelsToMove;
			break;
		case(180):
			db[0] -= pixelsToMove;
			break; 
		case(270):
			db[1] -= pixelsToMove;
			break;	
	}
	return db;
}

bool StudentWorld::isBlocked(double x, double y, Actor* a)
{
	vector<Actor*>::iterator p = m_actors.begin();
	
	while(p != m_actors.end())
	{
		if ((*p) == a || !(*p)->blocksMovement())
		{
			p++;
			continue;
		}

		//	Boundary box for character passed in in coordinates terms
		double left1[2] = { x, y + SPRITE_HEIGHT - 1 };
		double right1[2] = { x + SPRITE_WIDTH - 1, y };


		//	Boundary box for curr actor in loop
		double left2[2] = { (*p)->getX(), (*p)->getY() + SPRITE_HEIGHT - 1 };
		double right2[2] = { (*p)->getX() + SPRITE_WIDTH - 1, (*p)->getY() };
		//4 total coordinates

		// If rectangles not aligned horizontally, then continue
		if (left1[0] > right2[0] || left2[0] > right1[0])
		{
			p++;
			continue;
		}

		// If rectangles not alighed vertically
		if (left1[1] < right2[1] || left2[1] < right1[1])
		{
			p++;
			continue;
		}

		return true; //means it is blocked
	}

	return false; //does not block
}

bool StudentWorld::doesOverlap(Actor* a1, Actor* a2)
{
		double delX = a1->getX() - a2->getX();
		double delY = a1->getY() - a2->getY();
		return pow(delX,2) + pow(delY,2) <= 100;
}

bool StudentWorld::doesOverlap(double x, double y) // written for flame
{
	vector<Actor*>::iterator p = m_actors.begin();
	for (; p != m_actors.end(); p++) //loop through the array of actors
	{
		if (pow((*p)->getX() - x, 2) + pow((*p)->getY() - y, 2) <= 100) //doesOverlap 
		{
			if ((*p)->blocksFlame()) //doesBlockFlame (allows to not need to check for selfchecks)
				return  true;
		}
	}
	
	return false;
}


void StudentWorld::addFlame(double x, double y, int dir)
{
	m_actors.push_back(new Flame(x, y, dir, this));
}

void StudentWorld::addPit(double x, double y)
{
	m_actors.push_back(new Pit(x, y, this));
}

void StudentWorld::addLandmine(double x, double y)
{
	m_actors.push_back(new Landmine(x, y, this));
}

double StudentWorld::distance(double dx, double dy, Actor* a1, Actor* a2)
{
	// distance formul
	double delX = (a1->getX()+dx) - a2->getX();
	double delY = (a1->getY()+dy) - a2->getY();
	return sqrt(pow(delX,2) + pow(delY,2));
}

double StudentWorld::closestZombieDist(double dx, double dy, Actor* a)
{
	vector<Actor *>::iterator curr = m_actors.begin();
	double currMin = 100000;
	//loop through 
	for (; curr != m_actors.end(); curr++)
		if ((*curr)->triggersMines() && (*curr)->threatensCitizens()) //thus is a zombie
			if (distance(dx, dy, a, *curr) < currMin) //this zombies distance is less than min dist
				currMin = distance(dx, dy, a, *curr);
	return currMin;
}


string StudentWorld::readInLevel()
{
	// reading in file with string stream
	ostringstream levelFile;
	int numLevel = getLevel();
	numLevel < 10 ? levelFile << "level0" : levelFile << "level";
	levelFile << numLevel << ".txt";
	return levelFile.str();
}

void StudentWorld::updateGameStatText()
{
//Score:	1200		Level : 14		Lives : 2		Vacc : 4		Flames : 19		Mines : 3		Infected : 0
	ostringstream temp; //use oss to do this
	temp << "Score: "    << setw(6) << getScore()<< temp.fill('0')	<< "  "; //add two spaces
	temp << "Level: "    << getLevel()								<< "  ";
	temp << "Lives: "    << getLives()								<< "  ";
	temp << "Vacc: "     << m_player->getNumVaccines()				<< "  ";
	temp << "Flames: "   << m_player->getNumFlames()				<< "  ";
	temp << "Mines: "    << m_player->getNumLandmines()				<< "  ";
	temp << "Infected: " << m_player->getNumInfected()				<< "  ";

	setGameStatText(temp.str());
}


