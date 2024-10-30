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
#if 0
	{
	std::vector<Point2d> v1{
		Point2d( 5,0),
		Point2d( 10,1),
		Point2d( 20,0),
		Point2d( 30, 2),
		Point2d( 40,0),
	};

	OPolyline poly_o( v1 );
	poly_o.minimize();
	std::cout << "\n AFTER poly_o1 size=" << poly_o.size() << '\n';
	std::cout << poly_o << '\n';

	OPolyline poly_o2( v1 );
	PolyMinimParams p;
	p.setStopCrit( PminimStopCrit::NoStop ).setMetric( PminimMetric::AbsDistance ).setThreshold( 10 );
	poly_o2.minimize(p);
	std::cout << "\n AFTER poly_o2 size=" << poly_o2.size() << '\n';
	std::cout << poly_o2 << '\n';


	CPolyline poly_c( v1 );
	poly_c.minimize();
	std::cout << "\n AFTER poly_c size=" << poly_c.size() << '\n';
	std::cout << poly_c << '\n';
	}
	{
		FRect r1(0,0,2,2);
		CPolyline p1(r1);
		p1.minimize();
		std::cout << "p1=" << p1 << "\n";
	}

	{
		CPolyline pl(
			std::vector<Point2d>{
				{0,1},{1,1},{2,1}
			}
		);
		pl.minimize();
		std::cout << "pl=" << pl << "\n";
	}
#endif
	{
		CPolyline pl(
			std::vector<Point2d>{
				{0,1},{1,1},{3,2}
			}
		);
		pl.minimize();
		std::cout << "pl=" << pl << "\n";
	}
}
