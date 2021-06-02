/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr

    Licence: MPL v2

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at https://mozilla.org/MPL/2.0/.

**************************************************************************/

/**
\file homog2d_test.cpp
\brief A test file for homog2d, needs Catch https://github.com/catchorg/Catch2,
run with "make test"
*/

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#define NUMTYPE double

#include "../homog2d.hpp"

#include <list>

#define IS_ALMOST_ZERO(a) \
	( ( std::fabs(a) > std::numeric_limits<double>::epsilon() ) ? true : false )

#define DIFFERENCE_IS_NULL(a,b) \
	( ( std::fabs((a)-(b)) <= std::numeric_limits<double>::epsilon() ) ? true : false )

double g_epsilon = std::numeric_limits<double>::epsilon()*10.;

using namespace homog2d;

TEST_CASE( "types testing", "[testtypes]" )
{
	std::cout << "Running tests with catch " << CATCH_VERSION_MAJOR << '.' << CATCH_VERSION_MINOR << '.' << CATCH_VERSION_PATCH << '\n';
	SECTION( "type size" )
	{
		Point2dF ptF;
		Point2dD ptD;
		Point2dL ptL;

		Line2dF liF;
		Line2dD liD;
		Line2dL liL;

		HomogrF HF;
		HomogrD HD;
		HomogrL HL;

		Point2d_<float> pt2F1;
		Point2d_<double> pt2F2;
		Point2d_<long double> pt2F3;

		CHECK( sizeof(Point2dF) == 12 );
		CHECK( sizeof(Point2dD) == 24 );
		CHECK( sizeof(Point2dL) == 48 );
	}

	Point2dD ptD0(1,1);
	Point2dF ptF0(2,2);
	Point2dL ptL0(3,3);

	Line2dD li_D0(1,1);
	Line2dF li_F0(2,2);
	Line2dL li_L0(3,3);

	SECTION( "numerical type conversions (assignment)" )
	{
		Point2dD ptD(4,4);
		Point2dF ptF(5,5);
		Point2dL ptL(6,6);

		ptL = ptD0;
		CHECK( ptL.getX() == 1. );
		ptL = ptF0;
		CHECK( ptL.getX() == 2. );

		ptF = ptD0;
		CHECK( ptF.getX() == 1. );
		ptF = ptL0;
		CHECK( ptF.getX() == 3. );

		ptD = ptF0;
		CHECK( ptD.getX() == 2. );
		ptD = ptL0;
		CHECK( ptD.getX() == 3. );
	}
	SECTION( "numerical type conversions (constructor)" )
	{
		Point2dL ptL1 = ptD0;
		Point2dL ptL2 = ptF0;
		CHECK( ptL1.getX() == 1. );
		CHECK( ptL2.getX() == 2. );

		Point2dF ptF1 = ptD0;
		Point2dF ptF2 = ptL0;
		CHECK( ptF1.getX() == 1. );
		CHECK( ptF2.getX() == 3. );

		Point2dD ptD1 = ptF0;
		Point2dD ptD2 = ptL0;
		CHECK( ptD1.getX() == 2. );
		CHECK( ptD2.getX() == 3. );

		Line2dD li_D1 = li_F0;
		Line2dD li_L1 = li_F0;

		Line2dD li_F2 = li_D0;
		Line2dD li_L2 = li_D0;

		Line2dD li_F3 = li_L0;
		Line2dD li_D3 = li_L0;
	}
}

