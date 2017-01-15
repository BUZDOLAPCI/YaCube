#version 150 

in vec4 color;
uniform vec4 LightPosition;
in vec3 normal;
in vec3 pos;
out vec4 fColor;
uniform int toonEnable;
uniform mat4 model_view;
uniform vec3 eye_position;

void main()
{
	float intensity;
	vec4 color2;
	mat4 normal_matrix = transpose(inverse(model_view));
	vec3 n = normalize(normal);
	if(toonEnable == 1){
		//Flat + toon shading
		intensity = dot(normalize(vec3(normal_matrix*LightPosition)),n);
		if (intensity > 0.95)
			fColor = color;
		else if (intensity > 0.5)
			fColor = vec4(color.x*0.8,color.y*0.8,color.z*0.8,1.0);
		else if (intensity > 0.25)
			fColor = vec4(color.x*0.5,color.y*0.5,color.z*0.5,1.0);
		else
			fColor = vec4(color.x*0.2,color.y*0.2,color.z*0.2,1.0);
	}
	else if (toonEnable == 2){
		//Pure toon shading
		//vec3 V = normalize(eye_position - pos);
		//float edgeDetection = (dot(V, vec3(normal)) > 0.3) ? 1 : 0;
		float edgeDetection = (dot(normalize(vec3(normal_matrix*LightPosition)),n) > 0.1) ? 1 : 0;
		intensity = dot(normalize(vec3(normal_matrix*LightPosition)),n);
		if (edgeDetection == 0) {
			fColor = vec4(0.0,0.0,0.0,1.0);		
		}
		else {	
			if (intensity > 0.95)
				fColor = vec4(1.0,1.0,1.0,1.0);
			else if (intensity > 0.8)
				fColor = vec4(0.8,0.8,0.8,1.0);
			else if (intensity > 0.25)
				fColor = vec4(0.5,0.5,0.5,1.0);
			else
				fColor = vec4(0.2,0.2,0.2,1.0);
		}
	}
	else
		fColor = color;
}