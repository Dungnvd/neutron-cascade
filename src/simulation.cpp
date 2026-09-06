#include "simulation.hpp"

#include <algorithm>
#include <cmath>
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

    neutrons_.reserve(config_.max_population);
    next_neutrons_.reserve(config_.max_population);

    for(std::uint32_t index = 0; index < config_.initial_neutrons; ++index){
        neutrons_.push_back(
            create_neutron({0.0F, 0.0F, 0.0F}, 0)
        );
    }

    stats_.population = neutrons_.size();
    stats_.peak_population = neutrons_.size();

}

void Simulation::update(float delta_time){
    fission_events_.clear();
    next_neutrons_.clear();

    for(const Neutron& original_neutron : neutrons_){
        Neutron neutron = original_neutron;

        move_neutron(neutron, delta_time);

        if(!is_inside_boundary(neutron)){
            ++stats_.escaped;
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

        const float fission_value = random_float(0.0F, 1.0F);

        if(fission_value >= config_.fission_probability){
            continue;
        }

        const auto spawned_count = static_cast<std::uint32_t>(
            random_float(
                static_cast<float>(config_.min_spawned_neutrons),
                static_cast<float>(config_.max_spawned_neutrons) + 0.999F
            )
        );

        ++stats_.fissions;
        stats_.released_energy += neutron.energy;

        fission_events_.push_back({
            neutron.position,
            spawned_count,
            neutron.generation,
            neutron.energy
        });

        for(std::uint32_t child = 0; child < spawned_count; ++child){
            if(next_neutrons_.size() >= config_.max_population){
                break;
            }
            next_neutrons_.push_back(
                create_neutron(
                    neutron.position,
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
        std::cos(angle),
        std::sin(angle),
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

