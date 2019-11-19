varying vec3 eye_position;
varying vec3 eye_normal;

uniform bool phong; // Use phong shader?

void main(void) {
    vec3 color = vec3(1.0, 0.0, 0.0);
    gl_FragColor = vec4(color, 1.0);
}