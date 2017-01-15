
#version 150 


in  vec4 vPosition;
in  vec3 vNormal;
out vec4 color;
out vec3 pos;
out vec3 normal;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 model_view;
uniform mat4 projection;
uniform mat4 trs_matrix;
uniform vec4 LightPosition;
uniform float Shininess;
uniform vec3 eye_position;
void main()
{
	mat4 normal_matrix = transpose(inverse(model_view));
    // Transform vertex  position into eye coordinates
    vec3 pos = (normal_matrix * vPosition).xyz;

    vec3 L = normalize( (normal_matrix*LightPosition).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( model_view*vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = projection * model_view * trs_matrix * vPosition;

    color = ambient + diffuse + specular;
    color.a = 1.0;

	//For toon shading
	normal = normalize(mat3(normal_matrix)*vNormal);
}
