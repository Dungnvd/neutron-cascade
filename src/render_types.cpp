#include "render_types.hpp"

Vector2 world_to_screen(
    const Vec3& position,
    const FieldView& field,
    float space_size
){
    const float scale = field.size / space_size;
    const float screen_x = field.x + field.size / 2.0F + position.x * scale;
    const float screen_y = field.y + field.size / 2.0F - position.y * scale;

    return {screen_x, screen_y};
}
