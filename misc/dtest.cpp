



#define HOMOG2D_INUMTYPE long double
//#define HOMOG2D_INUMTYPE float
#define HOMOG2D_INUMTYPE double

#include "../homog2d.hpp"
#include <limits>
#include <iostream>

#define NUMTYPE HOMOG2D_INUMTYPE


using namespace h2d;

double gk = 1000.;

NUMTYPE grmin = std::numeric_limits<NUMTYPE>::max();
NUMTYPE grmax = std::numeric_limits<NUMTYPE>::min();
NUMTYPE grsum = 0.;

NUMTYPE getRandom()
{
	NUMTYPE r = (2.0*rand()/RAND_MAX -1.) * 1.0*rand()/RAND_MAX*gk;
	grmin = std::min( r, grmin );
	grmax = std::max( r, grmax );
	grsum += r;
	return r;
}

int main( int argc, const char **argv )
{
	auto nb = 1E6;

	if( argc>1)
		gk = std::atof( argv[1] );
	std::cout << "Running with k=" << gk << "\n";
	std::srand( time(0) );
	NUMTYPE sum  = 0.;
	NUMTYPE vmax = 0.;

	for( auto i=0; i<nb; i++ )
	{
		Point2d_<NUMTYPE> pt1( getRandom(), getRandom() );
		Point2d_<NUMTYPE> pt2( getRandom(), getRandom() );
		auto li = pt1 * pt2;

		NUMTYPE d = li.distTo(pt1);
		vmax = std::max( vmax, d );
		sum += d;
	}

	std::cout << std::scientific
		<< "random values: min=" << grmin << " max=" << grmax << " mean=" << grsum/nb/4. << "\n";
	std::cout << std::scientific
		<< "-results:"
		<< "mean=" << sum / nb
		<< " max=" << vmax
		<< "\n";
}

