#version 460 core
layout (location = 0) out vec4 out_color;

layout (location = 0) in vec4 v_color;
layout (location = 1) in vec4 v_normals;
layout (location = 2) in vec2 v_uvs0;

void main()
{
    out_color = vec4(v_uvs0.xy, 1.0, 1.0);
}