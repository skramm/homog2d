/**
\file
\brief This is a test file for polygon splitting



Build and run with <code>$ make test-splitpol</code>
*/

//#define HOMOG2D_NOCHECKS

#include "../../homog2d.hpp"

using namespace h2d;

int main()
{
	std::vector<Point2d> vpts{
		{0,0},{10,10},{20,0}
	};

/*
	std::vector<Point2d> vpts{
		{-2,0},{-1,1},{1,1},{2,0},{1,-1},{-1,-1}
	};

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
*/

	CPolyline pol(vpts);
//	Line2d li;           // vertical line at x=0
	Line2d li( LineDir::H, 5 );           // horizontal line at y=1

	auto res = pol.split(li);
	std::cout << "\n* RESULTATsize=" << res.size() << '\n';
	for( const auto& p: res )
		std::cout << p << '\n';

}
