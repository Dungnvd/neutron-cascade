#pragma once

#include <cstddef>
#include <cstdint>

struct SimulationStats{
    std::uint64_t collisions = 0;
    std::uint64_t fissions = 0;
    std::uint64_t escaped = 0;
    float released_energy = 0.0F;
    float tick_released_energy = 0.0F;

    std::size_t population = 0;
    std::size_t peak_population = 0;
};


