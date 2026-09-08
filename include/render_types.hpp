#pragma once

#include "vec3.hpp"

#include "raylib.h"

struct FieldView{
    const float size = 600.0F;
    const float x = 300.0F;
    const float y = 80.0F;
};

Vector2 world_to_screen(
    const Vec3& position,
    const FieldView& field,
    float space_size
);
