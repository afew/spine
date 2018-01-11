
// fragment shader.
varying   lowp    vec4    vr_dif;
varying   mediump vec2    vr_tx0;
uniform   lowp sampler2D  us_tx0;

void main() {
	lowp vec4	ret = vr_dif * texture2D(us_tx0, vr_tx0);
	gl_FragColor = ret;
}