TEST_CASE( "test1", "[test1]" )
{
	Point2d_<NUMTYPE> ptA1; // 0,0
	CHECK( ptA1 == Point2d_<NUMTYPE>(0,0) );

	Point2d_<NUMTYPE> ptA2(2,2);
	CHECK( ptA2.getX() == 2. );
	CHECK( ptA2.getY() == 2. );

	{ // build line from one point, other one will be (0,0)
		Line2d_<NUMTYPE> lA1( ptA2 );
		CHECK( lA1.distTo( ptA1 ) == 0 );
		CHECK( lA1.distTo( ptA2 ) == 0 );

		CHECK( ptA1.distTo( lA1 ) == 0 );
		CHECK( ptA2.distTo( lA1 ) == 0 );
	}

	{ // build line from two points
		Line2d_<NUMTYPE> lA1 = ptA1 * ptA2;
		Line2d_<NUMTYPE> lA2 = ptA2 * ptA1;

// auto dist = lA1.distTo( lA2 );   // THIS WOULD BREAK THE BUILD (can't compute distance between two lines)

		CHECK( lA1 == lA2 );
		CHECK( lA1.getAngle(lA2) == 0. );
		CHECK( lA2.getAngle(lA1) == 0. );
		CHECK( getAngle(lA1,lA2) == 0. );

		Point2d_<NUMTYPE> ptB1(0,2);
		Point2d_<NUMTYPE> ptB2(2,0);
		Line2d_<NUMTYPE>  lB = ptB1 * ptB2;
		CHECK( lB.getCoord( GivenCoord::X, 1 ) == 1. );
		CHECK( lB.getPoint( GivenCoord::X, 1) == Point2d_<NUMTYPE>(1,1) );
	}
	{
// build point from two diagonal lines
		Line2d_<NUMTYPE> liA( Point2d_<NUMTYPE>(0,0), Point2d_<NUMTYPE>(2,2) );
		Line2d_<NUMTYPE> liB( Point2d_<NUMTYPE>(0,2), Point2d_<NUMTYPE>(2,0) );
		CHECK( Line2d_<NUMTYPE>() != liA );

		auto mA1 = liA * liB;
		auto mA2 = liB * liA;
		CHECK( mA1 == Point2d_<NUMTYPE>(1.,1.) );
		CHECK( mA2 == Point2d_<NUMTYPE>(1.,1.) );
		CHECK( mA1 != Point2d_<NUMTYPE>() );

// build point from two H/V lines
		Line2d_<NUMTYPE> lv0( 0, 1 );  // vertical, x=0
		Line2d_<NUMTYPE> lh0( 1, 0 );  // horizontal, x=0

		CHECK( lv0 * lh0 == Point2d_<NUMTYPE>(0.,0.) );
		CHECK( lh0 * lv0 == Point2d_<NUMTYPE>(0.,0.) );

		Line2d_<NUMTYPE> lv2( Point2d_<NUMTYPE>(2,0), Point2d_<NUMTYPE>(2,2) ); // vertical, x=2
		Line2d_<NUMTYPE> lh2( Point2d_<NUMTYPE>(0,2), Point2d_<NUMTYPE>(2,2) ); // horizontal, y=2

		CHECK( lv2*lh2 == Point2d_<NUMTYPE>(2.,2.) );

		CHECK( lv0*liA == Point2d_<NUMTYPE>() );
		CHECK( lh0*liA == Point2d_<NUMTYPE>() );

		CHECK( lv0*liB == Point2d_<NUMTYPE>(0,2) );
		CHECK( lh0*liB == Point2d_<NUMTYPE>(2,0) );

		CHECK( lv2*liA == Point2d_<NUMTYPE>(2.,2.) );
		CHECK( lh2*liA == Point2d_<NUMTYPE>(2.,2.) );

		CHECK( lv2*liB == Point2d_<NUMTYPE>(2.,0.) );
		CHECK( lh2*liB == Point2d_<NUMTYPE>(0.,2.) );
	}

	{ // test of getOrthogonal()
		Line2d_<NUMTYPE> lV; // vertical line at x=0

// get orthogonal line at y=100
		Line2d_<NUMTYPE> li2 = lV.getOrthogonalLine( GivenCoord::Y, 100 );
		CHECK( li2.getAngle( lV ) == M_PI/2. );

		CHECK( getAngle( li2,lV ) == M_PI/2. );

		Line2d_<NUMTYPE> lH2(1,0);                // build horizontal line
		Line2d_<NUMTYPE> lH3 = lH2;

		lH2.addOffset( LineOffset::vert, 100 );  // add vertical offset
		CHECK( li2 == lH2 );
		CHECK( lH2.getAngle(lH3) == 0. );

		Line2d_<NUMTYPE> lH(1,0);                // build horizontal line
		Line2d_<NUMTYPE> li3 = lH.getOrthogonalLine( GivenCoord::X, 100 );
		Line2d_<NUMTYPE> lV2;
		lV2.addOffset( LineOffset::horiz, 100 );  // add horizontal offset
		CHECK( li3 == lV2 );
	}
	{
		Line2d_<NUMTYPE> li(4,2);
		CHECK( li.getCoord( GivenCoord::X, 2 ) == 1 );
		CHECK( li.getCoord( GivenCoord::Y, 1 ) == 2 );
		CHECK( li.getPoint( GivenCoord::X, 2 ) == Point2d_<NUMTYPE>(2,1) );
		CHECK( li.getPoint( GivenCoord::Y, 1 ) == Point2d_<NUMTYPE>(2,1) );
	}
}

