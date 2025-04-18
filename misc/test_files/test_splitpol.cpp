/**
\file
\brief This is a test file for polygon splitting



Build and run with <code>$ make test-splitpol</code>
*/

//#define HOMOG2D_NOCHECKS
#define HOMOG2D_DEBUGMODE
#define HOMOG2D_TEST_MODE

#include "../../homog2d.hpp"

using namespace std;
using namespace h2d;
template<typename PTS, typename LI>
void process( const PTS& vpts, const LI& li )
{
	static int c;
	std::cout << "\n** TEST " << ++c << '\n';
	CPolyline pol(vpts);
	auto res = pol.splitC(li);
	std::cout << "\n* RESULTAT, size=" << res.size() << '\n';
	for( const auto& p: res )
		std::cout << p << '\n';
}

/// Helper function
/**
- POL: polyline type (OPolyline or CPolyline)
- VVPTS: vector of vector of points, corresponding to the expected split
*/
template<typename POL,typename VVPTS>
auto
buildOutputPolySet( POL, const VVPTS& vvpts )
{
	std::vector<POL> out;
	for( const auto& vpts: vvpts )
		out.emplace_back(POL( vpts ));
	return out;
}

/// Compare sets of polylines
/**
- \c split: the ones produces by the "split" function
- \c exp: the expected ones
*/
template<typename POL>
void
compareSetsPolylines( const std::vector<POL> split0, const std::vector<POL>& exp0 )
{
	auto split = split0;
	auto exp   = exp0;
	if( split.size() != exp.size() )
	{
		std::cout << "Failure: split size="<< split.size() << " expected size=" << exp.size() <<'\n';
		return;
	}
	for( auto& p: split )
		p.p_normalizePoly();
	for( auto& p: exp )
		p.p_normalizePoly();
	std::sort( split.begin(), split.end() );
	std::sort( exp.begin(), exp.end() );

	auto it = exp.begin();
	bool success = true;
	for( const auto& p1: split )
	{
		if( p1 != *it )
		{
			std::cout << "fail!\n - split result: " << p1
				<< "\n - expected: " << *it << "\n";
			success = false;
		}
		it++;
	}
	if( success )
		std::cout << "split successful!\n";
}

template<typename POLSET>
void
process1( const POLSET& split, const POLSET& exp )
{
	std::cout << "\n* Split result:\n";
	for( const auto& pol: split )
		std::cout << pol << '\n';
	std::cout << "\n* Expected result:\n";
	for( const auto& pol: exp )
		std::cout << pol << '\n';
	compareSetsPolylines( split, exp );
}

void process2(
	std::string                    id,
	const std::vector<Point2d>&    src,      ///< input set of points, used to build the polylines
	const Line2d&                  li,       ///< line that splits the polyline
	const vector<vector<Point2d>>& vv_pts_O, ///< Expected output set of points that make the split, for OPolyline
	const vector<vector<Point2d>>& vv_pts_C  ///< Expected output set of points that make the split, for CPolyline
)
{
	OPolyline psrc_o( src );
	std::cout << "\n* TEST " << id << ", Input: " << psrc_o << '\n';

	process1( psrc_o.splitO(li), buildOutputPolySet( OPolyline(), vv_pts_O ) );
	process1( psrc_o.splitC(li), buildOutputPolySet( CPolyline(), vv_pts_O ) );

	CPolyline psrc_c( src );
	std::cout << "\n* Input: " << psrc_c << '\n';

	process1( psrc_c.splitO(li), buildOutputPolySet( OPolyline(), vv_pts_C ) );
	process1( psrc_c.splitC(li), buildOutputPolySet( CPolyline(), vv_pts_C ) );
}

