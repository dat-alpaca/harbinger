#version 460
layout (location = 0) out vec4 v_color;
layout (location = 1) out vec4 v_normals;
layout (location = 2) out vec2 v_uvs0;
layout (location = 3) out uint v_user_index;

layout(std430, binding = 0) readonly buffer b_position_buffer
{
	vec4 positions[];
} b_positions;

layout(std430, binding = 1) readonly buffer b_normal_buffer
{
	vec4 normals[];
} b_normals;

layout(std430, binding = 2) readonly buffer b_uvs0_buffer
{
	vec2 uvs0[];
} b_uvs0;

struct instance_data { mat4 model; uint userIndex; uint padd[3]; };
layout(std430, binding = 3) readonly buffer b_instance_buffer
{
	instance_data data[];
} b_instances;

struct user_data { vec4 colors; uint textureIndex; };
layout(std430, binding = 4) readonly buffer b_user_buffer
{
	uint data[];
} b_user;

vec4 get_user_color(uint userIndex)
{
    float r = uintBitsToFloat(b_user.data[userIndex + 0]);
    float g = uintBitsToFloat(b_user.data[userIndex + 1]);
    float b = uintBitsToFloat(b_user.data[userIndex + 2]);
    float a = uintBitsToFloat(b_user.data[userIndex + 3]);

    return vec4(r, g, b, a);
}
 
void main()
{
    vec4 position = b_positions.positions[gl_VertexIndex];
    vec4 normals  = b_normals.normals[gl_VertexIndex];
    vec2 uvs0  = b_uvs0.uvs0[gl_VertexIndex];

    uint userIndex = b_instances.data[gl_InstanceIndex].userIndex;
    mat4 modelMatrix = b_instances.data[gl_InstanceIndex].model;
    gl_Position = modelMatrix * vec4(position.xyz, 1.0);

    v_color = get_user_color(userIndex);
    v_normals = normals;
    v_uvs0 = uvs0;
    v_user_index = userIndex;
}