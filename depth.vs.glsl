#version 410 core                                          
layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2texcoord;
layout(location = 2) in vec3 iv3normal;

//light camera
uniform mat4 um4vp;

uniform mat4 trans;                                                 
uniform vec3[300] offset; 


void main(void)                                            
{                                                          
	int id = gl_InstanceID;  
	vec4 pre_pos = trans * vec4(iv3vertex, 1.0);
	vec4 P = vec4(pre_pos.xyz + offset[id], 1.0);
	gl_Position = um4vp *  P;
}           
