///////////////////////////////////////////////////////////////////////////////
///  @file Main.cpp
///
///  @brief    Cette application implémente un engin simple de lancer de rayons a 
///            des fins éducationnelles du cours "Infographie avancée - INF8702".
///            L'implémentation et le design original du lancer de rayons de Polytechnique
///            fut écrite par Yves Martel (2003).
///            Cette version est plus modulaire et est plus facile à maintenir et à comprendre
///            Elle fut également conçue de telle façon à pouvoir être portée facilement sur GPU
///            en GLSL. Une démo technique de la version "temps réel" est déjà disponible.
///            Vous n'avez qu'à demander à votre chargé de Labo.
///
///  @remarks  Les vieux fichiers de donnnées (.dat) ne fonctionne plus avec cette version.
///            Vous devrez utiliser les fichiers .dat fournis avec le présent projet jusqu'à ce
///            qu'un meilleur format de fichier soit implémenté. Un format simple comme OBJ est 
///            présentement considéré à cet égard.
///
///  @author  Olivier Dionne 
///  @date    13/08/2008
///  @version 1.1.0
///
///////////////////////////////////////////////////////////////////////////////
#if _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Cst.h"
#include "Var.h"
#include "NuanceurProg.h"

using namespace std;

GLuint quad_vbo;
GLuint quad_ibo;
GLuint quad_vao;
GLuint quad_size;

// les programmes de nuanceurs
CNuanceurProg progNuanceurQuad("Nuanceurs/quadSommets.glsl", "Nuanceurs/quadFragments.glsl", false);


// Déclarations des méthodes
void Initialiser         ( void );
void Redimensionner      (GLFWwindow* fenetre, int w, int h);
void Liberer             ( void );
void Dessiner            ( void );
void initialiserQuad	 (void);

int main( int argc, char *argv[] )
{
	GLFWwindow* fenetre;
    // Traiter les arguments d'entrée
    if( argc > 1 )
    {
        int XRes = CVar::g_LargeurFenetre;
        int YRes = CVar::g_HauteurFenetre;

        if( argc > 2 )
        {
            for( int i = 2; i < argc; i++ )
            {
                if( *argv[ i ] == '-' )
                {
                    switch( argv[ i ][ 1 ] )
                    {
                        case 'x':
                            XRes = atoi( argv[ ++i ] );
                            break;
                        case 'y':
                            YRes = atoi( argv[ ++i ] );
                            break;
                    }
                }
            }
        }

        // S'assurer que la résolution fournie est une puissance de deux.
        if( ( ( XRes - 1 ) & XRes ) || ( ( YRes - 1 ) & YRes ) )
        {
            cerr<< "[ERREUR]: Resolution " << XRes << "x" << YRes << " n'est pas une puissance de deux" << endl;
            system( "pause" );
            exit( 1 );
        }

        // Construire la scène
        CVar::g_GestionnaireDeScene = CScene::ObtenirInstance();

        // Ajuster la résolution de la scène
        CVar::g_LargeurFenetre = XRes;
        CVar::g_HauteurFenetre = YRes;
        CVar::g_GestionnaireDeScene->AjusterResolution( CVar::g_LargeurFenetre, CVar::g_HauteurFenetre );

        // Traiter le fichier de données de la scène
        cout << "[ETAT]: Traitement du fichier de donnees de la scene..." << endl;
        CVar::g_GestionnaireDeScene->TraiterFichierDeScene( argv[ 1 ] );

        cout << "[ETAT]: Initialisation de glfw..." << endl;
        
		// start GL context and O/S window using the GLFW helper library
		if (!glfwInit()) {
			fprintf(stderr, "ERREUR: impossible d'initialiser GLFW3\n");
			return 1;
		}

		fenetre = glfwCreateWindow(CVar::g_LargeurFenetre, CVar::g_HauteurFenetre, "INF8702 - Labo", NULL, NULL);
		if (!fenetre) {
			fprintf(stderr, "ERREUR: impossibe d'initialiser la fenêtre avec GLFW3\n");
			glfwTerminate();
			return 1;
		}
		glfwSetWindowPos(fenetre, 600, 100);
		
		// Rendre le contexte openGL courrant celui de la fenêtre
		glfwMakeContextCurrent(fenetre);

		// Combien d'updates d'écran on attend après l'appel à glfwSwapBuffers()
		// pour effectivement échanger les buffers et retourner
		glfwSwapInterval(1);

		// Définir la fonction de redimensionnement
		glfwSetWindowSizeCallback(fenetre, Redimensionner);
		
		// vérification de la version 4.X d'openGL
		glewInit();
		if (glewIsSupported("GL_VERSION_4_5"))
			printf("Pret pour OpenGL 4.5\n\n");
		else {
			printf("\nOpenGL 4.5 n'est pas supporte! \n");
			exit(1);
		}

		// Specifier le context openGL
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    }
    else
    {
        cerr << "[ERREUR]: Aucune fichier de scene ne fut passe en argument !" << endl;
        system( "pause" );
        exit( 1 );
    }

    // Initialiser OpenGL
    Initialiser();
    
	// Boucle principale
	while (!glfwWindowShouldClose(fenetre))
	{
		glfwPollEvents();

		// Afficher nos modèlests
		Dessiner();

		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(fenetre);

	}
	// close GL context and any other GLFW resources
	glfwTerminate();

    // Libérer les données de l'application
    Liberer();

    return EXIT_SUCCESS;
}

