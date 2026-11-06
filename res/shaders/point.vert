#version 460
layout (location = 0) out vec4 v_color;


struct instance_data { mat4 model; uint userIndex; };
layout(std430, binding = 3) readonly buffer b_instance_buffer
{
	instance_data data[];
} b_instances;

struct user_data { vec4 color; float pointSize; };
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

float get_user_point_size(uint userIndex)
{
    return uintBitsToFloat(b_user.data[userIndex + 4]);
}

void main()
{
    mat4 modelMatrix = b_instances.data[gl_InstanceIndex].model;
    uint userIndex = b_instances.data[gl_InstanceIndex].userIndex;

    v_color = get_user_color(userIndex);

    gl_PointSize = get_user_point_size(userIndex);
    gl_Position = modelMatrix * vec4(0.0, 0.0, 0.0, 1.0);
}