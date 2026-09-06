#include "simulation.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct BenchmarkScenario{
    std::string name;
    SimulationConfig config;
    std::uint64_t steps = 1000;
    float dt = 1.0F / 60.0F;
};

static void run_scenario(const BenchmarkScenario& scenario){
    Simulation sim(scenario.config);

    //warmup to redce noise cache/branch predictor
    for(std::uint64_t i = 0; i < 100; i++){
        sim.update(scenario.dt);
    }

    const auto start = std::chrono::steady_clock::now();
    for(std::uint64_t i = 0; i < scenario.steps; i++){
        sim.update(scenario.dt);
    }

    const auto end = std::chrono::steady_clock::now();

    const auto elapsed_ns = 
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    const double elapsed_s = static_cast<double>(elapsed_ns) / 1e9;
    const double us_per_step = static_cast<double>(elapsed_ns) / 1000.0 / static_cast<double>(scenario.steps);
    const double steps_per_sec = static_cast<double>(scenario.steps) / elapsed_s;

    const SimulationStats& stats = sim.stats();

    std::cout << "Scenario: " << scenario.name << "\n";
    std::cout << "  steps: " << scenario.steps << "\n";
    std::cout << "  elapsed(s): " << std::fixed << std::setprecision(6) << elapsed_s << "\n";
    std::cout << "  us/step: " << std::fixed << std::setprecision(3) << us_per_step << "\n";
    std::cout << "  steps/s: " << std::fixed << std::setprecision(1) << steps_per_sec << "\n";
    std::cout << "  final population: " << stats.population << "\n";
    std::cout << "  peak population: " << stats.peak_population << "\n";
    std::cout << "  total fissions: " << stats.fissions << "\n";
    std::cout << "\n";
    
}

int main(){
    std::vector<BenchmarkScenario> scenarios;

    {
        SimulationConfig c{};
        c.initial_neutrons = 20;
        c.collision_probability = 0.01F;
        c.fission_probability = 1.0F;
        c.space_size = 100.0F;
        c.max_population = 1000;
        c.random_seed = 42ULL;
        scenarios.push_back({"baseline_demo", c, 5000, 1.0F / 60.0F});
    }

    {
        SimulationConfig c{};
        c.initial_neutrons = 100;
        c.collision_probability = 0.20F;
        c.fission_probability = 0.8F;
        c.space_size = 200.0F;
        c.max_population = 10'000;
        c.random_seed = 42ULL;
        scenarios.push_back({"stress_medium", c, 5000, 1.0F / 60.0F});
    }

    {
        SimulationConfig c{};
        c.initial_neutrons = 500;
        c.collision_probability = 0.40F;
        c.fission_probability = 0.9F;
        c.space_size = 400.0F;
        c.max_population = 50'000;
        c.random_seed = 42ULL;
        scenarios.push_back({"stress_high", c, 2000, 1.0F / 60.0F});
    }

    std::cout << "Neutron Cascade Benchmark\n";
    std::cout << "===========================\n\n";

    for(const auto& scenario: scenarios){
        run_scenario(scenario);
    }

    return 0;
}