void Initialiser( void )
{
    // Activer le test de profondeur d'OpenGL
    glEnable( GL_DEPTH_TEST );
    
    // Définir la fonction du test de profondeur
    glDepthFunc( GL_LEQUAL );
    
	//Compiler et lier le nuanceur
	progNuanceurQuad.compilerEtLier();

	// Initialiser les vbo, ibo, vao nécessaires pour un quad.
	initialiserQuad();

    cout << "[ETAT]: Lancer de rayons..." << endl;

    // Obtenir le temps courant
#if _WIN32
    double Time;
    LARGE_INTEGER lTime, lFrequency;
	QueryPerformanceCounter( &lTime );
	QueryPerformanceFrequency( &lFrequency );
	Time = static_cast<double>( lTime.QuadPart ) / static_cast<double>( lFrequency.QuadPart );
#else
	timeval Time;
	gettimeofday( &Time, 0 );
#endif

    // Générer la scène par lancers de rayons
    CVar::g_GestionnaireDeScene->LancerRayons();

    // Calculer le temps pris
#if _WIN32
	QueryPerformanceCounter( &lTime );
	QueryPerformanceFrequency( &lFrequency );
	Time = static_cast<double>( lTime.QuadPart ) / static_cast<double>( lFrequency.QuadPart ) - Time;
#else
	timeval CurrentTime;
	gettimeofday( &CurrentTime, 0 );

	const double T1 = static_cast<double>( Time.tv_sec ) + static_cast<double>( Time.tv_usec / ( 1000 * 1000 ) );
	const double T2 = static_cast<double>( CurrentTime.tv_sec ) + static_cast<double>( CurrentTime.tv_usec / ( 1000 * 1000 ) );

	Time = T2 - T1;
#endif

   cout << "[ETAT]: Termine! --> Temps total de rendu : " << Time << " secondes" << endl;
}

void Redimensionner(GLFWwindow* fenetre, int w, int h)
{
    CVar::g_LargeurViewport = w;
    CVar::g_HauteurViewport = h;
    glViewport( 0, 0, w, h );
}

void Liberer( void )
{
    CVar::g_GestionnaireDeScene->LibererInstance();
    CVar::g_GestionnaireDeScene = NULL;
}

void initialiserQuad(void)
{
	float sommets[] = {-1,-1,0, 1,-1,0, 1,1,0, -1,1,0};
	unsigned int indices[] = {0,1,2,3};

	// Notre Vertex Array Object - VAO
	glGenVertexArrays(1, &quad_vao);
	// Un buffer pour les sommets
	glGenBuffers(1, &quad_vbo);
	// Un buffer pour les indices
	glGenBuffers(1, &quad_ibo);

	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ibo);
	int s_byte_size = sizeof(sommets);
	int i_byte_size = sizeof(indices);
	glBufferData(GL_ARRAY_BUFFER, s_byte_size, &sommets[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_byte_size, &indices[0], GL_STATIC_DRAW);

	quad_size = i_byte_size/sizeof(unsigned int);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (char *)NULL + (0));//texcoords
	glEnableVertexAttribArray(0);

}

void Dessiner( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	progNuanceurQuad.activer();

    glEnable( GL_TEXTURE_2D );

    // Lier la texture de scène au quad OpenGL
    glBindTexture( GL_TEXTURE_2D, CVar::g_GestionnaireDeScene->ObtenirTextureGL() );
    
	glBindVertexArray(quad_vao);
	glDrawElements(
		GL_QUADS,			// mode
		quad_size,			// count
		GL_UNSIGNED_INT,    // type
		(void*)0            // element array buffer offset
	);

    glDisable( GL_TEXTURE_2D );
}