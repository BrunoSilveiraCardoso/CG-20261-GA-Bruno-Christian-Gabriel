#version 330 core
out vec4 FragColor;

in vec3 vNormal;
in vec3 vFragPos;
in vec3 vBarycentric;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

uniform bool wireframe;
uniform vec3 wireframeColor;
uniform float wireframeWidth;

void main()
{
	// Calculate distance to nearest edge
	float minDist = min(vBarycentric.x, min(vBarycentric.y, vBarycentric.z));

	// Draw wireframe only on the actual edges
	// Use a small threshold (0.01-0.05) and make sure it's exactly on edges
	if (minDist < wireframeWidth && wireframe) {
		FragColor = vec4(wireframeColor, 1.0);
	} else {
		float distance = length(light.position - vFragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance * light.quadratic * pow(distance, 2));

		vec3 ambient = light.ambient * material.ambient;
		vec3 norm = normalize(vNormal);
		vec3 lightDir = normalize(light.position - vFragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * (diff * material.diffuse);
		vec3 viewDir = normalize(viewPos - vFragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
		vec3 specular = light.specular * (spec * material.specular);

		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		vec3 result = ambient + diffuse + specular;
		FragColor = vec4(result, 1.0);
	}
}
