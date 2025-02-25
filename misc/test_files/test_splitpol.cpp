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
		{-2,0},{-1,1},{1,1},{2,0},{1,-1},{-1,-1}
	};
	CPolyline pol(vpts);
	Line2d li;
	auto res = pol.split(li);

}
