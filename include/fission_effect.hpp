#pragma once

#include "neutron.hpp"
#include "render_types.hpp"

#include <cstdint>
#include <vector>

struct FissionEffect{
    Vector2 position;
    float age = 0.0F;
    float duration = 0.8F;
    Vector2 split_axis{1.0F, 0.0F};
    std::uint32_t emitted_neutrons = 2;
};

void spawn_fission_effects(
    std::vector<FissionEffect>& effects,
    const std::vector<FissionEvent>& events,
    const FieldView& field,
    float space_size
);

void advance_fission_effects(std::vector<FissionEffect>& effects, float delta_time);
void remove_expired_fission_effects(std::vector<FissionEffect>& effects);
