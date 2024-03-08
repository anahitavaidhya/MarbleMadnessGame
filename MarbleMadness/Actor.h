#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "StudentWorld.h"
class StudentWorld;
class Agent;

// Actor Class
class Actor : public GraphObject{
public:
    Actor(StudentWorld* world, int imageID, double startX, double startY, int hitPoints, int startDir):
          GraphObject(imageID, startX, startY, startDir), m_world(world), m_alive(true), m_hp(hitPoints) {
            //Figure out Constructor
    }
    //Get World
    StudentWorld* getWorld() {return m_world;}
    
    //Do Something
    virtual void doSomething() = 0;
    
    //Get HP
    int getHp() const {return m_hp;}
    
    //Set HP
    void setHp(int change) {m_hp = change;}
    
    //Set Alive
    void setAlive(bool set) {m_alive = set;}
    
    // Mark this actor as dead
    void setDead() {m_alive = false; setVisible(false);}
    
    //Virtual Functions
    // Reduce this actor's hit points
    virtual void decHitPoints(int amt);
    
    //Check if Actor is alive based on m_alive
    virtual bool isAlive() const {return m_alive;}
    
    //Is this an obstacle?
    virtual bool isObstacle() const { return false; }
    
    // Can an agent occupy the same square as this actor?
    virtual bool allowsAgentColocation() const { return false; }
  
    // Can a marble occupy the same square as this actor?
    virtual bool allowsMarble() const { return false; }
  
    // Does this actor count when a factory counts items near it?
    virtual bool countsInFactoryCensus() const { return false; }
  
    // Does this actor stop peas from continuing?
    virtual bool stopsPea() const { return false; }
  
    // Can this actor be damaged by peas?
    virtual bool isDamageable() const { return false; }

    // Cause this Actor to sustain damageAmt hit points of damage.
    virtual void damage(int damageAmt) { decHitPoints(damageAmt); }
  
    // Can this actor be pushed by a to location x,y?
    virtual bool bePushedBy(Agent* a, int x, int y) { return false; }
  
    // Can this actor be swallowed by a pit?
    virtual bool isSwallowable() const { return false; }
  
    // Can this actor be picked up by a ThiefBot?
    virtual bool isStealable() const { return false; }
  
    // Make the actor sustain damage.  Return true if this kills the
    // actor, and false otherwise.
    virtual bool tryToBeKilled(int damageAmt) { damage(damageAmt); return isAlive(); }
private:
    StudentWorld* m_world;
    int m_hp;
    bool m_alive;
};

//Agent
class Agent : public Actor{
public:
    Agent(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int startDir) : Actor(world, imageID, startX, startY, hitPoints, startDir){
        setAlive(true);
    }
    virtual void doSomething() = 0;

    bool isAlive() const { return getHp() > 0; }
    
    // Move to the adjacent square in the direction the agent is facing
    // if it is not blocked, and return true.  Return false if the agent
    // can't move.
    bool moveIfPossible() { return true; }

    // Return true if this agent can push marbles (which means it's the
    // player).
    virtual bool canPushMarbles() const { return false; }

    // Return true if this agent doesn't shoot unless there's an unobstructed
    // path to the player.
    virtual bool needsClearShot() const { return false; }

    // Return the sound effect ID for a shot from this agent.
    virtual int shootingSound() const { return false; }
};

//Player
class Player : public Agent{
public:
    Player(StudentWorld* world, int startX, int startY) : Agent(world, startX, startY, IID_PLAYER, 20, 0){
        setVisible(true);
        m_peas = 20;
    }
    
    //virtual bool isAlive() const;
    virtual void doSomething();
    
    virtual void decHitPoints(int amt);
    
    virtual bool isDamageable() const {return true;}

    virtual bool canPushMarbles() const {return true;}

    virtual int shootingSound() const {return SOUND_PLAYER_FIRE;}

    // Get player's health percentage
    virtual int getHealthPct() const {return ((getHp()*100) / 20);}

    // Get player's amount of ammunition
    virtual int getAmmo() const {return m_peas;}

    // Restore player's health to the full amount.
    void restoreHealth() {setHp(20);}

    // Increase player's amount of ammunition.
    void increaseAmmo() {m_peas += 20;}
    
