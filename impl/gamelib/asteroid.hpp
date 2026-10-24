#pragma once

#include <sprite.hpp>
#include <game_object.hpp>
#include <tween_collection.hpp>
#include <functional>

class Asteroid : public jt::GameObject {
public:
    Asteroid(jt::Vector2f const& velocity, std::function<void()> onHitCallback,
        std::function<void()> onOutOfScreenCallback);

    void setPosition(jt::Vector2f const& pos);
    jt::Vector2f getPosition() const;

    // stop moving and ignore clicks
    void freeze();

private:
    std::shared_ptr<jt::Sprite> m_sprite;
    jt::Vector2f m_position { 0.0f, 0.0f };
    jt::Vector2f m_velocity { 0.0f, 0.0f };
    bool m_isHit { false };
    bool m_isFrozen { false };
    float m_hitTimer { 0.0f };
    jt::TweenCollection m_tweens;
    std::function<void()> m_onHitCallback;
    std::function<void()> m_onOutOfScreenCallback;

    void doCreate() override;
    void doUpdate(float elapsed) override;
    void doDraw() const override;

    bool isClicked() const;
    void onHit();
};
