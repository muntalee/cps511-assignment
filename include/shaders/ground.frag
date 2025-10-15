#version 120
varying vec3 vNormalWS;
varying vec3 vPosWS;

uniform vec3 uLightPos;
uniform vec3 uViewPos;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
uniform Material uMat;

void main() {
  vec3 N = normalize(vNormalWS);
  vec3 L = normalize(uLightPos - vPosWS);
  vec3 V = normalize(uViewPos - vPosWS);
  vec3 H = normalize(L + V);

  float NdotL = max(dot(N, L), 0.0);
  float NdotH = max(dot(N, H), 0.0);

  vec3 ambient = uMat.ambient;
  vec3 diffuse = uMat.diffuse * NdotL;
  vec3 spec = (NdotL > 0.0) ? uMat.specular * pow(NdotH, uMat.shininess) : vec3(0.0);

  vec3 color = ambient + diffuse + spec;
  gl_FragColor = vec4(color, 1.0);
}
