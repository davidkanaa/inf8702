///////////////////////////////////////////////////////////////////////////////
///  @file main.cpp
///  @brief   le main du programme ProjetNuanceur pour le cours INF8702 de Polytechnique
///  @author  Frédéric Plourde (2007)
///  @author  Félix Gingras Harvey (2016)
///  @date    2007 / 2016
///  @version 2.0
///
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_inverse.hpp>
#include <gtx/transform.hpp>
#include <gtx/matrix_cross_product.hpp>
#include "textfile.h"
#include "GrilleQuads.h"
#include "Cst.h"
#include "Var.h"
#include "NuanceurProg.h"
#include "Materiau.h"
#include "Texture2D.h"
#include "Modele3DOBJ.h"

#include <gtc/type_ptr.hpp>


///////////////////////////////////////////////
// LES OBJETS                                //
///////////////////////////////////////////////

// les programmes de nuanceurs
CNuanceurProg progNuanceurCube("Nuanceurs/cubeSommets.glsl", "Nuanceurs/cubeFragments.glsl", false);
CNuanceurProg progNuanceurGrille("Nuanceurs/basicSommets.glsl", "Nuanceurs/basicFragments.glsl", false);

// Les objets 3D graphiques (à instancier plus tard parce que les textures demandent le contexte graphique)
CGrilleQuads *grille;

// Infos cube
GLuint  cube_vbo_pos = 0;
GLuint  cube_vbo_col = 0;
GLuint  cube_ibo = 0;
GLuint  cube_vao = 0;
GLint   tailleCube = 0;

// Vecteurs caméra
glm::vec3 cam_position = glm::vec3(0, 0, -25);
glm::vec3 direction;
glm::vec3 cam_right;
glm::vec3 cam_up;

// Angle horizontale de la caméra: vers les Z
float horizontalAngle = 0.f;
// Angle vertical: vers l'horizon
float verticalAngle = 0.f;
// "Field of View" initial
float initialFoV = 45.0f;

// unités/secondes
float vitesseCamera = 15.0f; 
float vitesseSouris = 0.05f;

double sourisX = 0;
double sourisY = 0;

///////////////////////////////////////////////
// PROTOTYPES DES FONCTIONS DU MAIN          //
///////////////////////////////////////////////
void initialisation (void);
void initialiserCube(void);
void dessinerCube(void);
void dessinerScene(void);
void dessinerGrille(void);
void clavier(GLFWwindow *fenetre, int touche, int scancode, int action, int mods);
void mouvementSouris(GLFWwindow* window, double deltaT, glm::vec3& direction, glm::vec3& right, glm::vec3& up);
void redimensionnement(GLFWwindow *fenetre, int w, int h);
void rafraichirCamera(GLFWwindow* window, double deltaT);
void compilerNuanceurs();

