#include "simulation_logger.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <utility>

namespace{

std::string current_timestamp(){
    const auto now = std::chrono::system_clock::now();

    const std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};

#ifdef _WIN32
    localtime_s(&local_time, &current_time);
#else
    localtime_r(&current_time, &local_time);
#endif

    std::ostringstream timestamp;
    timestamp << std::put_time(&local_time, "%Y-%m-%d_%H-%M-%S");
    return timestamp.str();
}
}

SimulationLogger::SimulationLogger(
    const SimulationConfig&
){
    std::filesystem::create_directories("logs");

    const std::string file_path = "logs/run_" + current_timestamp() + ".CSV";

    output_file_.open(file_path);

    if(output_file_.is_open()){
        output_file_ << std::fixed << std::setprecision(3);
    }
}

bool SimulationLogger::is_open() const noexcept{
    return output_file_.is_open();
}

void SimulationLogger::write_header(){
    output_file_ 
        << "Tick,Population,Collisions,Fissions,"
        << "Escaped,Peak Population,Released Energy\n";
}

void SimulationLogger::write_tick(
    std::uint64_t tick,
    const SimulationStats& stats
) {
    output_file_
        << tick << ','
        << stats.population << ','
        << stats.collisions << ','
        << stats.fissions << ','
        << stats.escaped << ','
        << stats.peak_population << ','
        << stats.released_energy << '\n';
}