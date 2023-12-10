/**
\file ellipse_speed_test.cpp
\brief test app dedicated to measure speed improvements when using HOMOG2D_OPTIMIZE_SPEED
*/
//#define HOMOG2D_DEBUGMODE
#include "../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	size_t c1 = 0;
	size_t c2 = 0;
	size_t c3 = 0;
	double range_center = 1000;
	double esize = 500;

	size_t nb_runs = 1E7;
	if( argc > 1 )
		nb_runs = std::atoi( argv[1] );

	std::srand( std::time(nullptr) );
	Point2d dummy_pt( range_center * std::rand() / RAND_MAX, range_center * std::rand() / RAND_MAX );

	std::cout << "-Running with n=" << nb_runs
	<< "\n-HOMOG2D_OPTIMIZE_SPEED: "
#ifdef HOMOG2D_OPTIMIZE_SPEED
		<< "YES"
#else
		<< "NO"
#endif
	<< "\n-HOMOG2D_NOCHECKS: "
#ifdef HOMOG2D_NOCHECKS
		<< "YES"
#else
		<< "NO"
#endif
		<< "\n Ellipse memory footprint=" << sizeof( Ellipse ) << '\n';

	for( size_t i=0; i<nb_runs; i++ )
	{
		auto x0 = range_center * std::rand() / RAND_MAX;
		auto y0 = range_center * std::rand() / RAND_MAX;
		auto a = esize * std::rand() / RAND_MAX + 1.;
		auto b = esize * std::rand() / RAND_MAX + 1.;
		auto angle = 2.0 * M_PI * std::rand() / RAND_MAX;
		Ellipse ell( x0, y0, a, b, angle );
//		std::cout << ell;
		if( dummy_pt.isInside( ell ) )
			c1++;
		auto bb = ell.getOBB();
		c2 += bb.size();
//		if( dummy_pt.isInside(bb) )
//			c2++;
		auto center = ell.getCenter();
		if( dummy_pt.distTo(center) < range_center )
			c3++;

	}
	std::cout << "Result: c1=" << c1 << " c2=" << c2 << " c3=" << c3 << '\n';
}
