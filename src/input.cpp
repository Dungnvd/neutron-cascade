#include "input.hpp"

#include "raylib.h"

#include <algorithm>

void handle_input(
    bool& paused, float& simulation_speed, bool& show_debug_labels,
    bool& show_direction, bool& show_help, Simulation& simulation,
    std::vector<FissionEffect>& fission_effects, std::uint64_t& tick
){
    if(IsKeyPressed(KEY_SPACE)){
        paused = !paused;
    }

    if(IsKeyPressed(KEY_R)){
        simulation.reset();
        fission_effects.clear();
        paused = false;
        tick = 0;
    }

    if(IsKeyPressed(KEY_UP)){
        simulation_speed += 0.25F;
    }

    if(IsKeyPressed(KEY_DOWN)){
        simulation_speed -= 0.25F;
    }

    if(IsKeyPressed(KEY_G)){
        show_debug_labels = !show_debug_labels;
    }

    if(IsKeyPressed(KEY_D)){
        show_direction = !show_direction;
    }

    if(IsKeyPressed(KEY_H)){
        show_help = !show_help;
    }

    simulation_speed = std::clamp(simulation_speed, 0.25F, 5.0F);
}
