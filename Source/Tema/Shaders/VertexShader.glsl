#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 lightPosition;
uniform vec3 eyePosition;

uniform int materialShininess;

uniform float materialKd;
uniform float materialKs;

uniform vec3 objectColor;

// Output value to fragment shader
out vec3 color;

void main()
{
	// Constant
	float globalAmbientalColor = 0.25f;
	// Normalize
	vec3 N = normalize(v_normal);

	// Compute world space vectors
	vec3 world_pos = (Model * vec4(v_position,1)).xyz;
	vec3 world_normal = normalize( mat3(Model) * v_normal );

	// Light direction vector
	vec3 L = normalize( lightPosition - world_pos );

	// Direction vector of observer
	vec3 V = normalize( eyePosition - world_pos );

	// Median vector
	vec3 H = normalize( L + V );


	// Compute ambient light component
	float ambient_light = materialKd * globalAmbientalColor;

	// Compute diffuse light component
	float diffuse_light = materialKd * max(dot(N, L), 0);

	// Compute specular light component
	float specular_light = 0;

	if (diffuse_light > 0) {
		specular_light = materialKs * max(dot(N, L), 0) * pow(max(dot(N, H), 0), materialShininess);
	}

	// Compute atenuation
	float atenuation = distance(v_position, lightPosition);

	// Compute light
	float light = ambient_light + specular_light + diffuse_light;

	// Send color light output to fragment shader
	color = objectColor * light;

	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
