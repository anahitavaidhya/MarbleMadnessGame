#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor Decrease Health
void Actor:: decHitPoints(int amt){
    if((getHp()-amt) > 0){
        m_hp -= amt;
    } else {
        setDead();
    }
}

//Player
//Player specific decrease health
void Player:: decHitPoints(int amt){
    if((getHp()-amt) >= 0){
        setHp(getHp() - amt);
    } else {
    getWorld()->decLives();
    }
}
//Player specific doSomething
void Player :: doSomething(){
    if(!isAlive()){
        return;
    } else {
        int ch;
        if (getWorld()->getKey(ch))
        {
            //User hits key
            switch (ch){
                case KEY_PRESS_LEFT:
                    if(getWorld()->canAgentMoveTo(this, getX(), getY(), -1, 0)){
                        setDirection(180);
                        moveTo(getX() - 1, getY());
                    }
                    break;
                case KEY_PRESS_RIGHT:
                    if(getWorld()->canAgentMoveTo(this, getX(), getY(), 1, 0)){
                        setDirection(0);
                        moveTo(getX() + 1, getY());
                    }
                    break;
                case KEY_PRESS_UP:
                    if(getWorld()->canAgentMoveTo(this, getX(), getY(), 0, 1)){
                        setDirection(90);
                        moveTo(getX(), getY()+1);
                    }
                    break;
                case KEY_PRESS_DOWN:
                    if(getWorld()->canAgentMoveTo(this, getX(), getY(), 0, -1)){
                        setDirection(270);
                        moveTo(getX(), getY()-1);
                    }
                    break;
                case KEY_PRESS_SPACE:
                    if(getAmmo() > 0){
                        decreaseAmmo();
                        switch(getDirection()){
                            //Right
                            case 0:
                                getWorld()->addActor(new Pea(getWorld(), getX()+1, getY(), getDirection()));
                                getWorld()->playSound(SOUND_PLAYER_FIRE);
                                break;
                            //Up
                            case 90:
                                getWorld()->addActor(new Pea(getWorld(), getX(), getY()+1, getDirection()));
                                getWorld()->playSound(SOUND_PLAYER_FIRE);
                                break;
                            //Left
                            case 180:
                                getWorld()->addActor(new Pea(getWorld(), getX()-1, getY(), getDirection()));
                                getWorld()->playSound(SOUND_PLAYER_FIRE);
                                break;
                            //Down
                            case 270:
                                getWorld()->addActor(new Pea(getWorld(), getX(), getY()-1, getDirection()));
                                getWorld()->playSound(SOUND_PLAYER_FIRE);
                                break;
                        }
                    }
                    break;
                case KEY_PRESS_ESCAPE:
                    //  Set the player to dead
                    setDead();
                    setHp(-getHp());
                    break;
                }
            }
        }
    }

//Marble
// Can this actor be pushed by a to location x,y?
bool Marble::bePushedBy(Agent* a, int x, int y){
    if(!a->canPushMarbles())
        return false;
     //Check if the destination is an empty spot or a pit
    if (getWorld()->canMarbleMoveTo(x, y)) {
        moveTo(x,y);
        return true;
    }
    // Check if there is any other object at the destination
        return false;
}

//Pit
void Pit :: doSomething(){
    if(!isAlive())
        return;
    if(getWorld()->swallowSwallowable(this))
        setDead();
}

//Pea
void Pea::doSomething(){
//    The pea must check to see if it is currently alive
    if(!isAlive())
        return;
    
    Actor *a = getWorld()->canBeDamaged(getX(), getY());
    if(a != nullptr && a->isDamageable()){
        //Damage object
        a->damage(2);
        setDead();
    //Wall or Robot Factory
    } else if (a != nullptr && !a->isDamageable() && a->isObstacle()){
        setDead();
    }
    
     switch(getDirection()){
        //Right
        case 0:
            moveTo(getX()+1, getY());
            break;
        //Up
        case 90:
            moveTo(getX(), getY()+1);
            break;
        //Left
        case 180:
            moveTo(getX()-1, getY());
            break;
        //Down
        case 270:
            moveTo(getX(), getY()-1);
            break;

    }
}


//Crystals
void Crystal::doSomething(){
    if(!isAlive())
        return;
    //Check colocation
    if(getWorld()->isPlayerColocatedWith(getX(), getY())){
        setDead();
        getWorld()->increaseScore(getScore());
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->decCrystals();
    }
}

//Extra Life Goodie
void ExtraLifeGoodie::doSomething()
{
    if (!isAlive())
        return;
    //Check colocation
    if (getWorld()->isPlayerHere(this)) {
        getWorld()->increaseScore(1000);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->incLives();
        setDead();
    }
}

//Restore Health Goodie
void RestoreHealthGoodie::doSomething()
{
    if (!isAlive())
        return;
    //Check colocation
    if (getWorld()->isPlayerHere(this)) {
        getWorld()->increaseScore(500);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->restorePlayerHealth();
        setDead();
    }
}

void AmmoGoodie::doSomething(){
    if (!isAlive())
        return;
    //Check colocation
    if (getWorld()->isPlayerHere(this)) {
        getWorld()->increaseScore(100);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseAmmo();
        setDead();
    }
}

//Exit
Exit::Exit(StudentWorld* world, int startX, int startY):Actor(world, IID_EXIT, startX, startY, 0, 0){
   //The exit must start out invisible when it is created
       setVisible(false);
}

