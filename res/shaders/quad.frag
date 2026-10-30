#version 460 core
layout (location = 0) out vec4 out_color;

layout (location = 0) in vec4 v_color;
layout (location = 1) in vec4 v_normals;
layout (location = 2) in vec2 v_uvs0;

// Textures:
layout (binding = 7) uniform sampler2D u_textures[16];

layout(std430, binding = 6) readonly buffer b_user_data
{
    uint textureSlots[];
} b_users;

void main()
{
    out_color = texture(u_textures[uint(v_color.w)], v_uvs0);
}