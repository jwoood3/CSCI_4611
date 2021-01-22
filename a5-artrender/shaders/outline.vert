#version 330

// CSci-4611 Assignment 5:  Art Render

// TODO: You need to modify this vertex shader to move the edge vertex along
// the normal away from the mesh surface IF you determine that the vertex
// belongs to a silhouette edge.


uniform mat4 model_view_matrix;
uniform mat4 normal_matrix;
uniform mat4 proj_matrix;
uniform float thickness;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 left_normal;
layout(location = 3) in vec3 right_normal;

void main() {
  //Transform vertex into eye space
  vec3 veye = (model_view_matrix * vec4(vertex, 1)).xyz;

  vec3 v = vertex;

  //unit vector from the vertex to the eye point
  //Don't need vec3(0, 0, 0) but it helps to denote that e is in fact a vector rather than a point
  vec3 e = normalize(vec3(0, 0, 0) - veye);

  //left/right normals transformed into eye space
  vec3 nl = normalize((normal_matrix * vec4(left_normal, 1)).xyz);
  vec3 nr = normalize((normal_matrix * vec4(right_normal, 1)).xyz);

  //check if on edge, this works by checking if the dot products of nl/nr and e have different signs
  if (((dot(nl, e)) < 0.0) != ((dot(nr, e)) < 0.0)) {
    v += thickness * normal;
  }

  gl_Position = proj_matrix * model_view_matrix * vec4(v, 1);
}



