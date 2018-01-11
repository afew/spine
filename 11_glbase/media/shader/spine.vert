attribute lowp    vec4    at_dif;
attribute mediump vec2    at_tex;

varying   lowp    vec4    vr_dif;
varying   mediump vec2    vr_tx0;

void main() {
	vec4 pos    = um_Wld * at_pos;
         pos    = um_Viw * pos;
	gl_Position = um_Prj * pos;

	vr_dif = at_dif;
	vr_tx0 = at_tex;
}
