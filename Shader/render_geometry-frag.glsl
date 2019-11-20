#version 100
varying mediump vec3 eye_position;      // Precision must be specified for webgl.
varying mediump vec3 eye_normal;

uniform bool phong; // Use phong shader?

void main(void) {
    lowp vec3 color = vec3(1.0, 0.0, 0.0);
    gl_FragColor = vec4(color, 1.0);
}