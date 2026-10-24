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
#include <algorithm>
#include <cmath>

namespace {
constexpr float startAsteroidCount { 1.0f };
constexpr float endAsteroidCount { 8.0f };
constexpr float startFallSpeed { 30.0f };
constexpr float endFallSpeed { 3.0f * startFallSpeed };
constexpr float spawnTimeExponent { 2.5f };
constexpr float spawnTimeMax { 9.0f };
constexpr float spawnTimeAfterTimeout { 0.05f };
// default alpha of jt::Vignette
constexpr float startVignetteAlpha { 140.0f };
constexpr float endVignetteAlpha { 255.0f };
} // namespace

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
        house->setColor(GP::getPalette().getColor(2));
        m_houses.push_back(house);
    }

    createPlayer();

    // asteroids are added before vignette and hud, so they are drawn below them
    m_asteroids = std::make_shared<jt::ObjectGroup<Asteroid>>();
    add(m_asteroids);

    m_vignette = std::make_shared<jt::Vignette>(GP::GetScreenSize());
    add(m_vignette);
    m_hud = std::make_shared<Hud>();
    add(m_hud);
    m_hud->getObserverTime()->notify(m_timeRemaining);
    m_hud->getObserverScore()->notify(m_score);

    // StateGame will call drawObjects itself.
    setAutoDraw(false);
}

void StateGame::onEnter() { }

void StateGame::createPlayer() { }

void StateGame::spawnAsteroid()
{
    auto const asteroidSize = 32.0f;
    auto const fallSpeed = startFallSpeed + (endFallSpeed - startFallSpeed) * getProgress();
    auto const groundHeight = 32.0f;
    auto const screenSize = GP::GetScreenSize();

    // start just above the top edge, within the inner 80% of the screen width
    auto const start
        = jt::Vector2f { jt::Random::getFloat(0.1f * screenSize.x, 0.9f * screenSize.x),
              -asteroidSize / 2.0f };
    // reach the ground within the inner 90% of the screen width
    auto const target
        = jt::Vector2f { jt::Random::getFloat(0.05f * screenSize.x, 0.95f * screenSize.x),
              screenSize.y - groundHeight };

    auto const timeToGround = (target.y - start.y) / fallSpeed;
    auto const velocity = jt::Vector2f { (target.x - start.x) / timeToGround, fallSpeed };

    auto asteroid = std::make_shared<Asteroid>(
        velocity,
        [this]() {
            m_score++;
            m_hud->getObserverScore()->notify(m_score);
        },
        [this]() { endGame(); });
    asteroid->setPosition(start);
    add(asteroid);
    m_asteroids->push_back(asteroid);
}

void StateGame::updateAsteroidSpawning(float const elapsed)
{
    m_spawnTimer += elapsed;
    if (m_spawnTimer >= getSpawnTime()) {
        spawnAsteroid();
        m_spawnTimer = 0.0f;
    }
}

float StateGame::getProgress() const { return 1.0f - m_timeRemaining / GP::GameDuration(); }

float StateGame::getTargetAsteroidCount() const
{
    return startAsteroidCount + (endAsteroidCount - startAsteroidCount) * getProgress();
}

float StateGame::getSpawnTime() const
{
    if (m_timeRemaining <= 0.0f) {
        return spawnTimeAfterTimeout;
    }
    // the further the current count is below the target, the sooner the next asteroid spawns
    auto const currentCount = static_cast<float>(m_asteroids->size());
    return std::pow(currentCount / getTargetAsteroidCount(), spawnTimeExponent) * spawnTimeMax;
}

void StateGame::onUpdate(float const elapsed)
{
    if (m_running) {
        m_world->step(elapsed, GP::PhysicVelocityIterations(), GP::PhysicPositionIterations());
        // update game logic here
        auto timeStep = elapsed;
#ifdef JT_ENABLE_DEBUG
        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::K)) {
            timeStep += 10.0f;
        }
#endif
        m_timeRemaining = std::max(m_timeRemaining - timeStep, 0.0f);
        m_hud->getObserverTime()->notify(m_timeRemaining);
        updateAsteroidSpawning(elapsed);
        auto const vignetteAlpha
            = startVignetteAlpha + (endVignetteAlpha - startVignetteAlpha) * getProgress();
        m_vignette->setColor(jt::Color { 0, 0, 0, static_cast<std::uint8_t>(vignetteAlpha) });
        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::A)) {
            m_scoreP1++;
            m_hud->getObserverScoreP1()->notify(m_scoreP1);
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

    for (auto const& weakAsteroid : *m_asteroids) {
        if (auto const asteroid = weakAsteroid.lock()) {
            asteroid->freeze();
        }
    }

    if (auto const menu = std::dynamic_pointer_cast<StateMenu>(
            getGame()->stateManager().getStoredState("menu"))) {
        menu->setScore(m_score);
    }

    // the menu has set up a fade to black transition

    getGame()->stateManager().switchToStoredState("menu");
}

std::string StateGame::getName() const { return "State Game"; }
