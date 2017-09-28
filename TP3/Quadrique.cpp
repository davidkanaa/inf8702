#include "Quadrique.h"

using namespace Scene;

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CQuadrique \n
///  Description : Constructeur par défaut
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique::CQuadrique( void )
    : ISurface     (                  )
    , m_Quadratique( CVecteur3::ZERO  )
    , m_Lineaire   ( CVecteur3::ZERO  )
    , m_Mixte      ( CVecteur3::ZERO  )
    , m_Cst        ( RENDRE_REEL( 0 ) )
{}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CQuadrique \n
///  Description : Constructeur par défaut
///
///  @param [in]       Quadric const Scene::CQuadrique &   la quadrique à copier
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique::CQuadrique( const CQuadrique& Quadric )
    : ISurface     ( Quadric               )
    , m_Quadratique( Quadric.m_Quadratique )
    , m_Lineaire   ( Quadric.m_Lineaire    )
    , m_Mixte      ( Quadric.m_Mixte       )
    , m_Cst        ( Quadric.m_Cst         )
{}

///////////////////////////////////////////////////////////////////////////////
///  public virtual destructor  ~CQuadrique \n
///  Description : Destructeur
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique::~CQuadrique( void )
{
}

///////////////////////////////////////////////////////////////////////////////
///  public  operator = \n
///  Description : Opérateur de copie
///
///  @param [in]       Quadric const Scene::CQuadrique &    La quadrique à copier
///
///  @return Scene::CQuadrique & La quadrique modifiée
///
///  @author Olivier Dionne 
///  @date   14/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique& CQuadrique::operator = ( const CQuadrique& Quadric )
{
    ISurface::operator =( Quadric );
    m_Quadratique = Quadric.m_Quadratique;
    m_Lineaire    = Quadric.m_Lineaire;
    m_Mixte       = Quadric.m_Mixte;
    m_Cst         = Quadric.m_Cst;
    return ( *this );
}

