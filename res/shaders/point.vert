#version 460 core

layout(std430, binding = 0) readonly buffer b_instance_buffer
{
	mat4 models[];
} b_instances;

void main()
{
    mat4 modelMatrix = b_instances.models[gl_InstanceIndex];

    gl_PointSize = 30.0;
    gl_Position = modelMatrix * vec4(0.0, 0.0, 0.0, 1.0);
}