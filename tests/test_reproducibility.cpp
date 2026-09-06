#include "simulation.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>

int main(){
    SimulationConfig config{};
    config.initial_neutrons = 20;
    config.collision_probability = 0.20F;
    config.fission_probability = 0.60F;
    config.space_size = 100.0F;
    config.max_population = 1000;
    config.random_seed = 123456789ULL;

    Simulation sim_a(config);
    Simulation sim_b(config);

    constexpr float dt = 1.0F / 60.0F;
    constexpr int steps = 300;
    constexpr float espilon = 1e-6F;

    for(int i = 0; i < steps; i++){
        sim_a.update(dt);
        sim_b.update(dt);
    }

    const SimulationStats& a = sim_a.stats();
    const SimulationStats& b = sim_b.stats();

    assert(a.collisions == b.collisions);
    assert(a.fissions == b.fissions);
    assert(a.escaped == b.escaped);
    assert(a.population == b.population);
    assert(a.peak_population == b.peak_population);
    assert(std::fabs(a.released_energy - b.released_energy) < 1e-6F);

    const auto& na = sim_a.neutrons();
    const auto& nb = sim_b.neutrons();

    for(std::size_t i = 0; i < na.size(); i++){
        assert(std::fabs(na[i].position.x - nb[i].position.x) < espilon);
        assert(std::fabs(na[i].position.y - nb[i].position.y) < espilon);
        assert(std::fabs(na[i].position.z - nb[i].position.z) < espilon);
        assert(na[i].generation == nb[i].generation);
    }

    return 0;
}