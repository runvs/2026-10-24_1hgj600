#pragma once

#include <sprite.hpp>
#include <game_object.hpp>
#include <tween_collection.hpp>

class Asteroid : public jt::GameObject {
public:
    explicit Asteroid(jt::Vector2f const& velocity);

    void setPosition(jt::Vector2f const& pos);
    jt::Vector2f getPosition() const;

private:
    std::shared_ptr<jt::Sprite> m_sprite;
    jt::Vector2f m_position { 0.0f, 0.0f };
    jt::Vector2f m_velocity { 0.0f, 0.0f };
    bool m_isHit { false };
    float m_hitTimer { 0.0f };
    jt::TweenCollection m_tweens;

    void doCreate() override;
    void doUpdate(float elapsed) override;
    void doDraw() const override;

    bool isClicked() const;
    void onHit();
};
