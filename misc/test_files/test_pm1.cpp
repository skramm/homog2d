/**
\file
\brief WIP: a test file used to test the polygon minimisation algorithm

build with: $ make test_pm
*/
//#define HOMOG2D_DEBUGMODE
#define HOMOG2D_ENABLE_VRTP
#include "../../homog2d.hpp"

using namespace h2d;

static size_t count = 0;

template<typename T>
void process2( const T& pol_in, PminimMetric metric, const T& pol_out )
{
	PolyMinimParams par;
	par.setMetric( metric );

	T pol( pol_in );
	minimize( pol, par );
	count++;

	std::cout << "test " << count << ", metric=" << getString( metric ) << ": ";

	if( pol != pol_out )
		std::cout << "fail\n -input=" << pol_in
			<< "\n -result=" << pol
			<< "\n -expected=" << pol_out
			<< "\n" << par
			<< "\n";
	else
		std::cout << "success!\n";
}
void process1( const std::vector<Point2d>& vec_in, PminimMetric metric, const std::vector<Point2d>& vec_out )
{
	{
		CPolyline pol_in(vec_in);
		CPolyline pol_out(vec_out);
		process2( pol_in, metric, pol_out );
	}
	{
		OPolyline pol_in(vec_in);
		OPolyline pol_out(vec_out);
		process2( pol_in, metric, pol_out );
	}
}

template<typename T>
void process( const T& v1, const T& v0 )
{
	process1( v1, PminimMetric::AbsDistance,  v0 );
	process1( v1, PminimMetric::RelDistance,  v0 );
	process1( v1, PminimMetric::Angle,        v0 );
	process1( v1, PminimMetric::TriangleArea, v0 );
}

int main()
{
	{
		std::vector<Point2d> v1{ {0,0},{4,0.1},{8,0} };
		std::vector<Point2d> v0{ {0,0},{8,0} };
		process( v1, v0 );
	}
	{
		std::vector<Point2d> v1{ {0,1},{1,1.1},{2,1}, {3,1} };
		std::vector<Point2d> v0{ {0,1},{3,1} };
		process( v1, v0 );
	}
	{
		std::vector<Point2d> v1{ {0,0},{4,0.1},{8,0} };
		CPolyline cpol(v1);
		CommonType cc = cpol;
		minimize( cc );
		CPolyline resc = fct::VariantUnwrapper{cc};
		std::cout << "Variant-closed:" << resc << "\n";

		OPolyline opol(v1);
		CommonType co = opol;
		minimize( co );
		OPolyline reso = fct::VariantUnwrapper{co};
		std::cout << "Variant-open:" << reso << "\n";
	}
}
