/**
\file
\brief WIP: a test file used to test the polygon minimisation algorithm

build with: $ make test-pm
*/
//#define HOMOG2D_DEBUGMODE
#define HOMOG2D_ENABLE_VRTP
#include "../../homog2d.hpp"

using namespace h2d;
static size_t count2 = 0;

template<typename T>
bool process2(
	const T& pol_in, ///< input polyline (open or closed)
	PminimMetric metric,
	const T& pol_out ///< output expected polyline (open or closed)
)
{
	PolyMinimParams par;
	par.setMetric( metric );

	std::cout << "test " << count2 << ", type=" << (pol_in.isClosed()?"C":"O") << ", metric=" << getString( metric ) << ": ";

	T pol( pol_in );
	minimize( pol, par );
	count2++;

	if( pol != pol_out )
	{
		std::cout << "fail\n -input=" << pol_in
			<< "\n -result=" << pol
			<< "\n -expected=" << pol_out
			<< "\n" << par;
		return false;
	}
	std::cout << "success!\n";
	return true;
}

template<typename T>
void
process1(
	const T&            vec_in,   ///< input set of points
	PminimMetric        metric,   ///< metric
	const T&            vec_out,  ///< output expected set of points
	std::pair<int,int>& res       ///< result tracker
)
{
	static int p;
	std::cout << "*** START " << __FUNCTION__ << "(" << ++p<< ")\n";
	{
		CPolyline pol_in(vec_in);
		CPolyline pol_out(vec_out);
		res.first += process2( pol_in, metric, pol_out );
	}
#if 0
	{
		OPolyline pol_in(vec_in);
		OPolyline pol_out(vec_out);
		res.second += process2( pol_in, metric, pol_out );
	}
#endif
}

template<typename T>
void process(
	const T& v_in,  ///< input set of points
	const T& v_out ///< output expected set of points
)
{
	static int p;
	std::cout << "* START " << __FUNCTION__ << "(" << ++p<< ")\n";

	std::pair<int,int> res{0,0};
	process1( v_in, PminimMetric::AbsDistance,  v_out, res );
//	process1( v_in, PminimMetric::RelDistance,  v_out, res );
//	process1( v_in, PminimMetric::Angle,        v_out, res );
//	process1( v_in, PminimMetric::TriangleArea, v_out, res );
	std::cout << "Results: Nb success\n - closed=" << res.first << "\n - open=" << res.second << '\n';
}

int main()
{
	{
		std::vector<Point2d> v1{ {0,1},{1,1.1},{2,1}, {3,1} };
		std::vector<Point2d> v0{ {0,1},{3,1} };
		process( v1, v0 );
	}
	std::exit(1);

	{
//		std::vector<Point2d> v1{ {0,0},{5,0.2},{8,0} };
		std::vector<Point2d> v1{ {0,0},{4,0.3},{5,0.2},{6,0.15},{8,0} };
		std::vector<Point2d> v0{ {0,0},{8,0} };
		process( v1, v0 );
/*		PolyMinimParams par;
		par.setMetric( PminimMetric::TriangleArea );
//		par.setMetric( PminimMetric::Angle );
		CPolyline pol( v1 );
		std::cout << pol << '\n';
		pol.minimize( par );
		std::cout << pol << '\n';*/
	}
//	std::exit(1);

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
