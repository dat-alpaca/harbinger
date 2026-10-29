#version 460
layout (location = 0) out vec4 v_color;

struct Position
{
    vec3 data;
    float padding;
};

layout(std430, binding = 0) readonly buffer b_position_buffer
{
	Position positions[];
} b_positions;

layout(std430, binding = 3) readonly buffer b_instance_buffer
{
	mat4 models[];
} b_instances;

layout(std430, binding = 4) readonly buffer b_user_buffer
{
	vec4 colors[];
} b_user;

void main()
{
    vec3 position = b_positions.positions[gl_VertexIndex].data;
    mat4 modelMatrix = b_instances.models[gl_InstanceIndex];

    gl_Position = modelMatrix * vec4(position, 1.0);

    v_color = b_user.colors[gl_InstanceIndex];
}