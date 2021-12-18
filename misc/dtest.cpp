



//#define HOMOG2D_INUMTYPE long double
#define HOMOG2D_INUMTYPE float
//#define HOMOG2D_INUMTYPE double

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
	NUMTYPE M2   = 0.;
	NUMTYPE mean = 0.;
	for( auto i=0; i<nb; i++ )
	{
		Point2d_<NUMTYPE> pt1( getRandom(), getRandom() );
		Point2d_<NUMTYPE> pt2( getRandom(), getRandom() );
		auto li = pt1 * pt2;

		NUMTYPE d = li.distTo(pt1);
		vmax = std::max( vmax, d );
		sum += d;

/*		auto delta = d - mean;
		mean += delta / nb;
        M2 += delta*( d - mean );
  */
		auto delta = d - mean;
		mean += delta / (i+1);
		auto delta2 = d - mean;
		M2 += delta * delta2;

	}
	auto sigma = M2 / (nb-1);
//	std::cout << std::scientific
//		<< "random coordinates: min=" << grmin << " max=" << grmax << " mean=" << grsum/nb/4. << "\n";
	std::cout << std::scientific
		<< "-results:"
		<< "mean1=" << sum / nb
		<< " max=" << vmax
		<< " mean2=" << mean
		<< " sigma=" << sigma
		<< " mean2/k=" << mean/gk
		<< " sigma/k=" << sigma/gk
		<< "\n";
}

