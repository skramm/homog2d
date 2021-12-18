

//#define HOMOG2D_INUMTYPE long double
//#define HOMOG2D_INUMTYPE float
#define HOMOG2D_INUMTYPE double

#include "../homog2d.hpp"

#include <limits>
#include <iostream>

#define NUMTYPE double

using namespace h2d;

double gk = 1000.;

NUMTYPE grmin = 1000; //std::numeric_limits<NUMTYPE>::max();
NUMTYPE grmax = 0. //std::numeric_limits<NUMTYPE>::min();
NUMTYPE grsum = 0.

NUMTYPE getRandom()
{
	NUMTYPE r = 1.0*rand()/RAND_MAX * 1.0*rand()/RAND_MAX*gk;
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
	NUMTYPE sum1 = 0.;
	NUMTYPE sum2 = 0.;
	NUMTYPE vmax1 = 0.;
	NUMTYPE vmax2 = 0.;

	for( auto i=0; i<nb; i++ )
	{
		Point2d_<NUMTYPE> pt1( getRandom(), getRandom() );
		Point2d_<NUMTYPE> pt2( getRandom(), getRandom() );
		auto li = pt1 * pt2;

		NUMTYPE d1 = li.distTo(pt1);
		NUMTYPE d2 = li.distTo(pt2);
		vmax1 = std::max( vmax1, d1 );
		vmax2 = std::max( vmax2, d2 );
		sum1 += d1;
		sum2 += d2;

	}


	std::cout << std::scientific
		<< "random values: min=" << grmin << " max=" << grmax << " mean=" << grsum/nb/4. << "\n";
	std::cout << std::scientific
		<< "-results:"
		<< "mean_1=" << sum1 / nb
		<< " mean_2=" << sum2 / nb
		<< " max_1=" << vmax1
		<< " max_2=" << vmax2
		<< "\n";
}