TEST_CASE( "test parallel", "[test_para]" )
{
	SECTION( "Checking angle" )
	{
		Line2d_<NUMTYPE> l1;                                                   // vertical line
		Line2d_<NUMTYPE> l2a( Point2d_<NUMTYPE>(0,0),Point2d_<NUMTYPE>(1,1) ); // 45° line, starting at (0,0)
		CHECK( getAngle( l1, l2a ) == M_PI/4. );

		Line2d_<NUMTYPE> l2b( Point2d_<NUMTYPE>(3,0),Point2d_<NUMTYPE>(4,1) ); // 45° line, starting at (3,0)
		CHECK( getAngle( l1, l2b ) == M_PI/4. );
		l2b.addOffset( LineOffset::horiz, 10. );
		CHECK( getAngle( l1, l2b ) == M_PI/4. );
		l2b.addOffset( LineOffset::vert, 10. );
		CHECK( getAngle( l1, l2b ) == M_PI/4. );
	}

	SECTION( "Checking parallel lines" )
	{
		std::cout << "default null angle=" << Line2d::nullAngleValue() << " rad.\n";
		Line2d_<NUMTYPE> l1; // vertical line
		{
			Line2d_<NUMTYPE> l2a(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.001,1.) ); // almost vertical line
			CHECK( l1.isParallelTo(l2a) == true );

			Line2d_<NUMTYPE> l2b(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.002,1.) ); // almost vertical line
			CHECK( l1.isParallelTo(l2b) == false );
		}
		Line2d::nullAngleValue() = 0.01;
		{
			Line2d_<NUMTYPE> l2a(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.01,1.) ); // almost vertical line
			CHECK( l1.isParallelTo(l2a) == true );

			Line2d_<NUMTYPE> l2b(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.02,1.) ); // almost vertical line
			CHECK( l1.isParallelTo(l2b) == false );
		}
		//l1.isParallelTo( Point2d() ); // NO BUILD
	}
	SECTION( "Vertical line at x=0" )
	{
		Line2d_<NUMTYPE> l1; // vertical line

		Line2d_<NUMTYPE> l2 = l1.getParallelLine( Point2d_<NUMTYPE>(1,1) );
		CHECK_THROWS( l1 * l2 ); // two parallel lines never cross
		CHECK( l2.distTo( Point2d_<NUMTYPE>(0,0) ) == 1. );
		CHECK( l2.distTo( Point2d_<NUMTYPE>(0,2) ) == 1. );
		CHECK( getAngle(l2,l1) == 0. );

		Line2d_<NUMTYPE> l3 = l1.getParallelLine( Point2d_<NUMTYPE>(0,0) );
		CHECK( l3.distTo( Point2d_<NUMTYPE>(0,0) ) == 0. );
		CHECK( l3.distTo( Point2d_<NUMTYPE>(0,2) ) == 0. );
		CHECK( getAngle(l3,l1) == 0. );
	}
/*	SECTION( "Vertical line at x=1" )
	{
		Line2d_<NUMTYPE> l1( Point2d(1,0), Point2d(1,10) ); // vertical line at x=1

		Line2d_<NUMTYPE> l2 = l1.getParallelLine( Point2d(1,1) );
		CHECK_THROWS( l1 * l2 ); // two parallel lines never cross
		CHECK( l2.distTo( Point2d(0,0) ) == 1. );
		CHECK( l2.distTo( Point2d(0,2) ) == 1. );

		Line2d_<NUMTYPE> l3 = l1.getParallelLine( Point2d(0,0) );
		CHECK( l3.distTo( Point2d(0,0) ) == 1. );
		CHECK( l3.distTo( Point2d(0,2) ) == 1. );
	}*/
}

TEST_CASE( "dist2points", "[test2]" )
{
	Line2d_<NUMTYPE> li(2,1);
	auto d = li.distTo( Point2d_<NUMTYPE>() );
	CHECK( d == 0. );

	auto d2 = li.distTo( Point2d_<NUMTYPE>(4,2) );
	CHECK( d2 == 0. );

	CHECK( li.getCoord( GivenCoord::X, 0. ) == 0. );
	CHECK( li.getCoord( GivenCoord::X, 2. ) == 1. );

	CHECK( li.getCoord( GivenCoord::Y, 0. ) == 0. );
	CHECK( li.getCoord( GivenCoord::Y, 1. ) == 2. );

	Point2d_<NUMTYPE> p1( 3,3);
	Point2d_<NUMTYPE> p2( 4,4);
	CHECK( p1.distTo( p2 ) == std::sqrt(2) );
}

