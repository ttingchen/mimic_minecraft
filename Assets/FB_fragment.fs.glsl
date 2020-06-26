#version 410 core

uniform sampler2D tex;
//uniform sampler2D sb_tex;
//uniform sampler2D snoobj_tex;
//uniform sampler2D sobj_tex;
//
out vec4 color;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
	color = texture(tex, fs_in.texcoord);
}