// le main
int main(int argc,char *argv[])
{
	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERREUR: impossible d'initialiser GLFW3\n");
		return 1;
	}

	GLFWwindow* fenetre = glfwCreateWindow(CVar::currentW , CVar::currentH, "INF8702 - Labo", NULL, NULL);
	if (!fenetre) {
		fprintf(stderr, "ERREUR: impossibe d'initialiser la fenetre avec GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwSetWindowPos(fenetre, 600, 100);

	// Rendre le contexte openGL courrant celui de la fenêtre
	glfwMakeContextCurrent(fenetre);

	// Combien d'updates d'écran on attend après l'appel à glfwSwapBuffers()
	// pour effectivement échanger les buffers et retourner
	glfwSwapInterval(1);

	// Définir la fonction clavier
	glfwSetKeyCallback(fenetre, clavier);

	// Reset mouse position for next frame
	glfwSetCursorPos(fenetre, CVar::currentW / 2, CVar::currentH / 2);

	// Définire le comportement du curseur
	// glfwSetInputMode(fenetre, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Définir la fonction de redimensionnement
	glfwSetWindowSizeCallback(fenetre, redimensionnement);
	
	// vérification de la version 4.X d'openGL
	glewInit();
	if (glewIsSupported("GL_VERSION_4_3"))
		printf("Pret pour OpenGL 4.3\n\n");
	else {
		printf("\nOpenGL 4.3 n'est pas supportee! \n");
		exit(1);
	}

	// Specifier le context openGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // recueillir des informations sur le système de rendu
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Materiel de rendu graphique: %s\n", renderer);
	printf("Plus recente version d'OpenGL supportee: %s\n\n", version);

    GLint max;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max);
    printf("GL_MAX_TEXTURE_UNITS = %d\n", max);

    glGetIntegerv(GL_MAX_VARYING_FLOATS, &max);
    printf("GL_MAX_VARYING_FLOATS = %d\n\n", max);


	if (!glfwExtensionSupported ("GL_EXT_framebuffer_object") ){
        printf("Objets 'Frame Buffer' NON supportes! :( ... Je quitte !\n");
        exit (1);
    } else {
        printf("Objets 'Frame Buffer' supportes :)\n\n");
    }


    // compiler et lier les nuanceurs
    compilerNuanceurs();

    // initialisation de variables d'état openGL et création des listes
    initialisation();
	
	double dernierTemps = glfwGetTime();
	int nbFrames = 0;
	
    // boucle principale de gestion des evenements
	while (!glfwWindowShouldClose(fenetre))
	{
		glfwPollEvents();

		//Temps ecoule en secondes depuis l'initialisation de GLFW
		double temps = glfwGetTime();
		double deltaT = temps - CVar::temps;
		CVar::temps = temps;

		nbFrames++;
		// Si ça fait une seconde que l'on a pas affiché les infos
		if (temps - dernierTemps >= 1.0){
			if (CVar::showDebugInfo){
				printf("%f ms/frame\n", 1000.0 / double(nbFrames));
				printf("Position: (%f,%f,%f)\n", cam_position.x, cam_position.y, cam_position.z);
			}
			nbFrames = 0;
			dernierTemps += 1.0;
		}
		
		// Rafraichir le point de vue selon les input clavier et souris
		rafraichirCamera(fenetre, deltaT);
		
		// Afficher nos modèles
		dessinerScene();

		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(fenetre);

	}
	// close GL context and any other GLFW resources
	glfwTerminate();

    // on doit faire le ménage... !
	delete grille;


    // le programme n'arrivera jamais jusqu'ici
    return EXIT_SUCCESS;
}


// Initialisations variées
void initialisation (void) {

    // fixer la couleur de fond
    glClearColor (0.0, 0.0, 0.5, 1.0);

    // activer les etats openGL
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
	// fonction de profondeur
	glDepthFunc(GL_LEQUAL);

	grille = new CGrilleQuads("", 10.f, 10.f, 1, 1, 1.0f);
	// Donner à la grille une echelle de 20 pour l'agrandir à l'affichage
	grille->modifierEchelle(20);

	initialiserCube();
}