TEST_CASE( "offset test", "[test3]" )
{
	Line2d_<NUMTYPE> lA( Point2d_<NUMTYPE>(0,0), Point2d_<NUMTYPE>(2,2) );
	CHECK( lA.distTo( Point2d_<NUMTYPE>(1.,1.) ) == 0. );

	Line2d_<NUMTYPE> lB = lA;
	lA.addOffset( LineOffset::vert, 2. );
	CHECK( lA == Line2d_<NUMTYPE>( Point2d_<NUMTYPE>(0,2), Point2d_<NUMTYPE>(2,4) ) );
	lB.addOffset( LineOffset::horiz, 2. );
	CHECK( lB == Line2d_<NUMTYPE>( Point2d_<NUMTYPE>(2,0), Point2d_<NUMTYPE>(4,2) ) );
	{
		Line2d_<NUMTYPE> v;
		Line2d_<NUMTYPE> h(1,0);
		CHECK( v*h == Point2d_<NUMTYPE>() );    // intersection is (0,0)

		v.addOffset( LineOffset::horiz, 1 );
		CHECK( v*h == Point2d_<NUMTYPE>(1,0) ); // intersection is (1,0)

		h.addOffset( LineOffset::vert, 1 );
		CHECK( v*h == Point2d_<NUMTYPE>(1,1) ); // intersection is (1,0)
	}
	{
		Line2d_<NUMTYPE> liV;
		Line2d_<NUMTYPE> liV2 = liV;
		liV.addOffset( LineOffset::vert, 1 ); // adding vertical offset to vertical line does nothing
		CHECK( liV == liV2 );

		Line2d_<NUMTYPE> liH(1,0);
		Line2d_<NUMTYPE> liH2 = liH;
//		std::cout << "liH2=" << liH2 << "\n";
		liH.addOffset( LineOffset::horiz, 1 ); // adding horizontal offset to horizontal line does nothing
//		std::cout << "liH=" << liH << "\n";
		CHECK( liH == liH2 );
	}
}

TEST_CASE( "exceptions", "[testE]" )
{
	Line2d_<NUMTYPE> v1,v2; // 2 identical vertical lines

	CHECK_THROWS( v1*v2 );
	v2.addOffset( LineOffset::horiz, 1 );
	CHECK_THROWS( v1*v2 ); // still no intersection (they never cross)

	Point2d_<NUMTYPE> p1,p2;
	CHECK_THROWS( p1*p2 ); // same points can't define a line
}

/**
\todo Define a reasonable default value for Hmatrix (0's are probably not, not homogeneous)
*/
TEST_CASE( "test Matrix", "[testM]" )
{
	Hmatrix m;
	Point2d_<NUMTYPE> p1;
	Line2d_<NUMTYPE> l1 = m*p1;
	Point2d_<NUMTYPE> p2 = m*l1;
}

TEST_CASE( "Homogr constructors", "[testHC]" )
{
	{
		auto angle = 0.5;
		Homogr H0,H1( angle ); // set rotation with constructor
		H0.setRotation( angle );
//		std::cout << "H0:\n" << H0;
//		std::cout << "H1:\n" << H1;
		Line2d_<NUMTYPE> li1;
		Line2d_<NUMTYPE> li2 = H0 * li1;
		auto angle2 = getAngle( li1, li2 );
		CHECK( DIFFERENCE_IS_NULL(angle2, angle) );
//		std::cout << std::setprecision( std::numeric_limits<double>::digits10 ) << std::scientific << "angle2=" << angle2 << " angle=" << angle << "\n";
	}
	{
		Homogr H0( 4. , 7. );
		CHECK( H0.get( 0, 2 ) == 4. );
		CHECK( H0.get( 1, 2 ) == 7. );
	}
}

TEST_CASE( "test Homogr", "[testH]" )
{
	{
		Homogr H1,H2;
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
		Point2d_<NUMTYPE> pt1(1,1);
		H.setTranslation( 3., 2. );

		Point2d_<NUMTYPE> pt2 = H * pt1;

		CHECK( pt2.getX() == 4. );
		CHECK( pt2.getY() == 3. );

		H.setRotation( M_PI/2. );
		Point2d_<NUMTYPE> pt3 = H * pt1;

		CHECK( DIFFERENCE_IS_NULL( pt3.getX(), -1. ) );
		CHECK( pt3.getY() == 1. );
	}
	{
		Homogr H;
		H.setTranslation(5,6);

		std::vector<Point2d> v_pt(3);
		H.applyTo( v_pt );
		CHECK( v_pt[2].getX() == 5 );

		std::array<Point2d,3> a_pt;
		H.applyTo( a_pt );
		CHECK( a_pt[2].getX() == 5 );

		std::list<Point2d> l_pt(3);
		H.applyTo( l_pt );
		CHECK( std::begin(l_pt)->getX() == 5 );
	}
}

