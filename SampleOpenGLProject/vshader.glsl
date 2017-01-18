#version 150 


in  vec4 vPosition;
in  vec3 vNormal;
out vec4 color;
out vec3 pos;
out vec3 normal;

//texture trial
in vec2 vTexCoord;
out vec2 Texcoord;
uniform int textured;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 AmbientProduct2, DiffuseProduct2, SpecularProduct2;
uniform mat4 model_view;
uniform mat4 projection;
uniform mat4 trs_matrix;
uniform vec4 LightPosition;
uniform vec4 LightPosition2;
uniform float Shininess;
uniform vec3 eye_position;
void main()
{

    if(textured == 1) {Texcoord = vTexCoord;}
	vec4 light_1= vec4(0,0,0,0);
	vec4 light_2=vec4(0,0,0,0);


	vec3 L1,L2,E,H;
    mat4 normal_matrix = transpose(inverse(model_view));
    // Transform vertex  position into eye coordinates
    vec3 pos = (normal_matrix * vPosition).xyz;

	float distToL2 = distance(vPosition.xyz, pos);

    if (LightPosition.w == 0.0) {
        L2 = normalize( (normal_matrix*LightPosition2).xyz - pos );
		L1 = normalize(-LightPosition.xyz);
    } else {
		L2 = normalize( (normal_matrix*LightPosition2).xyz - pos );
        L1 = normalize(-LightPosition.xyz);
    }

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( model_view*vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;
	vec4 ambient2 = AmbientProduct2; //

    float Kd = max( dot(L1, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;
	float Kd2 = max( dot(L2, N), 0.0 );
	vec4  diffuse2 = Kd2*DiffuseProduct2; //

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
	float Ks2 = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular2 = Ks2 * SpecularProduct2; //
    
    if( dot(L1, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 
	if( dot(L2, N) < 0.0 ) {
	specular2 = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = projection * model_view * trs_matrix * vPosition;

	light_1 = ambient + diffuse + specular;
	if(distToL2 < 3){
		light_2 = ambient2 + diffuse2 + specular2;
	}
    color = light_2 + light_1/2;
    color.a = 1.0;

	//For toon shading
	normal = normalize(mat3(normal_matrix)*vNormal);
}

