#pragma once

#include "GameComponent.h"
#include "Rendering\Colour.h"

#include <string>

using std::string;

class SpinningComponent : public GameComponent
{
public:

#pragma region Serializable
    float   Speed = 50.0f;
    string  Label = "";
    bool    EnableSpinning = false;
    int     NumRotations;
    Vector3 Offset = Vector3(1.0f, 2.0f, 3.0f);
    ColourRGB SpinColor = ColourRGB(0.5f, 0.5f, 0.0f);
#pragma endregion

    void Update(float deltaTime);
};