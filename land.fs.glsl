#version 410 core    

in vec2 texcoord;    
in float height;
in vec4 shadow_coord;


in PHONG
{
	vec3 N;
	vec3 L;
	vec3 V;
}phong;

in NORMAL
{
	vec3 lightDir;
	vec3 eyeDir;
}normalMap;

uniform float sand_height;
uniform bool isN;
uniform bool isSH;
uniform bool isSsao;
uniform sampler2D tex;  //0
uniform sampler2D sand_tex; //1  
uniform sampler2D tex_norm; //2  
uniform sampler2D sand_tex_norm; //3 
uniform sampler2DShadow shadow_tex; //4
uniform sampler2D ao_tex; //5
in vec4 ssaoMap_coord;
                                                                           
layout (location = 0) out vec4 fragcolor;       
     
                                            
void main(void)                             
{  
	//material
	vec3 kd;
	vec3 kd_grass = texture(tex, texcoord).rgb;
	vec3 kd_sand = texture(sand_tex, texcoord).rgb;
	vec3 ks = vec3(1.0);
	vec3 ia = vec3(0.5);
	vec3 light_color = vec3(1.0, 0.9, 0.3);
	float light_pow = 5.0;

	//phong lighing
	vec3 N, L, V;
	vec3 N_origin = normalize(phong.N);
	vec3 V_origin = normalize(phong.V);
	vec3 L_origin = normalize(phong.L);

	//normal mapping
	vec3 N_grass = normalize(texture(tex_norm, texcoord).rgb * 
					2.0 - vec3(1.0));
	vec3 N_sand = normalize(texture(sand_tex_norm, texcoord).rgb *
					2.0 - vec3(1.0));
	vec3 V_n = normalize(normalMap.eyeDir);
	vec3 L_n = normalize(normalMap.lightDir);

	if(isN == false){
		V = V_origin;
		L = L_origin;
	}
	else{
		V = V_n;
		L = L_n;
	}

	if(height < sand_height){
		kd = kd_sand;
		if(isN == false) N = N_origin;
		else N = N_sand;
	}
	else{
		kd = kd_grass;
		if(isN == false) N = N_origin;
		else N = N_grass;
	}

	vec3 R = reflect(-L, N);
	vec3 H = normalize(L + V) ;
	vec3 diffuse = max(dot(N, L), 0.0) * kd;
	vec3 ambient = kd * ia * light_color;
	vec3 specular = pow(max(dot(R, V), 0.0), light_pow)	* ks * light_color ;
	
	vec4 coord;
	coord.xyz = ssaoMap_coord.xyz / ssaoMap_coord.w;
	coord.xyz = coord.xyz * 0.5 + 0.5;
	float ao_factor = texture(ao_tex, coord.xy).r;
 
	fragcolor = vec4(diffuse + specular , 1.0);
	
	if(isSH == true){
		fragcolor *= textureProj(shadow_tex, shadow_coord);
	}

	if(isSsao == true){
		fragcolor += vec4(ao_factor * ambient, 1.0); 
	}
	else{
		fragcolor += vec4(ambient, 1.0); 
	}
	//fragcolor = vec4(vec3(ao_factor), 1.0);
	//fragcolor = vec4(0.0, 0.0, 0.0 , 1.0);
}                                           
  