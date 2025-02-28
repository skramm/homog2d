/**
\file
\brief This is a test file for polygon splitting



Build and run with <code>$ make test-splitpol</code>
*/

//#define HOMOG2D_NOCHECKS
#define HOMOG2D_DEBUGMODE

#include "../../homog2d.hpp"

using namespace h2d;
template<typename PTS, typename LI>
void process( const PTS& vpts, const LI& li )
{
	static int c;
	std::cout << "\n** TEST " << ++c << '\n';
	CPolyline pol(vpts);
	auto res = pol.split(li);
	std::cout << "\n* RESULTAT, size=" << res.size() << '\n';
	for( const auto& p: res )
		std::cout << p << '\n';
}

int main()
{
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
		auto vpol = r.split(li);
		priv::printVector( vpol );
	}

	{
	std::cout << "\n** TEST FRECT 2" << '\n';
		FRect r(0,0,2,2);
		Line2d li( LineDir::H, 2 );
		auto vpol = r.split(li);
		priv::printVector( vpol );
	}

	{
	std::cout << "\n** TEST FRECT 3" << '\n';
		FRect r(0,0,2,2);
		Line2d li( 2,0, 4,2 );
		auto vpol = r.split(li);
		priv::printVector( vpol );
	}
}

