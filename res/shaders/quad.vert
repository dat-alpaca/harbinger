#version 460

layout(std430, binding = 0) readonly buffer b_position_buffer
{
	vec4 positions[];
} b_positions;

layout(std430, binding = 3) readonly buffer b_instance_buffer
{
	mat4 models[];
} b_instances;

void main()
{
    vec4 position = b_positions.positions[gl_VertexIndex];
    mat4 modelMatrix = b_instances.models[gl_InstanceIndex];

    gl_Position = modelMatrix * position;
}