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


uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 MV_N;
uniform mat4 M;
uniform mat4 V;
uniform float time;
uniform int animOn;
uniform int pointLightOn;
uniform int dirLightOn;
uniform int spotLightOn;
uniform Light Lights[3];
uniform Mat frontMat;
uniform Mat backMat;
uniform vec3 Tangent; 

layout(location = 0) in vec2 vt;
layout(location = 1) in vec3 vn;
layout(location = 2) in vec3 vp;


out float fragFogCoord;
out vec2 fragTexCoord;
out vec4 frontFragColor;
out vec4 backFragColor;
out vec3 VertexPosition_cameraSpace;
out vec3 Normal_cameraSpace;
out mat3 tsMatrix;
out vec3 Light0HV;   // HalfVector de la Lumière 0 en espace Tangent
out vec3 Light1HV;   // HalfVector de la Lumière 1 en espace Tangent
out vec3 Light2HV;   // HalfVector de la Lumière 2 en espace Tangent

// Accumulateurs pour les facteurs d'éclairage
vec4 Ambient;
vec4 Diffuse;

const float PI = 3.1415926535897932385;  // Pi
const float PI_INV = 1.0 / PI;        // Pi inversé

// Calcul pour une lumière ponctuelle
void pointLight(in int i, in vec3 normal, in vec3 eye, in vec3 csPosition3)
{
	float nDotVP;       // Produit scalaire entre VP et la normale
	float attenuation;  // facteur d'atténuation calculé
	float d;            // distance entre lumière et fragment
	vec3  VP;           // Vecteur lumière

	// compute the light vector
	vec3 lightVect = (Lights[i].Position.xyz) - csPosition3;
	VP = lightVect;

	// Calculer distance à la lumière
	d = length(lightVect);

	// Normaliser VP
	VP = normalize(VP);

	// Calculer l'atténuation due à la distance
	attenuation = 1.0 / dot(Lights[i].Attenuation, vec3(1.0, d, pow(d, 2)));   
   
	nDotVP = dot(normal, VP);

	// Calculer les contributions ambiantes et diffuses
	Ambient  += attenuation * vec4(Lights[i].Ambient, 1.0);
	Diffuse  += attenuation * vec4(Lights[i].Diffuse, 1.0) * nDotVP;
}


// Calcul pour une lumière "spot"
void spotLight(in int i, in vec3 normal, in vec3 eye, in vec3 csPosition3)
{
	float nDotVP;             // Produit scalaire entre VP et la normale
	float spotAttenuation;    // Facteur d'atténuation du spot
	float attenuation;        // Facteur d'atténuation du à la distance
	float angleEntreLumEtSpot;// Angle entre le rayon lumieux et le milieu du cone
	float d;                  // Distance à la lumière
	vec3  VP;                 // Vecteur lumière

    // compute the light vector
	vec3 lightVect = (Lights[i].Position.xyz) - csPosition3;
	VP = lightVect;

	// Calculer la distance à al lumière
	d = length(lightVect);

	// Normaliser VP
	VP = normalize(VP);

	// Calculer l'atténuation due à la distance
	attenuation = 1.0 / dot(Lights[i].Attenuation, vec3(1.0, d, pow(d, 2)));

	// Le fragment est-il à l'intérieur du cône de lumière ?
	vec3 spotDir = normalize(Lights[i].SpotDir);  // normalize in case the `Lights[1].SpotDir` isn't a unit vector 
	vec3 lightDir = -VP;
	angleEntreLumEtSpot = degrees(acos(dot(lightDir, spotDir)));

	if (angleEntreLumEtSpot > Lights[i].SpotCutoff)
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
	Ambient  += attenuation * vec4(Lights[i].Ambient, 1.0);
	Diffuse  += attenuation * vec4(Lights[i].Diffuse * nDotVP, 1.0);
}


// Calcul pour une lumière directionnelle
void directionalLight(in int i, in vec3 normal)
{
	vec3  VP;             // Vecteur lumière
	float nDotVP;         // Produit scalaire entre VP et la normale
	// compute the light vector
	vec3 lightVect = -(Lights[i].Position.xyz);
	VP = lightVect;

	// Normaliser VP
	VP = normalize(VP);

	// There is no attenuation due to distance (hypothesis for directional lights)
	// attenuation = 1.0;

	nDotVP = dot(normal, VP);

	// Calculer les contributions ambiantes et diffuses
	Ambient  += vec4(Lights[i].Ambient, 1.0);
	Diffuse  += vec4(Lights[i].Diffuse * nDotVP, 1.0);
}

// éclairage pour la surface du dessus
void frontLighting(in vec3 normal, in vec3 csPosition)
{
	vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
	const vec3 eye = normalize(-csPosition);

	// Clear the light intensity accumulators
	Ambient  = vec4 (0.0, 0.0, 0.0, 1.0);
	Diffuse  = vec4 (0.0, 0.0, 0.0, 1.0);

	// Calcul des 3 lumières
	if (pointLightOn == 1) {
		pointLight(0, normal, eye, csPosition);
	}
   
	if (dirLightOn == 1) {
		directionalLight(2, normal);
	}
   
	if (spotLightOn == 1) {
		spotLight(1, normal, eye, csPosition);
	}

	color = Ambient  * frontMat.Ambient + Diffuse  * frontMat.Diffuse;
	color = clamp( color, 0.0, 1.0 );
	frontFragColor = color;
}


