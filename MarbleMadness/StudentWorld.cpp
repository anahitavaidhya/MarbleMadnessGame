#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr), m_levelCompleted(false), m_bonus(1000){}

StudentWorld::~StudentWorld(){
    cleanUp();
}

bool StudentWorld::canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy) const{
    // Calculate the new position
    int newX = x + dx;
    int newY = y + dy;

    // Check if the new position is within bounds
    if (newX <= 0 || newX > VIEW_WIDTH || newY <= 0 || newY > VIEW_HEIGHT) {
        // Out of bounds
        return false;
    }
    auto it = m_actors.begin();
    while (it != m_actors.end()) {
        if((*it)->getX() == newX && (*it)->getY() == newY){
            if((*it)->bePushedBy(agent, newX+dx, newY+dy))
                return true;
            
            if((*it)->allowsAgentColocation())
                return true;
            
            return false;
        }
        it++;
    }
    // No actor at the new position prevents agent's movement
    return true;
}

bool StudentWorld::canMarbleMoveTo(int x, int y) const{
    /*check if empty spot return true;
     check if isAnyActorAt return false;*/
    auto it = m_actors.begin();
    while (it != m_actors.end()) {
        if((*it)->getX() == x && (*it)->getY() == y){
            if((*it)->allowsMarble()){
                return true;
            }
            return false;
        }
        it++;
    }
    return true;
}

