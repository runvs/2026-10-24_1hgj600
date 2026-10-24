#include "hud.hpp"
#include <color/color.hpp>
#include <drawable_helpers.hpp>
#include <game_interface.hpp>
#include <game_properties.hpp>
#include <hud/score_display.hpp>
#include <hud/time_display.hpp>

std::shared_ptr<ObserverInterface<int>> Hud::getObserverScoreP1() const { return m_scoreP1Display; }

std::shared_ptr<ObserverInterface<int>> Hud::getObserverScore() const { return m_scoreDisplay; }

std::shared_ptr<ObserverInterface<float>> Hud::getObserverTime() const { return m_timeDisplay; }

void Hud::doCreate()
{
    m_scoreP1Text = std::make_shared<jt::Text>();
    m_scoreP1Text = jt::dh::createText(renderTarget(), "", 16, jt::Color { 248, 249, 254 });
    m_scoreP1Text->setTextAlign(jt::Text::TextAlign::LEFT);
    m_scoreP1Text->setIgnoreCamMovement(true);
    m_scoreP1Text->setPosition({ 10, 4 });

    m_scoreP1Display = std::make_shared<ScoreDisplay>(m_scoreP1Text, "P1 Score: ");

    m_scoreText = jt::dh::createText(renderTarget(), "", 16, GP::getPalette().getColor(7));
    m_scoreText->setTextAlign(jt::Text::TextAlign::RIGHT);
    m_scoreText->setIgnoreCamMovement(true);
    m_scoreText->setPosition({ GP::GetScreenSize().x - 10, 4 });

    m_scoreDisplay = std::make_shared<ScoreDisplay>(m_scoreText, "Score: ");

    m_timeText = jt::dh::createText(renderTarget(), "", 16, GP::getPalette().getColor(7));
    m_timeText->setTextAlign(jt::Text::TextAlign::LEFT);
    m_timeText->setIgnoreCamMovement(true);
    m_timeText->setPosition({ 10, 4 });

    m_timeDisplay = std::make_shared<TimeDisplay>(m_timeText);
}

void Hud::doUpdate(float const elapsed)
{
    m_scoreP1Text->update(elapsed);
    m_scoreText->update(elapsed);
    m_timeText->update(elapsed);
}

void Hud::doDraw() const
{
    m_scoreP1Text->draw(renderTarget());
    m_scoreText->draw(renderTarget());
    m_timeText->draw(renderTarget());
}