int main()
{
#if 1
{
	#include "../figures_test/polysplit_01a.code"
	process2( "01a", src, li, vv_pts_O, vv_pts_C );
}
{
	#include "../figures_test/polysplit_01b.code"
	process2( "01b", src, li, vv_pts_O, vv_pts_C );
}
{
	#include "../figures_test/polysplit_01c.code"
	process2( "01c", src, li, vv_pts_O, vv_pts_C );
}
{
	#include "../figures_test/polysplit_02.code"
	process2( "02", src, li, vv_pts_O, vv_pts_C );
}
{
	#include "../figures_test/polysplit_03.code"
	process2( "03", src, li, vv_pts_O, vv_pts_C );
}
{
	#include "../figures_test/polysplit_04.code"
	process2( "04", src, li, vv_pts_O, vv_pts_C );
}
{
	#include "../figures_test/polysplit_05.code"
	process2( "05", src, li, vv_pts_O, vv_pts_C );
}
{
	#include "../figures_test/polysplit_06.code"
	process2( "06", src, li, vv_pts_O, vv_pts_C );
}
{
	#include "../figures_test/polysplit_07.code"
	process2( "07", src, li, vv_pts_O, vv_pts_C );
}

#else

	{
	std::cout << "\n** TEST FRECT 2" << '\n';
		FRect r(0,0,2,2);
		Line2d li( LineDir::H, 2 );
		auto vpol = r.splitC(li);
		priv::printVector( vpol );
	}
	std::exit(0);

// test sorting
	{
		CPolyline pol1( std::vector<Point2d>{ {0,0},{10,10},{20,0} } );
		CPolyline pol2( std::vector<Point2d>{ {1,0},{10,10},{20,0} } );
		assert( pol1<pol2 );
	}
	{
		CPolyline pol1( std::vector<Point2d>{ {0,0},{10,10},{20,0} } );
		CPolyline pol2( std::vector<Point2d>{ {0,0},{10,10},{20,0} } );
		assert( pol1<pol2 );
	}

	{
		std::vector<Point2d> vpts{
			{0,0},{10,10},{20,0}
		};
		Line2d li( LineDir::H, 5 );
		process( vpts, li );
	}
	{
		std::vector<Point2d> vpts{
			{0,0},{10,10},{20,0}
		};
//		Line2d li;           // vertical line at x=0
		Line2d li( LineDir::V, 5 );
		process( vpts, li );
	}
	{
		std::vector<Point2d> vpts{
			{-2,0},{-1,1},{1,1},{2,0},{1,-1},{-1,-1}
		};
		Line2d li;           // vertical line at x=0
		process( vpts, li );
	}
	{
		std::vector<Point2d> vpts{
			{ 2,-3},
			{ 2, 3},
			{ 1, 3},
			{ 1, 1},
			{-1, 1},
			{-1, 3},
			{-2, 3},
			{-2, -3},
			{-1, -3},
			{-1, -1},
			{ 1, -1},
			{ 1, -3}
		};
		Line2d li( LineDir::H, 2 );           // horizontal line at y=1
		process( vpts, li );
	}
	{
		std::vector<Point2d> vpts{
			{0,0},{4,4},{6,4},{6,0}
		};
		Line2d li( LineDir::H, 2 );
		process( vpts, li );
	}
	{
		std::vector<Point2d> vpts{
			{4,0},{0,4},{6,4},{6,0}
		};
		Line2d li( LineDir::H, 2 );
		process( vpts, li );
	}

	{
		std::vector<Point2d> vpts{
			{0,0},{0,2},{1,1},{2,2},{2,0}
		};
		Line2d li( LineDir::H, 2 );
		process( vpts, li );
	}
	{
	std::cout << "\n** TEST FRECT 1" << '\n';
		FRect r(0,0,2,2);
		Line2d li( LineDir::H, 1 );
		auto vpol = r.splitC(li);
		priv::printVector( vpol );
	}

	{
	std::cout << "\n** TEST FRECT 2" << '\n';
		FRect r(0,0,2,2);
		Line2d li( LineDir::H, 2 );
		auto vpol = r.splitC(li);
		priv::printVector( vpol );
	}

	{
	std::cout << "\n** TEST FRECT 3" << '\n';
		FRect r(0,0,2,2);
		Line2d li( 2,0, 4,2 );
		auto vpol = r.splitC(li);
		priv::printVector( vpol );
	}
#endif // 1
}