    // Decrease player's amount of ammunition when they shoot a pea
    void decreaseAmmo() {if(m_peas>0) m_peas -= 1;}
    
private:
    int m_peas;
};

//Wall
class Wall : public Actor
{
public:
    Wall(StudentWorld* world, int startX, int startY) : Actor(world, IID_WALL, startX, startY, 0, -1){
        setVisible(true);
        //setAlive(false);
    }
    virtual void doSomething(){}
    virtual bool isObstacle() const {return true;}
};

//Marble
class Marble : public Actor{
public:
    Marble(StudentWorld* world, int startX, int startY) : Actor(world, IID_MARBLE, startX, startY, 10, -1){
        setVisible(true);
    }
    virtual bool isAlive() const { return getHp() > 0;}
    virtual void doSomething(){}
  
    // Can this actor be damaged by peas?
    virtual bool isDamageable() const {return true;}
  
    // Can this actor be pushed by a to location x,y?
    virtual bool bePushedBy(Agent* a, int x, int y);
  
    // Can this actor be swallowed by a pit?
    virtual bool isSwallowable() const {return true;}
};

//Pit
class Pit : public Actor{
public:
    Pit(StudentWorld* world, int startX, int startY) : Actor(world, IID_PIT, startX, startY, 0, -1){
        setVisible(true);
    }
    virtual void doSomething();
    // Can a marble occupy the same square as this actor?
    virtual bool allowsMarble() const {return true;}
    
    virtual bool allowsMarbleColocation() const {return true;}

    // Cause this Actor to sustain damageAmt hit points of damage.
    virtual void damage(int damageAmt) { decHitPoints(damageAmt); }
    
    // Make the actor sustain damage.  Return true if this kills the
    // actor, and false otherwise.
    virtual bool tryToBeKilled(int damageAmt) {return false;}
};

//Pea
class Pea : public Actor{
public:
    Pea(StudentWorld* world, int startX, int startY, int startDir) : Actor(world, IID_PEA, startX, startY, 0, startDir){
        setVisible(true);
    }
    virtual void doSomething();
    virtual bool allowsAgentColocation() const {return true;}
  
    // Can a marble occupy the same square as this actor?
    virtual bool allowsMarble() const {return true;}

    // Cause this Actor to sustain damageAmt hit points of damage.
    virtual void damage(int damageAmt) { decHitPoints(damageAmt); }
    
    // Make the actor sustain damage.  Return true if this kills the
    // actor, and false otherwise.
    virtual bool tryToBeKilled(int damageAmt) {return false;}
};

//Pickupable Item
class PickupableItem : public Actor{
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID, int score) : Actor(world, imageID, startX, startY, 0, -1), m_score(score){
        setVisible(true);
    }
    void doSomething() = 0;
    virtual bool allowsAgentColocation() const {return true;}
    int getScore() const { return m_score; }
    
private:
    int m_score;
};

//Crystal
class Crystal : public PickupableItem {
public:
    Crystal(StudentWorld* world, int startX, int startY) : PickupableItem(world, startX, startY, IID_CRYSTAL, 50){
        setVisible(true);
    }
    void doSomething();
    
    virtual bool isAlive() const { return Actor::isAlive(); }
    
    // Can an agent occupy the same square as this actor?
    virtual bool allowsAgentColocation() const {return true;}

    // Cause this Actor to sustain damageAmt hit points of damage.
    virtual void damage(int damageAmt) { decHitPoints(damageAmt); }
  
    // Make the actor sustain damage.  Return true if this kills the
    // actor, and false otherwise.
    virtual bool tryToBeKilled(int damageAmt) {return false;}
};

//Goodie
class Goodie : public PickupableItem {
public:
    Goodie(StudentWorld* world, int startX, int startY, int imageID, int score) : PickupableItem(world, startX, startY, imageID, score){}
    virtual void doSomething() = 0;
    virtual bool isStealable() const { return true; }

    // Set whether this goodie is currently stolen.
    void setStolen(bool status);
};

//Extra Life
class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(StudentWorld* world, int startX, int startY) : Goodie(world, startX, startY, IID_EXTRA_LIFE, 1000){
        setVisible(true);
    }
    virtual void doSomething();
};

