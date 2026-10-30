#version 460
layout (location = 0) out vec4 v_color;
layout (location = 1) out vec4 v_normals;
layout (location = 2) out vec2 v_uvs0;

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
    vec4 position = b_positions.positions[gl_VertexIndex];
    vec4 normals  = b_normals.normals[gl_VertexIndex];
    vec2 uvs0  = b_uvs0.uvs0[gl_VertexIndex];

    mat4 modelMatrix = b_instances.models[gl_InstanceIndex];

    gl_Position = modelMatrix * vec4(position.xyz, 1.0);

    v_color = b_user.colors[gl_InstanceIndex];
    v_normals = normals;
    v_uvs0 = uvs0;
}