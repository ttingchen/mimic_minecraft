#version 410 core                           
                                            
uniform samplerCube tex_cubemap;            
                                            
in vec3 o_tc;
                                            
layout (location = 0) out vec4 color;       
                                            
void main(void)                             
{                                           
    color = texture(tex_cubemap, o_tc); 
}                                           
  