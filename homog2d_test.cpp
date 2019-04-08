/**
\file homog2d_test.cpp
*/

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "homog2d.hpp"

using namespace homog2d;

TEST_CASE( "a test", "[test1]" )
{
	Point2d pt1; // 0,0
	Point2d pt2(4,4);
	Line2d l1(4,4); // 45 degree angle
}