void initialiserCube(void)
{
	// Sommets du cube:
	float positions[] = {

		-1.0f, -1.0f, 1.0f,//0
		-1.0f, -1.0f, 1.0f,//1
		-1.0f, -1.0f, 1.0f,//2

		1.0f, -1.0f, 1.0f,//3
		1.0f, -1.0f, 1.0f,//4
		1.0f, -1.0f, 1.0f,//5

		1.0f, 1.0f, 1.0f,//6
		1.0f, 1.0f, 1.0f,//7
		1.0f, 1.0f, 1.0f,//8

		-1.0f, 1.0f, 1.0f,//9
		-1.0f, 1.0f, 1.0f,//10
		-1.0f, 1.0f, 1.0f,//11

		-1.0f, -1.0f, -1.0f,//12
		-1.0f, -1.0f, -1.0f,//13
		-1.0f, -1.0f, -1.0f,//14

		1.0f, -1.0f, -1.0f,//15
		1.0f, -1.0f, -1.0f,//16
		1.0f, -1.0f, -1.0f,//17

		1.0f, 1.0f, -1.0f,//18
		1.0f, 1.0f, -1.0f,//19
		1.0f, 1.0f, -1.0f,//20

		-1.0f, 1.0f, -1.0f,//21
		-1.0f, 1.0f, -1.0f,//22
		-1.0f, 1.0f, -1.0f,//23
	};

	float couleurs[] = {
		1.0f, 0.0f, 0.0f,//0
		1.0f, 0.0f, 0.0f,//1
		1.0f, 0.0f, 0.0f,//2

		0.0f, 1.0f, 0.0f,//3
		0.0f, 1.0f, 0.0f,//4
		0.0f, 1.0f, 0.0f,//5

		0.0f, 0.0f, 1.0f,//6
		0.0f, 0.0f, 1.0f,//7
		0.0f, 0.0f, 1.0f,//8

		1.0f, 1.0f, 1.0f,//9
		1.0f, 1.0f, 1.0f,//10
		1.0f, 1.0f, 1.0f,//11

		1.0f, 0.0f, 0.0f,//12
		1.0f, 0.0f, 0.0f,//13
		1.0f, 0.0f, 0.0f,//14

		0.0f, 1.0f, 0.0f,//15
		0.0f, 1.0f, 0.0f,//16
		0.0f, 1.0f, 0.0f,//17

		0.0f, 0.0f, 1.0f,//18
		0.0f, 0.0f, 1.0f,//19
		0.0f, 0.0f, 1.0f,//20

		1.0f, 1.0f, 1.0f,//21
		1.0f, 1.0f, 1.0f,//22
		1.0f, 1.0f, 1.0f,//23
	};

	// Indique les indexes des sommets composant les faces du cube (groupés en triangles)
	unsigned int indices_sommets[] = {
		// front
		2, 5, 8,
		8, 11, 2,
		// top
		3, 15, 18,
		18, 6, 3,
		// back
		20, 17, 14,
		14, 23, 20,
		// bottom
		12, 0, 9,
		9, 21, 12,
		// left
		13, 16, 4,
		4, 1, 13,
		// right
		10, 7, 19,
		19, 22, 10, };

	//
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	// Générer les buffers:
	glGenBuffers(1, &cube_vbo_pos);

	glGenBuffers(1, &cube_vbo_col);

	glGenBuffers(1, &cube_ibo);

	// Lier (dans le bon ordre) et remplir nos buffers dans le bon ordre:
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
	glBufferData(GL_ARRAY_BUFFER, sizeof(couleurs), couleurs, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cube_ibo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices_sommets), indices_sommets, GL_STATIC_DRAW);

	// Créer les pointeurs d'attributs, activer les bons attributs par rapport au nuanceur
	// Ici, les déclarations du nuanceur cubeSommets.glsl devraient vous être utile.
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_pos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL
	);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL
	);

}

// Construire la matrice MVP + dessiner la grille
void dessinerGrille()
{
	
	// Création d'une matrice-modèle - bouge et déforme l'objet dans le référenciel du monde
	// Utilisez ici les fonctionnalités de GLM


	// Matrice effectuant une translation de (0,-1.0,0)
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	// Mise à l'échelle à l'aide de l'attribut de CGrilleQuad
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(grille->obtenirEchelle()));

	// Effectuer une rotation pour situer le plan sous le cube:
	glm::mat4 rotationMatrix;
	if (CVar::isRotating)
	{
		rotationMatrix = glm::rotate(glm::mat4(1.0f), (glm::float32)glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}


	// Caluler la matrice modèle:
	glm::mat4 model = translationMatrix * rotationMatrix * scalingMatrix;

	
	// Activer le nuanceur approprié:
	progNuanceurGrille.activer();

	// printf("%i", progNuanceurGrille.getProg());

	// Définir la couleur que l'on veux pour la grille
	glm::vec4 couleur = glm::vec4(0.65f, 0.6f, 0.4f, 1.0f);

	//Envoyer la couleur au programme de nuanceurs:
	GLuint colorVecID = glGetUniformLocation(progNuanceurGrille.getProg(), "color");
	glUniform4fv(colorVecID, 1, glm::value_ptr(couleur));

	// Fournir les matrices de transformation ainsi que la couleur à appliquer sur l'ensemble du plan:
	GLuint modelMatID = glGetUniformLocation(progNuanceurGrille.getProg(), "model");
	GLuint viewMatID = glGetUniformLocation(progNuanceurGrille.getProg(), "view");
	GLuint projectionMatID = glGetUniformLocation(progNuanceurGrille.getProg(), "projection");

	// printf("%i, %i, %i, %i\n", colorVecID, modelMatID, viewMatID, projectionMatID);

	glUniformMatrix4fv(modelMatID, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewMatID, 1, GL_FALSE, glm::value_ptr(CVar::vue));
	glUniformMatrix4fv(projectionMatID, 1, GL_FALSE, glm::value_ptr(CVar::projection));
	
	//Dessiner la grille:
	grille->dessiner();
}

