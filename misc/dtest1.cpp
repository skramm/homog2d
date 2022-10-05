/**
\file
\brief generates data for precision test

This program will run 1 million of the following experiment:<br>
- generate two random points in the range given by the first argument
- compute the line joining these two points
- compute the distance between the points and the line

Eventually, it will print several computed value:
-# range
-# min coordinate value
-# max coordinate value
-# mean distance value
-# max distance value
-# mean distance value (computed with the "running std" method)
-# standard deviation of distance value

Standard deviation is computed using a "single pass" algorithm,
see https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance

To run that experiment on several ranges use the provided target:
<br>
<pre>
$ make dtest1
</pre>
*/


//#define HOMOG2D_INUMTYPE long double
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

NUMTYPE getRandom()
{
	double r = (2.0*rand()/RAND_MAX -1.) * gk;
	grmin = std::min( std::abs(r), grmin );
	grmax = std::max( std::abs(r), grmax );
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
		<< grmin << sep
		<< grmax << sep
		<< sep << sum / nb
		<< sep << vmax
		<< sep << mean
		<< sep << sigma
		<< "\n";
}

