#version 460

struct user_data
{
    vec3 color;
    float pointSize;
};

layout(std430, binding = 3) readonly buffer b_instance_buffer
{
	mat4 models[];
} b_instances;

layout(std430, binding = 4) readonly buffer b_user_buffer
{
	user_data data[];
} b_user;

layout (location = 0) out vec3 v_color;

void main()
{
    mat4 modelMatrix = b_instances.models[gl_InstanceIndex];

    v_color = b_user.data[gl_InstanceIndex].color;

    gl_PointSize = b_user.data[gl_InstanceIndex].pointSize;
    gl_Position = modelMatrix * vec4(0.0, 0.0, 0.0, 1.0);
}