//Restore Health
class RestoreHealthGoodie : public Goodie {
public:
    RestoreHealthGoodie(StudentWorld* world, int startX, int startY) : Goodie(world, startX, startY, IID_RESTORE_HEALTH, 500){
        setVisible(true);
    }
    virtual void doSomething();
};

//Ammo Goodie
class AmmoGoodie : public Goodie {
public:
    AmmoGoodie(StudentWorld* world, int startX, int startY) : Goodie(world, startX, startY, IID_AMMO, 100){
        setVisible(true);
    }
    virtual void doSomething();
};


//Exit
class Exit : public Actor{
public:
    Exit(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const {return true;}
};

//Robot
class Robot : public Agent{
public:
    Robot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score, int startDir);
    
    int getTicksMove() const { return m_ticksMove; }
    int getTickCount() const { return m_tickCount; }
    void incTickCount() { m_tickCount++; }
    void tickCountReset() { m_tickCount = 1; }
    
    virtual void doSomething() const {}
    virtual void damage(int damageAmt) = 0;
    virtual bool isDamageable() const { return true; }
    virtual bool needsClearShot() const { return true; }
    virtual int shootingSound() const {return SOUND_ENEMY_FIRE;}
    
    // Does this robot shoot?
    virtual bool isShootingRobot() const { return false; }
    
private:
    int m_score;
    int m_ticksMove;
    int m_tickCount;
};

//RageBot
class RageBot : public Robot {
public:
    RageBot(StudentWorld* world, int startX, int startY, int startDir) : Robot(world, startX, startY, IID_RAGEBOT, 10, 100, startDir){
        setVisible(true);
    }
    virtual void doSomething();
    virtual void damage(int damageAmt);
    bool isShootingRobot() const { return true; }
    bool stopsPea() const { return true; }
};

//ThiefBotFactory
class ThiefBotFactory : public Actor{
public:
    enum ProductType { REGULAR, MEAN };

    ThiefBotFactory(StudentWorld* world, int startX, int startY, ProductType type) : Actor(world, IID_ROBOT_FACTORY, startX, startY, 0, -1){
        setVisible(true);
        m_product = type;
    }
    char getProduct() const { return m_product; }
    virtual void doSomething();
    virtual bool isObstacle() const {return true;}
        
    private:
        char m_product;
    
};

//ThiefBot
class ThiefBot : public Robot {
public:
    ThiefBot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score) : Robot(world, startX, startY, imageID, hitPoints, score, 0){
       resetDistanceBeforeTurning();
       resetDistanceMoved();
       m_stolenGoodie = nullptr;
   }
    virtual void doSomething();
    virtual bool countsInFactoryCensus() const {return true;}
    virtual void damage(int damageAmt) = 0;
    
    int getDistanceBeforeTurning() const { return m_distanceBeforeTurning; }
    int getDistanceMoved() const { return m_distanceMoved; }
    Goodie* getStolenGoodie() const { return m_stolenGoodie; }
    
    void resetDistanceBeforeTurning() {m_distanceBeforeTurning = (rand() % 6) + 1;}
    void resetDistanceMoved() { m_distanceMoved = 0; }
    void incDistanceMoved() { m_distanceMoved++; }
    void setStolenGoodie(Goodie* goodie) { m_stolenGoodie = goodie; }
    bool canThiefMove();
        
private:
    int getRandDir() const;
    int m_distanceBeforeTurning;
    int m_distanceMoved;
    Goodie* m_stolenGoodie;
};

//RegularThiefBot
class RegularThiefBot : public ThiefBot {
public:
    RegularThiefBot(StudentWorld* world, int startX, int startY) : ThiefBot(world, startX, startY, IID_THIEFBOT, 5, 10)
   {
       setVisible(true);
   }
    virtual void doSomething();
    virtual void damage(int damageAmt);
    virtual bool isShootingRobot() const {return false;}
    bool stopsPea() const { return true; }
};

//MeanThiefBot
class MeanThiefBot : public ThiefBot {
public:
    MeanThiefBot(StudentWorld* world, int startX, int startY) : ThiefBot(world, startX, startY, IID_MEAN_THIEFBOT, 8, 20){
        setVisible(true);
    }
    virtual void doSomething();
    virtual void damage(int damageAmt);
    virtual bool isShootingRobot() const {return true;}
    bool stopsPea() const { return true; }
};


#endif // ACTOR_H_
