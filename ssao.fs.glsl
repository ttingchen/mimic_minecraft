#version 410 core                                                           
out vec4 frag_normal;
in vec3 normal;  

void main(void)                             
{                                           
    frag_normal = vec4(normalize(normal), 1.0);
}                                           