TEST_CASE( "matrix inversion", "[testH3]" )
{
	Homogr H;
	{
		Homogr HR=H;
		HR.inverse();
		CHECK( HR == H );
		HR.transpose();
		CHECK( HR == H );
	}

	{ // sample inversion
// checked with https://ncalculators.com/matrix/inverse-matrix.htm
		H = std::vector<std::vector<double>>{
			{ 1, -1,  2 },
			{ 4,  0,  6 },
			{ 5,  1, -1 }
		};

		Homogr H2;

		H2 = H;        // transposing twice = original matrix
		H2.transpose();
		H2.transpose();
		CHECK( H == H2 );

		H2 = H;        // transposing twice = original matrix
		H2.transpose().transpose();
		CHECK( H == H2 );

		H2 = H;        // inverting twice = original matrix
		H2.inverse();
		H2.inverse();
		CHECK( H == H2 );

		H2 = H;        // inverting twice = original matrix
		H2.inverse().inverse();
		CHECK( H == H2 );

		H2 = H;
		H.inverse();
		Homogr HR = std::vector<std::vector<double>>{
			{   6, - 1,  6 },
			{ -34,  11, -2 },
			{ - 4,   6, -4 }
		};
		CHECK( HR == H );

		H.transpose();
		H2.inverse().transpose();
		CHECK( H == H2 );
	}
}

/// Computation of the line passed through H^{-T} and computation of
/// the distance between the resulting line and the transformed point.
/// Should be 0, always.
double computeDistTransformedLined( Homogr H )
{
	Line2d_<NUMTYPE> line1( 5, 6 ); // line from (0,0) to (5,6)
	Point2d_<NUMTYPE> pt1( 5, 6);  // point is on line

	Point2d_<NUMTYPE> pt2 = H * pt1; // move the point with H

	H.inverse().transpose();

	Line2d_<NUMTYPE> line2 = H * line1; // move the line with H^{-T}

	return line2.distTo( pt2 ); // should be 0 !
}

// from https://stackoverflow.com/a/55868408/193789
std::string FullPrecision( double d )
{
    auto s = std::ostringstream{};
    s << std::setprecision( std::numeric_limits<double>::max_digits10 ) << d;
    return s.str();
}

TEST_CASE( "line transformation", "[testH3]" )
{
	{
		Line2d_<NUMTYPE> d1( 5, 6 ); // line from (0,0) to (5,6)
		Point2d_<NUMTYPE> pt1( 5, 6);  // point is on line
		CHECK( d1.distTo( pt1 ) <= g_epsilon  );
	}

	Homogr H;
	{
		H.setTranslation(4,5);
		auto d = computeDistTransformedLined( H );
		INFO( FullPrecision(d) );
		CHECK( d <= g_epsilon );
	}
	{
		H.setRotation( 22.*M_PI/180. );
		auto d = computeDistTransformedLined( H );
		INFO( FullPrecision(d) );
		CHECK( d <= g_epsilon );
	}
	{
		H.setScale(0.4, 4.2);
		auto d = computeDistTransformedLined( H );
		INFO( FullPrecision(d) );
		CHECK( d <= g_epsilon );
	}
	{
		H.setRotation( 1.456 ).addTranslation(4,5).addScale( 0.4, 1.2 ); // some random transformation
		auto d = computeDistTransformedLined( H );
		INFO( FullPrecision(d) );
		CHECK( d <= g_epsilon );
	}
}

TEST_CASE( "matrix chained operations", "[testH2]" )
{
	Homogr H1,H2;
	H1.addTranslation(4,5).addRotation( 1 ).addScale( 5,6);
	H2.addRotation( 1 ).addTranslation(4,5).addScale( 5,6);
	CHECK( H1 != H2 );
}

