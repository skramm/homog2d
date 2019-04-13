/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

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

#define DIFFERENCE_IS_NULL(a,b) \
	( ( std::fabs((a)-(b)) < std::numeric_limits<double>::epsilon() ) ? true : false )

using namespace homog2d;

TEST_CASE( "test1", "[test1]" )
{
	Point2d ptA1; // 0,0
	CHECK( ptA1 == Point2d(0,0) );

	Point2d ptA2(2,2);
	std::cout << "ptA2="<< ptA2 << '\n';
	CHECK( ptA2.getX() == 2. );
	CHECK( ptA2.getY() == 2. );
	std::cout << "ptA1="<< ptA1 << '\n';
	std::cout << "ptA2="<< ptA2 << '\n';

	{ // build line from two points
		Line2d lA1 = ptA1 * ptA2;
		std::cout << "lA1="<< lA1 << '\n';

		Line2d lA2 = ptA2 * ptA1;
		std::cout << "lA2="<< lA2 << '\n';

		CHECK( lA1 == lA2 );

		Point2d ptB1(0,2);
		Point2d ptB2(2,0);
	std::cout << "ptB1="<< ptB1 << '\n';
	std::cout << "ptB2="<< ptB2 << '\n';
		Line2d  lB = ptB1 * ptB2;
	}

	{

// build point from two diagonal lines
		Line2d lA( Point2d(0,0), Point2d(2,2) );
		Line2d lB( Point2d(0,2), Point2d(2,0) );

		Point2d mA1 = lA * lB;
		Point2d mA2 = lB * lA;
		CHECK( mA1 == Point2d(1.,1.) );
		CHECK( mA2 == Point2d(1.,1.) );

// build point from two H/V lines
		Line2d lv0( 0, 1 );  // vertical, x=0
		Line2d lh0( 1, 0 );  // horizontal, x=0

		CHECK( lv0 * lh0 == Point2d(0.,0.) );
		CHECK( lh0 * lv0 == Point2d(0.,0.) );

		Line2d lv2( Point2d(2,0), Point2d(2,2) ); // vertical, x=2
		Line2d lh2( Point2d(0,2), Point2d(2,2) ); // horizontal, y=2

		CHECK( lv2*lh2 == Point2d(2.,2.) );

		CHECK( lv0*lA == Point2d() );
		CHECK( lh0*lA == Point2d() );

		CHECK( lv0*lB == Point2d(0,2) );
		CHECK( lh0*lB == Point2d(2,0) );

		CHECK( lv2*lA == Point2d(2.,2.) );
		CHECK( lh2*lA == Point2d(2.,2.) );

		CHECK( lv2*lB == Point2d(2.,0.) );
		CHECK( lh2*lB == Point2d(0.,2.) );
	}

}

TEST_CASE( "dist2points", "[test2]" )
{
	Line2d li(2,1);
	auto d = li.distToPoint( Point2d() );
	CHECK( d == 0. );

	auto d2 = li.distToPoint( Point2d(4,2) );
	CHECK( d2 == 0. );

	CHECK( li.getValue( GC_X, 0. ) == 0. );
	CHECK( li.getValue( GC_X, 2. ) == 1. );

	CHECK( li.getValue( GC_Y, 0. ) == 0. );
	CHECK( li.getValue( GC_Y, 1. ) == 2. );

	Point2d p1( 3,3);
	Point2d p2( 4,4);
	CHECK( p1.distToPoint( p2 ) == std::sqrt(2) );
}

TEST_CASE( "offset test", "[test3]" )
{
	Line2d lA( Point2d(0,0), Point2d(2,2) );
	CHECK( lA.distToPoint( Point2d(1.,1.) ) == 0. );

	Line2d lB = lA;
	lA.addOffset( OD_Vert, 2. );
	CHECK( lA == Line2d( Point2d(0,2), Point2d(2,4) ) );

	lB.addOffset( OD_Horiz, 2. );
	CHECK( lB == Line2d( Point2d(2,0), Point2d(4,2) ) );
	{
		std::cout << "offset test\n";
		Line2d v;
		Line2d h(1,0);
		std::cout << "h=" << h << " v=" << v << " v*h=" << v*h << '\n';

		CHECK( v*h == Point2d() );    // intersection is (0,0)

		v.addOffset( OD_Horiz, 1 );
		std::cout << "A: h=" << h << " v=" << v << " v*h=" << v*h << '\n';
		CHECK( v*h == Point2d(1,0) ); // intersection is (1,0)

		h.addOffset( OD_Vert, 1 );
		std::cout << "B: h=" << h << " v=" << v << " v*h=" << v*h << '\n';


		CHECK( v*h == Point2d(1,1) ); // intersection is (1,0)
	}
}

TEST_CASE( "exceptions", "[testE]" )
{
	Line2d v1,v2; // 2 identical vertical lines

	CHECK_THROWS( v1*v2);
	v2.addOffset( OD_Horiz, 1 );
	CHECK_THROWS( v1*v2); // still no intersection (they never cross)

	Point2d p1,p2;
	CHECK_THROWS( p1*p2); // same points can't define a line
}

TEST_CASE( "manual code", "[testM]" )
{
	Point2d pt1;       // 0,0
	Point2d pt2(3,4);
	Line2d li1; // vertical line at x=0
	Line2d li2( pt1, pt2 ); // from two points:
	Point2d pt3(li1,li2);
	{
		Homogr h1; // unit transformation
		h1.setTranslation(3,4);
		Homogr h2( 45. * M_PI / 180.); // 45° rotation matrix
		Homogr h3a = h1*h2; // first, rotation, then translation
		Homogr h3b = h2*h1; // first, rotation, then translation
	}
}

TEST_CASE( "test matrix", "[testH]" )
{
	{
		Homogr H1,H2;
		std::cout << H1 << "\n";
		Homogr H = H1*H2;
		CHECK( H == H1 );
	}
	{
		std::vector<std::vector<float>> m1a(3);
		for( auto& li: m1a)
			li.resize(3,1);
		Homogr H1a(m1a);

		std::vector<std::vector<double>> m1b(3);
		for( auto& li: m1b)
			li.resize(3,1);
		Homogr H1b(m1b);

		std::vector<std::vector<int>> m1c(3);
		for( auto& li: m1c)
			li.resize(3,1);
		Homogr H1c(m1c);

		std::array<std::array<float,3>,3> m2a;
		m2a[2][2] = 1.;
		Homogr H2a(m2a);

		std::array<std::array<double,3>,3> m2b;
		m2b[2][2] = 1.;
		Homogr H2b(m2b);

		std::array<std::array<int,3>,3> m2c;
		m2c[2][2] = 1.;
		Homogr H2c(m2c);
	}

	{
		Homogr H;
		Point2d pt1(1,1);
		H.setTranslation( 3., 2. );
		Point2d pt2 = H * pt1;
		CHECK( pt2.getX() == 4. );
		CHECK( pt2.getY() == 3. );

		H.setRotation( M_PI/2. );
		Point2d pt3 = H * pt1;

		CHECK( DIFFERENCE_IS_NULL( pt3.getX(), -1. ) );
		CHECK( pt3.getY() == 1. );
	}
//	Line2d li;
//	Line2d liB = H * li;

}
