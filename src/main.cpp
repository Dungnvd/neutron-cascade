#include "simulation.hpp"
#include "simulation_logger.hpp"
#include "render_types.hpp"
#include "fission_effect.hpp"
#include "renderer.hpp"
#include "input.hpp"

#include <vector>

#include "raylib.h"

int main(){
    const int screen_width = 1000;
    const int screen_height = 700;

    InitWindow(screen_width, screen_height, "Neutron Cascade");
    SetTargetFPS(60);
    
    SimulationConfig config;
    config.initial_neutrons = 1;
    config.collision_probability = 1.0F;
    config.fission_probability = 1.0F;
    config.space_size = 100.0F;
    config.max_population = 2500;
    
    Simulation simulation(config);

    SimulationLogger logger(config);
    if(!logger.is_open()){
        CloseWindow();
        return 1;
    }
    logger.write_header();

    std::vector<FissionEffect> fission_effects;

    bool paused = false;
    float simulation_speed = 1.0F;
    bool show_debug_labels = false;
    std::uint64_t tick = 0;

    bool show_direction = false;
    bool show_help = false;

    while(!WindowShouldClose()){
        const float delta_time = GetFrameTime();
        handle_input(
            paused,simulation_speed,show_debug_labels,
            show_direction,show_help,simulation,fission_effects,tick
        );
        const FieldView field{};

        if(!paused){
            simulation.update(delta_time * simulation_speed);
            spawn_fission_effects(fission_effects, simulation.fission_events(), field, config.space_size);
            advance_fission_effects(fission_effects, delta_time);
        }

        remove_expired_fission_effects(fission_effects);

        BeginDrawing();
        ClearBackground(Color{10, 15, 25, 255});

        draw_equation_banner(field);
        draw_arena_bounds(field);
        draw_fissile_sites(simulation.fissile_sites(), field, config);
        draw_fission_effects(fission_effects);
        draw_neutrons(simulation.neutrons(), field, config, show_debug_labels, show_direction);

        const SimulationStats& stats = simulation.stats();
        logger.write_tick(tick, stats);
        tick++;

        draw_hud(paused, simulation_speed, stats, show_direction, show_help);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