TEST_CASE( "getPoints", "[test_points]" )
{
	Line2d_<NUMTYPE> liV; // vertical line
	auto pp = liV.getPoints( GivenCoord::Y, 0.0, 2.0 ); // get points at a distance 2 from (0,0)
	CHECK( pp.first  == Point2d_<NUMTYPE>(0,-2) );
	CHECK( pp.second == Point2d_<NUMTYPE>(0,+2) );

	pp = liV.getPoints( GivenCoord::Y, 3.0, 2.0 ); // get points at a distance 2 from (0,3)
	CHECK( pp.first  == Point2d_<NUMTYPE>(0,+1) );
	CHECK( pp.second == Point2d_<NUMTYPE>(0,+5) );

	Line2d_<NUMTYPE> liH(1,0); // horizontal line
	pp = liH.getPoints( GivenCoord::X, 0.0, 2.0 ); // get points at a distance 2 from (0,0)
	CHECK( pp.first  == Point2d_<NUMTYPE>(-2,0) );
	CHECK( pp.second == Point2d_<NUMTYPE>(+2,0) );

	pp = liH.getPoints( GivenCoord::X, 3.0, 2.0 ); // get points at a distance 2 from (3,0)
	CHECK( pp.first  == Point2d_<NUMTYPE>(+1,0) );
	CHECK( pp.second == Point2d_<NUMTYPE>(+5,0) );

	Line2d_<NUMTYPE> li( 1, 1 );                   // line with slope [1,1] starting from (0,0)
	auto k = 1.0 / std::sqrt(2.);
	pp = li.getPoints( GivenCoord::X, 5.0, 1.0 ); // get points at a distance 1 from (5,0)
	CHECK( pp.first  == Point2d_<NUMTYPE>( 5-k, 5-k ) );
	CHECK( pp.second == Point2d_<NUMTYPE>( 5+k, 5+k ) );

	li = Point2d_<NUMTYPE>(3,1) * Point2d_<NUMTYPE>(4,2); // line with slope [1,1] starting from (3,1)

	pp = li.getPoints( GivenCoord::X, 5.0, 1.0 ); // get points at a distance 2 from (3,0)
	CHECK( pp.first  == Point2d_<NUMTYPE>( 5-k, 3-k ) );
	CHECK( pp.second == Point2d_<NUMTYPE>( 5+k, 3+k ) );
}

TEST_CASE( "getAngle", "[test_angle]" )
{
	Line2d_<NUMTYPE> lid(1,1); // diagonal line going through (0,0)
	Line2d_<NUMTYPE> lih(1,0); // horizontal line
	Line2d_<NUMTYPE> liv;     // vertical line
	CHECK( lih.getAngle(lid) == M_PI/4. );
	CHECK( liv.getAngle(lid) == M_PI/4. );
	CHECK( liv.getAngle(lih) == M_PI/2. );

	CHECK( getAngle(lih,lid) == M_PI/4. );
	CHECK( getAngle(liv,lid) == M_PI/4. );
	CHECK( getAngle(lih,liv) == M_PI/2. );
//	liv.getAngle( Point2d() );          // NO BUILD
//	getAngle( liv, Point2d() );         // NO BUILD
}

TEST_CASE( "IsInsideRectangle", "[test_IsInside]" )
{
	Point2d_<NUMTYPE> pt1(2,10);
	Point2d_<NUMTYPE> pt2(10,2);

//	Line2d_<NUMTYPE> li;                              // THIS DOES NOT BUILD (on purpose)
//	li.isInsideRectangle( pt1, pt2 );       // (but we can't test this...)

	Point2d_<NUMTYPE> pt; // (0,0)
	CHECK( pt.isInsideRectangle( pt1, pt2 ) == false );
	pt.set(5,5);
	CHECK( pt.isInsideRectangle( pt1, pt2 ) == true );

	pt.set(10,5);                                         // on the edge
	CHECK( pt.isInsideRectangle( pt1, pt2 ) == true );
	pt.set(5,10);
	CHECK( pt.isInsideRectangle( pt1, pt2 ) == true );

	CHECK( Point2d_<NUMTYPE>( 2, 2).isInsideRectangle( pt1, pt2 ) == true );
	CHECK( Point2d_<NUMTYPE>( 2,10).isInsideRectangle( pt1, pt2 ) == true );
	CHECK( Point2d_<NUMTYPE>(10, 2).isInsideRectangle( pt1, pt2 ) == true );
	CHECK( Point2d_<NUMTYPE>(10,10).isInsideRectangle( pt1, pt2 ) == true );
}

TEST_CASE( "circle intersection", "[test_Circle]" )
{
	Line2d_<NUMTYPE> lid(1,1); // diagonal line going through (0,0)
	Line2d_<NUMTYPE> liv(0,1); // vertical line going through (0,0)
	Line2d_<NUMTYPE> lih(1,0); // horizontal line going through (0,0)
	Point2d_<NUMTYPE> pt;
	auto ri = lid.intersectsCircle( pt, 0.5 );
	CHECK( ri() == true );

	auto rih = lih.intersectsCircle( pt, 1.0 );
	CHECK( rih() == true );
	CHECK( rih.get().first  == Point2d_<NUMTYPE>(-1,0) );
	CHECK( rih.get().second == Point2d_<NUMTYPE>(+1,0) );

	auto riv = liv.intersectsCircle( pt, 1.0 );
	CHECK( riv() == true );
	CHECK( riv.get().first  == Point2d_<NUMTYPE>(0,-1) );
	CHECK( riv.get().second == Point2d_<NUMTYPE>(0,+1) );
}


