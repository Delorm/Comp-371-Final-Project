#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coord;

out vec2 tex_coord;

uniform mat4 mvp_matrix;
uniform mat4 m_matrix;

void main()
{
    gl_Position =  mvp_matrix * vec4(position.x, position.y, position.z, 1.0);

    vec4 model_position = m_matrix * vec4(position.x, position.y, position.z, 1.0); 
    tex_coord = uv_coord;
}

