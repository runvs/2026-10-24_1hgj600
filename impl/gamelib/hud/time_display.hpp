#pragma once

#include <hud/observer_interface.hpp>
#include <text.hpp>

class TimeDisplay : public ObserverInterface<float> {
public:
    explicit TimeDisplay(jt::Text::Sptr text);
    void notify(float value) override;

private:
    jt::Text::Sptr const m_timeText;
};

