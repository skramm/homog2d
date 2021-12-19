/**
\file
\brief generates data for precision test
*/


#define HOMOG2D_INUMTYPE long double
//#define HOMOG2D_INUMTYPE float
//#define HOMOG2D_INUMTYPE double

#include "../homog2d.hpp"
#include <limits>
#include <iostream>

#define NUMTYPE long double
//#define NUMTYPE float


using namespace h2d;

double gk = 1000.;

double grmin = std::numeric_limits<double>::max();
double grmax = std::numeric_limits<double>::min();
double grsum = 0.;

NUMTYPE getRandom()
{
	double r = (2.0*rand()/RAND_MAX -1.) * gk;
	grmin = std::min( std::abs(r), grmin );
	grmax = std::max( std::abs(r), grmax );
	grsum += r;
	return r;
}

int main( int argc, const char **argv )
{
	auto nb = 1E6; // nb of runs
	char sep = ';';
	if( argc>1)
		gk = std::atof( argv[1] );
//	std::cout << "Running with k=" << gk << "\n";

	std::srand( time(0) );
	long double sum  = 0.;
	long double vmax = 0.;
	long double M2   = 0.;
	long double mean = 0.;
	for( auto i=0; i<nb; i++ )
	{
		Point2d_<NUMTYPE> pt1( getRandom(), getRandom() );
		Point2d_<NUMTYPE> pt2( getRandom(), getRandom() );
		auto li = pt1 * pt2;

		long double d = li.distTo(pt1);
		vmax = std::max( vmax, d );
		sum += d;

		auto delta = d - mean;
		mean += delta / (i+1);
		auto delta2 = d - mean;
		M2 += delta * delta2;

	}
	auto sigma = M2 / (nb-1);
	std::cout << std::scientific
		<< gk << sep
//		<< "random coordinates: min=" << grmin << " max=" << grmax << " mean=" << grsum/nb/4. << "\n";
		 << grmin << sep
		 << grmax << sep
		 << grsum/nb/4.

//		<< "-results:"
		<< sep << sum / nb
		<< sep << vmax
		<< sep << mean
		<< sep << sigma
		<< "\n";
}

