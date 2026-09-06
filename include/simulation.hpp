#pragma once

#include "neutron.hpp"
#include "simulation_config.hpp"
#include "simulation_stats.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <random>

class Simulation{
public:
    explicit Simulation(SimulationConfig config = {});

    void update(float delta_time);
    void reset();

    const std::vector<Neutron>& neutrons() const noexcept;
    const std::vector<FissionEvent>& fission_events() const noexcept;
    const SimulationStats& stats() const noexcept;

private:
    void move_neutron(Neutron& neutron, float delta_time);
    bool is_inside_boundary(const Neutron& neutron) const;
    Vec3 random_direction();
    float random_float(float minimum, float maximum);
    Neutron create_neutron(Vec3 position, std::uint32_t generation);

    SimulationConfig config_;
    SimulationStats stats_;

    std::vector<Neutron> neutrons_;
    std::vector<Neutron> next_neutrons_;
    std::vector<FissionEvent> fission_events_;

    std::mt19937_64 random_engine_;
};
