///////////////////////////////////////////////////////////////////////////////
///  @file Main.cpp
///
///  @brief    Cette application impl�mente un engin simple de lancer de rayons a 
///            des fins �ducationnelles du cours "Infographie avanc�e - INF8702".
///            L'impl�mentation et le design original du lancer de rayons de Polytechnique
///            fut �crite par Yves Martel (2003).
///            Cette version est plus modulaire et est plus facile � maintenir et � comprendre
///            Elle fut �galement con�ue de telle fa�on � pouvoir �tre port�e facilement sur GPU
///            en GLSL. Une d�mo technique de la version "temps r�el" est d�j� disponible.
///            Vous n'avez qu'� demander � votre charg� de Labo.
///
///  @remarks  Les vieux fichiers de donnn�es (.dat) ne fonctionne plus avec cette version.
///            Vous devrez utiliser les fichiers .dat fournis avec le pr�sent projet jusqu'� ce
///            qu'un meilleur format de fichier soit impl�ment�. Un format simple comme OBJ est 
///            pr�sentement consid�r� � cet �gard.
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


// D�clarations des m�thodes
void Initialiser         ( void );
void Redimensionner      (GLFWwindow* fenetre, int w, int h);
void Liberer             ( void );
void Dessiner            ( void );
void initialiserQuad	 (void);

int main( int argc, char *argv[] )
{
	GLFWwindow* fenetre;
    // Traiter les arguments d'entr�e
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

        // S'assurer que la r�solution fournie est une puissance de deux.
        if( ( ( XRes - 1 ) & XRes ) || ( ( YRes - 1 ) & YRes ) )
        {
            cerr<< "[ERREUR]: Resolution " << XRes << "x" << YRes << " n'est pas une puissance de deux" << endl;
            system( "pause" );
            exit( 1 );
        }

        // Construire la sc�ne
        CVar::g_GestionnaireDeScene = CScene::ObtenirInstance();

        // Ajuster la r�solution de la sc�ne
        CVar::g_LargeurFenetre = XRes;
        CVar::g_HauteurFenetre = YRes;
        CVar::g_GestionnaireDeScene->AjusterResolution( CVar::g_LargeurFenetre, CVar::g_HauteurFenetre );

        // Traiter le fichier de donn�es de la sc�ne
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
			fprintf(stderr, "ERREUR: impossibe d'initialiser la fen�tre avec GLFW3\n");
			glfwTerminate();
			return 1;
		}
		glfwSetWindowPos(fenetre, 600, 100);
		
		// Rendre le contexte openGL courrant celui de la fen�tre
		glfwMakeContextCurrent(fenetre);

		// Combien d'updates d'�cran on attend apr�s l'appel � glfwSwapBuffers()
		// pour effectivement �changer les buffers et retourner
		glfwSwapInterval(1);

		// D�finir la fonction de redimensionnement
		glfwSetWindowSizeCallback(fenetre, Redimensionner);
		
		// v�rification de la version 4.X d'openGL
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

		// Afficher nos mod�lests
		Dessiner();

		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(fenetre);

	}
	// close GL context and any other GLFW resources
	glfwTerminate();

    // Lib�rer les donn�es de l'application
    Liberer();

    return EXIT_SUCCESS;
}

void Initialiser( void )
{
    // Activer le test de profondeur d'OpenGL
    glEnable( GL_DEPTH_TEST );
    
    // D�finir la fonction du test de profondeur
    glDepthFunc( GL_LEQUAL );
    
	//Compiler et lier le nuanceur
	progNuanceurQuad.compilerEtLier();

	// Initialiser les vbo, ibo, vao n�cessaires pour un quad.
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

    // G�n�rer la sc�ne par lancers de rayons
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

    // Lier la texture de sc�ne au quad OpenGL
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