TEST_CASE( "rectangle intersection", "[test_RI]" )
{
	SECTION( "with diagonal line" )
	{
		Line2d_<NUMTYPE> li(1,1); // diagonal line going through (0,0)

		Point2d_<NUMTYPE> pt1, pt2; // 0,0

//		Point2d_<NUMTYPE> pt3;                              // THIS WOULD GENERATE A COMPILE-TIME ERROR
//		pt3.intersectsRectangle( pt1, pt2 );

		CHECK_THROWS( li.intersectsRectangle( pt1, pt2 ) ); // point identical => unable

		pt2.set(0,4);
		CHECK_THROWS( li.intersectsRectangle( pt1, pt2 ) ); // still fails: x coordinate is identical => unable

		pt2.set(1,1);
		auto ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		auto sol = ri.get();
		CHECK( sol.first  == pt1 );
		CHECK( sol.second == pt2 );

		pt1.set( 5,0 );
		pt2.set( 6,1 );
		ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == false );
	}
	SECTION( "with H/V line" )
	{
		Point2d_<NUMTYPE> pt1, pt2(1,1);                //  rectangle (0,0) - (1,1)

		Line2d_<NUMTYPE> li = Point2d_<NUMTYPE>() * Point2d_<NUMTYPE>(0,1); // vertical line through (0,0)
		auto ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		CHECK( ri.get().first  == pt1 );
		CHECK( ri.get().second == pt2 );

		li = Point2d_<NUMTYPE>(1,0) * Point2d_<NUMTYPE>(1,1);     // vertical line through (1,0)
		ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		CHECK( ri.get().first  == pt1 );
		CHECK( ri.get().second == pt2 );

		li = Point2d_<NUMTYPE>() * Point2d_<NUMTYPE>(1,0);        // horizontal line through (0,0)
		ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		CHECK( ri.get().first  == pt1 );
		CHECK( ri.get().second == pt2 );

		li = Point2d_<NUMTYPE>(0,1) * Point2d_<NUMTYPE>(1,1);     // horizontal line through (0,1)
		ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		CHECK( ri.get().first  == pt1 );
		CHECK( ri.get().second == pt2 );
	}
}


TEST_CASE( "Segment", "[seg1]" )
{
	{
		Segment_<NUMTYPE> s1( Point2d_<NUMTYPE>(0,0), Point2d_<NUMTYPE>(2,2) );
		Segment_<NUMTYPE> s2( Point2d_<NUMTYPE>(2,2), Point2d_<NUMTYPE>(0,0) );
		CHECK( s1 == s2 );
		CHECK( s1.isParallelTo(s2) );
//		CHECK( s1.isParallelTo( Point2d() ) );
		Line2d_<NUMTYPE> l1( Point2d_<NUMTYPE>(10,0), Point2d_<NUMTYPE>(12,2) );
		CHECK( s1.isParallelTo(l1) );
		CHECK( l1.isParallelTo(s1) );
		l1.getAngle( s1 );               //just to make sure that this builds
		s1.getAngle( l1 );
	}
	{
		Segment_<NUMTYPE> s1( Point2d_<NUMTYPE>(0,0), Point2d_<NUMTYPE>(3,4) );
		CHECK( s1.length() == 5 );

		Segment_<NUMTYPE> s2( Point2d_<NUMTYPE>(9,9), Point2d_<NUMTYPE>(8,8) );
		auto pts = s2.get();
		CHECK( pts.first  == Point2d_<NUMTYPE>(8,8) );
		CHECK( pts.second == Point2d_<NUMTYPE>(9,9) );

		auto si = s1.intersects(s2);
		CHECK( si() == false );
	}
	{
		Segment_<NUMTYPE> s1( Point2d_<NUMTYPE>(0,0), Point2d_<NUMTYPE>(2,2) );
		Segment_<NUMTYPE> s2( Point2d_<NUMTYPE>(2,0), Point2d_<NUMTYPE>(0,2) );
		auto si = s1.intersects(s2);
		CHECK( si() == true );
		CHECK( si.get() == Point2d_<NUMTYPE>(1,1) );
	}
}


