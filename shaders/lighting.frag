#version 330

layout (location=0) out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragWorldPos;

uniform sampler2D baseTexture;
uniform vec4 material; // material.x = k_a, material.y = k_d, material.z = k_s, material.w = shininess

uniform vec3 ambientColor;
uniform vec3 directionalLight; // this is the direction of the light
uniform vec3 directionalColor;
uniform vec3 viewPos;

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform Light light;

void main() {
    // Compute ambient intensity
    vec3 ambient = ambientColor * material.x;

    // Compute diffuse intensity
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.y;

    // Compute specular intensity
    vec3 viewDir = normalize(viewPos - FragWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.w);
    vec3 specular = directionalColor * spec * material.z;


    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1) * texture(baseTexture, TexCoord);
 
}