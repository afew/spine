//attribute lowp    vec4    at_dif;
attribute mediump vec2    at_tex;

varying   lowp    vec4    vr_dif;
varying   mediump vec2    vr_tx0;

uniform   mediump mat4    um_WVP;
uniform   lowp    vec4    us_tc;

void main() {
	gl_Position = um_WVP * at_pos;

	vr_dif = us_tc;
	vr_tx0 = at_tex;
}
