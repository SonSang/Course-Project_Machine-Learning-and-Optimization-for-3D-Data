#version 100                // It is needed to use precision qualifiers.
attribute vec4 position;
attribute vec4 normal;      // If we do not use these variables, compiler can throw them away!

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// Position and Normal in View space.
varying mediump vec3 eye_position;
varying mediump vec3 eye_normal;

void main(void)
{
    mat4 MV = view * model;
    vec4 eye_pos4 = MV * position;
    eye_position = eye_pos4.xyz;
    vec4 tmp_normal = vec4(normal.xyz, 0.0);        // Normal vector is not affected by translation.
    eye_normal = normalize(MV * tmp_normal).xyz;

    vec4 result = projection * eye_pos4;
    gl_Position = result;
}