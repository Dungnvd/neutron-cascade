#include "simulation.hpp"

#include <cassert>
#include <cstddef>

int main(){
    SimulationConfig config{};
    config.initial_neutrons = 25;
    config.collision_probability = 0.0F;
    config.fission_probability = 1.0F;
    config.space_size = 1'000'000.0F;
    config.max_population = 1000;
    config.random_seed = 123ULL;

    Simulation sim(config);

    constexpr float dt = 1.0F / 60.0F;
    constexpr int steps = 500;

    for(int i = 0; i < steps; i++){
        sim.update(dt);

        const SimulationStats& stats = sim.stats();

        assert(stats.collisions == 0);
        assert(stats.fissions == 0);
        assert(stats.population == config.initial_neutrons);
        assert(sim.neutrons().size() == config.initial_neutrons);
    }

    return 0;
}