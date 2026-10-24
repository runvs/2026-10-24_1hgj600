#pragma once

#include <sprite.hpp>
#include <game_object.hpp>
#include <tween_collection.hpp>
#include <functional>

class Asteroid : public jt::GameObject {
public:
    Asteroid(jt::Vector2f const& velocity, std::function<void()> onHitCallback);

    void setPosition(jt::Vector2f const& pos);
    jt::Vector2f getPosition() const;

private:
    std::shared_ptr<jt::Sprite> m_sprite;
    jt::Vector2f m_position { 0.0f, 0.0f };
    jt::Vector2f m_velocity { 0.0f, 0.0f };
    bool m_isHit { false };
    float m_hitTimer { 0.0f };
    jt::TweenCollection m_tweens;
    std::function<void()> m_onHitCallback;

    void doCreate() override;
    void doUpdate(float elapsed) override;
    void doDraw() const override;

    bool isClicked() const;
    void onHit();
};
