#version 410 core                                          
layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2texcoord;
layout(location = 2) in vec3 iv3normal;
                                                           
uniform mat4 um4v;                                                 
uniform mat4 um4p;                                                 
uniform mat4 trans;                                                 
uniform vec3[300] offset; 
uniform vec3 eyepos; 
uniform float sand_height;
uniform vec3 light_pos;
uniform mat4 shadow_matrix;

out vec2 texcoord; 
//ssao
out vec4 ssaoMap_coord;

out float height;

// Light-space coordinates
out vec4 shadow_coord;

out PHONG
{
	vec3 N;
	vec3 L;
	vec3 V;
}phong;

out NORMAL
{
	vec3 lightDir;
	vec3 eyeDir;
}normalMap;


void main(void)                                            
{                                                          
	int id = gl_InstanceID;  
	vec4 pre_pos = trans * vec4(iv3vertex, 1.0);
	vec4 P = vec4(pre_pos.xyz  + offset[id], 1.0);
	gl_Position = um4p * um4v *  P;

	ssaoMap_coord =  um4p * um4v *  P;
	
	texcoord = iv2texcoord;
	height = offset[id].y;

	//shadow
	shadow_coord = shadow_matrix * P;

	//phong light
	P /= P.w;
	vec3 normal = mat3(transpose(inverse(trans))) * iv3normal;
	phong.N = normal; 
	phong.L = light_pos;//light_pos - P.xyz; //vec3(1.0);
	phong.V = eyepos - P.xyz;// - eyepos;

	vec3 tangent = vec3(1.0, 0.0, 0.0);
	// Eye space to tangent space TBN
	vec3 T = normalize(mat3(trans) * tangent);
	vec3 N = normalize(mat3(trans) * iv3normal);
	vec3 B = cross(N, T);
	vec3 L = phong.L;
	vec3 V = phong.V;
	normalMap.lightDir = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
	normalMap.eyeDir = normalize(vec3(dot(V, T), dot(V, B), dot(V, N))); 

}           
