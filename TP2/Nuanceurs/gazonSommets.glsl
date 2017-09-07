#version 430 core
struct Light
{
        vec3 Ambient; 
        vec3 Diffuse;
        vec3 Specular;
        vec4 Position;  // Si .w = 1.0 -> Direction de lumiere directionelle.
        vec3 SpotDir;
        float SpotExp;
        float SpotCutoff;
        vec3 Attenuation; //Constante, Lineraire, Quadratique
};

struct Mat
{
        vec4 Ambient; 
        vec4 Diffuse;
        vec4 Specular;
        vec4 Exponent;
        float Shininess;
};

layout(location = 0) in vec2 vt;
layout(location = 1) in vec3 vn;
layout(location = 2) in vec3 vp;

uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;
uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 MV_N;
uniform mat4 M;
uniform mat4 V;
uniform Light Lights[3]; // 0:ponctuelle  1:spot  2:directionnelle
uniform Mat Material;

out vec4 fragColor;
out vec2 fragTexCoord;

//Accumulateurs de contributions Ambiantes et Diffuses:
vec4 Ambient;
vec4 Diffuse;

// Calcul pour une lumière ponctuelle
void pointLight(in vec3 lightVect, in vec3 normal)
{
   float nDotVP;       // Produit scalaire entre VP et la normale
   float attenuation;  // facteur d'atténuation calculé
   float d;            // distance entre lumière et fragment
   vec3  VP;           // Vecteur lumière

   // Calculer vecteur lumière
   VP = lightVect;

   // Calculer distance à la lumière
   d = length(lightVect);

   // Normaliser VP
   VP = normalize(VP);

   // Calculer l'atténuation due à la distance
   attenuation = 1.0 / dot(Lights[0].Attenuation, vec3(1.0, d, pow(d, 2)));
   
   
   nDotVP = dot(normal, VP);

   // Calculer les contributions ambiantes et diffuses
   Ambient  += attenuation * vec4(Lights[0].Ambient, 1.0);
   Diffuse  += attenuation * vec4(Lights[0].Diffuse, 1.0) * nDotVP;
}


// Calcul pour une lumière directionnelle
void directionalLight(in vec3 lightVect, in vec3 normal)
{
   vec3  VP;             // Vecteur lumière
   float nDotVP;         // Produit scalaire entre VP et la normale

   // Calculer vecteur lumière
   VP = lightVect;

   // Normaliser VP
   VP = normalize(VP);

   // There is no attenuation due to distance (hypothesis for directional lights)
   // attenuation = 1.0;

   nDotVP = dot(normal, VP);

   // Calculer les contributions ambiantes et diffuses
   Ambient  += vec4(Lights[2].Ambient, 1.0);
   Diffuse  += vec4(Lights[2].Diffuse * nDotVP, 1.0);
}


// Calcul pour une lumière "spot"
void spotLight(in vec3 lightVect, in vec3 normal)
{
   float nDotVP;             // Produit scalaire entre VP et la normale
   float spotAttenuation;    // Facteur d'atténuation du spot
   float attenuation;        // Facteur d'atténuation du à la distance
   float angleEntreLumEtSpot;// Angle entre le rayon lumieux et le milieu du cone
   float d;                  // Distance à la lumière
   vec3  VP;                 // Vecteur lumière

   // Calculer le vecteur Lumière
   VP = lightVect;

   // Calculer la distance à al lumière
   d = length(lightVect);

   // Normaliser VP
   VP = normalize(VP);

   // Calculer l'atténuation due à la distance
   attenuation = 1.0 / dot(Lights[0].Attenuation, vec3(1.0, d, pow(d, 2)));

   // Le fragment est-il à l'intérieur du cône de lumière ?
   vec3 spotDir = normalize(Lights[1].SpotDir);  // normalize in case the `Lights[1].SpotDir` isn't a unit vector 
   vec3 lightDir = -VP;
   angleEntreLumEtSpot = acos(dot(VP, spotDir));

   if (angleEntreLumEtSpot > Lights[1].SpotCutoff)
   {
       spotAttenuation = 0.0; // en dehors... aucune contribution
   }
   else
   {
       spotAttenuation = 1.0; // there's no spot attenuation whenever the ray is within the cone of the spotlight

   }

   // Combine les atténuation du spot et de la distance
   attenuation *= spotAttenuation;

   nDotVP = dot(normal, VP);

   // Calculer les contributions ambiantes et diffuses
   Ambient  += attenuation * vec4(Lights[1].Ambient, 1.0);
   Diffuse  += attenuation * vec4(Lights[1].Diffuse * nDotVP, 1.0);
}

vec4 flight(in vec3 light0Vect, in vec3 light1Vect, in vec3 light2Vect, in vec3 normal)
{
    vec4 color;
    vec3 ecPosition3;

    // Réinitialiser les accumulateurs
    Ambient  = vec4 (0.0);
    Diffuse  = vec4 (0.0);

    if (pointLightOn == 1) {
        pointLight(light0Vect, normal);
    }
    if (spotLightOn == 1) {
        spotLight(light1Vect, normal);
    }
    if (dirLightOn == 1) {
        directionalLight(light2Vect, normal);
    }
    
    //À dé-commenter!
    color = (Ambient * Material.Ambient + Diffuse  * Material.Diffuse);
    color = clamp( color, 0.0, 1.0 );
    
    // À supprimer !
    // color = vec4(0.0, 1.0, 0.0, 1.0);
    
    return color;
}

void main (void)
{
    vec3 csPosition3;
    vec3 light0Vect;
    vec3 light1Vect;
    vec3 light2Vect;
    vec3 normal_cameraSpace;
    
    // Transformation de la position
    gl_Position = MVP * vec4(vp,1.0);
    vec4 csPosition = MV * vec4 (vp, 1.0);
    csPosition3 = (csPosition.xyz);
    
    //Vecteurs de la surface vers la lumière
    light0Vect = (Lights[0].Position.xyz) - csPosition3;
    light1Vect = (Lights[1].Position.xyz) - csPosition3;
    light2Vect = (Lights[2].Position.xyz) - csPosition3;

    //Normale en référentiel caméra:
    normal_cameraSpace = normalize(MV_N * vn);
    
    //Coordonée de texture:
    fragTexCoord = vt;  // Pass the tex coordinates to the fragment shader
    
    // Transformation de la position
    gl_Position = MVP * vec4(vp,1.0);
    
    
    fragColor = flight(light0Vect, light1Vect, light2Vect, normal_cameraSpace);
}
