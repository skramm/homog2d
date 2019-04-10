/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points

    Author & Copyright 2019 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr, or http://www.litislab.eu/

    Licence: LGPL v3

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

	See included lgpl.txt and gpl.txt files.

**************************************************************************/

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

	std::vector<std::vector<float>> m1a(3);
	for( auto& li: m1a)
		li.resize(3,1);
	Homogr H3a(m1a);

	std::vector<std::vector<double>> m1b(3);
	for( auto& li: m1b)
		li.resize(3,1);
	Homogr H3b(m1b);

	std::vector<std::vector<int>> m1c(3);
	for( auto& li: m1c)
		li.resize(3,1);
	Homogr H3c(m1c);

	Homogr Ht;
	Ht.setTranslation( 3., 2. );
	Point2d pt1(1,1);
	Point2d pt2 = Ht * pt1;
	std::cout << "Ht="<<  Ht << " pt2=" << pt2 << "\n";
	CHECK( pt2.getX() == 4. );
	CHECK( pt2.getY() == 3. );

}
