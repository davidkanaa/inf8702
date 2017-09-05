///////////////////////////////////////////////////////////////////////////////
///  @file Texture2D.cpp
///  @author  Frédéric Plourde 
///  @brief   Définit la classe CTexture2D qui implémente une texture2D openGL
///  @date    2008-01-18
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#include "Texture2D.h"

///////////////////////////////////////////////////////////////////////////////
///  public constructor  CTexture2D \n
///  Construit une texture simple 2D (de type GL_TEXTURE_2D). Construit aussi
///  une texture de base abstraite
///
///  @param [in, out]  filename char *    le nom de fichier de la texture 2D
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CTexture2D::CTexture2D(char* filename, bool mipmap):
CTextureAbstraite(filename, GL_TEXTURE_2D, mipmap)
{
   chargerTexture();
}


///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CTexture2D \n
///  Construit une texture simple 2D (de type GL_TEXTURE_2D) à partir des données
///  brutes de type float (format RGB) et (type GL_FLOAT)
///
///  @param [in, out]  data vect3D *  le pointeur sur les données float R-G-B-R-G-B...
///  @param [in]       hauteur int    la hauteur de la texture
///  @param [in]       largeur int    la largeur de la texture
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-03-09
///
///////////////////////////////////////////////////////////////////////////////
CTexture2D::CTexture2D(vect3D* data, int hauteur, int largeur, bool mipmap):
CTextureAbstraite(GL_TEXTURE_2D, mipmap)
{
   // aiguiller openGL vers la zone mémoire contenant le graphisme de la texture
   // Futur TP...
}


///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CTexture2D \n
///  Destructeur par défaut d'une texture 2D
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CTexture2D::~CTexture2D(void)
{
}



///////////////////////////////////////////////////////////////////////////////
///  private virtual  chargerTexture \n
///  Charge en mémoire d'application la texture spécifiée au niveau d'openGL.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CTexture2D::chargerTexture()
{
   // Futur TP...
}