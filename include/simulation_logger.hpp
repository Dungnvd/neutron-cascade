#pragma once

#include "simulation_config.hpp"
#include "simulation_stats.hpp"

#include <cstdint>
#include <fstream>
#include <string>

class SimulationLogger{
public:
    explicit SimulationLogger(const SimulationConfig& config);

    bool is_open() const noexcept;

    void write_header();
    void write_tick(
        std::uint64_t tick,
        const SimulationStats& stats
    );

private:
    std::ofstream output_file_;
};