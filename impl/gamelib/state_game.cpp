#include "state_game.hpp"

#include "random/random.hpp"
#include <asteroid.hpp>
#include <box2dwrapper/box2d_world_impl.hpp>
#include <color/color.hpp>
#include <game_interface.hpp>
#include <game_properties.hpp>
#include <hud/hud.hpp>
#include <screeneffects/vignette.hpp>
#include <shape.hpp>
#include <state_menu.hpp>

void StateGame::onCreate()
{
    m_world = std::make_shared<jt::Box2DWorldImpl>(jt::Vector2f { 0.0f, 0.0f });

    float const w = static_cast<float>(GP::GetWindowSize().x);
    float const h = static_cast<float>(GP::GetWindowSize().y);

    using jt::Shape;

    m_background = std::make_shared<jt::Sprite>("assets/background.png", textureManager());
    m_background->setIgnoreCamMovement(true);
    m_background->update(0.0f);

    for (auto i = 0; i != 10; ++i) {
        auto house = std::make_shared<jt::Shape>();
        auto size = jt::Random::getRandomPointIn(jt::Rectf { 16.0f, 24.0f, 32.0f, 54.0f });
        house->makeRect(size, textureManager());
        house->setOffset(jt::Vector2f { -size.x / 2.0f, -size.y });
        house->setPosition(jt::Vector2f {
            jt::Random::getFloat(0.0f, GP::GetScreenSize().x), GP::GetScreenSize().y - 31.0f });
        house->setColor(GP::getPalette().getColor(1));
        m_houses.push_back(house);
    }

    createPlayer();

    // asteroids are added before vignette and hud, so they are drawn below them
    m_asteroids = std::make_shared<jt::ObjectGroup<Asteroid>>();
    add(m_asteroids);
    spawnAsteroid();

    m_vignette = std::make_shared<jt::Vignette>(GP::GetScreenSize());
    add(m_vignette);
    m_hud = std::make_shared<Hud>();
    add(m_hud);

    // StateGame will call drawObjects itself.
    setAutoDraw(false);
}

void StateGame::onEnter() { }

void StateGame::createPlayer() { }

void StateGame::spawnAsteroid()
{
    auto const asteroidSize = 32.0f;
    auto const fallSpeed = 30.0f;
    auto const velocity = jt::Vector2f { jt::Random::getFloat(-20.0f, 20.0f), fallSpeed };
    auto asteroid = std::make_shared<Asteroid>(velocity);
    asteroid->setPosition(jt::Random::getRandomPointIn(
        GP::GetScreenSize() - jt::Vector2f { asteroidSize, asteroidSize }));
    add(asteroid);
    m_asteroids->push_back(asteroid);
}

void StateGame::onUpdate(float const elapsed)
{
    if (m_running) {
        m_world->step(elapsed, GP::PhysicVelocityIterations(), GP::PhysicPositionIterations());
        // update game logic here
        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::A)) {
            m_scoreP1++;
            m_hud->getObserverScoreP1()->notify(m_scoreP1);
        }
        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::D)) {
            m_scoreP2++;
            m_hud->getObserverScoreP2()->notify(m_scoreP2);
        }
        if (getGame()->input().keyboard()->pressed(jt::KeyCode::LShift)
            && getGame()->input().keyboard()->pressed(jt::KeyCode::Escape)) {
            endGame();
        }
    }

    m_background->update(elapsed);
    for (auto& h : m_houses) {
        h->update(elapsed);
    }
    m_vignette->update(elapsed);
}

void StateGame::onDraw() const
{
    m_background->draw(renderTarget());
    for (auto& h : m_houses) {
        h->draw(renderTarget());
    }
    drawObjects();
    m_vignette->draw();
    m_hud->draw();
}

void StateGame::endGame()
{
    if (m_hasEnded) {
        // trigger this function only once
        return;
    }
    m_hasEnded = true;
    m_running = false;

    getGame()->stateManager().switchToStoredState("menu");
}

std::string StateGame::getName() const { return "State Game"; }
