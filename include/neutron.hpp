#pragma once

#include "vec3.hpp"
#include "neutron_state.hpp"

#include <cstdint>

struct Neutron {
    Vec3 position{};
    Vec3 direction{};
    float energy = 1.0F;
    std::uint32_t generation = 0;
    NeutronState state = NeutronState::Active;
};

struct FissionEvent{
    Vec3 position{};
    std::uint32_t spawned_count = 0;
    std::uint32_t generation = 0;
    float released_energy = 0.0F;
};