void Exit :: doSomething(){
    if (getWorld()->anyCrystals() && !isVisible()) {
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        //Check for colocation
    } else if (isVisible() && getWorld()->isPlayerHere(this)) {
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->increaseScore(2000);
        getWorld()->setLevelFinished();
    }
}

//Random Direction
int ThiefBot::getRandDir() const
{
    int dir = rand() % 4;
    switch (dir) {
        case 0:
            //Up
            return 90;
            break;
        case 1:
            //Down
            return 270;
            break;
        case 2:
            //Right
            return 0;
            break;
        case 3:
            //Left
            return 180;
            break;
    }
    return -1;
}


//Robot
Robot::Robot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score, int startDir) : Agent(world, startX, startY, IID_RAGEBOT, hitPoints, startDir), m_score(score){
        m_ticksMove = (28 - world->getLevel() / 4);

        if (m_ticksMove < 3)
            m_ticksMove = 3;
    
        m_tickCount = 1;
}

    
//Rage Bot
void RageBot :: doSomething(){
    if (!isAlive())
        return;
    
    if (getTickCount() != getTicksMove()) {
        //  Rest
        incTickCount();
        return;
    }
    tickCountReset();
    
    bool clearShotSuccess = getWorld()->existsClearShotToPlayer(this);
    int dx = 0;
    int dy = 0;
    int dir = 0;
    if(!clearShotSuccess){
        switch (getDirection()) {
            //Right
            case 0:
                dir = 180;
                dx = 1;
                break;
            //Up
            case 180:
                dir = 0;
                dy = 1;
                break;
            //Left
            case 90:
                dir = 270;
                dx = -1;
                break;
            //Down
            case 270:
                dir = 90;
                dy = -1;
                break;
        }
        
        int tryX = getX() + dx;
        int tryY = getY() + dy;
        if (!getWorld()->robotBlocked(this, tryX, tryY)) {
            moveTo(tryX, tryY);
        } else {
            setDirection(dir);
        }
    }
    
    
}


void RageBot::damage(int damageAmt){
    decHitPoints(damageAmt);
    if (isAlive())
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    else {
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(100);
    }
}


/*The ThiefBot factory must count how many ThiefBots of either type are present
 in the square region that extends from itself 3 squares up, 3 squares left through 3
 squares down, 3 squares right*/
void ThiefBotFactory::doSomething() {
    int random = rand() % 50;
    int count = 0;
    if (random == 1) {
        if (getWorld()->doFactoryCensus(getX(), getY(), 3, count) && count < 3){
            getWorld()->playSound(SOUND_ROBOT_BORN);
            if(m_product == REGULAR)
                getWorld()->addActor(new RegularThiefBot(getWorld(), getX(), getY()));
            else
                getWorld()->addActor(new MeanThiefBot(getWorld(), getX(), getY()));
        }
    }
}

bool ThiefBot::canThiefMove() {
    int tryX = getX();
    int tryY = getY();
    if (getDistanceMoved() < getDistanceBeforeTurning() && !getWorld()->robotBlocked(this, tryX, tryY)) {
        moveTo(tryX, tryY);
        if (getStolenGoodie())
            getStolenGoodie()->moveTo(tryX, tryY);
        incDistanceMoved();
        return true;
    }
    return false;
}

void ThiefBot::doSomething() {
    int random = rand() % 10;
    Goodie* goodie = nullptr;
    
    if (getStolenGoodie() == nullptr && random == 5) {
        goodie = getWorld()->getColocatedStealable(getX(), getY());
        if (goodie != nullptr) {
            setStolenGoodie(goodie);
            goodie->setVisible(false);
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
            return;
        }
    }
    //If ThiefBot can move
    //Update Position and Move
    if (canThiefMove())
        return;
    
    //If ThiefBot can't move
    //Change Direction
    resetDistanceBeforeTurning();
    resetDistanceMoved();

    int rd[4] = { -1, -1, -1, -1 };
    for (int i = 0; i < 4; i++) {
        while (rd[i] == -1) {
            int dir = getRandDir();
            bool uni = true;
            for (int j = 0; j < i; j++) {
                if (dir == rd[j])
                    uni = false;
            }
            if (uni)
                rd[i] = dir;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        setDirection(rd[i]);
        if (canThiefMove())
            return;
    }
    
    setDirection(rd[0]);
}

void RegularThiefBot::doSomething(){
    if (!isAlive())
        return;
    
    if (getTickCount() != getTicksMove()) {
        //Rest
        incTickCount();
        return;
    }

    tickCountReset();
    
    //Call original
    ThiefBot::doSomething();
}

void RegularThiefBot::damage(int damageAmt){
    decHitPoints(damageAmt);
    if (isAlive())
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    else {
        //Drops Stolen Goodie
        if (getStolenGoodie() != nullptr)
            getStolenGoodie()->setVisible(true);
        setDead();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(10);
    }
}

void MeanThiefBot::doSomething() {
    if (!isAlive())
        return;
    
    if (getTickCount() != getTicksMove()) {
        incTickCount();
        return;
    }
    tickCountReset();
    
    //Shoots Player
    if (isShootingRobot()) {
        bool clearShotSuccess = getWorld()->existsClearShotToPlayer(this);
        if(clearShotSuccess)
            return;
    }
    
    ThiefBot::doSomething();
}

void MeanThiefBot::damage(int damageAmt){
    decHitPoints(damageAmt);
    if (isAlive())
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    else {
        //Drops Stolen Goodie
        if (getStolenGoodie() != nullptr)
            getStolenGoodie()->setVisible(true);
        setDead();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(20);
    }
}
