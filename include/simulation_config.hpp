#pragma once

#include <cstddef>
#include <cstdint>

struct SimulationConfig{
    std::uint32_t initial_neutrons = 2;
    float neutron_speed = 3.0F;
    float space_size = 10.0F;
    float collision_probability = 0.5F;
    float fission_probability = 1.0F;

    int min_spawned_neutrons = 2;
    int max_spawned_neutrons = 3;

    std::size_t max_population = 1000;
    std::uint64_t random_seed = 42;
};