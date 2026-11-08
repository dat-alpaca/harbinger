#version 460 core
layout (location = 0) out vec4 out_color;

layout (location = 0) in vec4 v_color;
layout (location = 1) in vec4 v_normals;
layout (location = 2) in vec2 v_uvs0;
layout (location = 3) in flat uint v_user_index;

layout (binding = 6) uniform sampler2DArray u_textures[8];

struct user_data { vec4 colors; uint textureIndex; };
layout(std430, binding = 4) readonly buffer b_user_buffer
{
	uint data[];
} b_user;

struct texture_entry { vec2 uvScale; uint bin; uint layer; };
layout(std430, binding = 5) readonly buffer b_texture_table 
{
    texture_entry entries[];
} b_tables;

uint get_user_texture_index(uint userIndex)
{
    return uint(b_user.data[userIndex + 4]);
}

void main()
{
    texture_entry entry = b_tables.entries[get_user_texture_index(v_user_index)];

    out_color = texture(u_textures[uint(entry.bin)], vec3(v_uvs0, entry.layer));
}