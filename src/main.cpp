#include "simulation.hpp"
#include "simulation_logger.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

#include "raylib.h"

struct FissionEffect{
    Vector2 position;
    float age = 0.0F;
    float duration = 0.6F;
};

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

struct FieldView{
    const float size = 600.0F;
    const float x = 300.0F;
    const float y = 80.0F;
};

Vector2 world_to_screen(
    const Vec3& position,
    const FieldView& field,
    float space_size
){
    const float scale = field.size / space_size;
    const float screen_x = field.x + field.size / 2.0F + position.x * scale;
    const float screen_y = field.y + field.size / 2.0F - position.y * scale;
    
    return {screen_x, screen_y};
}

int main(){
    const int screen_width = 1000;
    const int screen_height = 700;

    InitWindow(screen_width, screen_height, "Neutron Cascade");
    SetTargetFPS(60);
    
    SimulationConfig config;
    config.initial_neutrons = 20;
    config.collision_probability = 0.01F;
    config.fission_probability = 1.0F;
    config.space_size = 100.0F;
    
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
        }
        if(!paused){
            for(const FissionEvent& event: simulation.fission_events()){
                const Vector2 screen_pos = world_to_screen(
                    event.position,
                    field,
                    config.space_size
                );
                fission_effects.push_back({
                    screen_pos.x, screen_pos.y, 0.0F, 0.6F
                });
            }
        }

        if(!paused){
            for(FissionEffect& effect: fission_effects){
                effect.age += delta_time;
            }
        }

        fission_effects.erase(
            std::remove_if(
                fission_effects.begin(),
                fission_effects.end(),
                [](const FissionEffect& effect){
                    return effect.age >= effect.duration;
                }
            ),
            fission_effects.end()
        );

        BeginDrawing();

        ClearBackground(Color{10, 15, 25, 255});
        DrawText("Neutron Cascade", 30, 25, 30, WHITE);

        DrawText(
            paused ? "PAUSED" : "RUNNING",
            30, 60, 20,
            paused ? RED : GREEN
        );

        DrawText(
            TextFormat("Speed: x%.2f", simulation_speed),
            30, 90, 20,
            WHITE
        );

        DrawRectangleLines(
            static_cast<int>(field.x),
            static_cast<int>(field.y),
            static_cast<int>(field.size),
            static_cast<int>(field.size),
            GRAY
        );

        for(const FissionEffect& effect: fission_effects){
            const float progress = effect.age / effect.duration;
            const float radius = 10.0F + progress * 35.0F;
            const unsigned char alpha = static_cast<unsigned char>(
                255.0F * (1.0F - progress)
            );

            DrawCircleLines(
                static_cast<int>(effect.position.x),
                static_cast<int>(effect.position.y),
                radius,
                Color{255,170,40, alpha}
            );

            DrawCircleV(
                effect.position,
                5.0F * (1.0F - progress),
                Color{255, 240, 150, alpha}
            );
        }

        for(const Neutron& neutron: simulation.neutrons()){
            const Vector2 screen_pos = world_to_screen(
                neutron.position,
                field,
                config.space_size
            );
            const float screen_x = screen_pos.x;
            const float screen_y = screen_pos.y;
            const float direction_length = 12.0F;

            Color neutron_color = Color{80, 220, 255, 255};

            if(show_debug_labels){
                switch(neutron.generation % 4){
                    case 0:
                        neutron_color = SKYBLUE;
                        break;
                    case 1:
                        neutron_color = GREEN;
                        break;
                    case 2:
                        neutron_color = GOLD;
                        break;
                    default:
                        neutron_color = PINK;
                        break;
                }
            }

            if(show_direction){
                DrawLineEx(
                    {screen_x, screen_y},
                    {
                        screen_x + neutron.direction.x * direction_length,
                        screen_y - neutron.direction.y * direction_length
                    },
                        2.0F,
                        neutron_color
                );
            }

            DrawCircle(
                static_cast<int>(screen_x), 
                static_cast<int>(screen_y), 
                3.0F, 
                neutron_color
            );

            if(show_debug_labels){
                DrawText(
                    TextFormat(
                        "Generation: %u", neutron.generation
                    ),
                    static_cast<int>(screen_x + 6.0F),
                    static_cast<int>(screen_y - 8.0F),
                    10,
                    neutron_color
                );
            }
        }

        const SimulationStats& stats = simulation.stats();

        logger.write_tick(tick, stats);
        tick++;

        DrawText(TextFormat("Population: %llu", static_cast<unsigned long long>(stats.population)), 30, 120, 20, WHITE);
        DrawText(TextFormat("Fissions: %llu", stats.fissions), 30, 140, 20, ORANGE);
        DrawText(TextFormat("Direction[D]: %s", show_direction ? "ON" : "OFF"), 30, 160, 20, WHITE);
        DrawText("Help[H]", 30, 180, 20, WHITE);
        if(show_help){
            DrawText("SPACE: Pause/Resume", 30, 200, 20, WHITE);
            DrawText("R: Reset", 30, 220, 20, WHITE);
            DrawText("UP/DOWN: Speed +/-", 30, 240, 20, WHITE);
            DrawText("D: Toggle Direction", 30, 260, 20, WHITE);
            DrawText("G: Toggle Debug Labels", 30, 280, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;

}