/**
\file homog2d_test.cpp
\brief A test file for homog2d, needs Catch https://github.com/catchorg/Catch2,
run with "make test"
*/

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "homog2d.hpp"


using namespace homog2d;

TEST_CASE( "a test", "[test1]" )
{
	Point2d ptA1; // 0,0
	CHECK( ptA1 == Point2d(0,0) );

	Point2d ptA2(2,2);
	std::cout << "ptA2="<< ptA2 << '\n';

	Line2d lA, lB;
	std::cout << "lineA="<< lA << '\n';
	lA = ptA1 * ptA2;

	std::cout << "lineA="<< lA << '\n';
}

