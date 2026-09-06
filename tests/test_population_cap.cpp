#include "simulation.hpp"

#include <cassert>
#include <cstddef>

int main(){
    SimulationConfig config{};
    config.initial_neutrons = 20;
    config.collision_probability = 1.0F;
    config.fission_probability = 1.0F;
    config.min_spawned_neutrons = 3;
    config.max_population = 200;
    config.space_size = 100.0F;
    config.random_seed = 42;

    Simulation sim(config);

    constexpr float dt = 1.0F / 60.0F;
    constexpr int steps = 300;

    for(int i = 0; i < steps; i++){
        sim.update(dt);

        const SimulationStats& s = sim.stats();

        assert(s.population <= config.max_population);
        assert(s.peak_population <= config.max_population);
        assert(sim.neutrons().size() == s.population);

    }

    return 0;
}