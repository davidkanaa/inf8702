///////////////////////////////////////////////////////////////////////////////
///  @file GrilleQuads.cpp
///  @author  Félix Harvey
///  @brief   Définit la classe CGrilleQuads qui encapsule les face d'un cube, 
///			  ou le mesh du gazon, ou autre.
///  @date    2016
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include "GrilleQuads.h"

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CGrilleQuads \n
///  constructeur d'un modèle 3D avec 1 seule texture
///
///  @param [in, out]  nomsFichierOBJ char *	    La chaîne de caractères contenant le nom de fichier OBJ du modèle 3D
///  @param [in, out]  nomFichierTexture char *     l'unique nom de fichier de l'unique texture
///  @param [in]       echelle float [=1.000000]    mise à l'échelle qui sera appliquée automatique (glScale)
///
///  @return Aucune
///
///  @author Félix G. Harvey
///  @date   2016
///
///////////////////////////////////////////////////////////////////////////////
CGrilleQuads::CGrilleQuads(char * nomFichierTexture, float largeur, float hauteur, int nbColonnes, int nbRangees, float echelle) :
CModele3DAbstrait(echelle, nomFichierTexture, TYPE_TEXTURE2D, true, false, false)
{
	// initialisations...
	largeur_ = largeur;
	hauteur_ = hauteur;
	nbColonnes_ = nbColonnes;
	nbRangees_ = nbRangees;

	creerLaGrille(largeur_, hauteur_, nbRangees_, nbColonnes_, true);
}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CGrilleQuads \n
///  constructeur d'un modèle 3D avec plusieurs
///
///  @param[in, out]  nomsFichiersTextures std::vector<char*> *    Le vecteur contenant la liste des noms de fichiers.Cette liste est copiée, alors l'appelant peut en disposer.
///  @param [in]      largeur float la largeur en unitées du monde de la grille
///  @param [in]      hauteur float La hauteur en unitées du monde de la grille
///  @param [in]      nbColonnes int Le nombre de colonnes de quads de la grille
///  @param [in]      nbRangees int Le nombre de rangées de quads de la grille
///  @param [in]      echelle float Le "scaling" du modèle
///
///  @return Aucune
///
///  @author Félix G. Harvey
///  @date   2016
///
///////////////////////////////////////////////////////////////////////////////
CGrilleQuads::CGrilleQuads(std::vector<char*> *nomsFichiersTextures, float largeur, float hauteur, int nbColonnes, int nbRangees, float echelle) :
CModele3DAbstrait(echelle, nomsFichiersTextures, TYPE_TEXTURE2D, true, false, false)
{
	// initialisations...
	largeur_ = largeur;
	hauteur_ = hauteur;
	nbColonnes_ = nbColonnes;
	nbRangees_ = nbRangees;

	creerLaGrille(largeur_, hauteur_, nbRangees_, nbColonnes_, true);
}


///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CGrilleQuads \n
///  Destructeur par défaut.
///
///  @return Aucune
///
///  @author Félix G. Harvey
///  @date   2016
///
///////////////////////////////////////////////////////////////////////////////
CGrilleQuads::~CGrilleQuads(void)
{
}


