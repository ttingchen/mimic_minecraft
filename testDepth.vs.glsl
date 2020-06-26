#version 410 core                                          

out vec2 texcoord;  


void main(void)                                            
{                                                          
	vec4 positions[4] = vec4[4](
	vec4(-1, -1, 0, 1),
	vec4(1, -1, 0, 1),
	vec4(-1, 1, 0, 1),
	vec4(1, 1, 0, 1)
	);
	texcoord = positions[gl_VertexID].xy * 0.5 + 0.5;
	gl_Position = positions[gl_VertexID];
}           
