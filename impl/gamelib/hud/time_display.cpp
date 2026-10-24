#include "time_display.hpp"
#include <math_helper.hpp>
#include <algorithm>

TimeDisplay::TimeDisplay(jt::Text::Sptr text)
    : m_timeText { text }
{
}

void TimeDisplay::notify(float value)
{
    auto const clamped = std::max(value, 0.0f);
    m_timeText->setText(jt::MathHelper::floatToStringWithXDecimalDigits(clamped, 1) + "s");
    m_timeText->update(0.0f);
}
