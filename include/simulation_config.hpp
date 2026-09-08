#pragma once

#include <cstddef>
#include <cstdint>

struct SimulationConfig{
    std::uint32_t initial_neutrons = 1;
    float neutron_speed = 3.0F;
    float space_size = 10.0F;
    float u235_radius = 4.0F;
    float u235_gap = 1.0F;
    float collision_probability = 0.5F;
    float fission_probability = 1.0F;

    std::size_t max_population = 1000;
    std::uint64_t random_seed = 42;
};