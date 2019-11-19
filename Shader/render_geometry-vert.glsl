attribute vec4 position;
attribute vec4 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// Position and Normal in View space.
varying vec3 eye_position;
varying vec3 eye_normal;

void main(void)
{
    mat4 MV = view * model;
    vec4 eye_pos4 = MV * position;
    eye_position = eye_pos4.xyz;
    eye_normal = normalize(MV * normal).xyz;
    gl_Position = projection * eye_pos4;
}