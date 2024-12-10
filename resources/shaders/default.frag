#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 mat_ka;
in vec3 mat_kd;
in vec3 mat_ks;
//in float mat_shininess;
out vec4 fragColor;

in vec2 TextureCoords;
in mat3 TBN;

uniform int lightCount;

const int LIGHT_POINT = 0;
const int LIGHT_DIRECTIONAL = 1;
const int LIGHT_SPOT = 2;

uniform int lightTypes[8];
uniform vec4 lightPositions[8];
uniform vec4 lightDirections[8];
uniform vec4 lightColors[8];
uniform vec3 lightAttenuations[8];
uniform vec2 lightAngles[8];

uniform float k_a, k_d, k_s, shininess;
uniform vec4 materialAmbient, materialDiffuse, materialSpecular;
uniform vec4 cameraPos;

uniform sampler2D normalMap;

float calculateAttenuation(vec3 attenuation, float distance) {
    float constant = attenuation.x;
    float linear = attenuation.y;
    float quadratic = attenuation.z;
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}

vec4 calculatePhong(vec3 L, vec4 lightColor, float attenuation) {
     vec3 N;
    if(mat_kd == vec3(0.061246,0.038204,0.027321)){
        vec3 normalMap = texture(normalMap, TextureCoords).rgb * 2.0 - 1.0;
        N = normalize(TBN * normalMap);
    } else{
        N = normalize(worldNormal);
    }

    vec3 V = normalize(vec3(cameraPos) - worldPosition);
    vec3 lightDir = L;

    //diffuse
    float diffuseIntensity = max(dot(N, lightDir), 0.0);
    vec4 diffuse = k_d * diffuseIntensity * vec4(mat_kd, 1) * lightColor;

    //specular
    vec3 reflectedLight = reflect(-lightDir, N);
    float specIntensity = pow(max(dot(reflectedLight, V), 0.0), max(shininess, 0.001));
    vec4 specular = k_s * specIntensity * vec4(mat_ks, 1) * lightColor;

    return (diffuse + specular) * attenuation;
}

float falloffFunction(float theta, float thetaInner, float thetaOuter){
    float t = (theta - thetaInner) / (thetaOuter - thetaInner);
    return -2 * pow(t, 3) + 3 * pow(t, 2);
}

void main() {
    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
    //fragColor = vec4(1.0);

    //output color to the absolute value of your world-space normals, for debugging
    //fragColor = vec4(abs(worldNormal), 1.0);

    vec4 finalColor = k_d * vec4(mat_kd, 1.0);

    for (int i =0; i < lightCount; i++){
        vec4 lightColor = lightColors[i];
        vec3 attenuation = lightAttenuations[i];
        float atten = 1.0;
        vec3 L;

        if(lightTypes[i] == LIGHT_DIRECTIONAL){
            L = normalize(vec3(-lightDirections[i]));
            finalColor += calculatePhong(L, lightColor, 1.0);
        }
        else if(lightTypes[i] == LIGHT_POINT){
            vec3 lightVec = vec3(lightPositions[i]) - worldPosition;
            float distance = length(lightVec);
            L = normalize(lightVec);

            atten = min(calculateAttenuation(attenuation, distance), 1.0);

            finalColor += calculatePhong(L, lightColor, atten);

        }else if(lightTypes[i] == LIGHT_SPOT){
            vec3 spotDir = normalize(vec3(lightDirections[i]));

            vec3 lightVec = vec3(lightPositions[i]) - worldPosition;
            float distance = length(lightVec);
            L = normalize(lightVec);

            float distFatt = calculateAttenuation(attenuation, distance);

            float theta = acos(dot(L, -spotDir));

            float angle = lightAngles[i].x;
            float penumbra = lightAngles[i].y;


            float theta_outer = angle;
            float theta_inner = angle - penumbra;

            //falloff
            float spotFatt;
               if (theta < theta_inner) {
                   spotFatt = 1.0;  //full intensity within the inner cone
               } else if (theta < theta_outer && theta >= theta_inner) {
                   spotFatt = 1.0 - falloffFunction(theta, theta_inner, theta_outer);  //falloff between inner and outer cones
               } else {
                   spotFatt = 0.0;  //no light outside the outer cone
               }

            atten = min(1.0, spotFatt * distFatt);

            finalColor += calculatePhong(L, lightColor, atten);


        }
    }

    fragColor = finalColor;
    // fragColor += k_a * vec4(mat_ka, 1.0);



}
