#pragma once

#include "simulation.hpp"
#include "fission_effect.hpp"

#include <cstdint>
#include <vector>

void handle_input(
    bool& paused, float& simulation_speed, bool& show_debug_labels,
    bool& show_direction, bool& show_help, Simulation& simulation,
    std::vector<FissionEffect>& fission_effects, std::uint64_t& tick
);
