#version 100

// Point light structure.
struct light_point {
    mediump vec3 position;
    lowp vec3 ambient;
    lowp vec3 diffuse;
    lowp vec3 specular;
};

// Material structure.
struct material {
    lowp vec3 emmision;
    lowp vec3 ambient;
    lowp vec3 diffuse;
    lowp vec3 specular;
    lowp float shininess;
};

varying mediump vec3 eye_position;      // Precision must be specified for webgl.
varying mediump vec3 eye_normal;

uniform bool phong;         // Use phong shader?
uniform lowp vec3 icolor;   // Face color
uniform lowp vec3 ocolor;   // Bdry color
uniform light_point ML;     // Main Light
uniform material    MM;     // Main Material

lowp vec3 apply_light_point();

void main(void) {
    lowp vec3 color;

    if(phong) {
        color = apply_light_point();
    }
    else {
        color = MM.diffuse;
    }

    gl_FragColor = vec4(color, 0.5);
}

lowp vec3 apply_light_point() {
    // Ambient.
    lowp vec3 ambient = vec3(0, 0, 0);
    if(gl_FrontFacing)
        ambient = ML.ambient.rgb * MM.ambient.rgb;

    // Diffuse.
    mediump vec3 normal = normalize(eye_normal);
    if(!gl_FrontFacing)
        normal = normal * -1.0;
    mediump vec3 lightdirraw = ML.position.xyz - eye_position;
    mediump vec3 lightdir = normalize(lightdirraw);
    mediump float diffuse_factor = max(dot(normal, lightdir), 0.0);
    lowp vec3 diffuse = ML.diffuse.rgb * (diffuse_factor * MM.diffuse.rgb);

    // Specular.
    mediump vec3 viewdir = normalize(-eye_position);    // In eye space, camera is at (0, 0, 0);
    mediump vec3 reflectdir = reflect(-lightdir, normal);
    mediump float specular_factor = pow(max(dot(viewdir, reflectdir), 0.0), MM.shininess);
    lowp vec3 specular = ML.specular.rgb * (specular_factor * MM.specular.rgb);

    return ambient + diffuse + specular;
}