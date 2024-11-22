/**
\file
\brief WIP: a test file used to test the polygon minimisation algorithm

build with: $ make test_pm
*/
#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;


void process( const std::vector<Point2d>& vec, PminimMetric metric )
{
	PolyMinimParams par;
	std::cout << "metric: " << getString( metric ) << "\n";
	CPolyline cpol(vec);
	CPolyline opol(vec);
	par.setMetric( metric );
	cpol.minimize(par);
	std::cout << "Closed: pol=" << cpol << "\n";

	opol.minimize(par);
	std::cout << "Open: pol=" << opol << "\n";
}

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
		std::vector<Point2d> v1{ {0,1},{1,1},{2,1} };
		process( v1, PminimMetric::AbsDistance );
		process( v1, PminimMetric::Angle );
		process( v1, PminimMetric::RelDistance );
		process( v1, PminimMetric::TriangleArea );
/*		CPolyline p1(v1);
		p1.minimize();
		std::cout << "p1=" << p1 << "\n";

		CPolyline p2(v1);
		PolyMinimParams par;
		par.setMetric( PminimMetric::AbsDistance );
		p2.minimize(par);
		std::cout << "p2=" << p2 << "\n";
*/
	}
}
