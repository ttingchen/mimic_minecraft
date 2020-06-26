#version 410 core                                                  
	                                                                   
uniform samplerCube tex_cubemap;                                   
//uniform sampler2DShadow shadow_tex; 


in VS_OUT                                                          
{  
	//vec4 shadow_coord;
	vec3 normal;                                                   
	vec3 view;
	vec3 N;	                                                                            
	vec3 L;	                                                                            
	vec3 V;	   
} fs_in;                                                           
	                                                                   
out vec4 color;
uniform vec3 diffuse_albedo = vec3(0.35);	                                        
uniform vec3 specular_albedo = vec3(0.7);	                                                
uniform float specular_power = 200.0;	                                                    
uniform bool isquad ;//= true;	 
vec4 blphong;
vec4 env_light;
vec4 shadow_color = vec4 (0.41, 0.36, 0.37, 1.0);
vec4 quad_color = vec4 (0.64, 0.57, 0.49, 1.0);
	                                                                   
void main(void)                                                    
{   
	vec3 N = normalize(fs_in.N);	                                                        
	vec3 L = normalize(fs_in.L);	                                                        
	vec3 V = normalize(fs_in.V);	                                                        
	vec3 H = normalize(L + V);	                                                            
	 	                                                                                        
	// Compute the diffuse and specular components for each fragment                    	
	vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;	                                
	vec3 specular = pow(max(dot(N, H), 0.0), specular_power) * specular_albedo;	        
	//float shadow_factor = textureProj(shadow_tex, fs_in.shadow_coord);//0.2;                     

	// Reflect view vector about the plane defined by the normal   
	// at the fragment                                             
	vec3 r = reflect(normalize(fs_in.view), normalize(fs_in.normal));         
	//vec3 r = vec3(1.0);// reflect(fs_in.view, normalize(fs_in.normal));         
	blphong = vec4(diffuse + specular, 1.0)*0.65;                                                    
	// Sample from scaled using reflection vector                  
	env_light = texture(tex_cubemap, r)*0.35; 
	color = (blphong + env_light);
	//color = blphong;// + env_light);
//	if(shadow_factor < 0.2)
//		color = (blphong + env_light);// * 0.2;
//	else if(shadow_factor > 1.0)
//		color = (blphong + env_light);// * 1.0;
//	else
//		color = (blphong + env_light) ;//* shadow_factor;
//	
		//color = (blphong*shadow_factor + env_light*0.2);// + env_light) * shadow_factor;
 }                                                              
	    

