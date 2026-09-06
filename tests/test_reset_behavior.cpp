#include "simulation.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>

int main(){
    SimulationConfig config{};
    config.initial_neutrons = 20;
    config.collision_probability = 0.25F;
    config.fission_probability = 0.6F;
    config.space_size = 100.0F;
    config.max_population = 1000;
    config.random_seed = 2026ULL;

    Simulation sim(config);

    constexpr float dt = 1.0F / 60.0F;
    constexpr int warmup_steps = 120;
    constexpr int check_steps = 80;
    constexpr float epsilon = 1e-6F;

    for(int i = 0; i < warmup_steps; i++){
        sim.update(dt);
    }

    const SimulationStats changed_stats = sim.stats();
    assert(
        changed_stats.collisions > 0 ||
        changed_stats.fissions > 0 ||
        changed_stats.escaped > 0
    );

    sim.reset();

    const SimulationStats& after_reset = sim.stats();
    assert(after_reset.collisions == 0);
    assert(after_reset.fissions == 0);
    assert(after_reset.escaped == 0);
    assert(std::fabs(after_reset.released_energy) < epsilon);
    assert(after_reset.population == config.initial_neutrons);
    assert(after_reset.peak_population == config.initial_neutrons);
    assert(sim.neutrons().size() == config.initial_neutrons);

    Simulation baseline(config);

    for(int i = 0; i < check_steps; i++){
        sim.update(dt);
        baseline.update(dt);
    }

    const SimulationStats& a = sim.stats();
    const SimulationStats& b = baseline.stats();

    assert(a.collisions == b.collisions);
    assert(a.fissions == b.fissions);
    assert(a.escaped == b.escaped);
    assert(a.population == b.population);
    assert(a.peak_population == b.peak_population);
    assert(std::fabs(a.released_energy - b.released_energy) < epsilon);

    return 0;
}