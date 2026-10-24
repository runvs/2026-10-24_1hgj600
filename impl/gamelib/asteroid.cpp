#include "asteroid.hpp"

Asteroid::Asteroid(jt::Vector2f const& velocity)
    : m_velocity { velocity }
{
}

void Asteroid::doCreate()
{
    m_sprite = std::make_shared<jt::Sprite>("assets/asteroid.png", textureManager());
    m_sprite->setScale(jt::Vector2f { 0.5f, 0.5f });
}

void Asteroid::doUpdate(float elapsed)
{
    m_position += m_velocity * elapsed;
    m_sprite->setPosition(m_position);
    m_sprite->update(elapsed);
}

void Asteroid::doDraw() const { m_sprite->draw(renderTarget()); }

void Asteroid::setPosition(jt::Vector2f const& pos) { m_position = pos; }

jt::Vector2f Asteroid::getPosition() const { return m_position; }
