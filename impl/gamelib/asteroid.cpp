#include "asteroid.hpp"
#include <game_properties.hpp>
#include <math_helper.hpp>
#include <tweens/tween_alpha.hpp>
#include <tweens/tween_scale.hpp>

namespace {
constexpr float baseScale { 0.5f };
constexpr float hitScaleFactor { 1.5f };
constexpr float flashDuration { 0.3f };
constexpr float killDelay { flashDuration * 0.5f };
} // namespace

Asteroid::Asteroid(jt::Vector2f const& velocity, std::function<void()> onHitCallback,
    std::function<void()> onOutOfScreenCallback)
    : m_velocity { velocity }
    , m_onHitCallback { std::move(onHitCallback) }
    , m_onOutOfScreenCallback { std::move(onOutOfScreenCallback) }
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
    } else if (!m_isFrozen) {
        m_position += m_velocity * elapsed;
        if (m_sprite->getGlobalBounds().top > GP::GetScreenSize().y) {
            // fell out of the screen
            if (m_onOutOfScreenCallback) {
                m_onOutOfScreenCallback();
            }
            kill();
        } else if (isClicked()) {
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
    if (m_onHitCallback) {
        m_onHitCallback();
    }

    auto const hitScale = baseScale * hitScaleFactor;
    m_tweens.add(jt::TweenScale::create(m_sprite, killDelay, jt::Vector2f { baseScale, baseScale },
        jt::Vector2f { hitScale, hitScale }));
    m_tweens.add(jt::TweenAlpha::create(m_sprite, killDelay, 255u, 0u));
}

void Asteroid::doDraw() const { m_sprite->draw(renderTarget()); }

void Asteroid::setPosition(jt::Vector2f const& pos) { m_position = pos; }

jt::Vector2f Asteroid::getPosition() const { return m_position; }

void Asteroid::freeze() { m_isFrozen = true; }
