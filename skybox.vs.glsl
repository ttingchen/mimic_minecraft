#version 410 core                                          
	                                                           
out vec3 o_tc;                                                
	                                                           
uniform mat4 pvm;                                  
	                                                           
void main(void)                                            
{                                                          
	vec3[4] vertices = vec3[4](vec3(-1.0, -1.0, 1.0),      
	                            vec3( 1.0, -1.0, 1.0),      
	                            vec3(-1.0,  1.0, 1.0),      
	                            vec3( 1.0,  1.0, 1.0));     
	                                                           
	//vs_out.tc = mat3(pvm) * vertices[gl_VertexID]; 
	vec4 tc = inverse(pvm) * vec4(vertices[gl_VertexID], 1.0);
	o_tc = tc.xyz / tc.w - vec3(0.0);	                                                           
	gl_Position = vec4(vertices[gl_VertexID], 1.0);        
}                                                         
	     