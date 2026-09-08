#include "fission_effect.hpp"

#include <algorithm>
#include <cmath>

void spawn_fission_effects(
    std::vector<FissionEffect>& effects,
    const std::vector<FissionEvent>& events,
    const FieldView& field,
    float space_size
){
    for(const FissionEvent& event: events){
        const Vector2 screen_pos = world_to_screen(event.position, field, space_size);

        const float angle = static_cast<float>((event.generation * 53U) % 360U) * DEG2RAD;
        const Vector2 split_axis{std::cos(angle), std::sin(angle)};

        effects.push_back({
            screen_pos,
            0.0F,
            0.8F,
            split_axis,
            std::max<std::uint32_t>(2U, event.spawned_count)
        });
    }
}

void advance_fission_effects(std::vector<FissionEffect>& effects, float delta_time){
    for(FissionEffect& effect: effects){
        effect.age += delta_time;
    }
}

void remove_expired_fission_effects(std::vector<FissionEffect>& effects){
    effects.erase(
        std::remove_if(
            effects.begin(),
            effects.end(),
            [](const FissionEffect& effect){
                return effect.age >= effect.duration;
            }
        ),
        effects.end()
    );
}
