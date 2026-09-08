#pragma once

#include "simulation.hpp"
#include "simulation_config.hpp"
#include "simulation_stats.hpp"
#include "fission_effect.hpp"
#include "render_types.hpp"

#include <vector>

void draw_equation_banner(const FieldView& field);
void draw_arena_bounds(const FieldView& field);

void draw_fissile_sites(
    const std::vector<Vec3>& sites,
    const FieldView& field,
    const SimulationConfig& config
);

void draw_fission_effects(const std::vector<FissionEffect>& effects);

void draw_neutrons(
    const std::vector<Neutron>& neutrons,
    const FieldView& field,
    const SimulationConfig& config,
    bool show_debug_labels,
    bool show_direction
);

void draw_hud(
    bool paused,
    float simulation_speed,
    const SimulationStats& stats,
    bool show_direction,
    bool show_help
);
