#pragma once

#include <JuceHeader.h>

//==============================================================================
namespace ColourPalette
{

    enum Palette
    {
        Eggshell,
        Salmon,
        Blue,
        Green
    };

    inline std::map<Palette, juce::Colour> colourMap = {
            { Eggshell, juce::Colour(244u, 241u, 222u) },
            { Salmon,   juce::Colour(224u, 122u, 95u)  },
            { Blue,     juce::Colour(61u,  64u,  91u)  },
            { Green,    juce::Colour(129u, 178u, 154u) }
    };

    inline juce::Colour getColour(Palette c) { return colourMap[c]; }

}
