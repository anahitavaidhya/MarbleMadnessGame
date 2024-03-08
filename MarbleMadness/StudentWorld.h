#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include "Actor.h"
#include <vector>


// Note:  A convention used in a number of interfaces is to represent a
// direction with the adjustments to x and y needed to move one step in
// that direction:
// left:   dx is -1, dy is  0
// right:  dx is  1, dy is  0
// up:     dx is  0, dy is  1
// down:   dx is  0, dy is -1

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    //Get Bonus
    int getBonus() const { return m_bonus; }
    
    //Decrease Bonus
    void decBonus() { if(m_bonus > 0) m_bonus -= 1; }
    
    //Damage by pea
    Actor* canBeDamaged(int x, int y) const;
    
    // Can agent move to x,y?  (dx and dy indicate the direction of motion)
    bool canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy) const;

    // Can a marble move to x,y?
    bool canMarbleMoveTo(int x, int y) const;

    // Is the player on the same square as an Actor?
    bool isPlayerColocatedWith(int x, int y) const;
    
    //Check is player is on Actor
    bool isPlayerHere(Actor* a) const;

    // Swallow any swallowable object at a's location.  (a is only ever
    // going to be a pit.)
    bool swallowSwallowable(Actor* a);

    // If a pea were at x,y moving in direction dx,dy, could it hit the
    // player without encountering any obstructions?
    bool existsClearShotToPlayer(Robot* r);

    // If a factory is at x,y, how many items of the type that should be
    // counted are in the rectangle bounded by x-distance,y-distance and
    // x+distance,y+distance?  Set count to that number and return true,
    // unless an item is on the factory itself, in which case return false
    // and don't care about count.  (The items counted are only ever going
    // ThiefBots.)
    bool doFactoryCensus(int x, int y, int distance, int& count) const;

    // If an item that can be stolen is at x,y, return a pointer to it;
    // otherwise, return a null pointer.  (Stealable items are only ever
    // going be goodies.)
    Goodie* getColocatedStealable(int x, int y) const;

    // Restore player's health to the full amount.
    void restorePlayerHealth() {m_player->restoreHealth();}

    // Increase the amount of ammunition the player has
    void increaseAmmo() {m_player->increaseAmmo();}

    // Are there any crystals left on this level?
    bool anyCrystals() const {return (m_crystals == 0);}

    // Reduce the count of crystals on this level by 1.
    void decCrystals() { m_crystals--; }
  
    // Indicate that the player has finished the level.
    void setLevelFinished() { m_levelCompleted = true; }
  
    // Add an actor to the world
    void addActor(Actor* a) {m_actorsAdd.push_back(a);}
    
    //Check if there is any actor there
    bool isAnyActorAt(int x, int y) const;
    
    //Check if there is a ThiefBot here
    bool isThiefBotAt(int x, int y) const;
    
    //Check if the robot is being blocked by something
    bool robotBlocked(Agent* a, int& tryX, int& tryY) const;
    
    //Check if something is blocked
    bool isBlocked(int startX, int endX, int startY, int endY) const;
   
private:
    Player* m_player;
    std::vector<Actor*> m_actors;
    std::vector<Actor*> m_actorsAdd;
    bool m_levelCompleted;
    int m_bonus;
    int m_crystals;
    
};

#endif // STUDENTWORLD_H_
