#version 120
attribute vec3 aPos;
attribute vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;

varying vec3 vNormalWS;
varying vec3 vPosWS;

void main() {
  vec4 posWS = uModel * vec4(aPos, 1.0);
  vPosWS = posWS.xyz;
  vNormalWS = normalize(uNormalMatrix * aNormal);
  gl_Position = uProj * uView * posWS;
}
