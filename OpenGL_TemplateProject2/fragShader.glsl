#version 430
out vec4 color;
void main(void){
color =vec4(gl_FragCoord.x<495 ? 1.0 : 0.0 ,gl_FragCoord.x<495 ? 0.0 : 1.0,0.0,1.0);
}