///////////////////////////////////////////////////////////////////////////////
///  protected virtual constant  AfficherInfoDebug \n
///  Description : Implémente le déboguage polymorphique par flux de sortie
///
///  @param [in, out]  Out std::ostream &    Le flux de sortie
///
///  @return std::ostream & Le flux de sortie modifié
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
ostream& CQuadrique::AfficherInfoDebug( ostream& Out ) const
{
    Out << "[DEBUG]: Quadric.Quadratique       = " << m_Quadratique << endl;
    Out << "[DEBUG]: Quadric.Lineaire          = " << m_Lineaire    << endl;
    Out << "[DEBUG]: Quadric.Mixte             = " << m_Mixte       << endl;
    Out << "[DEBUG]: Quadric.Constante         = " << m_Cst;
    return Out;
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual  Pretraitement \n
///  Description : Pretraitement des données de la quadrique( appelé AVANT le lancer)
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CQuadrique::Pretraitement( void )
{
   // Algorithme tiré de ... 
   // R. Goldman, "Two Approach to a Computer Model for Quadric Surfaces",
   // IEEE CG&A, Sept 1983, pp.21
   
    REAL A = m_Quadratique.x;
	REAL B = m_Quadratique.y;
    REAL C = m_Quadratique.z;
    REAL D = m_Mixte.z    * RENDRE_REEL( 0.5 );
	REAL E = m_Mixte.x    * RENDRE_REEL( 0.5 );
	REAL F = m_Mixte.y    * RENDRE_REEL( 0.5 );
	REAL G = m_Lineaire.x * RENDRE_REEL( 0.5 );
	REAL H = m_Lineaire.y * RENDRE_REEL( 0.5 );
	REAL J = m_Lineaire.z * RENDRE_REEL( 0.5 );
	REAL K = m_Cst;

	CMatrice4 Q( A, D, F, G,
			     D, B, E, H,
			     F, E, C, J,
			     G, H, J, K );

    CMatrice4 Inverse = m_Transformation.Inverse();

    Q = Inverse * Q * Inverse.Transpose();

    m_Quadratique.x = Q[ 0 ][ 0 ];
    m_Quadratique.y = Q[ 1 ][ 1 ];
	m_Quadratique.z = Q[ 2 ][ 2 ];
	m_Cst           = Q[ 3 ][ 3 ];
	m_Mixte.x       = Q[ 1 ][ 2 ] * RENDRE_REEL( 2.0 );
	m_Mixte.y       = Q[ 0 ][ 2 ] * RENDRE_REEL( 2.0 );
	m_Mixte.z       = Q[ 0 ][ 1 ] * RENDRE_REEL( 2.0 );
	m_Lineaire.x    = Q[ 0 ][ 3 ] * RENDRE_REEL( 2.0 );
	m_Lineaire.y    = Q[ 1 ][ 3 ] * RENDRE_REEL( 2.0 );
    m_Lineaire.z    = Q[ 2 ][ 3 ] * RENDRE_REEL( 2.0 );
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual  Intersection \n
///  Description : Effectue l'intersection Rayon/Quadrique
///
///  @param [in]       Rayon const CRayon &    Le rayon à tester
///
///  @return Scene::CIntersection Le résultat de l'ntersection
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CIntersection CQuadrique::Intersection( const CRayon& Rayon )
{
	CIntersection Result;

	// TODO: À COMPLÉTER ...

	// La référence pour l'algorithme d'intersection des quadriques est : 
	// Eric Haines, Paul Heckbert "An Introduction to Rayon Tracing",
	// Academic Press, Edited by Andrw S. Glassner, pp.68-73 & 288-293

	// S'il y a collision, ajuster les variables suivantes de la structure intersection :
	// Normale, Surface intersectée et la distance

#if 1

	// light ray origin coordinates
	CVecteur3 origin = Rayon.ObtenirOrigine();
	REAL x0 = origin.x;
	REAL y0 = origin.y;
	REAL z0 = origin.z;

	// light ray direction
	CVecteur3 direction = Rayon.ObtenirDirection();
	REAL xd = direction.x;
	REAL yd = direction.y;
	REAL zd = direction.z;

	// quadric's matrix coefficients
	//
	REAL A = m_Quadratique.x;
	REAL E = m_Quadratique.y;
	REAL H = m_Quadratique.z;
	//
	REAL B = m_Mixte.z;
	REAL C = m_Mixte.y;
	REAL F = m_Mixte.x;
	//
	REAL D = m_Lineaire.x;
	REAL G = m_Lineaire.y;
	REAL I = m_Lineaire.z;
	//
	REAL J = m_Cst;

	// intersection coefficients
	REAL aq = A*xd*xd + E*yd*yd + H*zd*zd + B*xd*yd + C*xd*zd + F*yd*zd;
	REAL bq = (2.0f*A*x0*xd + B*(x0*yd + xd*y0) + C*(x0*zd + xd*z0) + D*xd + 2.0f*E*y0*yd + F*(y0*zd + yd*z0) + G*yd + 2.0f*H*z0*zd + I*zd);
	REAL cq = A*x0*x0 + B*x0*y0 + C*x0*z0 + D*x0 + E*y0*y0 + F*y0*z0 + G*y0 + H*z0*z0 + I*z0 + J;

	// determine t
	REAL t;
	if (aq == EPSILON)
	{
		t = Max<REAL>(0.0f, -cq / bq);
		// t = -cq / bq;
	}
	else
	{
		REAL delta = bq*bq - 4.0f*aq*cq;

		if (delta < 0.0f)
		{
			// no solution
			return Result;
		}

		REAL sqrtDelta = sqrt(delta);

		REAL t0, t1;
		t0 = (-bq + sqrtDelta) / (2.0f * aq);
		t1 = (-bq - sqrtDelta) / (2.0f * aq);

		if (t0 <= EPSILON && t1 <= EPSILON)
		{
			// no solution
			return Result;
		}
		else if (t0 <= EPSILON && t1 > EPSILON)
		{
			t = t1;
		}
		else if (t0 > EPSILON && t1 <= EPSILON)
		{
			t = t0;
		}
		else
		{
			t = Min<REAL>(t0, t1);
		}
		
	}

	// compute intersection point
	CVecteur3 intersect(origin + t*direction);

	// compute the normal
	CVecteur3 normal( 
					  ( 2.0f*A*intersect.x + B*intersect.y + C*intersect.z + D), 
					  ( B*intersect.x + 2.0f*E*intersect.y + F*intersect.z + G),
					  ( C*intersect.x + F*intersect.y + 2.0f*H*intersect.z + I)
					);

	normal = CVecteur3::Normaliser(normal);

	//
	Result.AjusterSurface(this);
	Result.AjusterNormale(normal);
	Result.AjusterDistance(abs(t));

# endif
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual constant  Copier \n
///  Description : Alloue une copie de la quadrique courante
///
///  @return Scene::CQuadrique *la copie de la quadrique
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique* CQuadrique::Copier( void ) const
{
    return new CQuadrique( *this );
}