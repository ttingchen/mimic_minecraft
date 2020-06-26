 #version 410 core                             
	                                               
	 uniform mat4 mv_matrix;                       
	 uniform mat4 m_matrix;                       
	 uniform mat4 view_matrix;                       
	 uniform mat4 proj_matrix;                     
	 //uniform mat4 shadow_matrix;                     
	                                               
	 layout (location = 0) in vec4 position;       
	 layout (location = 1) in vec3 normal;         
	                                               
	 out VS_OUT                                    
	 {   
		 //vec4 shadow_coord;
	     vec3 normal;                              
	     vec3 view;  
		 vec3 N;                                                
	     vec3 L;                                                
	     vec3 V; 
	 } vs_out;                                     
	 vec3 light_pos = mat3(view_matrix) * vec3(-31.75, 26.05, -97.72) ;        
                                              
	 void main(void)                               
	 {     
		 // Calculate view-space coordinate                     
	    vec4 P = mv_matrix * vec4(position.xyz,1.0);	                        
	    // Calculate normal in view-space	                    
	    vs_out.N = mat3(mv_matrix) * normal;	                
	    // Calculate light vector	                            
	    vs_out.L = light_pos - P.xyz;	                        
	    // Calculate view vector	                            
	    vs_out.V = -P.xyz;
	    vs_out.normal = mat3(transpose(inverse(m_matrix))) * normal;
		vec4 pos_vs = m_matrix * position;	
	    vs_out.view = pos_vs.xyz - vec3(0.0f, 0.0f, 0.0f);                 

	    // Light-space coordinates                          	
	    //vs_out.shadow_coord = shadow_matrix * position;	    
	    // Calculate the clip-space position of each vertex	
	    gl_Position = proj_matrix * P;
	     
	 }                                             
	  
	 