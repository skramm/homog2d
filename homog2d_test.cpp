/**
\file homog2d_test.cpp
\brief A test file for homog2d, needs Catch https://github.com/catchorg/Catch2,
run with "make test"
*/

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "homog2d.hpp"

using namespace homog2d;

TEST_CASE( "test1", "[test1]" )
{
	Point2d ptA1; // 0,0
	CHECK( ptA1 == Point2d(0,0) );

	Point2d ptA2(2,2);
	std::cout << "ptA2="<< ptA2 << '\n';
	CHECK( ptA2.getX() == 2. );
	CHECK( ptA2.getY() == 2. );
	{
		Line2d lA = ptA1 * ptA2;
		Line2d lA2 = ptA2 * ptA1;
		CHECK( lA == lA2 );

		Point2d ptB1(0,2);
		Point2d ptB2(2,0);
		Line2d  lB = ptB1 * ptB2;

		Point2d middle = lA * lB;

		std::cout << "middle="<< middle << '\n';
		CHECK( middle == Point2d(1.,1.) );
	}
	{
		Line2d lA( Point2d(0,0), Point2d(2,2) );
		Line2d lB( Point2d(0,2), Point2d(2,0) );

		Point2d middle = lA * lB;

		std::cout << "middle="<< middle << '\n';
		CHECK( middle == Point2d(1.,1.) );
	}
}

TEST_CASE( "dist2points", "[test2]" )
{
	Line2d li(2,1);
	auto d = li.distToPoint( Point2d() );
	CHECK( d == 0. );
	CHECK( li.getValue( GC_X, 0. ) == 0. );
	CHECK( li.getValue( GC_X, 2. ) == 1. );

	CHECK( li.getValue( GC_Y, 0. ) == 0. );
	CHECK( li.getValue( GC_Y, 1. ) == 2. );
}

TEST_CASE( "offset test", "[test3]" )
{
	Line2d lA( Point2d(0,0), Point2d(2,2) );
	CHECK( lA.distToPoint( Point2d(1.,1.) ) == 0. );

	lA.addVertOffset( 2. );
	CHECK( lA == Line2d( Point2d(0,2), Point2d(2,4) ) );
}

TEST_CASE( "test matrix", "[testH]" )
{
	Homogr H1,H2;
	std::cout << H1 << "\n";
	Homogr H = H1*H2;
	CHECK( H == H1 );

}
