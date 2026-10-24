#include "game_properties.hpp"
#include <color/palette_builder.hpp>

namespace {

jt::Palette createPalette()
{
    jt::PaletteBuilder builder;
    return builder
        .addColorsFromGPL(R"(GIMP Palette
#Palette Name: COSMO ZOIDS
#Description: A Star Command approved set of 8 colors.
#Colors: 8
129	214	151	81d697
58	162	136	3aa288
37	113	113	257171
19	67	78	13434e
14	35	78	0e234e
101	72	169	6548a9
170	107	208	aa6bd0
237	158	237	ed9eed
)")
        .create();
}

} // namespace

jt::Palette GP::getPalette()
{
    static auto const p = createPalette();
    return p;
}

int GP::PhysicVelocityIterations() { return 20; }

int GP::PhysicPositionIterations() { return 20; }

jt::Vector2f GP::PlayerSize() { return jt::Vector2f { 16.0f, 16.0f }; }
