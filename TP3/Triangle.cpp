#include "Triangle.h"

using namespace Scene;

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CTriangle \n
///  Description : Constructeur par d�faut
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CTriangle::CTriangle( void )
    : ISurface (                 )
    , m_Normale( CVecteur3::ZERO )
{}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CTriangle \n
///  Description : Constructeur par d�faut
///
///  @param [in]       Triangle const Scene::CTriangle &    Le triangle � copier
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CTriangle::CTriangle( const CTriangle& Triangle )
    : ISurface ( Triangle           )
    , m_Normale( Triangle.m_Normale )
{
    for( int i = 0; i < 3; i++ )
        m_Pts[ i ] = Triangle.m_Pts[ i ];
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual destructor  ~CTriangle \n
///  Description : Destructeur
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CTriangle::~CTriangle( void )
{
}

///////////////////////////////////////////////////////////////////////////////
///  public  operator = \n
///  Description : Op�rateur de copie
///
///  @param [in]       Triangle const Scene::CTriangle &    Le triangle � copier
///
///  @return Scene::CTriangle & Le triangle modifi�
///
///  @author Olivier Dionne 
///  @date   14/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CTriangle& CTriangle::operator = ( const CTriangle& Triangle )
{
    ISurface::operator =( Triangle );
    m_Normale = Triangle.m_Normale;

    for( int i = 0; i < 3; i++ )
        m_Pts[ i ] = Triangle.m_Pts[ i ];

    return ( *this );
}

///////////////////////////////////////////////////////////////////////////////
///  protected virtual constant  AfficherInfoDebug \n
///  Description : Impl�mente le d�boguage polymorphique par flux de sortie
///
///  @param [in, out]  Out std::ostream &   Le flux de sortie
///
///  @return std::ostream & Le flux de sortie modifi�
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
ostream& CTriangle::AfficherInfoDebug( ostream& Out ) const
{
    Out << "[DEBUG]: Triangle.Point1         = " << m_Pts[ 0 ] << endl;
    Out << "[DEBUG]: Triangle.Point2         = " << m_Pts[ 1 ] << endl;
    Out << "[DEBUG]: Triangle.Point3         = " << m_Pts[ 2 ] << endl;
    Out << "[DEBUG]: Triangle.Normale        = " << m_Normale;
    return Out;
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual  Pretraitement \n
///  Description : Pretraitement des donn�es du triangle ( Appel� AVANT le lancer)
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CTriangle::Pretraitement( void )
{
    for( int i = 0; i < 3; i++ )
        m_Pts[ i ] = m_Pts[ i ] * m_Transformation;
    CalculerNormale();
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual  Intersection \n
///  Description : Effectue l'intersection Rayon/Triangle
///
///  @param [in]       Rayon const CRayon &    Le rayon � tester
///
///  @return Scene::CIntersection Le r�sultat de l'ntersection
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CIntersection CTriangle::Intersection( const CRayon& Rayon )
{
	CIntersection Result;

	// � COMPL�TER ... 

	// Voici deux r�f�rences pour acomplir le d�veloppement :
	// 1) Tomas Akenine-Moller and Eric Haines "Real-Time Rendering 2nd Ed." 2002, p.581
	// 2) Son article: http://www.graphics.cornell.edu/pubs/1997/MT97.pdf

	// Notez que la normale du triangle est d�j� calcul�e lors du pr�traitement
	// il suffit que de la passer � la structure d'intersection.

    return Result;
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual constant  Copier \n
///  Description : Alloue une copie du triangle courant
///
///  @return Scene::CTriangle * Nouvelle copie du triangle 
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CTriangle* CTriangle::Copier( void ) const
{
    return new CTriangle( *this );
}

///////////////////////////////////////////////////////////////////////////////
///  private  CalculerNormale \n
///  Description : Calculer la normale du triangle � partir des c�t�s
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CTriangle::CalculerNormale( void )
{
    CVecteur3 Edge1 = m_Pts[ 1 ] - m_Pts[ 0 ];
    CVecteur3 Edge2 = m_Pts[ 2 ] - m_Pts[ 0 ];
    m_Normale = CVecteur3::Normaliser( CVecteur3::ProdVect( Edge1, Edge2 ) );
}
