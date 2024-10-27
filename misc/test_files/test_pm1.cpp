/**
\file
\brief WIP: a test file used to test the polygon minimisation algorithm

build with: $ make test_pm
*/
#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;
int main()
{
	std::vector<Point2d> v1{
		Point2d( 0,0),
		Point2d( 10,1),
		Point2d( 20,0),
		Point2d( 30, 2),
		Point2d( 40,0),
	};

	OPolyline poly_o( v1 );
	poly_o.minimize();
	std::cout << "\n AFTER poly_o size=" << poly_o.size() << '\n';
	std::cout << poly_o << '\n';

	CPolyline poly_c( v1 );
	poly_c.minimize();
	std::cout << "\n AFTER poly_c size=" << poly_c.size() << '\n';
	std::cout << poly_c << '\n';
}
