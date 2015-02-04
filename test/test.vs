attribute vec4 position;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix;

varying vec4 v_colorVarying;
varying vec2 v_texcoord;

void main()
{
	vec3 eyeNormal = normalize(normalMatrix * normal);
	vec3 lightPosition = vec3(0.0, 0.0, 1.0);
	vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);
	float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));
	v_colorVarying = diffuseColor * nDotVP;
	gl_Position = modelViewProjectionMatrix * position;
	v_texcoord = texcoord;
}