#ifdef HOMOG2D_USE_OPENCV
TEST_CASE( "Opencv binding", "[test_opencv]" )
{
	cv::Mat mat_64 = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat mat_32 = cv::Mat::eye(3, 3, CV_32F);
	SECTION( "assignement operator()" )
	{
		cv::Mat cvmat = cv::Mat::ones(3,3, CV_32F );
		Homogr H;
		H = cvmat;
		CHECK( H.get(0,0) == 1.);
		CHECK( H.get(1,1) == 1.);
		CHECK( H.get(1,0) == 1.);
		CHECK( H.get(0,1) == 1.);

		cv::Mat cvmat2 = cv::Mat::ones(3,3, CV_32F );
		Homogr H2 = cvmat;
		CHECK( H2.get(0,0) == 1.);
		CHECK( H2.get(1,1) == 1.);
		CHECK( H2.get(1,0) == 1.);
		CHECK( H2.get(0,1) == 1.);

		H = mat_64;
		CHECK( H.get(0,0) == 1.);
		CHECK( H.get(1,1) == 1.);
		CHECK( H.get(1,0) == 0.);
		CHECK( H.get(0,1) == 0.);

		H = mat_32;
		CHECK( H.get(0,0) == 1.);
		CHECK( H.get(1,1) == 1.);
		CHECK( H.get(1,0) == 0.);
		CHECK( H.get(0,1) == 0.);
	}
	SECTION( "default copyTo()" )
	{
		Homogr H;
		cv::Mat mat;
		CHECK_THROWS( H.copyTo( mat, 111 ) );
		CHECK_NOTHROW( H.copyTo( mat ) );
		CHECK( (
			(mat.at<double>(0,0) == 1.0)
			&& (mat.at<double>(0,1) == 0.0)
			&& (mat.at<double>(0,2) == 0.0)
		) );
		CHECK( mat.channels() == 1 );
		CHECK( mat.type() == CV_64F );
	}
	SECTION( "copyTo() with CV_64F" )
	{
		Homogr H;
		cv::Mat mat;
		H.copyTo( mat, CV_64F );
		CHECK( (
			(mat.at<double>(0,0) == 1.0)
			&& (mat.at<double>(0,1) == 0.0)
			&& (mat.at<double>(0,2) == 0.0)
		) );
		CHECK( mat.channels() == 1 );
		CHECK( mat.type() == CV_64F );
	}
	SECTION( "copyTo() with CV_32F" )
	{
		Homogr H;
		cv::Mat mat;
		H.copyTo( mat, CV_32F );
		CHECK( (
			(mat.at<float>(0,0) == 1.0)
			&& (mat.at<float>(0,1) == 0.0)
			&& (mat.at<float>(0,2) == 0.0)
		) );
		CHECK( mat.channels() == 1 );
		CHECK( mat.type() == CV_32F );
	}
	SECTION( "Copy to OpenCv points" )
	{
		Point2d_<NUMTYPE> pt(1.,2.);
		{
			cv::Point2d cvpt1 = getCvPtd( pt );
			CHECK( (cvpt1.x == 1. && cvpt1.y == 2.) );
			cv::Point2f cvpt2 = getCvPtf( pt );
			CHECK( (cvpt2.x == 1. && cvpt2.y == 2.) );
			cv::Point2i cvpt3 = getCvPti( pt );           // integer point
			CHECK( (cvpt3.x == 1 && cvpt3.y == 2) );
		}
		{
			cv::Point2d cvpt1 = pt.getCvPtd() ;
			CHECK( (cvpt1.x == 1. && cvpt1.y == 2.) );
			cv::Point2f cvpt2 = pt.getCvPtf() ;
			CHECK( (cvpt2.x == 1. && cvpt2.y == 2.) );
			cv::Point2i cvpt3 = pt.getCvPti() ;          // integer point
			CHECK( (cvpt3.x == 1  && cvpt3.y == 2 ) );
		}
	}
	SECTION( "Fetch from OpenCv points" )
	{
		cv::Point2d ptd(1,2);
		cv::Point2f ptf(1,2);
		cv::Point2f pti(1,2);
		{                            // test of constructor
			Point2d p1(ptd);
			CHECK(( p1.getX() == 1. && p1.getY() == 2. ));
			Point2d_<NUMTYPE> p2(ptf);
			CHECK(( p2.getX() == 1. && p2.getY() == 2. ));
			Point2d_<NUMTYPE> p3(pti);
			CHECK(( p3.getX() == 1. && p3.getY() == 2. ));
		}
		{                            // test of assignment operator
			Point2d p1 = ptd;
			CHECK(( p1.getX() == 1 && p1.getY() == 2. ));
			Point2d_<NUMTYPE> p2 = ptf;
			CHECK(( p2.getX() == 1 && p2.getY() == 2. ));
			Point2d_<NUMTYPE> p3 = pti;
			CHECK(( p3.getX() == 1 && p3.getY() == 2. ));
		}
	}
	SECTION( "Build line using OpenCv points" )
	{
		Line2d_<NUMTYPE> lia( cv::Point2d(100,200) );
		Line2d_<NUMTYPE> lib( Point2d(100,200) );
		CHECK( lia == lib );
	}

}
#endif

