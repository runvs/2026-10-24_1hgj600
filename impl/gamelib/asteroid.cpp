#include "asteroid.hpp"
#include <math_helper.hpp>
#include <tweens/tween_alpha.hpp>
#include <tweens/tween_scale.hpp>

namespace {
constexpr float baseScale { 0.5f };
constexpr float hitScaleFactor { 1.5f };
constexpr float flashDuration { 0.3f };
constexpr float killDelay { flashDuration * 0.5f };
} // namespace

Asteroid::Asteroid(jt::Vector2f const& velocity)
    : m_velocity { velocity }
{
}

void Asteroid::doCreate()
{
    m_sprite = std::make_shared<jt::Sprite>("assets/asteroid.png", textureManager());
    m_sprite->setScale(jt::Vector2f { baseScale, baseScale });
    m_sprite->setOrigin(jt::OriginMode::CENTER);
}

void Asteroid::doUpdate(float elapsed)
{
    if (m_isHit) {
        m_hitTimer -= elapsed;
        if (m_hitTimer <= 0.0f) {
            kill();
        }
    } else {
        m_position += m_velocity * elapsed;
        if (isClicked()) {
            onHit();
        }
    }

    m_tweens.update(elapsed);
    m_sprite->setPosition(m_position);
    m_sprite->update(elapsed);
}

bool Asteroid::isClicked() const
{
    return mouse()->justPressed(jt::MouseButtonCode::MBLeft)
        && jt::MathHelper::checkIsIn(m_sprite->getGlobalBounds(), mouse()->getMousePositionScreen());
}

void Asteroid::onHit()
{
    m_isHit = true;
    m_hitTimer = killDelay;
    m_sprite->flash(flashDuration);

    auto const hitScale = baseScale * hitScaleFactor;
    m_tweens.add(jt::TweenScale::create(m_sprite, killDelay, jt::Vector2f { baseScale, baseScale },
        jt::Vector2f { hitScale, hitScale }));
    m_tweens.add(jt::TweenAlpha::create(m_sprite, killDelay, 255u, 0u));
}

void Asteroid::doDraw() const { m_sprite->draw(renderTarget()); }

void Asteroid::setPosition(jt::Vector2f const& pos) { m_position = pos; }

jt::Vector2f Asteroid::getPosition() const { return m_position; }
