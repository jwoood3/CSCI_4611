#version 330

// CSci-4611 Assignment 5:  Art Render

// TODO: You need to calculate per-fragment shading here using a toon shading model

in vec3 position_in_eye_space;
in vec3 normal_in_eye_space;

out vec4 color;

uniform vec3 light_in_eye_space;
uniform vec4 Ia, Id, Is;

uniform vec4 ka, kd, ks;
uniform float s;

uniform sampler2D diffuse_ramp;
uniform sampler2D specular_ramp;


void main() {

  //unit vector from the vertex to the light (light_in_eye_space and position_in_eye_space are both points!!!)
  vec3 l = normalize(light_in_eye_space - position_in_eye_space);

  //normal vector
  vec3 n = normalize(normal_in_eye_space);

  //unit vector from the vertex to the eye point, which is at 0,0,0 in "eye space"
  //Don't need vec3(0, 0, 0) but it helps to denote that e is in fact a vector rather than a point
  vec3 e = normalize(vec3(0, 0, 0) - position_in_eye_space);

  //halfway vector
  vec3 h = normalize(l + e);

  //lighting output for color
  vec4 ambient = ka * Ia;
  vec4 diffuse = kd * Id * texture2D(diffuse_ramp, vec2(max(0, 0.5 * dot(n, l) + 0.5), 0));
  vec4 specular = ks * Is * texture2D(specular_ramp, vec2(pow(max(0, dot(h, n)), s), 0));

  //calculate color based on lighting equations
  color = ambient + diffuse + specular;
}
