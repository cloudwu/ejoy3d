varying vec2 v_texcoord;
varying vec4 v_colorVarying;
uniform sampler2D texture0;

void main() {
	vec4 tmp = texture2D(texture0, v_texcoord);
	gl_FragColor = tmp * (0.2 + v_colorVarying * 0.8);
}