// elairage pour la surface du dessous
void backLighting(in vec3 invNormal, in vec3 csPosition)
{
   vec4 color;
   const vec3 eye = normalize(-csPosition);

   // Clear the light intensity accumulators
   Ambient  = vec4 (0.0);
   Diffuse  = vec4 (0.0);

   // Calcul des 3 lumières
   if (pointLightOn == 1) {
     pointLight(0, invNormal, eye, csPosition);
   }
   
   if (dirLightOn == 1) {
     directionalLight(2, invNormal);
   }
   
   if (spotLightOn == 1) {
      spotLight(1, invNormal, eye, csPosition);
   }

   color = Ambient  * backMat.Ambient + Diffuse * backMat.Diffuse;
   color = clamp( color, 0.0, 1.0 );
   backFragColor = color;
}

// Retourne la matrice d'une rotation de theta radians autour de Y
mat3 rotMatX(float theta)
{
   return mat3(1.0, 0.0,        0.0,
      0.0, cos(theta), -sin(theta),
      0.0, sin(theta), cos(theta));
}


// Retourne la matrice d'une rotation de theta radians autour de Y
mat3 rotMatY(float theta)
{
   return mat3(cos(theta), 0.0, -sin(theta),
      0.0,        1.0, 0.0,
      sin(theta), 0.0, cos(theta));
}

// Retourne la matrice d'une rotation de theta radians autour de Y
mat3 rotMatZ(float theta)
{
   return mat3(cos(theta),  sin(theta), 0.0,
      -sin(theta), cos(theta), 0.0,
      0.0,         0.0,        1.0);
}

// Animation de flexion de la carte (displacement mapping)
// Cette fonction transforme les variables en entrée
void animation(inout vec4 position, inout vec3 normal, inout vec3 tangent)
{ 
	float amplitude; // Amplitude selon le temps et l'espace (animation) 
	float deltaPos;    // Variation de la hauteur du vertex
	mat3 rotMat;     // Matrice de tranformation pour la normale et tangente
	float theta;	 // Angle pour calculer la normale et la tangente
	
	amplitude = 2.0 * sin(time);
	// Déformation selon l'amplitude (time)
    if(amplitude < 0.0) {
        // Déformation sur l'axe des X selon la position X
		// TODO: 
		// Remplacer le commentaire ci-bas par la valeur de position necessaire
        deltaPos = amplitude * sin(/*position X * */ PI);
		theta = 0.5 * (vt.x - 0.5) * PI * sin(-amplitude);
		 // TODO:
		 //rotMat= ...
    } else {
        // Déformation sur l'axe des Y, selon la position Y
		// TODO: 
		// Remplacer le commentaire ci-bas par la valeur de position necessaire
        deltaPos = amplitude * sin(/*position Y * */ PI);
		float theta = 0.5 * (vt.y - 0.5) * PI * sin(amplitude);
		 // TODO:
		//rotMat = ...
    }
    // TODO:
    // Obtenir le déplacement du sommets en cours
    // position = ..

    // TODO:
    // Trouver les nouvelles normale + tangente après déplacement du sommet
    // normal = ...
    // tangent = ...
}

// Transformation des coordonnées d'espace tangent
void tsTransform(in vec3 csNormal, vec3 csTangent, vec3 csPosition)
{
    // Complétez cette fonction.
    // Vous trouverez ci-bas un genre de pseudo-code commenté 
    // qui vous aidera à structurer votre solution.

    // Calcul de la binormale
    // vec3 ecBinormal = ...

    // Construction de la matrice de transformation pour passer en espace tangent
    // mat3 tsMatrix = mat3(...);

    // Construction et calcul des vecteurs pertinants
    // Nous sommes en coordonnées de visualisation
    //vec3 EyeDir    = ... 
    // Light0HV  = ... en fonction de EyeDir et ...
    // Light1HV  = ... 
    // Light2HV  = ... en fonction de EyeDir et ...

    // Transformation dans l'espace tangent (on applique la matrice tsMatrix)
    // Light0HV  = ...
    // Light1HV  = ...
    // Light2HV  = ...
   
}

float ffog(in float distance_cameraSpace)
{
   return(abs(distance_cameraSpace));
}

void main () {

   // Variables qui seront transformées par l'animation
   vec4 position = vec4(vp,1.0);
   vec3 normal = vn;
   vec3 tangent = Tangent;
   vec3 Tangent_cameraSpace; 
  
   // Pasage des coordonées des textures
   fragTexCoord = vt;
   
   // Transformation du vertex selon le temps
   // if (animOn == 1) {
   //   animate(position, normal, tangent);
   // }
   
    //On passe au référenciel de caméra (ou eye-coordinate)
    VertexPosition_cameraSpace = ( V * M * position).xyz;
    Normal_cameraSpace = normalize(MV_N * normal);
    Tangent_cameraSpace  = normalize(MV_N * tangent); 

    // Transformation du vertex dans l'espace de clipping/écran
    gl_Position = MVP * position;

    // Calcul de la lumière ambiente et diffuse
    frontLighting(Normal_cameraSpace, VertexPosition_cameraSpace); 
    backLighting(-Normal_cameraSpace, VertexPosition_cameraSpace);
    
    // Transformation des coordonées en espace tangent
    tsTransform(Normal_cameraSpace, Tangent_cameraSpace, VertexPosition_cameraSpace);

}