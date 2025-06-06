#pragma once

#include "Boilerplate/Defines.h"
#include "Vertex.h"

#include <vector>

struct Cone {
    static std::vector<Vertex> vertices;
    static std::vector<u16> indices;
};

std::vector<Vertex> Cone::vertices = {
    {{ 0.f, 1.f, -0.f }, { 0.894f, 0.447f, -0.f }, { 0.f, 0.f }},
    {{ 0.f, 1.f, -0.f }, { 0.877f, 0.447f, -0.174f }, { 0.f, 0.031f }},
    {{ 0.f, 1.f, -0.f }, { 0.826f, 0.447f, -0.342f }, { 0.f, 0.062f }},
    {{ 0.f, 1.f, -0.f }, { 0.744f, 0.447f, -0.497f }, { 0.f, 0.094f }},
    {{ 0.f, 1.f, -0.f }, { 0.632f, 0.447f, -0.632f }, { 0.f, 0.125f }},
    {{ 0.f, 1.f, -0.f }, { 0.497f, 0.447f, -0.744f }, { 0.f, 0.156f }},
    {{ 0.f, 1.f, -0.f }, { 0.342f, 0.447f, -0.826f }, { 0.f, 0.188f }},
    {{ 0.f, 1.f, -0.f }, { 0.174f, 0.447f, -0.877f }, { 0.f, 0.219f }},
    {{ 0.f, 1.f, -0.f }, { 0.f, 0.447f, -0.894f }, { 0.f, 0.25f }},
    {{ 0.f, 1.f, -0.f }, { -0.174f, 0.447f, -0.877f }, { 0.f, 0.281f }},
    {{ 0.f, 1.f, -0.f }, { -0.342f, 0.447f, -0.826f }, { 0.f, 0.312f }},
    {{ 0.f, 1.f, -0.f }, { -0.497f, 0.447f, -0.744f }, { 0.f, 0.344f }},
    {{ 0.f, 1.f, -0.f }, { -0.632f, 0.447f, -0.632f }, { 0.f, 0.375f }},
    {{ 0.f, 1.f, -0.f }, { -0.744f, 0.447f, -0.497f }, { 0.f, 0.406f }},
    {{ 0.f, 1.f, -0.f }, { -0.826f, 0.447f, -0.342f }, { 0.f, 0.438f }},
    {{ 0.f, 1.f, -0.f }, { -0.877f, 0.447f, -0.174f }, { 0.f, 0.469f }},
    {{ 0.f, 1.f, -0.f }, { -0.894f, 0.447f, -0.f }, { 0.f, 0.5f }},
    {{ 0.f, 1.f, -0.f }, { -0.877f, 0.447f, 0.174f }, { 0.f, 0.531f }},
    {{ 0.f, 1.f, -0.f }, { -0.826f, 0.447f, 0.342f }, { 0.f, 0.562f }},
    {{ 0.f, 1.f, -0.f }, { -0.744f, 0.447f, 0.497f }, { 0.f, 0.594f }},
    {{ 0.f, 1.f, -0.f }, { -0.632f, 0.447f, 0.632f }, { 0.f, 0.625f }},
    {{ 0.f, 1.f, -0.f }, { -0.497f, 0.447f, 0.744f }, { 0.f, 0.656f }},
    {{ 0.f, 1.f, -0.f }, { -0.342f, 0.447f, 0.826f }, { 0.f, 0.687f }},
    {{ 0.f, 1.f, -0.f }, { -0.174f, 0.447f, 0.877f }, { 0.f, 0.719f }},
    {{ 0.f, 1.f, -0.f }, { 0.f, 0.447f, 0.894f }, { 0.f, 0.75f }},
    {{ 0.f, 1.f, -0.f }, { 0.174f, 0.447f, 0.877f }, { 0.f, 0.781f }},
    {{ 0.f, 1.f, -0.f }, { 0.342f, 0.447f, 0.826f }, { 0.f, 0.813f }},
    {{ 0.f, 1.f, -0.f }, { 0.497f, 0.447f, 0.744f }, { 0.f, 0.844f }},
    {{ 0.f, 1.f, -0.f }, { 0.632f, 0.447f, 0.632f }, { 0.f, 0.875f }},
    {{ 0.f, 1.f, -0.f }, { 0.744f, 0.447f, 0.497f }, { 0.f, 0.906f }},
    {{ 0.f, 1.f, -0.f }, { 0.826f, 0.447f, 0.342f }, { 0.f, 0.937f }},
    {{ 0.f, 1.f, -0.f }, { 0.877f, 0.447f, 0.174f }, { 0.f, 0.969f }},
    {{ 0.f, 1.f, -0.f }, { 0.894f, 0.447f, -0.f }, { 0.f, 1.f }},
    {{ 0.125f, 0.75f, -0.f }, { 0.894f, 0.447f, -0.f }, { 0.125f, 0.f }},
    {{ 0.123f, 0.75f, -0.024f }, { 0.877f, 0.447f, -0.174f }, { 0.125f, 0.031f }},
    {{ 0.115f, 0.75f, -0.048f }, { 0.826f, 0.447f, -0.342f }, { 0.125f, 0.062f }},
    {{ 0.104f, 0.75f, -0.069f }, { 0.744f, 0.447f, -0.497f }, { 0.125f, 0.094f }},
    {{ 0.088f, 0.75f, -0.088f }, { 0.632f, 0.447f, -0.632f }, { 0.125f, 0.125f }},
    {{ 0.069f, 0.75f, -0.104f }, { 0.497f, 0.447f, -0.744f }, { 0.125f, 0.156f }},
    {{ 0.048f, 0.75f, -0.115f }, { 0.342f, 0.447f, -0.826f }, { 0.125f, 0.188f }},
    {{ 0.024f, 0.75f, -0.123f }, { 0.174f, 0.447f, -0.877f }, { 0.125f, 0.219f }},
    {{ 0.f, 0.75f, -0.125f }, { 0.f, 0.447f, -0.894f }, { 0.125f, 0.25f }},
    {{ -0.024f, 0.75f, -0.123f }, { -0.174f, 0.447f, -0.877f }, { 0.125f, 0.281f }},
    {{ -0.048f, 0.75f, -0.115f }, { -0.342f, 0.447f, -0.826f }, { 0.125f, 0.312f }},
    {{ -0.069f, 0.75f, -0.104f }, { -0.497f, 0.447f, -0.744f }, { 0.125f, 0.344f }},
    {{ -0.088f, 0.75f, -0.088f }, { -0.632f, 0.447f, -0.632f }, { 0.125f, 0.375f }},
    {{ -0.104f, 0.75f, -0.069f }, { -0.744f, 0.447f, -0.497f }, { 0.125f, 0.406f }},
    {{ -0.115f, 0.75f, -0.048f }, { -0.826f, 0.447f, -0.342f }, { 0.125f, 0.438f }},
    {{ -0.123f, 0.75f, -0.024f }, { -0.877f, 0.447f, -0.174f }, { 0.125f, 0.469f }},
    {{ -0.125f, 0.75f, -0.f }, { -0.894f, 0.447f, -0.f }, { 0.125f, 0.5f }},
    {{ -0.123f, 0.75f, 0.024f }, { -0.877f, 0.447f, 0.174f }, { 0.125f, 0.531f }},
    {{ -0.115f, 0.75f, 0.048f }, { -0.826f, 0.447f, 0.342f }, { 0.125f, 0.562f }},
    {{ -0.104f, 0.75f, 0.069f }, { -0.744f, 0.447f, 0.497f }, { 0.125f, 0.594f }},
    {{ -0.088f, 0.75f, 0.088f }, { -0.632f, 0.447f, 0.632f }, { 0.125f, 0.625f }},
    {{ -0.069f, 0.75f, 0.104f }, { -0.497f, 0.447f, 0.744f }, { 0.125f, 0.656f }},
    {{ -0.048f, 0.75f, 0.115f }, { -0.342f, 0.447f, 0.826f }, { 0.125f, 0.687f }},
    {{ -0.024f, 0.75f, 0.123f }, { -0.174f, 0.447f, 0.877f }, { 0.125f, 0.719f }},
    {{ 0.f, 0.75f, 0.125f }, { 0.f, 0.447f, 0.894f }, { 0.125f, 0.75f }},
    {{ 0.024f, 0.75f, 0.123f }, { 0.174f, 0.447f, 0.877f }, { 0.125f, 0.781f }},
    {{ 0.048f, 0.75f, 0.115f }, { 0.342f, 0.447f, 0.826f }, { 0.125f, 0.813f }},
    {{ 0.069f, 0.75f, 0.104f }, { 0.497f, 0.447f, 0.744f }, { 0.125f, 0.844f }},
    {{ 0.088f, 0.75f, 0.088f }, { 0.632f, 0.447f, 0.632f }, { 0.125f, 0.875f }},
    {{ 0.104f, 0.75f, 0.069f }, { 0.744f, 0.447f, 0.497f }, { 0.125f, 0.906f }},
    {{ 0.115f, 0.75f, 0.048f }, { 0.826f, 0.447f, 0.342f }, { 0.125f, 0.937f }},
    {{ 0.123f, 0.75f, 0.024f }, { 0.877f, 0.447f, 0.174f }, { 0.125f, 0.969f }},
    {{ 0.125f, 0.75f, -0.f }, { 0.894f, 0.447f, -0.f }, { 0.125f, 1.f }},
    {{ 0.25f, 0.5f, -0.f }, { 0.894f, 0.447f, -0.f }, { 0.25f, 0.f }},
    {{ 0.245f, 0.5f, -0.049f }, { 0.877f, 0.447f, -0.174f }, { 0.25f, 0.031f }},
    {{ 0.231f, 0.5f, -0.096f }, { 0.826f, 0.447f, -0.342f }, { 0.25f, 0.062f }},
    {{ 0.208f, 0.5f, -0.139f }, { 0.744f, 0.447f, -0.497f }, { 0.25f, 0.094f }},
    {{ 0.177f, 0.5f, -0.177f }, { 0.632f, 0.447f, -0.632f }, { 0.25f, 0.125f }},
    {{ 0.139f, 0.5f, -0.208f }, { 0.497f, 0.447f, -0.744f }, { 0.25f, 0.156f }},
    {{ 0.096f, 0.5f, -0.231f }, { 0.342f, 0.447f, -0.826f }, { 0.25f, 0.188f }},
    {{ 0.049f, 0.5f, -0.245f }, { 0.174f, 0.447f, -0.877f }, { 0.25f, 0.219f }},
    {{ 0.f, 0.5f, -0.25f }, { 0.f, 0.447f, -0.894f }, { 0.25f, 0.25f }},
    {{ -0.049f, 0.5f, -0.245f }, { -0.174f, 0.447f, -0.877f }, { 0.25f, 0.281f }},
    {{ -0.096f, 0.5f, -0.231f }, { -0.342f, 0.447f, -0.826f }, { 0.25f, 0.312f }},
    {{ -0.139f, 0.5f, -0.208f }, { -0.497f, 0.447f, -0.744f }, { 0.25f, 0.344f }},
    {{ -0.177f, 0.5f, -0.177f }, { -0.632f, 0.447f, -0.632f }, { 0.25f, 0.375f }},
    {{ -0.208f, 0.5f, -0.139f }, { -0.744f, 0.447f, -0.497f }, { 0.25f, 0.406f }},
    {{ -0.231f, 0.5f, -0.096f }, { -0.826f, 0.447f, -0.342f }, { 0.25f, 0.438f }},
    {{ -0.245f, 0.5f, -0.049f }, { -0.877f, 0.447f, -0.174f }, { 0.25f, 0.469f }},
    {{ -0.25f, 0.5f, -0.f }, { -0.894f, 0.447f, -0.f }, { 0.25f, 0.5f }},
    {{ -0.245f, 0.5f, 0.049f }, { -0.877f, 0.447f, 0.174f }, { 0.25f, 0.531f }},
    {{ -0.231f, 0.5f, 0.096f }, { -0.826f, 0.447f, 0.342f }, { 0.25f, 0.562f }},
    {{ -0.208f, 0.5f, 0.139f }, { -0.744f, 0.447f, 0.497f }, { 0.25f, 0.594f }},
    {{ -0.177f, 0.5f, 0.177f }, { -0.632f, 0.447f, 0.632f }, { 0.25f, 0.625f }},
    {{ -0.139f, 0.5f, 0.208f }, { -0.497f, 0.447f, 0.744f }, { 0.25f, 0.656f }},
    {{ -0.096f, 0.5f, 0.231f }, { -0.342f, 0.447f, 0.826f }, { 0.25f, 0.687f }},
    {{ -0.049f, 0.5f, 0.245f }, { -0.174f, 0.447f, 0.877f }, { 0.25f, 0.719f }},
    {{ 0.f, 0.5f, 0.25f }, { 0.f, 0.447f, 0.894f }, { 0.25f, 0.75f }},
    {{ 0.049f, 0.5f, 0.245f }, { 0.174f, 0.447f, 0.877f }, { 0.25f, 0.781f }},
    {{ 0.096f, 0.5f, 0.231f }, { 0.342f, 0.447f, 0.826f }, { 0.25f, 0.813f }},
    {{ 0.139f, 0.5f, 0.208f }, { 0.497f, 0.447f, 0.744f }, { 0.25f, 0.844f }},
    {{ 0.177f, 0.5f, 0.177f }, { 0.632f, 0.447f, 0.632f }, { 0.25f, 0.875f }},
    {{ 0.208f, 0.5f, 0.139f }, { 0.744f, 0.447f, 0.497f }, { 0.25f, 0.906f }},
    {{ 0.231f, 0.5f, 0.096f }, { 0.826f, 0.447f, 0.342f }, { 0.25f, 0.937f }},
    {{ 0.245f, 0.5f, 0.049f }, { 0.877f, 0.447f, 0.174f }, { 0.25f, 0.969f }},
    {{ 0.25f, 0.5f, -0.f }, { 0.894f, 0.447f, -0.f }, { 0.25f, 1.f }},
    {{ 0.375f, 0.25f, -0.f }, { 0.894f, 0.447f, -0.f }, { 0.375f, 0.f }},
    {{ 0.368f, 0.25f, -0.073f }, { 0.877f, 0.447f, -0.174f }, { 0.375f, 0.031f }},
    {{ 0.346f, 0.25f, -0.144f }, { 0.826f, 0.447f, -0.342f }, { 0.375f, 0.062f }},
    {{ 0.312f, 0.25f, -0.208f }, { 0.744f, 0.447f, -0.497f }, { 0.375f, 0.094f }},
    {{ 0.265f, 0.25f, -0.265f }, { 0.632f, 0.447f, -0.632f }, { 0.375f, 0.125f }},
    {{ 0.208f, 0.25f, -0.312f }, { 0.497f, 0.447f, -0.744f }, { 0.375f, 0.156f }},
    {{ 0.144f, 0.25f, -0.346f }, { 0.342f, 0.447f, -0.826f }, { 0.375f, 0.188f }},
    {{ 0.073f, 0.25f, -0.368f }, { 0.174f, 0.447f, -0.877f }, { 0.375f, 0.219f }},
    {{ 0.f, 0.25f, -0.375f }, { 0.f, 0.447f, -0.894f }, { 0.375f, 0.25f }},
    {{ -0.073f, 0.25f, -0.368f }, { -0.174f, 0.447f, -0.877f }, { 0.375f, 0.281f }},
    {{ -0.144f, 0.25f, -0.346f }, { -0.342f, 0.447f, -0.826f }, { 0.375f, 0.312f }},
    {{ -0.208f, 0.25f, -0.312f }, { -0.497f, 0.447f, -0.744f }, { 0.375f, 0.344f }},
    {{ -0.265f, 0.25f, -0.265f }, { -0.632f, 0.447f, -0.632f }, { 0.375f, 0.375f }},
    {{ -0.312f, 0.25f, -0.208f }, { -0.744f, 0.447f, -0.497f }, { 0.375f, 0.406f }},
    {{ -0.346f, 0.25f, -0.144f }, { -0.826f, 0.447f, -0.342f }, { 0.375f, 0.438f }},
    {{ -0.368f, 0.25f, -0.073f }, { -0.877f, 0.447f, -0.174f }, { 0.375f, 0.469f }},
    {{ -0.375f, 0.25f, -0.f }, { -0.894f, 0.447f, -0.f }, { 0.375f, 0.5f }},
    {{ -0.368f, 0.25f, 0.073f }, { -0.877f, 0.447f, 0.174f }, { 0.375f, 0.531f }},
    {{ -0.346f, 0.25f, 0.144f }, { -0.826f, 0.447f, 0.342f }, { 0.375f, 0.562f }},
    {{ -0.312f, 0.25f, 0.208f }, { -0.744f, 0.447f, 0.497f }, { 0.375f, 0.594f }},
    {{ -0.265f, 0.25f, 0.265f }, { -0.632f, 0.447f, 0.632f }, { 0.375f, 0.625f }},
    {{ -0.208f, 0.25f, 0.312f }, { -0.497f, 0.447f, 0.744f }, { 0.375f, 0.656f }},
    {{ -0.144f, 0.25f, 0.346f }, { -0.342f, 0.447f, 0.826f }, { 0.375f, 0.687f }},
    {{ -0.073f, 0.25f, 0.368f }, { -0.174f, 0.447f, 0.877f }, { 0.375f, 0.719f }},
    {{ 0.f, 0.25f, 0.375f }, { 0.f, 0.447f, 0.894f }, { 0.375f, 0.75f }},
    {{ 0.073f, 0.25f, 0.368f }, { 0.174f, 0.447f, 0.877f }, { 0.375f, 0.781f }},
    {{ 0.144f, 0.25f, 0.346f }, { 0.342f, 0.447f, 0.826f }, { 0.375f, 0.813f }},
    {{ 0.208f, 0.25f, 0.312f }, { 0.497f, 0.447f, 0.744f }, { 0.375f, 0.844f }},
    {{ 0.265f, 0.25f, 0.265f }, { 0.632f, 0.447f, 0.632f }, { 0.375f, 0.875f }},
    {{ 0.312f, 0.25f, 0.208f }, { 0.744f, 0.447f, 0.497f }, { 0.375f, 0.906f }},
    {{ 0.346f, 0.25f, 0.144f }, { 0.826f, 0.447f, 0.342f }, { 0.375f, 0.937f }},
    {{ 0.368f, 0.25f, 0.073f }, { 0.877f, 0.447f, 0.174f }, { 0.375f, 0.969f }},
    {{ 0.375f, 0.25f, -0.f }, { 0.894f, 0.447f, -0.f }, { 0.375f, 1.f }},
    {{ 0.5f, 0.f, 0.f }, { 0.894f, 0.447f, -0.f }, { 0.5f, 0.f }},
    {{ 0.49f, -0.f, -0.098f }, { 0.877f, 0.447f, -0.174f }, { 0.5f, 0.031f }},
    {{ 0.462f, -0.f, -0.191f }, { 0.826f, 0.447f, -0.342f }, { 0.5f, 0.062f }},
    {{ 0.416f, -0.f, -0.278f }, { 0.744f, 0.447f, -0.497f }, { 0.5f, 0.094f }},
    {{ 0.354f, -0.f, -0.354f }, { 0.632f, 0.447f, -0.632f }, { 0.5f, 0.125f }},
    {{ 0.278f, -0.f, -0.416f }, { 0.497f, 0.447f, -0.744f }, { 0.5f, 0.156f }},
    {{ 0.191f, -0.f, -0.462f }, { 0.342f, 0.447f, -0.826f }, { 0.5f, 0.188f }},
    {{ 0.098f, -0.f, -0.49f }, { 0.174f, 0.447f, -0.877f }, { 0.5f, 0.219f }},
    {{ 0.f, -0.f, -0.5f }, { 0.f, 0.447f, -0.894f }, { 0.5f, 0.25f }},
    {{ -0.098f, -0.f, -0.49f }, { -0.174f, 0.447f, -0.877f }, { 0.5f, 0.281f }},
    {{ -0.191f, -0.f, -0.462f }, { -0.342f, 0.447f, -0.826f }, { 0.5f, 0.312f }},
    {{ -0.278f, -0.f, -0.416f }, { -0.497f, 0.447f, -0.744f }, { 0.5f, 0.344f }},
    {{ -0.354f, -0.f, -0.354f }, { -0.632f, 0.447f, -0.632f }, { 0.5f, 0.375f }},
    {{ -0.416f, -0.f, -0.278f }, { -0.744f, 0.447f, -0.497f }, { 0.5f, 0.406f }},
    {{ -0.462f, -0.f, -0.191f }, { -0.826f, 0.447f, -0.342f }, { 0.5f, 0.438f }},
    {{ -0.49f, -0.f, -0.098f }, { -0.877f, 0.447f, -0.174f }, { 0.5f, 0.469f }},
    {{ -0.5f, 0.f, 0.f }, { -0.894f, 0.447f, -0.f }, { 0.5f, 0.5f }},
    {{ -0.49f, 0.f, 0.098f }, { -0.877f, 0.447f, 0.174f }, { 0.5f, 0.531f }},
    {{ -0.462f, 0.f, 0.191f }, { -0.826f, 0.447f, 0.342f }, { 0.5f, 0.562f }},
    {{ -0.416f, 0.f, 0.278f }, { -0.744f, 0.447f, 0.497f }, { 0.5f, 0.594f }},
    {{ -0.354f, 0.f, 0.354f }, { -0.632f, 0.447f, 0.632f }, { 0.5f, 0.625f }},
    {{ -0.278f, 0.f, 0.416f }, { -0.497f, 0.447f, 0.744f }, { 0.5f, 0.656f }},
    {{ -0.191f, 0.f, 0.462f }, { -0.342f, 0.447f, 0.826f }, { 0.5f, 0.687f }},
    {{ -0.098f, 0.f, 0.49f }, { -0.174f, 0.447f, 0.877f }, { 0.5f, 0.719f }},
    {{ 0.f, 0.f, 0.5f }, { 0.f, 0.447f, 0.894f }, { 0.5f, 0.75f }},
    {{ 0.098f, 0.f, 0.49f }, { 0.174f, 0.447f, 0.877f }, { 0.5f, 0.781f }},
    {{ 0.191f, 0.f, 0.462f }, { 0.342f, 0.447f, 0.826f }, { 0.5f, 0.813f }},
    {{ 0.278f, 0.f, 0.416f }, { 0.497f, 0.447f, 0.744f }, { 0.5f, 0.844f }},
    {{ 0.354f, 0.f, 0.354f }, { 0.632f, 0.447f, 0.632f }, { 0.5f, 0.875f }},
    {{ 0.416f, 0.f, 0.278f }, { 0.744f, 0.447f, 0.497f }, { 0.5f, 0.906f }},
    {{ 0.462f, 0.f, 0.191f }, { 0.826f, 0.447f, 0.342f }, { 0.5f, 0.937f }},
    {{ 0.49f, 0.f, 0.098f }, { 0.877f, 0.447f, 0.174f }, { 0.5f, 0.969f }},
    {{ 0.5f, 0.f, 0.f }, { 0.894f, 0.447f, -0.f }, { 0.5f, 1.f }},
    {{ 0.625f, -0.25f, 0.f }, { 0.894f, 0.447f, -0.f }, { 0.625f, 0.f }},
    {{ 0.613f, -0.25f, -0.122f }, { 0.877f, 0.447f, -0.174f }, { 0.625f, 0.031f }},
    {{ 0.577f, -0.25f, -0.239f }, { 0.826f, 0.447f, -0.342f }, { 0.625f, 0.062f }},
    {{ 0.52f, -0.25f, -0.347f }, { 0.744f, 0.447f, -0.497f }, { 0.625f, 0.094f }},
    {{ 0.442f, -0.25f, -0.442f }, { 0.632f, 0.447f, -0.632f }, { 0.625f, 0.125f }},
    {{ 0.347f, -0.25f, -0.52f }, { 0.497f, 0.447f, -0.744f }, { 0.625f, 0.156f }},
    {{ 0.239f, -0.25f, -0.577f }, { 0.342f, 0.447f, -0.826f }, { 0.625f, 0.188f }},
    {{ 0.122f, -0.25f, -0.613f }, { 0.174f, 0.447f, -0.877f }, { 0.625f, 0.219f }},
    {{ 0.f, -0.25f, -0.625f }, { 0.f, 0.447f, -0.894f }, { 0.625f, 0.25f }},
    {{ -0.122f, -0.25f, -0.613f }, { -0.174f, 0.447f, -0.877f }, { 0.625f, 0.281f }},
    {{ -0.239f, -0.25f, -0.577f }, { -0.342f, 0.447f, -0.826f }, { 0.625f, 0.312f }},
    {{ -0.347f, -0.25f, -0.52f }, { -0.497f, 0.447f, -0.744f }, { 0.625f, 0.344f }},
    {{ -0.442f, -0.25f, -0.442f }, { -0.632f, 0.447f, -0.632f }, { 0.625f, 0.375f }},
    {{ -0.52f, -0.25f, -0.347f }, { -0.744f, 0.447f, -0.497f }, { 0.625f, 0.406f }},
    {{ -0.577f, -0.25f, -0.239f }, { -0.826f, 0.447f, -0.342f }, { 0.625f, 0.438f }},
    {{ -0.613f, -0.25f, -0.122f }, { -0.877f, 0.447f, -0.174f }, { 0.625f, 0.469f }},
    {{ -0.625f, -0.25f, 0.f }, { -0.894f, 0.447f, -0.f }, { 0.625f, 0.5f }},
    {{ -0.613f, -0.25f, 0.122f }, { -0.877f, 0.447f, 0.174f }, { 0.625f, 0.531f }},
    {{ -0.577f, -0.25f, 0.239f }, { -0.826f, 0.447f, 0.342f }, { 0.625f, 0.562f }},
    {{ -0.52f, -0.25f, 0.347f }, { -0.744f, 0.447f, 0.497f }, { 0.625f, 0.594f }},
    {{ -0.442f, -0.25f, 0.442f }, { -0.632f, 0.447f, 0.632f }, { 0.625f, 0.625f }},
    {{ -0.347f, -0.25f, 0.52f }, { -0.497f, 0.447f, 0.744f }, { 0.625f, 0.656f }},
    {{ -0.239f, -0.25f, 0.577f }, { -0.342f, 0.447f, 0.826f }, { 0.625f, 0.687f }},
    {{ -0.122f, -0.25f, 0.613f }, { -0.174f, 0.447f, 0.877f }, { 0.625f, 0.719f }},
    {{ 0.f, -0.25f, 0.625f }, { 0.f, 0.447f, 0.894f }, { 0.625f, 0.75f }},
    {{ 0.122f, -0.25f, 0.613f }, { 0.174f, 0.447f, 0.877f }, { 0.625f, 0.781f }},
    {{ 0.239f, -0.25f, 0.577f }, { 0.342f, 0.447f, 0.826f }, { 0.625f, 0.813f }},
    {{ 0.347f, -0.25f, 0.52f }, { 0.497f, 0.447f, 0.744f }, { 0.625f, 0.844f }},
    {{ 0.442f, -0.25f, 0.442f }, { 0.632f, 0.447f, 0.632f }, { 0.625f, 0.875f }},
    {{ 0.52f, -0.25f, 0.347f }, { 0.744f, 0.447f, 0.497f }, { 0.625f, 0.906f }},
    {{ 0.577f, -0.25f, 0.239f }, { 0.826f, 0.447f, 0.342f }, { 0.625f, 0.937f }},
    {{ 0.613f, -0.25f, 0.122f }, { 0.877f, 0.447f, 0.174f }, { 0.625f, 0.969f }},
    {{ 0.625f, -0.25f, 0.f }, { 0.894f, 0.447f, -0.f }, { 0.625f, 1.f }},
    {{ 0.75f, -0.5f, 0.f }, { 0.894f, 0.447f, -0.f }, { 0.75f, 0.f }},
    {{ 0.736f, -0.5f, -0.146f }, { 0.877f, 0.447f, -0.174f }, { 0.75f, 0.031f }},
    {{ 0.693f, -0.5f, -0.287f }, { 0.826f, 0.447f, -0.342f }, { 0.75f, 0.062f }},
    {{ 0.624f, -0.5f, -0.417f }, { 0.744f, 0.447f, -0.497f }, { 0.75f, 0.094f }},
    {{ 0.53f, -0.5f, -0.53f }, { 0.632f, 0.447f, -0.632f }, { 0.75f, 0.125f }},
    {{ 0.417f, -0.5f, -0.624f }, { 0.497f, 0.447f, -0.744f }, { 0.75f, 0.156f }},
    {{ 0.287f, -0.5f, -0.693f }, { 0.342f, 0.447f, -0.826f }, { 0.75f, 0.188f }},
    {{ 0.146f, -0.5f, -0.736f }, { 0.174f, 0.447f, -0.877f }, { 0.75f, 0.219f }},
    {{ 0.f, -0.5f, -0.75f }, { 0.f, 0.447f, -0.894f }, { 0.75f, 0.25f }},
    {{ -0.146f, -0.5f, -0.736f }, { -0.174f, 0.447f, -0.877f }, { 0.75f, 0.281f }},
    {{ -0.287f, -0.5f, -0.693f }, { -0.342f, 0.447f, -0.826f }, { 0.75f, 0.312f }},
    {{ -0.417f, -0.5f, -0.624f }, { -0.497f, 0.447f, -0.744f }, { 0.75f, 0.344f }},
    {{ -0.53f, -0.5f, -0.53f }, { -0.632f, 0.447f, -0.632f }, { 0.75f, 0.375f }},
    {{ -0.624f, -0.5f, -0.417f }, { -0.744f, 0.447f, -0.497f }, { 0.75f, 0.406f }},
    {{ -0.693f, -0.5f, -0.287f }, { -0.826f, 0.447f, -0.342f }, { 0.75f, 0.438f }},
    {{ -0.736f, -0.5f, -0.146f }, { -0.877f, 0.447f, -0.174f }, { 0.75f, 0.469f }},
    {{ -0.75f, -0.5f, 0.f }, { -0.894f, 0.447f, -0.f }, { 0.75f, 0.5f }},
    {{ -0.736f, -0.5f, 0.146f }, { -0.877f, 0.447f, 0.174f }, { 0.75f, 0.531f }},
    {{ -0.693f, -0.5f, 0.287f }, { -0.826f, 0.447f, 0.342f }, { 0.75f, 0.562f }},
    {{ -0.624f, -0.5f, 0.417f }, { -0.744f, 0.447f, 0.497f }, { 0.75f, 0.594f }},
    {{ -0.53f, -0.5f, 0.53f }, { -0.632f, 0.447f, 0.632f }, { 0.75f, 0.625f }},
    {{ -0.417f, -0.5f, 0.624f }, { -0.497f, 0.447f, 0.744f }, { 0.75f, 0.656f }},
    {{ -0.287f, -0.5f, 0.693f }, { -0.342f, 0.447f, 0.826f }, { 0.75f, 0.687f }},
    {{ -0.146f, -0.5f, 0.736f }, { -0.174f, 0.447f, 0.877f }, { 0.75f, 0.719f }},
    {{ 0.f, -0.5f, 0.75f }, { 0.f, 0.447f, 0.894f }, { 0.75f, 0.75f }},
    {{ 0.146f, -0.5f, 0.736f }, { 0.174f, 0.447f, 0.877f }, { 0.75f, 0.781f }},
    {{ 0.287f, -0.5f, 0.693f }, { 0.342f, 0.447f, 0.826f }, { 0.75f, 0.813f }},
    {{ 0.417f, -0.5f, 0.624f }, { 0.497f, 0.447f, 0.744f }, { 0.75f, 0.844f }},
    {{ 0.53f, -0.5f, 0.53f }, { 0.632f, 0.447f, 0.632f }, { 0.75f, 0.875f }},
    {{ 0.624f, -0.5f, 0.417f }, { 0.744f, 0.447f, 0.497f }, { 0.75f, 0.906f }},
    {{ 0.693f, -0.5f, 0.287f }, { 0.826f, 0.447f, 0.342f }, { 0.75f, 0.937f }},
    {{ 0.736f, -0.5f, 0.146f }, { 0.877f, 0.447f, 0.174f }, { 0.75f, 0.969f }},
    {{ 0.75f, -0.5f, 0.f }, { 0.894f, 0.447f, -0.f }, { 0.75f, 1.f }},
    {{ 0.875f, -0.75f, 0.f }, { 0.894f, 0.447f, -0.f }, { 0.875f, 0.f }},
    {{ 0.858f, -0.75f, -0.171f }, { 0.877f, 0.447f, -0.174f }, { 0.875f, 0.031f }},
    {{ 0.808f, -0.75f, -0.335f }, { 0.826f, 0.447f, -0.342f }, { 0.875f, 0.062f }},
    {{ 0.728f, -0.75f, -0.486f }, { 0.744f, 0.447f, -0.497f }, { 0.875f, 0.094f }},
    {{ 0.619f, -0.75f, -0.619f }, { 0.632f, 0.447f, -0.632f }, { 0.875f, 0.125f }},
    {{ 0.486f, -0.75f, -0.728f }, { 0.497f, 0.447f, -0.744f }, { 0.875f, 0.156f }},
    {{ 0.335f, -0.75f, -0.808f }, { 0.342f, 0.447f, -0.826f }, { 0.875f, 0.188f }},
    {{ 0.171f, -0.75f, -0.858f }, { 0.174f, 0.447f, -0.877f }, { 0.875f, 0.219f }},
    {{ 0.f, -0.75f, -0.875f }, { 0.f, 0.447f, -0.894f }, { 0.875f, 0.25f }},
    {{ -0.171f, -0.75f, -0.858f }, { -0.174f, 0.447f, -0.877f }, { 0.875f, 0.281f }},
    {{ -0.335f, -0.75f, -0.808f }, { -0.342f, 0.447f, -0.826f }, { 0.875f, 0.312f }},
    {{ -0.486f, -0.75f, -0.728f }, { -0.497f, 0.447f, -0.744f }, { 0.875f, 0.344f }},
    {{ -0.619f, -0.75f, -0.619f }, { -0.632f, 0.447f, -0.632f }, { 0.875f, 0.375f }},
    {{ -0.728f, -0.75f, -0.486f }, { -0.744f, 0.447f, -0.497f }, { 0.875f, 0.406f }},
    {{ -0.808f, -0.75f, -0.335f }, { -0.826f, 0.447f, -0.342f }, { 0.875f, 0.438f }},
    {{ -0.858f, -0.75f, -0.171f }, { -0.877f, 0.447f, -0.174f }, { 0.875f, 0.469f }},
    {{ -0.875f, -0.75f, 0.f }, { -0.894f, 0.447f, -0.f }, { 0.875f, 0.5f }},
    {{ -0.858f, -0.75f, 0.171f }, { -0.877f, 0.447f, 0.174f }, { 0.875f, 0.531f }},
    {{ -0.808f, -0.75f, 0.335f }, { -0.826f, 0.447f, 0.342f }, { 0.875f, 0.562f }},
    {{ -0.728f, -0.75f, 0.486f }, { -0.744f, 0.447f, 0.497f }, { 0.875f, 0.594f }},
    {{ -0.619f, -0.75f, 0.619f }, { -0.632f, 0.447f, 0.632f }, { 0.875f, 0.625f }},
    {{ -0.486f, -0.75f, 0.728f }, { -0.497f, 0.447f, 0.744f }, { 0.875f, 0.656f }},
    {{ -0.335f, -0.75f, 0.808f }, { -0.342f, 0.447f, 0.826f }, { 0.875f, 0.687f }},
    {{ -0.171f, -0.75f, 0.858f }, { -0.174f, 0.447f, 0.877f }, { 0.875f, 0.719f }},
    {{ 0.f, -0.75f, 0.875f }, { 0.f, 0.447f, 0.894f }, { 0.875f, 0.75f }},
    {{ 0.171f, -0.75f, 0.858f }, { 0.174f, 0.447f, 0.877f }, { 0.875f, 0.781f }},
    {{ 0.335f, -0.75f, 0.808f }, { 0.342f, 0.447f, 0.826f }, { 0.875f, 0.813f }},
    {{ 0.486f, -0.75f, 0.728f }, { 0.497f, 0.447f, 0.744f }, { 0.875f, 0.844f }},
    {{ 0.619f, -0.75f, 0.619f }, { 0.632f, 0.447f, 0.632f }, { 0.875f, 0.875f }},
    {{ 0.728f, -0.75f, 0.486f }, { 0.744f, 0.447f, 0.497f }, { 0.875f, 0.906f }},
    {{ 0.808f, -0.75f, 0.335f }, { 0.826f, 0.447f, 0.342f }, { 0.875f, 0.937f }},
    {{ 0.858f, -0.75f, 0.171f }, { 0.877f, 0.447f, 0.174f }, { 0.875f, 0.969f }},
    {{ 0.875f, -0.75f, 0.f }, { 0.894f, 0.447f, -0.f }, { 0.875f, 1.f }},
    {{ 1.f, -1.f, 0.f }, { 0.894f, 0.447f, -0.f }, { 1.f, 0.f }},
    {{ 0.981f, -1.f, -0.195f }, { 0.877f, 0.447f, -0.174f }, { 1.f, 0.031f }},
    {{ 0.924f, -1.f, -0.383f }, { 0.826f, 0.447f, -0.342f }, { 1.f, 0.062f }},
    {{ 0.831f, -1.f, -0.556f }, { 0.744f, 0.447f, -0.497f }, { 1.f, 0.094f }},
    {{ 0.707f, -1.f, -0.707f }, { 0.632f, 0.447f, -0.632f }, { 1.f, 0.125f }},
    {{ 0.556f, -1.f, -0.831f }, { 0.497f, 0.447f, -0.744f }, { 1.f, 0.156f }},
    {{ 0.383f, -1.f, -0.924f }, { 0.342f, 0.447f, -0.826f }, { 1.f, 0.188f }},
    {{ 0.195f, -1.f, -0.981f }, { 0.174f, 0.447f, -0.877f }, { 1.f, 0.219f }},
    {{ 0.f, -1.f, -1.f }, { 0.f, 0.447f, -0.894f }, { 1.f, 0.25f }},
    {{ -0.195f, -1.f, -0.981f }, { -0.174f, 0.447f, -0.877f }, { 1.f, 0.281f }},
    {{ -0.383f, -1.f, -0.924f }, { -0.342f, 0.447f, -0.826f }, { 1.f, 0.312f }},
    {{ -0.556f, -1.f, -0.831f }, { -0.497f, 0.447f, -0.744f }, { 1.f, 0.344f }},
    {{ -0.707f, -1.f, -0.707f }, { -0.632f, 0.447f, -0.632f }, { 1.f, 0.375f }},
    {{ -0.831f, -1.f, -0.556f }, { -0.744f, 0.447f, -0.497f }, { 1.f, 0.406f }},
    {{ -0.924f, -1.f, -0.383f }, { -0.826f, 0.447f, -0.342f }, { 1.f, 0.438f }},
    {{ -0.981f, -1.f, -0.195f }, { -0.877f, 0.447f, -0.174f }, { 1.f, 0.469f }},
    {{ -1.f, -1.f, 0.f }, { -0.894f, 0.447f, -0.f }, { 1.f, 0.5f }},
    {{ -0.981f, -1.f, 0.195f }, { -0.877f, 0.447f, 0.174f }, { 1.f, 0.531f }},
    {{ -0.924f, -1.f, 0.383f }, { -0.826f, 0.447f, 0.342f }, { 1.f, 0.562f }},
    {{ -0.831f, -1.f, 0.556f }, { -0.744f, 0.447f, 0.497f }, { 1.f, 0.594f }},
    {{ -0.707f, -1.f, 0.707f }, { -0.632f, 0.447f, 0.632f }, { 1.f, 0.625f }},
    {{ -0.556f, -1.f, 0.831f }, { -0.497f, 0.447f, 0.744f }, { 1.f, 0.656f }},
    {{ -0.383f, -1.f, 0.924f }, { -0.342f, 0.447f, 0.826f }, { 1.f, 0.687f }},
    {{ -0.195f, -1.f, 0.981f }, { -0.174f, 0.447f, 0.877f }, { 1.f, 0.719f }},
    {{ 0.f, -1.f, 1.f }, { 0.f, 0.447f, 0.894f }, { 1.f, 0.75f }},
    {{ 0.195f, -1.f, 0.981f }, { 0.174f, 0.447f, 0.877f }, { 1.f, 0.781f }},
    {{ 0.383f, -1.f, 0.924f }, { 0.342f, 0.447f, 0.826f }, { 1.f, 0.813f }},
    {{ 0.556f, -1.f, 0.831f }, { 0.497f, 0.447f, 0.744f }, { 1.f, 0.844f }},
    {{ 0.707f, -1.f, 0.707f }, { 0.632f, 0.447f, 0.632f }, { 1.f, 0.875f }},
    {{ 0.831f, -1.f, 0.556f }, { 0.744f, 0.447f, 0.497f }, { 1.f, 0.906f }},
    {{ 0.924f, -1.f, 0.383f }, { 0.826f, 0.447f, 0.342f }, { 1.f, 0.937f }},
    {{ 0.981f, -1.f, 0.195f }, { 0.877f, 0.447f, 0.174f }, { 1.f, 0.969f }},
    {{ 1.f, -1.f, 0.f }, { 0.894f, 0.447f, -0.f }, { 1.f, 1.f }}};

