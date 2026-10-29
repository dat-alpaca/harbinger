#version 460

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

void main()
{
    vec3 position = b_positions.positions[gl_VertexIndex].data;
    mat4 modelMatrix = b_instances.models[gl_InstanceIndex];

    gl_Position = modelMatrix * vec4(position, 1.0);
}