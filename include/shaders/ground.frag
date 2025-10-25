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

void main()
{
  // lighting calculations
  vec3 norm = normalize(vNormalWS);
  vec3 lightDir = normalize(uLightPos - vPosWS);
  vec3 viewDir = normalize(uViewPos - vPosWS);
  vec3 reflectDir = reflect(-lightDir, norm);

  // Phong model
  vec3 ambient = uMat.ambient;
  vec3 diffuse = uMat.diffuse * max(dot(norm, lightDir), 0.0);
  vec3 specular = uMat.specular * pow(max(dot(viewDir, reflectDir), 0.0), uMat.shininess);

  vec3 color = ambient + diffuse + specular;
  gl_FragColor = vec4(color, 1.0);
  // mixed color 1
  // gl_FragColor = vec4(abs(sin(vPosWS.x)), abs(sin(vPosWS.z)), 0.5, 1.0);

  // mixed color 2
  // vec3 tint = vec3(abs(sin(vPosWS.x)), abs(sin(vPosWS.z)), 0.5);
  // vec3 finalColor = mix(color, tint, 0.4);
  // gl_FragColor = vec4(finalColor, 1.0);
}
