#version 410 core                                                           
in vec2 texcoord;  
layout (location = 0) out vec4 fragcolor;
//for ssao
uniform mat4 ssao_um4p;
uniform sampler2D gDepth_tex; //0
uniform sampler2D gNorm_tex; //1
uniform sampler2D gNoise; //2
uniform vec2 noise_scale;

layout(std140) uniform Kernals
{                             
    vec4 kernals[32];         
}; 

void main(void)                             
{                                           
	//ssao
    float depth = texture(gDepth_tex, texcoord).r;
    //if(depth == 1.0) { discard; }                                  
    mat4 invproj = inverse(ssao_um4p);
    vec4 position = invproj * vec4(vec3(texcoord, depth) * 2.0 - 1.0, 1.0);
    position /= position.w;                                                                     
    vec3 sN = texture(gNorm_tex, texcoord).xyz;
    vec3 randvec = normalize(texture(gNoise, texcoord * noise_scale).xyz * 2.0 - 1.0); 
    vec3 sT = normalize(randvec - sN * dot(randvec, sN));                                          
    vec3 sB = cross(sN, sT);                                                                       
    mat3 tbn = mat3(sT, sB, sN); // tangent to eye matrix                                          
    const float radius = 2.0;                                                                   
    float ao = 0.0;                                                                             
    for(int i = 0; i < 32; ++i)                                                                 
    {                                                                                           
        vec4 sampleEye = position + vec4(tbn * kernals[i].xyz * radius, 0.0);                   
        vec4 sampleP = ssao_um4p * sampleEye;                                                        
        sampleP /= sampleP.w;                                                                   
        sampleP = sampleP * 0.5 + 0.5;                                                          
        float sampleDepth = texture(gDepth_tex, sampleP.xy).r;
        vec4 invP = invproj * vec4(vec3(sampleP.xy, sampleDepth) * 2.0 - 1.0, 1.0);             
        invP /= invP.w;                                                                         
        if(sampleDepth > sampleP.z || length(invP - position) > radius)                         
        {                                                                                       
            ao += 1.0;                                                                          
        }                                                                                       
    }                                                                                           
    fragcolor = vec4(vec3(ao / 32.0), 1.0);  
	//fragcolor =   vec4((ao / 32.0) * texture(diffuse_tex, texcoord).rgb, 1.0);

}                                           