// If a pea were at x,y moving in direction dx,dy, could it hit the
// player without encountering any obstructions?
bool StudentWorld::existsClearShotToPlayer(Robot* r) {
    int playerX = m_player->getX();
    int playerY = m_player->getY();
    
    int robotX = r->getX();
    int robotY = r->getY();
    
    switch(r->getDirection()){
        //Right
        case 0:
            if(playerY == robotY && playerX > robotX){
                for(int i = robotX+1; i < playerX; i++){
                    if(isAnyActorAt(i, robotY)){
                        return false;
                    }
                }
                addActor(new Pea(r->getWorld(), robotX+1, robotY, 0));
                playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            return false;
        //Up
        case 90:
            if(playerX == robotX && playerY > robotY){
                for(int i = robotY+1; i < playerY; i++){
                    if(isAnyActorAt(robotX, i)){
                        return false;
                    }
                }
                
                addActor(new Pea(r->getWorld(), robotX, robotY+1, 90));
                playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            return false;
        //Left
        case 180:
            if(playerY == robotY && playerX < robotX){
                for(int i = playerX+1; i < robotX; i++){
                    if(isAnyActorAt(i, robotY)){
                        return false;
                    }
                }
                
                addActor(new Pea(r->getWorld(), robotX-1, robotY, 180));
                playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            return false;
        //Down
        case 270:
            if(playerX == robotX && playerY < robotY){
                for(int i = playerY+1; i < robotY; i++){
                    if(isAnyActorAt(robotX, i)){
                        return false;
                    }
                }
                
                addActor(new Pea(r->getWorld(), robotX, robotY-1, 270));
                playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            return false;
    }
    return false;
}

int StudentWorld::init()
{
    //Check if vector is empty
    if (!m_actors.empty())
        m_actors.clear();
    
        m_bonus = 1000;
        m_crystals = 0;
        m_levelCompleted = false;
    
        //Get Level
        Level lev(assetPath());
        string levelFile = "";
        if(getLevel() < 10)
            levelFile = "level0" + to_string(getLevel()) + ".txt";
        else
            levelFile = "level" + to_string(getLevel()) + ".txt";
        
        Level::LoadResult result = lev.loadLevel(levelFile);
        
        //Check Level
        if (result == Level::load_fail_file_not_found || getLevel() == 100){
            cerr << "Could not find " << levelFile << ".txt data file\n";
            return GWSTATUS_PLAYER_WON;
        } else if (result == Level::load_fail_bad_format){
            cerr << "Your level was improperly formatted\n";
            return GWSTATUS_LEVEL_ERROR;
        } else if (result == Level::load_success)
        {
            cerr << "Successfully loaded level\n";
            Level::MazeEntry loc;
            //Push players into vector
            for(int x = 0; x < VIEW_WIDTH; x++){
                for(int y = 0; y < VIEW_HEIGHT; y++){
                    loc = lev.getContentsOf(x, y);
                    switch (loc){
                        case Level::empty:
                            break;
                        case Level::exit:
                            m_actors.push_back(new Exit(this, x, y));
                            break;
                        case Level::player:
                            m_player = new Player(this, x, y);
                            break;
                        case Level::horiz_ragebot:
                            m_actors.push_back(new RageBot(this, x, y, 0));
                            break;
                        case Level::vert_ragebot:
                            m_actors.push_back(new RageBot(this, x, y, 270));
                            break;
                        case Level::thiefbot_factory:
                            m_actors.push_back(new ThiefBotFactory(this, x, y, ThiefBotFactory::REGULAR));
                            break;
                        case Level::mean_thiefbot_factory:
                            m_actors.push_back(new ThiefBotFactory(this, x, y, ThiefBotFactory::MEAN));
                            break;
                        case Level::wall:
                            m_actors.push_back(new Wall(this, x, y));
                            break;
                        case Level::marble:
                            m_actors.push_back(new Marble(this, x, y));
                            break;
                        case Level::pit:
                            m_actors.push_back(new Pit(this, x, y));
                            break;
                        case Level::crystal:
                            m_actors.push_back(new Crystal(this, x, y));
                            m_crystals++;
                            break;
                        case Level::restore_health:
                            m_actors.push_back(new RestoreHealthGoodie(this, x, y));
                            break;
                        case Level::extra_life:
                            m_actors.push_back(new ExtraLifeGoodie(this, x, y));
                            break;
                        case Level::ammo:
                            m_actors.push_back(new AmmoGoodie(this, x, y));
                            break;
                    }
                }
            }
        }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //Set Game Text
    int score = getScore();
    int level = getLevel();
    int bonus = getBonus();
    int livesLeft = getLives();
    // Next, create a string from your statistics, of the form:
    // Score: 0000100 Level: 03 Lives: 3 Health: 70% Ammo: 216 Bonus: 34
    ostringstream oss;
    
    oss.fill('0');
    oss << "Score: ";
    oss << setw(7) << score << "  ";
    
    oss.fill('0');
    oss << "Level: ";
    oss << setw(2) << level << "  ";
    
    oss.fill(' ');
    oss << "Lives: ";
    oss << setw(2) << livesLeft << "  ";
    
    oss.fill(' ');
    oss << "Health: ";
    oss << setw(3) << m_player->getHealthPct() << "%  ";
    
    oss.fill(' ');
    oss << "Ammo: ";
    oss << setw(3) << m_player->getAmmo() << "  ";
    
    oss.fill(' ');
    oss << "Bonus: ";
    oss << setw(3) << bonus << "  ";

    setGameStatText(oss.str());
    
    // Give each actor a chance to do something
    m_player->doSomething();
    
    if(getLives() <= 0){
        return GWSTATUS_PLAYER_DIED;
    }
    
    if (m_levelCompleted) {
        increaseScore(getBonus());
        m_levelCompleted = false;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    // Each actor doSomething
    for(std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++){
        if((*it)->isAlive()){
            (*it)->doSomething();
            if (!m_player->isAlive()) {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
        }
    }

    //Delete Dead Actors
    if(!m_player->isAlive()){
        delete m_player;
        m_player = nullptr;
    }
    
    //Delete every actor
    std::vector<Actor*>::iterator it;
    it = m_actors.end();
    it--;
    while (it != m_actors.begin()) {
        if(!(*it)->isAlive()){
            std::vector<Actor*>::iterator it2;
            it2 = it;
            it2--;
            delete *it;
            m_actors.erase(it);
            it = it2;
        } else {
            it--;
        }
    }
    
    //Add all the new actors to Actor Vector
    for(std::vector<Actor*>::iterator no = m_actorsAdd.begin();  no != m_actorsAdd.end(); no++){
        Actor* mp = *no;
        m_actors.push_back(mp);
    }
    
    m_actorsAdd.clear();
    
    // Reduce the current bonus for the Level by one
    decBonus();
    
    // If the player has collected all of the crystals on the level, then we
    // must expose the exit so the player can advance to the next level
    if (!m_player->isAlive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    } else {
        return GWSTATUS_CONTINUE_GAME;
    }
}

void StudentWorld::cleanUp() {

    delete m_player;
    m_player = nullptr;
    
    if (m_actors.end() == m_actors.begin()) return; // empty vector
        //Delete every actor
        std::vector<Actor*>::iterator it;
        it = m_actors.end();
        it--;
        while (it != m_actors.begin()) {
            std::vector<Actor*>::iterator it2;
            it2 = it;
            it2--;
            delete *it;
            m_actors.erase(it);
            it = it2;
        }
        delete *it;
        m_actors.erase(it);
}

bool StudentWorld::isPlayerHere(Actor* a) const {
    if (m_player->getX() == a->getX() && m_player->getY() == a->getY())
        return true;
    else
        return false;
}

//Check is player is on Actor
bool StudentWorld::swallowSwallowable(Actor* a){
    std::vector<Actor*>::const_iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()) {
        if((*it)->isSwallowable()){
            if ((*it)->getX() == a->getX() && (*it)->getY() == a->getY()) {
                (*it)->setDead();
                (*it)->setHp(0);
                return true;
            }
        }
        it++;
    }
    return false;
}

//Damage by pea
Actor* StudentWorld::canBeDamaged(int x, int y) const{
    std::vector<Actor*>::const_iterator it;
    it = m_actors.begin();
    if((m_player->getX() == x && m_player->getY() == y))
        return m_player;
    while (it != m_actors.end()) {
        if((*it)->isSwallowable() || (*it)->isObstacle() || (*it)->stopsPea()){
                if ((*it)->getX() == x && (*it)->getY() == y) {
                    return *it;
                }
            }
        it++;
    }
    return nullptr;
}

//Check if something is blocked
bool StudentWorld::isBlocked(int startX, int endX, int startY, int endY) const
{
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        for (int x = startX; x <= endX; x++) {
            for (int y = startY; y <= endY; y++) {
                if ((*it)->getX() == x && (*it)->getY() == y) {
                    if ((*it)->isSwallowable() || (*it)->isObstacle() || (*it)->stopsPea())
                        return true;
                }
            }
        }
    }
    return false;
}

// Is the player on the same square as an Actor?
bool StudentWorld::isPlayerColocatedWith(int x, int y) const{
    if(m_player->getX() == x && m_player->getY() == y)
        return true;
    return false;
}

//Check if there is any actor there
bool StudentWorld::isAnyActorAt(int x, int y) const{
    // Iterate through all actors in the vector
    std::vector<Actor*>::const_iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()) {
        // Check if the actor's coordinates match the specified coordinates
        if ((*it)->getX() == x && (*it)->getY() == y) {
            // Actor found at the specified coordinates
            return true;
        }
        it++;
    }
    // No actor found at the specified coordinates
    return false;
}

//Check if the robot is being blocked by something
bool StudentWorld::robotBlocked(Agent* a, int& tryX, int& tryY) const{
    tryX = a->getX();
    tryY = a->getY();
    int tryDir = a->getDirection();
    switch (tryDir) {
        case 0:
            tryX++;
            break;
        case 180:
            tryX--;
            break;
        case 90:
            tryY++;
            break;
        case 270:
            tryY--;
            break;
    }
    
    if (tryX == m_player->getX() && tryY == m_player->getY())
        return true;
   for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
       if ((*it)->getX() == tryX && (*it)->getY() == tryY) {
           if ( (*it)->isObstacle() || (*it)->isSwallowable() || (*it)->allowsMarble() || (*it)->countsInFactoryCensus())
               return true;
       }
   }
    return false;
}

// If an item that can be stolen is at x,y, return a pointer to it;
// otherwise, return a null pointer.  (Stealable items are only ever
// going be goodies.)
Goodie* StudentWorld::getColocatedStealable(int x, int y) const{
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if((*it)->isStealable()){
            if((*it)->getX() == x && (*it)->getY() == y){
                return dynamic_cast<Goodie*>((*it));
            }
        }
    }
    return nullptr;
}

//Check if there is a ThiefBot here
bool StudentWorld::isThiefBotAt(int x, int y) const{
    // Iterate through all actors in the vector
    std::vector<Actor*>::const_iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()) {
        // Check if the actor's coordinates match the specified coordinates
        if ((*it)->getX() == x && (*it)->getY() == y) {
            if((*it)->countsInFactoryCensus()){
                return true;
            }
        }
        it++;
    }
    // No actor found at the specified coordinates
    return false;
}

// If a factory is at x,y, how many items of the type that should be
// counted are in the rectangle bounded by x-distance,y-distance and
// x+distance,y+distance?  Set count to that number and return true,
// unless an item is on the factory itself, in which case return false
// and don't care about count.  (The items counted are only ever going
// ThiefBots.)
bool StudentWorld::doFactoryCensus(int x, int y, int distance, int& count) const{
    if(isThiefBotAt(x, y)){
        return false;
    }
    
    for(int i = x-distance; i <= x+distance; i++){
        for(int j = y-distance; j <= y+distance; j++){
            if(isThiefBotAt(i, j))
                count++;
        }
    }
    return true;
}
