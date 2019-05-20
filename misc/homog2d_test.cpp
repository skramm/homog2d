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

TEST_CASE( "test1", "[test1]" )
{
	Point2d ptA1; // 0,0
	CHECK( ptA1 == Point2d(0,0) );

	Point2d ptA2(2,2);
	CHECK( ptA2.getX() == 2. );
	CHECK( ptA2.getY() == 2. );

	{ // build line from one point, other one will be (0,0)
		Line2d lA1( ptA2 );
		CHECK( lA1.distTo( ptA1 ) == 0 );
		CHECK( lA1.distTo( ptA2 ) == 0 );

		CHECK( ptA1.distTo( lA1 ) == 0 );
		CHECK( ptA2.distTo( lA1 ) == 0 );
	}

	{ // build line from two points
		Line2d lA1 = ptA1 * ptA2;
		Line2d lA2 = ptA2 * ptA1;

auto dist = lA1.distTo( lA2 );   //

		CHECK( lA1 == lA2 );
		CHECK( lA1.getAngle(lA2) == 0. );
		CHECK( lA2.getAngle(lA1) == 0. );
		CHECK( getAngle(lA1,lA2) == 0. );

		Point2d ptB1(0,2);
		Point2d ptB2(2,0);
		Line2d  lB = ptB1 * ptB2;
		CHECK( lB.getCoord( GC_X, 1 ) == 1. );
		CHECK( lB.getPoint(GC_X, 1) == Point2d(1,1) );
	}

	{

// build point from two diagonal lines
		Line2d lA( Point2d(0,0), Point2d(2,2) );
		Line2d lB( Point2d(0,2), Point2d(2,0) );
		CHECK( Line2d() != lA );

		Point2d mA1 = lA * lB;
		Point2d mA2 = lB * lA;
		CHECK( mA1 == Point2d(1.,1.) );
		CHECK( mA2 == Point2d(1.,1.) );
		CHECK( mA1 != Point2d() );

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

	{ // test of getOrthogonal()
		Line2d lV; // vertical line at x=0

// get orthogonal line at y=100
		Line2d li2 = lV.getOrthogonalLine( GC_Y, 100 );
		CHECK( li2.getAngle( lV ) == M_PI/2. );

//		CHECK( getAngle( li2,lV ) == M_PI/2. ); \TODO

		Line2d lH2(1,0);                // build horizontal line
		Line2d lH3 = lH2;

		lH2.addOffset( OD_Vert, 100 );  // add vertical offset
		CHECK( li2 == lH2 );
		CHECK( lH2.getAngle(lH3) == 0. );

		Line2d lH(1,0);                // build horizontal line
		Line2d li3 = lH.getOrthogonalLine( GC_X, 100 );
		Line2d lV2;
		lV2.addOffset( OD_Horiz, 100 );  // add horizontal offset
		CHECK( li3 == lV2 );
	}
	{
		Line2d li(4,2);
		CHECK( li.getCoord( GC_X, 2 ) == 1 );
		CHECK( li.getCoord( GC_Y, 1 ) == 2 );
		CHECK( li.getPoint( GC_X, 2 ) == Point2d(2,1) );
		CHECK( li.getPoint( GC_Y, 1 ) == Point2d(2,1) );
	}
}

TEST_CASE( "dist2points", "[test2]" )
{
	Line2d li(2,1);
	auto d = li.distTo( Point2d() );
	CHECK( d == 0. );

	auto d2 = li.distTo( Point2d(4,2) );
	CHECK( d2 == 0. );

	CHECK( li.getCoord( GC_X, 0. ) == 0. );
	CHECK( li.getCoord( GC_X, 2. ) == 1. );

	CHECK( li.getCoord( GC_Y, 0. ) == 0. );
	CHECK( li.getCoord( GC_Y, 1. ) == 2. );

	Point2d p1( 3,3);
	Point2d p2( 4,4);
	CHECK( p1.distTo( p2 ) == std::sqrt(2) );
}

TEST_CASE( "offset test", "[test3]" )
{
	Line2d lA( Point2d(0,0), Point2d(2,2) );
	CHECK( lA.distTo( Point2d(1.,1.) ) == 0. );

	Line2d lB = lA;
	lA.addOffset( OD_Vert, 2. );
	CHECK( lA == Line2d( Point2d(0,2), Point2d(2,4) ) );

	lB.addOffset( OD_Horiz, 2. );
	CHECK( lB == Line2d( Point2d(2,0), Point2d(4,2) ) );
	{
		Line2d v;
		Line2d h(1,0);
		CHECK( v*h == Point2d() );    // intersection is (0,0)

		v.addOffset( OD_Horiz, 1 );
		CHECK( v*h == Point2d(1,0) ); // intersection is (1,0)

		h.addOffset( OD_Vert, 1 );
		CHECK( v*h == Point2d(1,1) ); // intersection is (1,0)
	}
	{
		Line2d liV;
		Line2d liV2 = liV;
		liV.addOffset( OD_Vert, 1 ); // adding vertical offset to vertical line does nothing
		CHECK( liV == liV2 );

		Line2d liH(1,0);
		Line2d liH2 = liH;
		std::cout << "liH2=" << liH2 << "\n";
		liH.addOffset( OD_Horiz, 1 ); // adding horizontal offset to horizontal line does nothing
		std::cout << "liH=" << liH << "\n";
		CHECK( liH == liH2 );
	}
}

TEST_CASE( "exceptions", "[testE]" )
{
	Line2d v1,v2; // 2 identical vertical lines

	CHECK_THROWS( v1*v2 );
	v2.addOffset( OD_Horiz, 1 );
	CHECK_THROWS( v1*v2 ); // still no intersection (they never cross)

	Point2d p1,p2;
	CHECK_THROWS( p1*p2 ); // same points can't define a line
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
		Homogr_<double> H;
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
	Line2d line1( 5, 6 ); // line from (0,0) to (5,6)
	Point2d pt1( 5, 6);  // point is on line

	Point2d pt2 = H * pt1; // move the point with H

	H.inverse().transpose();

	Line2d line2 = H * line1; // move the line with H^{-T}

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
		Line2d d1( 5, 6 ); // line from (0,0) to (5,6)
		Point2d pt1( 5, 6);  // point is on line
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
	Line2d liV; // vertical line
	auto pp = liV.getPoints( GC_Y, 0.0, 2.0 ); // get points at a distance 2 from (0,0)
	CHECK( pp.first  == Point2d(0,-2) );
	CHECK( pp.second == Point2d(0,+2) );

	pp = liV.getPoints( GC_Y, 3.0, 2.0 ); // get points at a distance 2 from (0,3)
	CHECK( pp.first  == Point2d(0,+1) );
	CHECK( pp.second == Point2d(0,+5) );

	Line2d liH(1,0); // horizontal line
	pp = liH.getPoints( GC_X, 0.0, 2.0 ); // get points at a distance 2 from (0,0)
	CHECK( pp.first  == Point2d(-2,0) );
	CHECK( pp.second == Point2d(+2,0) );

	pp = liH.getPoints( GC_X, 3.0, 2.0 ); // get points at a distance 2 from (3,0)
	CHECK( pp.first  == Point2d(+1,0) );
	CHECK( pp.second == Point2d(+5,0) );

	Line2d li( 1, 1 );                   // line with slope [1,1] starting from (0,0)
	auto k = 1.0 / std::sqrt(2.);
	pp = li.getPoints( GC_X, 5.0, 1.0 ); // get points at a distance 1 from (5,0)
	CHECK( pp.first  == Point2d( 5-k, 5-k ) );
	CHECK( pp.second == Point2d( 5+k, 5+k ) );


	li = Point2d(3,1) * Point2d(4,2); // line with slope [1,1] starting from (3,1)

	pp = li.getPoints( GC_X, 5.0, 1.0 ); // get points at a distance 2 from (3,0)
	CHECK( pp.first  == Point2d( 5-k, 3-k ) );
	CHECK( pp.second == Point2d( 5+k, 3+k ) );
}

TEST_CASE( "getAngle", "[test_angle]" )
{
	Line2d lid(1,1); // diagonal line going through (0,0)
	Line2d lih(1,0); // horizontal line
	Line2d liv;     // vertical line
	CHECK( lih.getAngle(lid) == M_PI/4. );
	CHECK( liv.getAngle(lid) == M_PI/4. );
	CHECK( liv.getAngle(lih) == M_PI/2. );

	CHECK( getAngle(lih,lid) == M_PI/4. );
	CHECK( getAngle(liv,lid) == M_PI/4. );
	CHECK( getAngle(lih,liv) == M_PI/2. );
}

TEST_CASE( "IsInsideRectangle", "[test_IsInside]" )
{
	Point2d pt1(2,10);
	Point2d pt2(10,2);

//	Line2d li;                              // THIS DOES NOT BUILD (on purpose)
//	li.isInsideRectangle( pt1, pt2 );       // (but we can't test this...)

	Point2d pt; // (0,0)
	CHECK( pt.isInsideRectangle( pt1, pt2 ) == false );
	pt.set(5,5);
	CHECK( pt.isInsideRectangle( pt1, pt2 ) == true );

	pt.set(10,5);                                         // on the edge
	CHECK( pt.isInsideRectangle( pt1, pt2 ) == true );
	pt.set(5,10);
	CHECK( pt.isInsideRectangle( pt1, pt2 ) == true );
}

TEST_CASE( "circle intersection", "[test_Circle]" )
{
	Line2d lid(1,1); // diagonal line going through (0,0)
	Line2d liv(0,1); // vertical line going through (0,0)
	Line2d lih(1,0); // horizontal line going through (0,0)
	Point2d pt;
	auto ri = lid.intersectsCircle( pt, 0.5 );
	CHECK( ri() == true );

	auto rih = lih.intersectsCircle( pt, 1.0 );
	CHECK( rih() == true );
	CHECK( rih.get().first  == Point2d(-1,0) );
	CHECK( rih.get().second == Point2d(+1,0) );

	auto riv = liv.intersectsCircle( pt, 1.0 );
	CHECK( riv() == true );
	CHECK( riv.get().first  == Point2d(0,-1) );
	CHECK( riv.get().second == Point2d(0,+1) );
}


TEST_CASE( "rectangle intersection", "[test_RI]" )
{
	SECTION( "with diagonal line" )
	{
		Line2d li(1,1); // diagonal line going through (0,0)

		Point2d pt1, pt2; // 0,0

//		Point2d pt3;                              // THIS WOULD GENERATE A COMPILE-TIME ERROR
//		pt3.intersectsRectangle( pt1, pt2 );

		CHECK_THROWS( li.intersectsRectangle( pt1, pt2 ) ); // point identical => unable

		pt2.set(0,4);
		CHECK_THROWS( li.intersectsRectangle( pt1, pt2 ) ); // still fails: x coordinate is identical => unable

		pt2.set(1,1);
		Intersect ri = li.intersectsRectangle( pt1, pt2 );
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
		Point2d pt1, pt2(1,1);                //  rectangle (0,0) - (1,1)

		Line2d li = Point2d() * Point2d(0,1); // vertical line through (0,0)
		Intersect ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		CHECK( ri.get().first  == pt1 );
		CHECK( ri.get().second == pt2 );

		li = Point2d(1,0) * Point2d(1,1);     // vertical line through (1,0)
		ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		CHECK( ri.get().first  == pt1 );
		CHECK( ri.get().second == pt2 );

		li = Point2d() * Point2d(1,0);        // horizontal line through (0,0)
		ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		CHECK( ri.get().first  == pt1 );
		CHECK( ri.get().second == pt2 );

		li = Point2d(0,1) * Point2d(1,1);     // horizontal line through (0,1)
		ri = li.intersectsRectangle( pt1, pt2 );
		CHECK( ri() == true );
		CHECK( ri.get().first  == pt1 );
		CHECK( ri.get().second == pt2 );
	}
}

#ifdef HOMOG2D_USE_OPENCV
TEST_CASE( "Opencv binding", "[test_opencv]" )
{
	cv::Mat mat_64 = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat mat_32 = cv::Mat::eye(3, 3, CV_32F);
	SECTION( "getFrom()" )
	{
		cv::Mat mat(3,4, CV_32F );
		Homogr H, H2;
		CHECK_THROWS( H.getFrom( mat ) ); // invalid size
		mat.create( 3,3, CV_8U );
		CHECK_THROWS( H.getFrom( mat ) ); // invalid type

		CHECK_NOTHROW( H.getFrom( mat_64 ) );
		CHECK( H == H2 );

		CHECK_NOTHROW( H.getFrom( mat_32 ) );
		CHECK( H == H2 );
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
}
#endif

