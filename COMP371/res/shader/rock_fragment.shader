#version 330 core

in vec2 tex_coord;
in vec3 light_dir;
in vec3 view_vector;

out vec4 color;
uniform sampler2D ourTexture1;
uniform sampler2D nor_map;

in float visibility;
uniform vec3 skyColor;

void main()
{
    vec3 normal = texture(nor_map, tex_coord).xyz;
    vec4 myTex = texture(ourTexture1, tex_coord);

    if (myTex.a < 0.5) discard;

    vec4 object_color = myTex;

    float color_strength = 0.0f;

    float dim = 1; 
    float dot_prod = dot(-light_dir, vec3(0, 1, 0));
    if (dot_prod < 0) {
	dim += dot_prod;
    }
    // Texture is too bright
    dim -= 0.2;


    // Ambience
    float ka = 0.2;
    color_strength += ka;

    // Diffuse
    float strength = max(dot(-light_dir, normal), 0);
    float kd = 0.6f;
    color_strength +=  dim * kd * strength;

    // Specular
    float a = 1;
    float ks = 0.4;
    vec3 reflection_vector = reflect(-light_dir, normal);
    float prod = max(dot(view_vector, reflection_vector) , 0);
    strength = pow(prod, a);
    color_strength += dim * ks * strength;

    color = color_strength * object_color;
    color = clamp(color, 0.0f, 1.0f);

    color = mix(vec4(skyColor, 1.0f), color, visibility);
 

}