std::vector<u16> Cone::indices = {
    0, 33, 34,
    0, 34, 1,
    1, 34, 35,
    1, 35, 2,
    2, 35, 36,
    2, 36, 3,
    3, 36, 37,
    3, 37, 4,
    4, 37, 38,
    4, 38, 5,
    5, 38, 39,
    5, 39, 6,
    6, 39, 40,
    6, 40, 7,
    7, 40, 41,
    7, 41, 8,
    8, 41, 42,
    8, 42, 9,
    9, 42, 43,
    9, 43, 10,
    10, 43, 44,
    10, 44, 11,
    11, 44, 45,
    11, 45, 12,
    12, 45, 46,
    12, 46, 13,
    13, 46, 47,
    13, 47, 14,
    14, 47, 48,
    14, 48, 15,
    15, 48, 49,
    15, 49, 16,
    16, 49, 50,
    16, 50, 17,
    17, 50, 51,
    17, 51, 18,
    18, 51, 52,
    18, 52, 19,
    19, 52, 53,
    19, 53, 20,
    20, 53, 54,
    20, 54, 21,
    21, 54, 55,
    21, 55, 22,
    22, 55, 56,
    22, 56, 23,
    23, 56, 57,
    23, 57, 24,
    24, 57, 58,
    24, 58, 25,
    25, 58, 59,
    25, 59, 26,
    26, 59, 60,
    26, 60, 27,
    27, 60, 61,
    27, 61, 28,
    28, 61, 62,
    28, 62, 29,
    29, 62, 63,
    29, 63, 30,
    30, 63, 64,
    30, 64, 31,
    31, 64, 65,
    31, 65, 32,
    33, 66, 67,
    33, 67, 34,
    34, 67, 68,
    34, 68, 35,
    35, 68, 69,
    35, 69, 36,
    36, 69, 70,
    36, 70, 37,
    37, 70, 71,
    37, 71, 38,
    38, 71, 72,
    38, 72, 39,
    39, 72, 73,
    39, 73, 40,
    40, 73, 74,
    40, 74, 41,
    41, 74, 75,
    41, 75, 42,
    42, 75, 76,
    42, 76, 43,
    43, 76, 77,
    43, 77, 44,
    44, 77, 78,
    44, 78, 45,
    45, 78, 79,
    45, 79, 46,
    46, 79, 80,
    46, 80, 47,
    47, 80, 81,
    47, 81, 48,
    48, 81, 82,
    48, 82, 49,
    49, 82, 83,
    49, 83, 50,
    50, 83, 84,
    50, 84, 51,
    51, 84, 85,
    51, 85, 52,
    52, 85, 86,
    52, 86, 53,
    53, 86, 87,
    53, 87, 54,
    54, 87, 88,
    54, 88, 55,
    55, 88, 89,
    55, 89, 56,
    56, 89, 90,
    56, 90, 57,
    57, 90, 91,
    57, 91, 58,
    58, 91, 92,
    58, 92, 59,
    59, 92, 93,
    59, 93, 60,
    60, 93, 94,
    60, 94, 61,
    61, 94, 95,
    61, 95, 62,
    62, 95, 96,
    62, 96, 63,
    63, 96, 97,
    63, 97, 64,
    64, 97, 98,
    64, 98, 65,
    66, 99, 100,
    66, 100, 67,
    67, 100, 101,
    67, 101, 68,
    68, 101, 102,
    68, 102, 69,
    69, 102, 103,
    69, 103, 70,
    70, 103, 104,
    70, 104, 71,
    71, 104, 105,
    71, 105, 72,
    72, 105, 106,
    72, 106, 73,
    73, 106, 107,
    73, 107, 74,
    74, 107, 108,
    74, 108, 75,
    75, 108, 109,
    75, 109, 76,
    76, 109, 110,
    76, 110, 77,
    77, 110, 111,
    77, 111, 78,
    78, 111, 112,
    78, 112, 79,
    79, 112, 113,
    79, 113, 80,
    80, 113, 114,
    80, 114, 81,
    81, 114, 115,
    81, 115, 82,
    82, 115, 116,
    82, 116, 83,
    83, 116, 117,
    83, 117, 84,
    84, 117, 118,
    84, 118, 85,
    85, 118, 119,
    85, 119, 86,
    86, 119, 120,
    86, 120, 87,
    87, 120, 121,
    87, 121, 88,
    88, 121, 122,
    88, 122, 89,
    89, 122, 123,
    89, 123, 90,
    90, 123, 124,
    90, 124, 91,
    91, 124, 125,
    91, 125, 92,
    92, 125, 126,
    92, 126, 93,
    93, 126, 127,
    93, 127, 94,
    94, 127, 128,
    94, 128, 95,
    95, 128, 129,
    95, 129, 96,
    96, 129, 130,
    96, 130, 97,
    97, 130, 131,
    97, 131, 98,
    99, 132, 133,
    99, 133, 100,
    100, 133, 134,
    100, 134, 101,
    101, 134, 135,
    101, 135, 102,
    102, 135, 136,
    102, 136, 103,
    103, 136, 137,
    103, 137, 104,
    104, 137, 138,
    104, 138, 105,
    105, 138, 139,
    105, 139, 106,
    106, 139, 140,
    106, 140, 107,
    107, 140, 141,
    107, 141, 108,
    108, 141, 142,
    108, 142, 109,
    109, 142, 143,
    109, 143, 110,
    110, 143, 144,
    110, 144, 111,
    111, 144, 145,
    111, 145, 112,
    112, 145, 146,
    112, 146, 113,
    113, 146, 147,
    113, 147, 114,
    114, 147, 148,
    114, 148, 115,
    115, 148, 149,
    115, 149, 116,
    116, 149, 150,
    116, 150, 117,
    117, 150, 151,
    117, 151, 118,
    118, 151, 152,
    118, 152, 119,
    119, 152, 153,
    119, 153, 120,
    120, 153, 154,
    120, 154, 121,
    121, 154, 155,
    121, 155, 122,
    122, 155, 156,
    122, 156, 123,
    123, 156, 157,
    123, 157, 124,
    124, 157, 158,
    124, 158, 125,
    125, 158, 159,
    125, 159, 126,
    126, 159, 160,
    126, 160, 127,
    127, 160, 161,
    127, 161, 128,
    128, 161, 162,
    128, 162, 129,
    129, 162, 163,
    129, 163, 130,
    130, 163, 164,
    130, 164, 131,
    132, 165, 166,
    132, 166, 133,
    133, 166, 167,
    133, 167, 134,
    134, 167, 168,
    134, 168, 135,
    135, 168, 169,
    135, 169, 136,
    136, 169, 170,
    136, 170, 137,
    137, 170, 171,
    137, 171, 138,
    138, 171, 172,
    138, 172, 139,
    139, 172, 173,
    139, 173, 140,
    140, 173, 174,
    140, 174, 141,
    141, 174, 175,
    141, 175, 142,
    142, 175, 176,
    142, 176, 143,
    143, 176, 177,
    143, 177, 144,
    144, 177, 178,
    144, 178, 145,
    145, 178, 179,
    145, 179, 146,
    146, 179, 180,
    146, 180, 147,
    147, 180, 181,
    147, 181, 148,
    148, 181, 182,
    148, 182, 149,
    149, 182, 183,
    149, 183, 150,
    150, 183, 184,
    150, 184, 151,
    151, 184, 185,
    151, 185, 152,
    152, 185, 186,
    152, 186, 153,
    153, 186, 187,
    153, 187, 154,
    154, 187, 188,
    154, 188, 155,
    155, 188, 189,
    155, 189, 156,
    156, 189, 190,
    156, 190, 157,
    157, 190, 191,
    157, 191, 158,
    158, 191, 192,
    158, 192, 159,
    159, 192, 193,
    159, 193, 160,
    160, 193, 194,
    160, 194, 161,
    161, 194, 195,
    161, 195, 162,
    162, 195, 196,
    162, 196, 163,
    163, 196, 197,
    163, 197, 164,
    165, 198, 199,
    165, 199, 166,
    166, 199, 200,
    166, 200, 167,
    167, 200, 201,
    167, 201, 168,
    168, 201, 202,
    168, 202, 169,
    169, 202, 203,
    169, 203, 170,
    170, 203, 204,
    170, 204, 171,
    171, 204, 205,
    171, 205, 172,
    172, 205, 206,
    172, 206, 173,
    173, 206, 207,
    173, 207, 174,
    174, 207, 208,
    174, 208, 175,
    175, 208, 209,
    175, 209, 176,
    176, 209, 210,
    176, 210, 177,
    177, 210, 211,
    177, 211, 178,
    178, 211, 212,
    178, 212, 179,
    179, 212, 213,
    179, 213, 180,
    180, 213, 214,
    180, 214, 181,
    181, 214, 215,
    181, 215, 182,
    182, 215, 216,
    182, 216, 183,
    183, 216, 217,
    183, 217, 184,
    184, 217, 218,
    184, 218, 185,
    185, 218, 219,
    185, 219, 186,
    186, 219, 220,
    186, 220, 187,
    187, 220, 221,
    187, 221, 188,
    188, 221, 222,
    188, 222, 189,
    189, 222, 223,
    189, 223, 190,
    190, 223, 224,
    190, 224, 191,
    191, 224, 225,
    191, 225, 192,
    192, 225, 226,
    192, 226, 193,
    193, 226, 227,
    193, 227, 194,
    194, 227, 228,
    194, 228, 195,
    195, 228, 229,
    195, 229, 196,
    196, 229, 230,
    196, 230, 197,
    198, 231, 232,
    198, 232, 199,
    199, 232, 233,
    199, 233, 200,
    200, 233, 234,
    200, 234, 201,
    201, 234, 235,
    201, 235, 202,
    202, 235, 236,
    202, 236, 203,
    203, 236, 237,
    203, 237, 204,
    204, 237, 238,
    204, 238, 205,
    205, 238, 239,
    205, 239, 206,
    206, 239, 240,
    206, 240, 207,
    207, 240, 241,
    207, 241, 208,
    208, 241, 242,
    208, 242, 209,
    209, 242, 243,
    209, 243, 210,
    210, 243, 244,
    210, 244, 211,
    211, 244, 245,
    211, 245, 212,
    212, 245, 246,
    212, 246, 213,
    213, 246, 247,
    213, 247, 214,
    214, 247, 248,
    214, 248, 215,
    215, 248, 249,
    215, 249, 216,
    216, 249, 250,
    216, 250, 217,
    217, 250, 251,
    217, 251, 218,
    218, 251, 252,
    218, 252, 219,
    219, 252, 253,
    219, 253, 220,
    220, 253, 254,
    220, 254, 221,
    221, 254, 255,
    221, 255, 222,
    222, 255, 256,
    222, 256, 223,
    223, 256, 257,
    223, 257, 224,
    224, 257, 258,
    224, 258, 225,
    225, 258, 259,
    225, 259, 226,
    226, 259, 260,
    226, 260, 227,
    227, 260, 261,
    227, 261, 228,
    228, 261, 262,
    228, 262, 229,
    229, 262, 263,
    229, 263, 230,
    231, 264, 265,
    231, 265, 232,
    232, 265, 266,
    232, 266, 233,
    233, 266, 267,
    233, 267, 234,
    234, 267, 268,
    234, 268, 235,
    235, 268, 269,
    235, 269, 236,
    236, 269, 270,
    236, 270, 237,
    237, 270, 271,
    237, 271, 238,
    238, 271, 272,
    238, 272, 239,
    239, 272, 273,
    239, 273, 240,
    240, 273, 274,
    240, 274, 241,
    241, 274, 275,
    241, 275, 242,
    242, 275, 276,
    242, 276, 243,
    243, 276, 277,
    243, 277, 244,
    244, 277, 278,
    244, 278, 245,
    245, 278, 279,
    245, 279, 246,
    246, 279, 280,
    246, 280, 247,
    247, 280, 281,
    247, 281, 248,
    248, 281, 282,
    248, 282, 249,
    249, 282, 283,
    249, 283, 250,
    250, 283, 284,
    250, 284, 251,
    251, 284, 285,
    251, 285, 252,
    252, 285, 286,
    252, 286, 253,
    253, 286, 287,
    253, 287, 254,
    254, 287, 288,
    254, 288, 255,
    255, 288, 289,
    255, 289, 256,
    256, 289, 290,
    256, 290, 257,
    257, 290, 291,
    257, 291, 258,
    258, 291, 292,
    258, 292, 259,
    259, 292, 293,
    259, 293, 260,
    260, 293, 294,
    260, 294, 261,
    261, 294, 295,
    261, 295, 262,
    262, 295, 296,
    262, 296, 263 };
