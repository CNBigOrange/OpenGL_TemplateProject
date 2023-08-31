#version 430
out vec4 color;
void main(void)
{color =vec4(gl_FragCoord.x/1000,gl_FragCoord.y/600,0.0,1.0);}