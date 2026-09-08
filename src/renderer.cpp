#include "renderer.hpp"

#include "raylib.h"

#include <algorithm>
#include <cmath>
#include <numbers>

void draw_equation_banner(const FieldView& field){
    const char* equation_text = "n + U-235 -> U-236* -> Ba-141 + U-92 + 3n + Q";
    const int equation_font_size = 18;
    const int equation_width = MeasureText(equation_text, equation_font_size);
    const int equation_x = static_cast<int>(field.x + (field.size - static_cast<float>(equation_width)) * 0.5F);
    const int equation_y = static_cast<int>(field.y) - equation_font_size - 6;
    DrawText(equation_text, equation_x, equation_y, equation_font_size, WHITE);
}

void draw_arena_bounds(const FieldView& field){
    DrawRectangleLines(
        static_cast<int>(field.x),
        static_cast<int>(field.y),
        static_cast<int>(field.size),
        static_cast<int>(field.size),
        GRAY
    );
}

void draw_fissile_sites(
    const std::vector<Vec3>& sites,
    const FieldView& field,
    const SimulationConfig& config
){
    const float site_radius_world = config.u235_radius;
    const float site_radius_screen = site_radius_world * (field.size / config.space_size);

    for(const Vec3& site: sites){
        const Vector2 site_pos = world_to_screen(site, field, config.space_size);
        DrawCircleV(site_pos, site_radius_screen, Color{40, 90, 150, 90});
        DrawCircleLines(
            static_cast<int>(site_pos.x),
            static_cast<int>(site_pos.y),
            site_radius_screen,
            Color{100,180,255,230}
        );

        const int label_font_size = 12;
        const int label_width = MeasureText("U-235", label_font_size);
        DrawText(
            "U-235",
            static_cast<int>(site_pos.x - static_cast<float>(label_width) * 0.5F),
            static_cast<int>(site_pos.y - static_cast<float>(label_font_size) * 0.5F),
            label_font_size,
            SKYBLUE
        );
    }
}

void draw_fission_effects(const std::vector<FissionEffect>& effects){
    for(const FissionEffect& effect: effects){
        const float progress = effect.age / effect.duration;
        const float clamped_progress = std::clamp(progress, 0.0F, 1.0F);
        const unsigned char alpha = static_cast<unsigned char>(
            255.0F * (1.0F - clamped_progress)
        );

        //phase 1: absorption + nuclear excitation
        if(clamped_progress < 0.25F){
            const float local = clamped_progress / 0.25F;
            const float excitation_radius = 8.0F + 10.0F * local;

            DrawCircleV(
                effect.position,
                excitation_radius,
                Color{255, 240, 150, alpha}
            );

            DrawCircleLines(
                static_cast<int>(effect.position.x),
                static_cast<int>(effect.position.y),
                excitation_radius + 2.0F,
                Color{255,170,40, alpha}
            );
        }else{
            //phase 2: separation into 2 fragments + secondary neutrons
            const float local = (clamped_progress - 0.25F) / 0.75F;
            const float fragment_distance = 8.0F + 30.0F * local;
            const float fragment_radius = 9.0F - 2.5F * local;

            const Vector2 fragment_a{
                effect.position.x + effect.split_axis.x * fragment_distance,
                effect.position.y + effect.split_axis.y * fragment_distance
            };

            const Vector2 fragment_b{
                effect.position.x - effect.split_axis.x * fragment_distance,
                effect.position.y - effect.split_axis.y * fragment_distance
            };

            DrawCircleV(fragment_a, fragment_radius, Color{120, 175, 240, alpha});
            DrawCircleV(fragment_b, fragment_radius, Color{120, 175, 240, alpha});
            DrawCircleLines(static_cast<int>(fragment_a.x), static_cast<int>(fragment_a.y), fragment_radius, Color{20, 30, 45, alpha});
            DrawCircleLines(static_cast<int>(fragment_b.x), static_cast<int>(fragment_b.y), fragment_radius, Color{20, 30, 45, alpha});

            DrawCircleV(
                effect.position,
                6.0F * (1.0F - local),
                Color{255, 220, 120, alpha}
            );

            const std::uint32_t neutron_count = std::max<std::uint32_t>(2U, effect.emitted_neutrons);
            const float neutron_travel = 14.0F + 40.0F * local;
            const float base_angle = std::atan2(effect.split_axis.y, effect.split_axis.x);
            const float full_turn = 2.0F * std::numbers::pi_v<float>;

            for(std::uint32_t i = 0; i < neutron_count; ++i){
                const float t = (neutron_count == 1U)
                    ? 0.0F
                    : static_cast<float>(i) / static_cast<float>(neutron_count);
                const float emission_angle = base_angle + t * full_turn;
                const Vector2 dir{std::cos(emission_angle), std::sin(emission_angle)};

                const Vector2 neutron_pos{
                    effect.position.x + dir.x * neutron_travel,
                    effect.position.y + dir.y * neutron_travel
                };

                DrawLineEx(effect.position, neutron_pos, 1.5F, Color{130, 180, 255, static_cast<unsigned char>(alpha / 2)});
                DrawCircleV(neutron_pos, 3.0F, Color{130, 240, 255, alpha});
            }
        }
    }
}

void draw_neutrons(
    const std::vector<Neutron>& neutrons,
    const FieldView& field,
    const SimulationConfig& config,
    bool show_debug_labels,
    bool show_direction
){
    for(const Neutron& neutron: neutrons){
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
}

void draw_hud(
    bool paused,
    float simulation_speed,
    const SimulationStats& stats,
    bool show_direction,
    bool show_help
){
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
}
