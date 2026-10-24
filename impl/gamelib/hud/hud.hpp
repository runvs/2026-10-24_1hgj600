#ifndef GAME_HUD_HPP
#define GAME_HUD_HPP

#include <game_object.hpp>
#include <hud/observer_interface.hpp>
#include <sprite.hpp>
#include <text.hpp>

class ScoreDisplay;
class TimeDisplay;

class Hud : public jt::GameObject {
public:
    std::shared_ptr<ObserverInterface<int>> getObserverScoreP1() const;
    std::shared_ptr<ObserverInterface<int>> getObserverScore() const;
    std::shared_ptr<ObserverInterface<float>> getObserverTime() const;

private:
    std::shared_ptr<ScoreDisplay> m_scoreP1Display;
    std::shared_ptr<ScoreDisplay> m_scoreDisplay;
    std::shared_ptr<TimeDisplay> m_timeDisplay;

    jt::Text::Sptr m_scoreP1Text;
    jt::Text::Sptr m_scoreText;
    jt::Text::Sptr m_timeText;

    void doCreate() override;

    void doUpdate(float const elapsed) override;

    void doDraw() const override;
};

#endif
