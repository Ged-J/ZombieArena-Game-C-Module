//
//  bullet.cpp
//  Zombie Arena
//
//

#include "bullet.h"

// The constructor
Bullet::Bullet()
{
    m_BulletShape.setSize(Vector2f(2, 2));
}

void Bullet::shoot(float startX, float startY,
    float targetX, float targetY)
{

    // TO BE COMPLETED IN THIS CLASS

    //Keep track of the bullet
    m_InFlight = true;
    m_Position.x = startX;
    m_Position.y = startY;

}

void Bullet::stop()
{
    m_InFlight = false;
}

bool Bullet::isInFlight()
{
    return m_InFlight;
}

FloatRect Bullet::getPosition()
{
    return m_BulletShape.getGlobalBounds();
}

RectangleShape Bullet::getShape()
{
    return m_BulletShape;
}


void Bullet::update(float elapsedTime)
{

    // TO BE COMPLETED IN THIS CLASS

}
