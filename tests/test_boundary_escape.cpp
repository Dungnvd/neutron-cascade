#include "simulation.hpp"

#include <cassert>

int main(){
    SimulationConfig config{};
    config.initial_neutrons = 20;
    config.neutron_speed = 3.0F;
    config.space_size = 10.0F;
    config.collision_probability = 0.0F;
    config.fission_probability = 0.0F;
    config.max_population = 1000;
    config.random_seed = 42;

    Simulation sim(config);

    constexpr float dt = 0.5F;
    constexpr int steps = 10;

    for(int i = 0; i < steps; i++){
        sim.update(dt);
    }

    const SimulationStats& stats = sim.stats();

    assert(stats.population == 0);
    assert(sim.neutrons().empty());
    assert(stats.fissions == 0);
    assert(stats.collisions == 0);
    assert(stats.escaped == config.initial_neutrons);

    return 0;
}