///////////////////////////////////////////////////////////////////////////////
///  public void creerLaGrille\n
///  Créer les sommets de la grille et sauvegarde les infos utiles au dessin dans un VAO
///
///  @param [in]      largeur float la largeur en unitées du monde de la grille
///  @param [in]      hauteur float La hauteur en unitées du monde de la grille
///  @param [in]      nbColonnes int Le nombre de colonnes de quads de la grille
///  @param [in]      nbRangees int Le nombre de rangées de quads de la grille
///  @param [in]      normaleVersExt bool Indicateur du sens des normales
///
///  @return Aucune
///
///  @author Félix G. Harvey
///  @date   2016
///
///////////////////////////////////////////////////////////////////////////////
void CGrilleQuads::creerLaGrille(float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt)
{
	//On overwrite nos sommets.
	m_sommets.clear();

	float normFact = normaleVersExt ? 1.f : -1.f;

	float startX = -largeur / 2.0f;
	float startY = hauteur / 2.0f;
	float s1, t1, s2, t2;

	float incrementX = largeur / (float)nbColonnes;
	float incrementY = hauteur / (float)nbRangees;

	float val;
	for (int i = 0; i<nbColonnes; i++) {
		for (int k = 0; k<nbRangees; k++) {

			s1 = (i*incrementX) / largeur;
			s2 = ((i + 1)*incrementX) / largeur;
			t1 = (k*incrementY) / hauteur;
			t2 = ((k + 1)*incrementY) / hauteur;

			// Texture
			m_sommets.push_back(s1);
			m_sommets.push_back(t1);
			// Normale
			m_sommets.push_back(0.0f);
			m_sommets.push_back(0.0f);
			m_sommets.push_back(normFact);
			// Position
			m_sommets.push_back(startX + i*incrementX);
			m_sommets.push_back(startY - k*incrementY);
			m_sommets.push_back(0.0f);

			m_sommets.push_back(s1);
			m_sommets.push_back(t2);

			m_sommets.push_back(0.0f);
			m_sommets.push_back(0.0f);
			m_sommets.push_back(normFact);

			m_sommets.push_back(startX + i*incrementX);
			m_sommets.push_back((startY - k*incrementY) - incrementY);
			m_sommets.push_back(0.0f);

			m_sommets.push_back(s2);
			m_sommets.push_back(t2);

			m_sommets.push_back(0.0f);
			m_sommets.push_back(0.0f);
			m_sommets.push_back(normFact);

			m_sommets.push_back((startX + i*incrementX) + incrementX);
			m_sommets.push_back((startY - k*incrementY) - incrementY);
			m_sommets.push_back(0.0f);

			m_sommets.push_back(s2);
			m_sommets.push_back(t1);

			m_sommets.push_back(0.0f);
			m_sommets.push_back(0.0f);
			m_sommets.push_back(normFact);

			m_sommets.push_back((startX + i*incrementX) + incrementX);
			m_sommets.push_back(startY - k*incrementY);
			m_sommets.push_back(0.0f);
		}
	}

	// Un dummy IBO 
	std::vector<int> indices(m_sommets.size() / 8);
	for (int i = 0; i < m_sommets.size() / 8; i++)
	{
		indices[i] = i;
	}

	// printf("%i", indices.size());

	//
	glGenVertexArrays(1, &m_quads_vao);
	glBindVertexArray(m_quads_vao);

	// Générer les buffers:
	glGenBuffers(1, &m_sommets_vbo);
	glGenBuffers(1, &m_quads_ibo);

	// Lier (dans le bon ordre) et remplir les buffers:
	glBindBuffer(GL_ARRAY_BUFFER, m_sommets_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_sommets.size() * sizeof(float), &m_sommets[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_quads_ibo);
	glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	// Créer les pointeurs d'attributs, activer les bons attributs par rapport au nuanceur
	glBindBuffer(GL_ARRAY_BUFFER, m_sommets_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		2,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(GLvoid*)0
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(GLvoid*)(2 * sizeof(float))
	);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(GLvoid*)(5 * sizeof(float))
	);

}

///////////////////////////////////////////////////////////////////////////////
///  public void dessiner\n
///  Dessine la grille en utilisant son VAO.
///
///  @return Aucune
///
///  @author Félix G. Harvey
///  @date   2016
///
///////////////////////////////////////////////////////////////////////////////
void CGrilleQuads::dessiner()
{
	// Utiliser le VAO pour dessiner des quads:
	glBindVertexArray(m_quads_vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quads_ibo);
	glDrawElements(GL_QUADS, m_sommets.size() / 8, GL_UNSIGNED_INT, NULL);
}