// Construire la matrice MVP + dessiner le cube
void dessinerCube(void)
{
	// Création d'une matrice-modèle - bouge et déforme l'objet dans le référenciel du monde
	// Utilisez ici les fonctionnalités de GLM

	// Matrice effectuant une translation de (0,0,0):
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));

	// Matrice effectuant une mise à l'échelle de (1.0, 1.0, 1.0)
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

	// Matrice effectuant une rotation égale à 0.75*(le temps écoulé) autour de l'axe Y (up) en radians
	glm::mat4 rotationMatrix;
	if (CVar::isRotating)
	{
		rotationMatrix = glm::rotate(glm::mat4(1.0f), (glm::float32)glm::radians(0.75f * CVar::temps), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// Caluler la matrice modèle (attention à l'ordre de multiplcations.:
	glm::mat4 model = rotationMatrix * scalingMatrix * translationMatrix;

	// Activer le nuanceur approprié
	progNuanceurCube.activer();

	//printf("%i", progNuanceurCube.getProg());

	

	// Fournir les trois matrices Modele / Vue / Projection au nuanceur, comme en type "uniforms"
	GLuint modelMatID = glGetUniformLocation(progNuanceurCube.getProg(), "model");
	GLuint viewMatID = glGetUniformLocation(progNuanceurCube.getProg(), "view");
	GLuint projectionMatID = glGetUniformLocation(progNuanceurCube.getProg(), "projection");

	//printf("%i, %i, %i\n", modelMatID, viewMatID, projectionMatID);

	glUniformMatrix4fv(modelMatID, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewMatID, 1, GL_FALSE, glm::value_ptr(CVar::vue));
	glUniformMatrix4fv(projectionMatID, 1, GL_FALSE, glm::value_ptr(CVar::projection));

	// Utiliser le VAO pour dessiner les triangles du cube:
	glBindVertexArray(cube_vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
	glDrawElements(GL_QUAD_STRIP, 12 * 3, GL_UNSIGNED_INT, NULL);
}

// Dessine tous les modèeles.
void dessinerScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	dessinerCube();
    dessinerGrille();

    // flush les derniers vertex du pipeline graphique
    glFlush();
}



///////////////////////////////////////////////////////////////////////////////
///  global public  clavier \n
///
///  fonction de rappel pour la gestion du clavier
///
///  @param [in]       pointeur GLFWwindow	Référence à la fenetre GLFW
///  @param [in]       touche	int			ID de la touche
///  @param [in]       scancode int			Code spécifique à la plateforme et à l'ID
///  @param [in]       action	int			Action appliquée à la touche
///  @param [in]       mods		int			Modifier bits
///
///  @return Aucune
///
///  @author Félix G. Harvey 
///  @date   2016-06-03
///
///////////////////////////////////////////////////////////////////////////////
void clavier(GLFWwindow* fenetre, int touche, int scancode, int action, int mods)
{

	switch (touche) {
	case GLFW_KEY_Q:{
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(fenetre, GL_TRUE);
		break;
	}
	case  GLFW_KEY_ESCAPE:{
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(fenetre, GL_TRUE);
		break;
	}
	case  GLFW_KEY_P:{
		if (action == GLFW_PRESS){
			if (CVar::isPerspective)
				CVar::isPerspective = false;
			else
				CVar::isPerspective = true;
		}
		break;
	}
	case  GLFW_KEY_R:{
		if (action == GLFW_PRESS){
			if (CVar::isRotating)
				CVar::isRotating = false;
			else
				CVar::isRotating = true;
		}
		break;
	}
	case  GLFW_KEY_I:{
		if (action == GLFW_PRESS){
			if (CVar::showDebugInfo)
				CVar::showDebugInfo = false;
			else
				CVar::showDebugInfo = true;
		}
		break;
	}
	case  GLFW_KEY_C:{
		if (action == GLFW_PRESS){
			if (CVar::mouseControl){
				CVar::mouseControl = false;
				glfwSetInputMode(fenetre, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}	
			else{
				CVar::mouseControl = true;
				glfwSetInputMode(fenetre, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			}		
		}

		break;
	}
	case  GLFW_KEY_1:{
		if (action == GLFW_PRESS){
			if (CVar::lumieres[ENUM_LUM::LumDirectionnelle]->estAllumee())
				CVar::lumieres[ENUM_LUM::LumDirectionnelle]->eteindre();
			else
				CVar::lumieres[ENUM_LUM::LumDirectionnelle]->allumer();
		}

		break;
	}
	
	}

}




///////////////////////////////////////////////////////////////////////////////
///  global public  redimensionnement \n
///
///  fonction de rappel pour le redimensionnement de la fenêtre graphique
///
///  @param [in]       w GLsizei    nouvelle largeur "w" en pixels
///  @param [in]       h GLsizei    nouvelle hauteur "h" en pixels
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void redimensionnement(GLFWwindow* fenetre, int w, int h)
{
    CVar::currentW = w;
    CVar::currentH = h;
    glViewport(0, 0, w, h);
    dessinerScene();
}


//////////////////////////////////////////////////////////
////////////  FONCTIONS POUR LA SOURIS ///////////////////
//////////////////////////////////////////////////////////

void mouvementSouris(GLFWwindow* window, double deltaT, glm::vec3& direction, glm::vec3& right, glm::vec3& up)
{
	if (CVar::mouseControl)
	{
		// Déplacement de la souris:
		// Taille actuelle de la fenetre
		int mid_width, mid_height;
		glfwGetWindowSize(window, &mid_width, &mid_height);
		mid_width /= 2;
		mid_height /= 2;

		// Get mouse position
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetCursorPos(window, mid_width, mid_height);

		// Nouvelle orientation
		horizontalAngle += vitesseSouris * deltaT * float(mid_width - xpos);
		verticalAngle += vitesseSouris * deltaT * float(mid_height - ypos);
	}
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector : perpendicular to both direction and right
	up = glm::cross(right, direction);
}

//////////////////////////////////////////////////////////
////////////  FONCTIONS POUR LA CAMÉRA ///////////////////
//////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  global public  rafraichirCamera \n
///
///  Fonction de gestion de la position de la caméra en coordonnées sphériques.
///  Elle s'occuper de trouver les coordonnées x et y de la caméra à partir 
///  des theta et phi courants, puis fixe dans openGL la position de la caméra
///  à l'aide de gluLookAt().
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void rafraichirCamera(GLFWwindow* fenetre, double deltaT)
{
	mouvementSouris(fenetre, deltaT, direction, cam_right, cam_up);

	// Move forward
	if (glfwGetKey(fenetre, GLFW_KEY_W) == GLFW_PRESS){
		cam_position += direction * (float)deltaT * vitesseCamera;
	}
	// Move backward
	if (glfwGetKey(fenetre, GLFW_KEY_S) == GLFW_PRESS){
		cam_position -= direction * (float)deltaT * vitesseCamera;
	}
	// Strafe right
	if (glfwGetKey(fenetre, GLFW_KEY_D) == GLFW_PRESS){
		cam_position += cam_right * (float)deltaT * vitesseCamera;
		}
	// Strafe left
	if (glfwGetKey(fenetre, GLFW_KEY_A) == GLFW_PRESS){
		cam_position -= cam_right * (float)deltaT * vitesseCamera;
	}

	//Matrice de projection:
	float ratio = (float)CVar::currentW / (float)CVar::currentH;
	if (CVar::isPerspective){
		// Caméra perspective: 
		// 
		
		CVar::projection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 1000.0f);
	}
	else
	{
		// Caméra orthographique :
		CVar::projection = glm::ortho(-5.0f*ratio, 5.0f*ratio,-5.0f, 5.0f, 0.0f, 1000.0f); // In world coordinates
	}

	// Matrice de vue:
	CVar::vue = glm::lookAt(
		cam_position,				// Position de la caméra
		cam_position + direction,   // regarde vers position + direction
		cam_up                  // Vecteur "haut"
		);
}


//////////////////////////////////////////////////////////
//////////  FONCTIONS POUR LES NUANCEURS /////////////////
//////////////////////////////////////////////////////////
void compilerNuanceurs () 
{
    // on compiler ici les programmes de nuanceurs qui furent prédéfinis
	progNuanceurCube.compilerEtLier();
	progNuanceurGrille.compilerEtLier();
}
