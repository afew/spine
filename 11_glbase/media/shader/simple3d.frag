
varying   mediump  vec3 vr_nor;
varying   lowp     vec4 vr_dif;
varying   mediump  vec2 vr_tx0;

uniform   lowp     vec4 uc_dif;

uniform   lowp sampler2D  us_tx0;
void main() {
	lowp vec4 tex_dif = texture2D(us_tx0, vr_tx0);
	gl_FragColor = tex_dif;
}
