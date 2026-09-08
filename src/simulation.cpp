#include "simulation.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <numbers>

Simulation::Simulation(SimulationConfig config)
    : config_(config), random_engine_(config.random_seed){
                reset();
}

void Simulation::reset(){
    random_engine_.seed(config_.random_seed);

    stats_ = {};
    fission_events_.clear();
    neutrons_.clear();
    fissile_sites_.clear();

    neutrons_.reserve(config_.max_population);
    next_neutrons_.reserve(config_.max_population);
    fissile_sites_.reserve(256);

    initialize_fissile_sites();

    for(std::uint32_t i = 0; i < config_.initial_neutrons; i++){
        Neutron source{};
        source.position = {0.0F, config_.space_size * 0.3F, 0.0F};
        source.direction = {0.0, -1.0F, 0.0F};
        source.energy = 1.0F;
        source.generation = 0;
        source.state = NeutronState::Active;
        neutrons_.push_back(source);
    }

    stats_.population = neutrons_.size();
    stats_.peak_population = neutrons_.size();

}

void Simulation::update(float delta_time){
    constexpr std::uint32_t kEquationSpawnedNeutrons = 3U;
    constexpr float kEquationReleasedEnergyMeV = 200.0F;

    fission_events_.clear();
    next_neutrons_.clear();

    for(const Neutron& original_neutron : neutrons_){
        Neutron neutron = original_neutron;

        move_neutron(neutron, delta_time);

        if(!is_inside_boundary(neutron)){
            ++stats_.escaped;
            continue;
        }

        const float interaction_radius = config_.u235_radius;
        const float interaction_radius2 = interaction_radius * interaction_radius;

        std::size_t hit_site_index = fissile_sites_.size();
        for(std::size_t i = 0; i < fissile_sites_.size(); ++i){
            const float dx = neutron.position.x - fissile_sites_[i].x;
            const float dy = neutron.position.y - fissile_sites_[i].y;
            const float dz = neutron.position.z - fissile_sites_[i].z;
            const float distance2 = dx * dx + dy * dy + dz * dz;

            if(distance2 <= interaction_radius2){
                hit_site_index = i;
                break;
            }
        }

        if(hit_site_index == fissile_sites_.size()){
            if(next_neutrons_.size() < config_.max_population){
                next_neutrons_.push_back(neutron);
            }

            continue;
        }

        const float collision_value = random_float(0.0F,1.0F);

        if(collision_value >= config_.collision_probability){
            if(next_neutrons_.size() < config_.max_population){
                next_neutrons_.push_back(neutron);
            }
            continue;
        }

        ++stats_.collisions;
        const Vec3 reaction_site = fissile_sites_[hit_site_index];
        fissile_sites_.erase(fissile_sites_.begin() + static_cast<std::ptrdiff_t>(hit_site_index));

        const float fission_value = random_float(0.0F, 1.0F);

        if(fission_value >= config_.fission_probability){
            continue;
        }

        const std::uint32_t spawned_count = kEquationSpawnedNeutrons;

        ++stats_.fissions;
        stats_.released_energy += kEquationReleasedEnergyMeV;

        fission_events_.push_back({
            reaction_site,
            spawned_count,
            neutron.generation,
            kEquationReleasedEnergyMeV
        });

        for(std::uint32_t child = 0; child < spawned_count; ++child){
            if(next_neutrons_.size() >= config_.max_population){
                break;
            }
            next_neutrons_.push_back(
                create_neutron(
                    reaction_site,
                    neutron.generation + 1
                )
            );
        }
    }
    neutrons_.swap(next_neutrons_);

    stats_.population = neutrons_.size();
    stats_.peak_population = std::max(
        stats_.peak_population,
        stats_.population
    );
}

void Simulation::move_neutron(
    Neutron& neutron,
    float delta_time
){
    neutron.position.x += neutron.direction.x * config_.neutron_speed * delta_time;
    neutron.position.y += neutron.direction.y * config_.neutron_speed * delta_time;
    neutron.position.z += neutron.direction.z * config_.neutron_speed * delta_time;
}

bool Simulation::is_inside_boundary(
    const Neutron& neutron
) const{
    const float boundary = config_.space_size / 2.0F;

    return  neutron.position.x >= -boundary && neutron.position.x <= boundary &&
            neutron.position.y >= -boundary && neutron.position.y <= boundary &&
            neutron.position.z >= -boundary && neutron.position.z <= boundary;
}

Vec3 Simulation::random_direction(){
    const float angle = random_float(
        0.0F,
        2.0F * std::numbers::pi_v<float>
    );

    return {
        5 * std::cos(angle),
        5 * std::sin(angle),
        0.0F
    };
}

float Simulation::random_float(
    float minimum,
    float maximum
){
    std::uniform_real_distribution<float> distribution(
        minimum,
        maximum
    );

    return distribution(random_engine_);
}

Neutron Simulation::create_neutron(
    Vec3 position,
    std::uint32_t generation
){
    return {
        position,
        random_direction(),
        1.0F,
        generation,
        NeutronState::Active
    };
}

const std::vector<Neutron>& Simulation::neutrons() const noexcept{
    return neutrons_;
}

const std::vector<FissionEvent>& Simulation::fission_events() const noexcept{
    return fission_events_;
}

const SimulationStats& Simulation::stats() const noexcept{
    return stats_;
}

const std::vector<Vec3>& Simulation::fissile_sites() const noexcept{
    return fissile_sites_;
}

void Simulation::initialize_fissile_sites(){
    fissile_sites_.clear();
    const float radius = std::max(0.001F, config_.u235_radius);
    const float boundary = config_.space_size * 0.5F - radius;

    if(boundary <= 0.0F){
        fissile_sites_.push_back({0.0F, 0.0F, 0.0F});
        return;
    }

    // Cartesian grid, non-offset rows/columns: (0,0) always lands on a site.
    const float pitch = 2.0F * radius + config_.u235_gap;
    const int max_index = static_cast<int>(std::floor(boundary / pitch));

    for(int row = -max_index; row <= max_index; ++row){
        const float y = static_cast<float>(row) * pitch;

        for(int col = -max_index; col <= max_index; ++col){
            const float x = static_cast<float>(col) * pitch;

            fissile_sites_.push_back({x, y, 0.0F});
        }
    }
}

