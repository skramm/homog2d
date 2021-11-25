/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019-2021 Sebastien Kramm

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

#define HOMOG2D_BIND_X xxx

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_RUNNER   // alternative: main provided here
#include "catch.hpp"

/// Numerical type for object storage. This is usually defined by makefile
#ifndef NUMTYPE
	#define NUMTYPE double
#endif

//#define HOMOG2D_DEBUGMODE
#define HOMOG2D_TEST_MODE
#include "../homog2d.hpp"

#include <list>

#define LOCALLOG(a) std::cout << " - line " << __LINE__ << ": " << a << '\n'

double g_epsilon = std::numeric_limits<NUMTYPE>::epsilon()*10000.;

using namespace h2d;

#define XSTR(s) STR(s)
#define STR(s) #s

int main( int argc, char* argv[] )
{
	std::cout << "START TESTS:\n - numerical type: " << XSTR(NUMTYPE)
		<< "\n - internal numerical type=" << XSTR(HOMOG2D_INUMTYPE)
		<< "\n - Catch lib version: " << CATCH_VERSION_MAJOR << '.' << CATCH_VERSION_MINOR << '.' << CATCH_VERSION_PATCH
		<< "\n - build option:"
		<< "\n  - HOMOG2D_OPTIMIZE_SPEED: "
#ifdef HOMOG2D_OPTIMIZE_SPEED
		<< "YES"
#else
		<< "NO"
#endif // HOMOG2D_OPTIMIZE_SPEED
		<< "\n  - HOMOG2D_USE_OPENCV: "
#ifdef HOMOG2D_USE_OPENCV
		<< "YES"
#else
		<< "NO"
#endif // HOMOG2D_USE_OPENCV
		<< '\n';

  // global setup...
	Catch::StringMaker<float>::precision = 25;
	Catch::StringMaker<double>::precision = 25;

// removed until PR merged into next release, see https://github.com/catchorg/Catch2/pull/2245
//	Catch::StringMaker<long double>::precision = 25;

	int result = Catch::Session().run( argc, argv );

  // global clean-up...
	return result;
}


TEST_CASE( "types testing", "[testtypes]" )
{
	INFO( "type size" )
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

		Point2d pt(4.,5); // checking with 2 different types
		Point2d_<float> pt2F1;
		Point2d_<double> pt2F2;
		Point2d_<long double> pt2F3;
		pt2F1.set(4.,5); // checking with 2 different types

		CHECK( sizeof(Point2dF) == 12 );
		CHECK( sizeof(Point2dD) == 24 );
		CHECK( sizeof(Point2dL) == 48 );

		CHECK( ptF.type()  == Type::Point2d );
		CHECK( ptF.dtype() == Dtype::Float );
		CHECK( liF.type()  == Type::Line2d );
		CHECK( liF.dtype() == Dtype::Float );
	}

	Point2dD ptD0(1,1);
	Point2dF ptF0(2,2);
	Point2dL ptL0(3,3);

	Line2dD li_D0(1,1);
	Line2dF li_F0(2,2);
	Line2dL li_L0(3,3);

	INFO( "numerical type conversions (assignment)" )
	{
		Point2dD ptD(4,4);
		Point2dF ptF(5,5);
		Point2dL ptL(6,6);
		CHECK( ptF.dtype() == Dtype::Float );
		CHECK( ptD.dtype() == Dtype::Double );
		CHECK( ptL.dtype() == Dtype::LongDouble );

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

		CircleF cf;
		CircleD cd;
		CircleL cl;
		cl = cd;
		cf = cd;
		cd = cf;
		cl = cf;
		cf = cl;
		cd = cl;

		SegmentF sf;
		SegmentD sd;
		SegmentL sl;
		sl = sd;
		sf = sd;
		sd = sf;
		sl = sf;
		sf = sl;
		sd = sl;

		FRectF rf;
		FRectD rd;
		FRectL rl;
		rl = rd;
		rf = rd;
		rd = rf;
		rl = rf;
		rf = rl;
		rd = rl;

		PolylineF pf;
		PolylineD pd;
		PolylineL pl;
		pl = pd;
		pf = pd;
		pd = pf;
		pl = pf;
		pf = pl;
		pd = pl;
	}
	INFO( "numerical type conversions (constructor)" )
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

		CircleF cf;
		CircleD cd;
		CircleL cl;

		CircleL cl2 = cd;
		CircleF cf2 = cd;
		CircleD cd2 = cf;
		CircleL cl3 = cf;
		CircleF cf3 = cl;
		CircleD cd3 = cl;

		SegmentF sf;
		SegmentD sd;
		SegmentL sl;

		SegmentL sl2 = sd;
		SegmentF sf2 = sd;
		SegmentD sd2 = sf;
		SegmentL sl3 = sf;
		SegmentF sf3 = sl;
		SegmentD sd3 = sl;

		FRectF rf;
		FRectD rd;
		FRectL rl;

		FRectL rl2 = rd;
		FRectF rf2 = rd;
		FRectD rd2 = rf;
		FRectL rl3 = rf;
		FRectF rf3 = rl;
		FRectD rd3 = rl;

		PolylineF pf;
		PolylineD pd;
		PolylineL pl;

		PolylineL pl2 = pd;
		PolylineF pf2 = pd;
		PolylineD pd2 = pf;
		PolylineL pl3 = pf;
		PolylineF pf3 = pl;
		PolylineD pd3 = pl;

/**
The goal of theses is to make sure that the conversion (float to double, ...)
does not trigger a build failure. The checking is only there to avoid a warning/
\todo Once we switch to C++17, we can remove the checking and use:
https://en.cppreference.com/w/cpp/language/attributes/maybe_unused
*/
		Line2dD li_D1 = li_F0; CHECK( li_D1.get()[2] == 0. );
		Line2dD li_L1 = li_F0; CHECK( li_L1.get()[2] == 0. );

		Line2dD li_F2 = li_D0; CHECK( li_F2.get()[2] == 0. );
		Line2dD li_L2 = li_D0; CHECK( li_L2.get()[2] == 0. );

		Line2dD li_F3 = li_L0; CHECK( li_F3.get()[2] == 0. );
		Line2dD li_D3 = li_L0; CHECK( li_D3.get()[2] == 0. );
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

		CHECK( lA1 == lA2 );
		CHECK( lA1.getAngle(lA2) == 0. );
		CHECK( lA2.getAngle(lA1) == 0. );
		CHECK( getAngle(lA1,lA2) == 0. );

		Point2d_<NUMTYPE> ptB1(0,2);
		Point2d_<NUMTYPE> ptB2(2,0);
		Line2d_<NUMTYPE>  lB = ptB1 * ptB2;

		auto v1 = lB.getCoord( GivenCoord::X, 1 );
		CHECK( v1 == 1. );
		CHECK( lB.getPoint( GivenCoord::X, 1) == Point2d_<NUMTYPE>(1,1) );
	}
	{
// build point from two diagonal lines
		Line2d_<NUMTYPE> liA( Point2d(0,0), Point2d(2,2) );
		Line2d_<NUMTYPE> liB( Point2d(0,2), Point2d(2,0) );
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

		Line2d_<NUMTYPE> lv2( Point2d(2,0), Point2d(2,2) ); // vertical, x=2
		Line2d_<NUMTYPE> lh2( Point2d(0,2), Point2d(2,2) ); // horizontal, y=2

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
		CHECK( li2.getAngle( lV ) == Approx(M_PI/2.) );
		CHECK( getAngle( li2,lV ) == Approx(M_PI/2.) );

		Line2d_<NUMTYPE> lH2(1,0);                // build horizontal line
		Line2d_<NUMTYPE> lH3 = lH2;

//		lH2.addOffset( LineOffset::vert, 100 );  // add vertical offset
//		CHECK( li2 == lH2 );
		CHECK( lH2.getAngle(lH3) == 0. );

		Line2d_<NUMTYPE> lH(1,0);                // build horizontal line
//		Line2d_<NUMTYPE> li3 = lH.getOrthogonalLine( GivenCoord::X, 100 );
//		Line2d_<NUMTYPE> lV2;
//		lV2.addOffset( LineOffset::horiz, 100 );  // add horizontal offset
//		CHECK( li3 == lV2 );
	}
	{
		Line2d_<NUMTYPE> li(4,2);
		CHECK( li.getCoord( GivenCoord::X, 2 ) == 1 );
		CHECK( li.getCoord( GivenCoord::Y, 1 ) == 2 );
		CHECK( li.getPoint( GivenCoord::X, 2 ) == Point2d_<NUMTYPE>(2,1) );
		CHECK( li.getPoint( GivenCoord::Y, 1 ) == Point2d_<NUMTYPE>(2,1) );
	}
	{
		Line2d_<NUMTYPE> liv1( LineDir::V, 10. );
		Line2d_<NUMTYPE> liv2( Point2d(10,0), Point2d(10,20) );
		CHECK( liv1 == liv2 );
		Line2d_<NUMTYPE> lih1( LineDir::H, 10. );
		Line2d_<NUMTYPE> lih2( Point2d(0,10), Point2d(20,10) );
		CHECK( lih1 == lih2 );
	}
}

TEST_CASE( "test throw", "[test_thr]" )
{
	Line2d li;
	CHECK_THROWS( li.getCoord( GivenCoord::X, 0 ) );

	INFO("Lines and points");
	{
		Line2d_<NUMTYPE> v1,v2; // 2 identical vertical lines
		CHECK_THROWS( v1*v2 );

		Point2d_<NUMTYPE> p1,p2;
		CHECK_THROWS( p1*p2 ); // same points can't define a line
	}
	INFO("rectangle constructor")
	{
		Point2d_<NUMTYPE> p1, p2;
		CHECK_THROWS( FRect( p1,p2) );
		p2.set( 1, 1);
		CHECK_NOTHROW( FRect( p1,p2) );
		CHECK( FRect_<NUMTYPE>( p1,p2).getPts()==std::make_pair(Point2d_<NUMTYPE>(0,0),Point2d_<NUMTYPE>(1,1) ) );
		p1.set( 4, 4 );
		p2.set( 5, 5 );
		CHECK_NOTHROW( FRect( p1,p2) );
		p1.set( 4, 5 );
		p2.set( 5, 4 );
		CHECK_NOTHROW( FRect( p1,p2) );
		p1.set( 5, 4 );
		p2.set( 4, 5 );
		CHECK_NOTHROW( FRect( p1,p2) );

		p1.set( 4, 4 );
		p2.set( 5, 4 );
		CHECK_THROWS( FRect( p1,p2) );
		p2.set( 4, 5 );
		CHECK_THROWS( FRect( p1,p2) );
	}
	INFO("circle constructor")           // 0 not allowed as radius
	{
		Point2d_<NUMTYPE> pt;
		CHECK_THROWS(  Circle_<NUMTYPE> ( pt, 0 ) );
		CHECK_THROWS(  Circle_<NUMTYPE> ( pt, Point2d_<NUMTYPE>::nullDistance()/1.1 ) );
		CHECK_NOTHROW( Circle_<NUMTYPE> ( pt, Point2d_<NUMTYPE>::nullDistance()*1.1 ) );
	}
	INFO("segment constructor")           // can't have identical points
	{
		CHECK_THROWS( Segment_<NUMTYPE> ( Point2d(), Point2d() ) );
		CHECK_THROWS( Segment_<NUMTYPE> ( Point2d_<NUMTYPE>(1,5), Point2d_<NUMTYPE>(1,5) ) );
	}
}

TEST_CASE( "test parallel", "[test_para]" )
{
	INFO( "Checking angle" )
	{
		Line2d_<NUMTYPE> l1;                                                   // vertical line
		Line2d_<NUMTYPE> l2a( Point2d_<NUMTYPE>(0,0),Point2d_<NUMTYPE>(1,1) ); // 45° line, starting at (0,0)
		CHECK( getAngle( l1, l2a ) == Approx( M_PI/4. ) );

		Line2d_<NUMTYPE> l2b( Point2d_<NUMTYPE>(3,0),Point2d_<NUMTYPE>(4,1) ); // 45° line, starting at (3,0)
		CHECK( getAngle( l1, l2b ) == Approx( M_PI/4. ) );

//		l2b.addOffset( LineOffset::horiz, 10. );
//		CHECK( getAngle( l1, l2b ) == Approx( M_PI/4. ) );
//		l2b.addOffset( LineOffset::vert, 10. );
//		CHECK( getAngle( l1, l2b ) == Approx( M_PI/4. ) );
	}

	INFO( "Checking parallel lines" )
	{
		Line2d_<NUMTYPE> l1, l1b; // vertical line
		CHECK( l1.isParallelTo(l1b) );
		{
			Line2d_<NUMTYPE> l2a(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.0005,1.) ); // almost vertical line
			CHECK( l1.isParallelTo(l2a) == true );

			Line2d_<NUMTYPE> l2b(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.002,1.) ); // almost vertical line
			CHECK( l1.isParallelTo(l2b) == false );
		}
		Line2d_<NUMTYPE>::nullAngleValue() = 0.01;
		{
//			std::cout << "null angle=" << Line2d_<NUMTYPE>::nullAngleValue() << " rad.\n";
			Line2d_<NUMTYPE> l2a(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.005,1.) ); // almost vertical line
			INFO( "angle=" << getAngle( l1,l2a) );
			CHECK( l1.isParallelTo(l2a) == true );

			Line2d_<NUMTYPE> l2b(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.02,1.) ); // almost vertical line
			INFO( "angle=" << getAngle( l1,l2b) );
			CHECK( l1.isParallelTo(l2b) == false );
		}
		//l1.isParallelTo( Point2d() ); // NO BUILD
	}
	INFO( "Vertical line at x=0" )
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
	INFO( "dist parallel lines" )
	{
		Line2d_<NUMTYPE> l1;                                // vertical line at x=0
		Line2d_<NUMTYPE> l2( Point2d(1,0), Point2d(1,10) ); // vertical line at x=1
		CHECK( getParallelDistance( l1, l2 ) == 1. );
		Line2d_<NUMTYPE> l3( Point2d(-3,0), Point2d(-3,-10) ); // vertical line at x=1
		CHECK( getParallelDistance( l1, l3 ) == 3. );
		CHECK( getParallelDistance( l2, l3 ) == 4. );
	}
}

TEST_CASE( "dist2points", "[t_d2p]" )
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
#if 0
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
#endif

#ifdef HOMOG2D_FUTURE_STUFF
TEST_CASE( "test Epipmat", "[t_epipmat]" )
{
	Epipmat m;
	Point2d_<NUMTYPE> p1(0,10);

	auto l1 = m*p1;                  // applying an epipolar transformation to a point will return a line
	CHECK( l1.type() == Type::Line2d );

	Line2d_<NUMTYPE> lH( Point2d_<NUMTYPE>(-10,10), Point2d_<NUMTYPE>(10,10) ); // horizontal line at y=10
	CHECK( getAngle( l1, lH)  == 0. );

	auto p2 = m*l1;                  // applying an epipolar transformation to a line will return a point
	CHECK( p2.type() == Type::Point2d );
	CHECK( p2.getY() == 10. );
}
#endif

TEST_CASE( "Homogr constructors", "[testHC]" )
{
	{
		auto angle = 0.5;
		Homogr H0,H1( angle ); // set rotation with constructor
		H0.setRotation( angle );
		Line2d_<NUMTYPE> li1;
		Line2d_<NUMTYPE> li2 = H0 * li1;
		auto angle2 = getAngle( li1, li2 );
		CHECK( std::fabs(angle2 - angle) < Line2d::nullAngleValue() );
//		std::cout << std::setprecision( std::numeric_limits<double>::digits10 ) << std::scientific << "angle2=" << angle2 << " angle=" << angle << "\n";
	}
	{
		Homogr H0( 4. , 7. );
		CHECK( H0.value( 0, 2 ) == 4. );
		CHECK( H0.value( 1, 2 ) == 7. );
	}
}

TEST_CASE( "test Homogr", "[testH]" )
{
	{
		Homogr H1,H2;
		auto H = H1*H2;
		CHECK( H == H1 );
	}
	{
		std::vector<std::vector<float>> m1a(3);
		for( auto& li: m1a)
			li.resize(3,1);
		Homogr_<NUMTYPE> H1a(m1a);

		std::vector<std::vector<double>> m1b(3);
		for( auto& li: m1b)
			li.resize(3,1);
		Homogr_<NUMTYPE> H1b(m1b);

		std::vector<std::vector<int>> m1c(3);
		for( auto& li: m1c)
			li.resize(3,1);
		Homogr_<NUMTYPE> H1c(m1c);

		std::array<std::array<float,3>,3> m2a;
		m2a[2][2] = 1.;
		Homogr_<NUMTYPE> H2a(m2a);

		std::array<std::array<double,3>,3> m2b;
		m2b[2][2] = 1.;
		Homogr_<NUMTYPE> H2b(m2b);

		std::array<std::array<int,3>,3> m2c;
		m2c[2][2] = 1.;
		Homogr_<NUMTYPE> H2c(m2c);
	}
	{ // test of operator * for points
		Homogr_<NUMTYPE> H;
		Point2d_<NUMTYPE> pt1(1,1);
		H.setTranslation( 3., 2. );

		auto pt2 = H * pt1;

		CHECK( pt2.getX() == 4. );
		CHECK( pt2.getY() == 3. );

		H.setRotation( M_PI/2. );
		auto pt3 = H * pt1;

		CHECK( pt3.getX() == Approx( -1. ) );
		CHECK( pt3.getY() == Approx(  1. ) );
	}
	{ // test of operator * for container holding points, using applyTo()
		Homogr_<NUMTYPE> H;
		H.setTranslation(5,6);

		std::vector<Point2d> v_pt(3);
		H.applyTo( v_pt );
		CHECK( v_pt[2].getX() == 5 );
		auto vpt2 = H * v_pt;
		CHECK( v_pt.size() == 3 );

		std::array<Point2d,3> a_pt;
		H.applyTo( a_pt );
		CHECK( a_pt[2].getX() == 5 );

		std::list<Point2d> l_pt(3);
		H.applyTo( l_pt );
		CHECK( std::begin(l_pt)->getX() == 5 );
	}
	{ // test of operator * for container holding points
		Homogr_<NUMTYPE> H;
		H.setTranslation(5,6);

		std::vector<Point2d> v_pt(3);
		auto vpt2 = H * v_pt;
		CHECK( vpt2.size() == 3 );

		std::array<Point2d,3> a_pt;
		auto vpt3 = H * a_pt;
		CHECK( vpt3.size() == 3 );

		std::list<Point2d> l_pt(3);
		auto vpt4 = H * l_pt;
		CHECK( vpt4.size() == 3 );
	}
}

/*
Online inverse:
| 1 -1  2 | -1     |   3/16  -1/32   3/16 |    | -3/2    1/4  -3/2 |
| 4  0  6 |     =  | -17/16  11/32  -1/16 | =  | 17/2  -11/4   1/2 |
| 5  1  1 |        |  -1/8    3/16  -1/8  |    |  1     -3/2   1   |

*/
TEST_CASE( "matrix inversion", "[testH3]" )
{
	Homogr_<NUMTYPE> H;
	{
		Homogr_<NUMTYPE> HR=H;
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

		Homogr_<NUMTYPE> H2;

		H2 = H;        // transposing twice = original matrix
		H2.transpose();
		H2.transpose();
		CHECK( H == H2 );

		H2 = H;        // transposing twice = original matrix
		H2.transpose().transpose();
		CHECK( H == H2 );

		H2 = H;        // inverting twice = original matrix
		H2.inverse();
		Homogr_<NUMTYPE> HI = std::vector<std::vector<NUMTYPE>>{
				{ -3./2,   1./4,  -3./2 },
				{ 17./2, -11./4,   1./2 },
				{  1.  ,  -3./2,   1.   }
			};
		CHECK( H2 == HI );
		H2.inverse();
		CHECK( H == H2 );

		H2 = H;        // inverting twice = original matrix
		H2.inverse().inverse();
		CHECK( H == H2 );

		H2 = H;
		H.inverse();
		Homogr_<NUMTYPE> HR = std::vector<std::vector<double>>{
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
long double
computeDistTransformedLined( Homogr_<NUMTYPE>& H, Point2d_<NUMTYPE> pt1 )
{
	Line2d_<NUMTYPE> line1( pt1 );       // line from (0,0) to pt1
	Point2d_<NUMTYPE> pt2 = H * pt1;     // move the point with H
//	H.inverse().transpose();             // not needed, done automatically since release 2.4 !
	Line2d_<NUMTYPE> line2 = H * line1;  // move the line with H^{-T}
	return line2.distTo( pt2 );          // should be 0 !
}

// from https://stackoverflow.com/a/55868408/193789
std::string FullPrecision( long double d )
{
    auto s = std::ostringstream{};
    s << std::scientific << std::setprecision( std::numeric_limits<long double>::max_digits10 ) << d;
    return s.str();
}

TEST_CASE( "line transformation", "[testH3]" )
{
	{
		Line2d_<NUMTYPE> d1( 5, 6 ); // line from (0,0) to (5,6)
		Point2d_<NUMTYPE> pt1( 5, 6);  // point is on line
		CHECK( d1.distTo( pt1 ) < g_epsilon );
	}
	Point2d_<NUMTYPE> pt( 5, 6);
	Homogr_<NUMTYPE> H;
	{
		H.setTranslation(4,5);
		auto d = computeDistTransformedLined( H , pt );
		LOCALLOG( "T(4,5): d=" << FullPrecision(d) );
		CHECK( d < g_epsilon );
		H.setTranslation(4000,5);
		auto d2 = computeDistTransformedLined( H , pt );
		LOCALLOG( "T(4000,5): d=" << FullPrecision(d2) );
		CHECK( d2 < g_epsilon );
		H.setTranslation(4,5000);
		auto d3 = computeDistTransformedLined( H , pt );
		LOCALLOG( "T(4,5000): d=" << FullPrecision(d3) );
		CHECK( d3 < g_epsilon );
	}
	{
		H.setRotation( 22.*M_PI/180. );
		auto d = computeDistTransformedLined( H , pt );
		LOCALLOG( "rotation: d=" << FullPrecision(d) );
		CHECK( d < g_epsilon );
	}
	{
		H.setScale(0.4, 4.2);
		auto d = computeDistTransformedLined( H , pt );
		LOCALLOG( "scale: d=" << FullPrecision(d) );
		CHECK( d < g_epsilon );
	}
	{
		H.setRotation( 1.456 ).addTranslation(4,5).addScale( 0.4, 1.2 ); // some random transformation
		auto d = computeDistTransformedLined( H , pt );
		LOCALLOG( "complex transformation: d=" << FullPrecision(d) );
		CHECK( d < g_epsilon );
	}
}

TEST_CASE( "matrix chained operations", "[testH2]" )
{
	Homogr H1,H2;
	CHECK( H1 == H2 );
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
	CHECK( lih.getAngle(lid) == Approx(M_PI/4.) );
	CHECK( liv.getAngle(lid) == Approx(M_PI/4.) );
	CHECK( liv.getAngle(lih) == Approx(M_PI/2.) );

	CHECK( getAngle(lih,lid) == Approx(M_PI/4.) );
	CHECK( getAngle(liv,lid) == Approx(M_PI/4.) );
	CHECK( getAngle(lih,liv) == Approx(M_PI/2.) );
//	liv.getAngle( Point2d() );          // NO BUILD
//	getAngle( liv, Point2d() );         // NO BUILD
}


TEST_CASE( "getCorrectPoints", "[gcpts]" )
{
	{
		auto p1 = detail::getCorrectPoints( Point2d(0,0), Point2d(5,5) );
		CHECK( p1.first  == Point2d(0,0) );
		CHECK( p1.second == Point2d(5,5) );
	}
	{
		auto p1 = detail::getCorrectPoints( Point2d(0,5), Point2d(5,0) );
		CHECK( p1.first  == Point2d(0,0) );
		CHECK( p1.second == Point2d(5,5) );
	}
	{
		auto p1 = detail::getCorrectPoints( Point2d(5,0), Point2d(0,5) );
		CHECK( p1.first  == Point2d(0,0) );
		CHECK( p1.second == Point2d(5,5) );
	}
	{
		auto p1 = detail::getCorrectPoints( Point2d(5,5), Point2d(0,0) );
		CHECK( p1.first  == Point2d(0,0) );
		CHECK( p1.second == Point2d(5,5) );
	}
}

//////////////////////////////////////////////////////////////
/////           ISINSIDE TESTS                       /////
//////////////////////////////////////////////////////////////

// this test only makes sure that all theses situations compile
TEST_CASE( "IsInside - manual", "[IsInside_man]" )
{
	FRect rect;
	FRect rect2;
	Circle circle;
	Circle c2;
	Segment seg;
	Ellipse ell(5.,5.);

	CHECK( !rect2.isInside( rect ) );
	CHECK( !rect2.isInside( circle ) );
	CHECK( !rect2.isInside( ell ) );
	CHECK( !c2.isInside( rect ) );
	CHECK( !c2.isInside( circle ) );
	CHECK( !seg.isInside( rect ) );
	CHECK( !seg.isInside( circle ) );
	CHECK( !seg.isInside( ell ) );
}

TEST_CASE( "IsInside Rectangle", "[test_IsInside]" )
{
	Point2d_<NUMTYPE> pt1(2,10);
	Point2d_<NUMTYPE> pt2(10,2);

//	Line2d_<NUMTYPE> li;                              // THIS DOES NOT BUILD (on purpose)
//	li.isInside( pt1, pt2 );       // (but we can't test this...)

	Point2d_<NUMTYPE> pt; // (0,0)
	CHECK( pt.isInside( pt1, pt2 ) == false );
	pt.set(5,5);
	CHECK( pt.isInside( pt1, pt2 ) == true );

	pt.set(10,5);                                         // on the edge
	CHECK( pt.isInside( pt1, pt2 ) == false );
	pt.set(5,10);
	CHECK( pt.isInside( pt1, pt2 ) == false );

	pt.set(4.999,9.99);
	CHECK( pt.isInside( pt1, pt2 ) == true );

	CHECK( Point2d_<NUMTYPE>( 2, 2).isInside( pt1, pt2 ) == false );
	CHECK( Point2d_<NUMTYPE>( 2,10).isInside( pt1, pt2 ) == false );
	CHECK( Point2d_<NUMTYPE>(10, 2).isInside( pt1, pt2 ) == false );
	CHECK( Point2d_<NUMTYPE>(10,10).isInside( pt1, pt2 ) == false );

	FRect_<NUMTYPE> r(2,3, 10,10);
	CHECK( r.length() == 30 );
	CHECK( r.area()   == 56 );
	CHECK( r.width()  == 8 );
	CHECK( r.height() == 7 );

	CHECK( Segment( 2,5, 4,5 ).isInside( r )           == false ); // on the contour
	CHECK( Segment( 2.00001, 5., 4.,5. ).isInside( r ) == true );
	CHECK( Segment( 3,5, 4,5 ).isInside( r )           == true );

	CHECK( Circle( 5,5, 2 ).isInside( r ) == false );   // touches rectangle at (5,3)
	CHECK( Circle( 5,5, 1 ).isInside( r ) == true );
	CHECK( Circle( 6,6, 2 ).isInside( r ) == true );
	CHECK( Circle().isInside( r )         == false );

	CHECK( Circle( 6,6, 22 ).isInside( r ) == false);
	CHECK( r.isInside( Circle( 6,6, 22 ) ) == true );

	Polyline pl( IsClosed::Yes );
	pl.add( 3,3 );
	CHECK( pl.isInside( r ) == false );  // on contour
	Polyline pl2( IsClosed::Yes );
	pl2.add( 4,4 );
	CHECK( pl2.isInside( r ) == true );
	pl2.add( 4,5 );
	CHECK( pl2.isInside( r ) == true );
}


TEST_CASE( "IsInside circle", "[tic]" )
{
	Circle_<NUMTYPE> c1(10.);
	Circle_<NUMTYPE> c2(2.);
	{
		CHECK( c2.isInside(c1) );  // circle inside circle
		CHECK( !c1.isInside(c2) );
		CHECK( !c1.isInside(c1) );
		CHECK( c1 != c2 );
		CHECK( c1 == c1 );
	}
	{
		Point2d p1( 3,3 );          // point inside circle
		CHECK( p1.isInside(c1) );
		CHECK( !p1.isInside(c2) );
		CHECK( p1.isInside(  Point2d(0,0), 8 ) );
		CHECK( !p1.isInside( Point2d(0,0), 2 ) );
	}
	{
		Circle_<NUMTYPE> cA( Point2d(),   10.);
		CHECK( cA.radius() == 10. );
		cA.radius() = 12;
		CHECK( cA.radius() == 12. );
		CHECK( cA.center() == Point2d(0,0) );

		Circle_<NUMTYPE> cB( Point2d(5,0), 2.);
		auto seg = getSegment<NUMTYPE>( cA, cB );
		CHECK( seg.getPts().first  == Point2d(0,0) );
		CHECK( seg.getPts().second == Point2d(5,0) );
		CHECK( seg.length() == 5 );
	}
}

//////////////////////////////////////////////////////////////
/////           INTERSECTION TESTS                       /////
//////////////////////////////////////////////////////////////

TEST_CASE( "Intersection", "[int_all]" )
{
	FRect r1,r2;
	Circle c1,c2;
	Segment s1,s2;
	Line2d l1,l2;

	CHECK( !r1.intersects( r2 )() );
	CHECK( !c1.intersects( c2 )() );
	CHECK( !s1.intersects( s2 )() );
	CHECK( !l1.intersects( l2 )() );

	CHECK( r1.intersects( c2 )() );
	CHECK( r1.intersects( s2 )() );
	CHECK( r1.intersects( l2 )() );

	CHECK( c1.intersects( r2 )() );
	CHECK( c1.intersects( s2 )() );
	CHECK( c1.intersects( l2 )() );

	CHECK( s1.intersects( r2 )() );
	CHECK( s1.intersects( c2 )() );
	CHECK( s1.intersects( l2 )() );

	CHECK( l1.intersects( r2 )() );
	CHECK( l1.intersects( c2 )() );
	CHECK( l1.intersects( s2 )() );
}

TEST_CASE( "Line/Line intersection", "[int_LL]" )
{
	Line2d_<NUMTYPE> liv1,liv3;
	Line2d_<NUMTYPE> liv2( Point2d(5,0), Point2d(5,10) );
	Line2d_<NUMTYPE> lih( Point2d(0,0), Point2d(1,0) );
	CHECK(  liv1.intersects( lih  )() );
	CHECK( !liv1.intersects( liv2 )() );
	CHECK(  lih.intersects(  liv2 )() );
	CHECK( !liv1.intersects( liv3 )() );

	auto i1 = liv1.intersects( lih  );
	auto i2 = liv1.intersects( liv2 );
	auto i3 = lih.intersects(  liv2 );

	CHECK(  i1() );
	CHECK( !i2() );
	CHECK(  i3() );

	CHECK( i1.size()==1 );
	CHECK( i2.size()==0 );
	CHECK( i3.size()==1 );
}

TEST_CASE( "Segment/Segment intersection", "[int_SS]" )
{
	Segment_<NUMTYPE> s1,s2;
	{
		CHECK( s1 == s2 );
		auto si = s1.intersects(s2);
		CHECK( si() == false );
		CHECK( si.size() == 0 );
	}
	{
		s1.set( Point2d(0,0), Point2d(4,4) ); // diagonal
		s2.set( Point2d(0,4), Point2d(4,0) );
		auto si = s1.intersects(s2);
		CHECK( si() == true );
		CHECK( si.size() == 1 );
		CHECK( si.get() == Point2d(2,2) );
	}
	{
		s1.set( Point2d(0,0), Point2d(10,0) ); // overlapping (complete
		s2.set( Point2d(5,0), Point2d(15,0) );
		auto si = s1.intersects(s2);
		CHECK( si() == false );
		CHECK( si.size() == 0 );
	}
	{
		s1.set( Point2d(0,0), Point2d(0,1) );  // vertical
		s2.set( Point2d(1,1), Point2d(0,1) );  // horizontal
		auto si = s1.intersects(s2);
		CHECK( si() );
		CHECK( si.size() == 1 );
		CHECK( si.get() == Point2d(0,1) );
	}
	{
		s1.set( Point2d(0,0), Point2d(0,2) );  // vertical

		s2.set( Point2d(1,1), Point2d(0,1) );  // horizontal, touches the other one in the middle
		{
			auto si = s1.intersects(s2);
			CHECK( si() );
			CHECK( si.size() == 1 );
			CHECK( si.get() == Point2d(0,1) );
		}
		s2.set( Point2d(0,0), Point2d(1,0) );  // horizontal, touches the edge of other one
		{
			auto si = s1.intersects(s2);
			CHECK( si() );
			CHECK( si.size() == 1 );
			CHECK( si.get() == Point2d(0,0) );
		}
		s2.set( Point2d(1,1), Point2d(0,1) );  // horizontal, touches the edge of other one
		{
			auto si = s1.intersects(s2);
			CHECK( si() );
			CHECK( si.size() == 1 );
			CHECK( si.get() == Point2d(0,1) );
		}
		s2.set( Point2d(-1,1), Point2d(0,1) );  // horizontal, touches the edge of other one
		{
			auto si = s1.intersects(s2);
			CHECK( si() );
			CHECK( si.size() == 1 );
			CHECK( si.get() == Point2d(0,1) );
		}
	}
}

TEST_CASE( "Circle/Circle intersection", "[int_CC]" )
{
	{
		Circle_<NUMTYPE> cA, cB;
		CHECK( cA == cB );
		CHECK( !cA.intersects(cB)() );
	}
	{
		Circle_<NUMTYPE> cA;
		Circle_<NUMTYPE> cB( Point2d(5,5), 2 );
		CHECK( cA != cB );
		CHECK( !cA.intersects(cB)() );
	}
	{
		Circle_<NUMTYPE> cA( Point2d(0,0), 2 );
		Circle_<NUMTYPE> cB( Point2d(3,0), 2 );
		CHECK( cA != cB );
		CHECK( cA.intersects(cB)() );
	}
	{
		Circle_<NUMTYPE> cA( Point2d(0,0), 1 );
		Circle_<NUMTYPE> cB( Point2d(2,0), 1 );
		CHECK( cA != cB );
		CHECK( cA.intersects(cB)() );
		auto inter = cA.intersects(cB);
		CHECK( inter() == true );
		CHECK( inter.size() == 2 );
		CHECK( inter.get().first  == Point2d( 1,0) );
		CHECK( inter.get().second == Point2d( 1,0) );
	}
}

// This part uses externally defined rectangles, check the files
// figures_test/frect_intersect_*.code
// this is done so we can, for a single defined rectangle pair,
// have both the test code here, and a graphical representation of the situation.
// The corresponding images are built with `$ make test_fig`
TEST_CASE( "FRect/FRect intersection", "[int_FF]" )
{
	{                                   // identical rectangles
		FRect_<NUMTYPE> r1, r2;
		CHECK( r1 == r2 );
		CHECK( !r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 0 );

		auto u1 = intersectArea(r1,r2);
		CHECK( u1() == true );
		CHECK( u1.get() == r1 );
	}
	{
#include "figures_test/frect_intersect_1.code"
		CHECK( r1 != r2 );
		CHECK( r1.height() == r2.height() );
		CHECK( !r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 0 );

		auto a = r1.intersectArea(r2);
		CHECK( a() == false );       // no intersection !
		auto b = r1&r2;
		CHECK( b() == false );       // no intersection !
	}
	{
#include "figures_test/frect_intersect_2.code"
		CHECK( r1 != r2 );
		CHECK( r1.width()  == r2.width() );
		CHECK( r1.height() == r2.height() );
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 2 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d(2,2) );
		CHECK( vpts[1] == Point2d(3,1) );

		auto rect_inter = r1.intersectArea(r2);
		CHECK( rect_inter() == true );
		CHECK( rect_inter.get() == FRect(2,1, 3,2) );
		auto rect_inter2 = r1&r2;
		CHECK( rect_inter2() == true );
		CHECK( rect_inter2.get() == FRect(2,1, 3,2) );
	}

	{      // 4 intersection points
#include "figures_test/frect_intersect_3.code"
		CHECK( r1 != r2 );
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 4 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d(2,2) );
		CHECK( vpts[1] == Point2d(2,4) );
		CHECK( vpts[2] == Point2d(4,2) );
		CHECK( vpts[3] == Point2d(4,4) );

		auto rect_inter = r1.intersectArea(r2);
		CHECK( rect_inter() == true );
		CHECK( rect_inter.get() == FRect(2,2, 4,4) );
	}

	{     // horizontal segment overlap
#include "figures_test/frect_intersect_4.code"
		CHECK( r1 != r2 );
		CHECK( r1.width()  == r2.width() );
		CHECK( r1.height() == r2.height() );
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 4 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d(2,0) );
		CHECK( vpts[1] == Point2d(2,2) );
		CHECK( vpts[2] == Point2d(3,0) );
		CHECK( vpts[3] == Point2d(3,2) );

		auto rect_inter = r1.intersectArea(r2);
		CHECK( rect_inter() == true );
		CHECK( rect_inter.get() == FRect(2,0, 3,2) );
	}
	{     // common vertical segment
#include "figures_test/frect_intersect_5.code"
		CHECK( r1 != r2 );
		CHECK( r1.width()  != r2.width() );
		CHECK( r1.height() == r2.height() );
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters() == true );
		CHECK( inters.size() == 2 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 3,0 ) );
		CHECK( vpts[1] == Point2d( 3,2 ) );
		CHECK( r1.intersectArea(r2)() == false ); // no intersection

		r2.translate( 0.000001, 0 );         // move it a bit left
		inters = r1.intersects(r2);          // => no more intersection
		CHECK( inters() == false );
		CHECK( inters.size() == 0 );
		CHECK( r1.intersectArea(r2)() == false ); // still no intersection

	}
	{     // two rectangles joined by corner at 3,2
#include "figures_test/frect_intersect_6.code"
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 1 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 3,2 ) );
		CHECK( r1.intersectArea(r2)() == false ); // only one point !
	}
	{     // two rectangles
#include "figures_test/frect_intersect_7.code"
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 2 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 2,2 ) );
		CHECK( vpts[1] == Point2d( 4,2 ) );

		auto rect_inter = r1.intersectArea(r2);
		CHECK( rect_inter() == true );
		CHECK( rect_inter.get() == FRect(2,2, 4,3) );
	}
	{     // two rectangles with a single common segment
#include "figures_test/frect_intersect_8.code"
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 3 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 3,3 ) );
		CHECK( vpts[1] == Point2d( 4,2 ) );
		CHECK( vpts[2] == Point2d( 4,3 ) );
//		auto rect_inter = r1.intersectArea(r2);
//		CHECK( rect_inter == FRect(3,2, 4,3) );
	}
	{     // two rectangles with a single common segment
#include "figures_test/frect_intersect_9.code"
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 3 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 3,3 ) );
		CHECK( vpts[1] == Point2d( 4,2 ) );
		CHECK( vpts[2] == Point2d( 4,3 ) );
//		auto rect_inter = r1.intersectArea(r2);
//		CHECK( rect_inter == FRect(3,2, 4,3) );
	}

	{ // one rectangle inside the other
#include "figures_test/frect_intersect_10.code"
		CHECK( r1.intersects(r2)() == false );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 0 );
		CHECK( r1.intersectArea(r2)() == false );
	}
	{ // one rectangle inside the other, with a common segment
#include "figures_test/frect_intersect_11.code"
		CHECK( r1.intersects(r2)() == true );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 2 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 2,3 ) );
		CHECK( vpts[1] == Point2d( 3,3 ) );
		CHECK( r1.intersectArea(r2)() == true );
		auto inter=r1.intersectArea(r2);
		CHECK( inter.get() == r2 );
	}
}


TEST_CASE( "Circle/Segment intersection", "[int_CS]" )
{
	Circle_<NUMTYPE> c1( Point2d(1,1), 1 ); // circle centered on 1,1, going horizontally from 0 to 2 at y=0
	{
		Segment_<NUMTYPE> s1( Point2d(0,20),Point2d(2,20) ); // horizontal segment at y=20

		CHECK( !c1.intersects( s1 )() );
		CHECK( !s1.intersects( c1 )() );
		auto int_a = c1.intersects( s1 );
		auto int_b = s1.intersects( c1 );
		CHECK( !int_a() );
		CHECK( !int_b() );
		CHECK( int_a.size() == 0 );
		CHECK( int_b.size() == 0 );

		Segment_<NUMTYPE> s2( Point2d(10,0),Point2d(10,20) ); // vertical segment at x=10
		CHECK( !c1.intersects( s2 )() );
		CHECK( !s2.intersects( c1 )() );
		auto int_a2 = c1.intersects( s2 );
		auto int_b2 = s2.intersects( c1 );
		CHECK( int_a2.size() == 0 );
		CHECK( int_b2.size() == 0 );
		CHECK( !int_a2() );
		CHECK( !int_b2() );
	}
	{
		Segment_<NUMTYPE> s2( Point2d(-5,1),Point2d(+5,1) ); // horizontal segment at y=1

		CHECK( c1.intersects( s2 )() );
		CHECK( s2.intersects( c1 )() );

		auto int_a = c1.intersects( s2 );
		auto int_b = s2.intersects( c1 );
		CHECK( int_a() );
		CHECK( int_b() );
		CHECK( int_a.size() == 2 );
		CHECK( int_b.size() == 2 );
		CHECK( int_a.get()[0] == Point2d_<NUMTYPE>(0,1) );
		CHECK( int_b.get()[1] == Point2d_<NUMTYPE>(2,1) );
	}
	{
		Segment_<NUMTYPE> s2( Point2d(-5,1),Point2d(1,1) ); // horizontal segment at y=1

		CHECK( c1.intersects( s2 )() );
		CHECK( s2.intersects( c1 )() );

		auto int_a = c1.intersects( s2 );
		auto int_b = s2.intersects( c1 );
		CHECK( int_a() );
		CHECK( int_b() );
		CHECK( int_a.size() == 1 );
		CHECK( int_b.size() == 1 );
		CHECK( int_a.get()[0] == Point2d_<NUMTYPE>(0,1) );
	}
	{
		Segment_<NUMTYPE> s2( Point2d(2,0),Point2d(4,0) ); // horizontal segment at y=0, outside circle

		CHECK( !c1.intersects( s2 )() );
		CHECK( !s2.intersects( c1 )() );

		auto int_a = c1.intersects( s2 );
		auto int_b = s2.intersects( c1 );
		CHECK( int_a() == false );
		CHECK( int_b() == false );
		CHECK( int_a.size() == 0 );
		CHECK( int_b.size() == 0 );
	}
	{
		Segment_<NUMTYPE> s2( Point2d(2,1),Point2d(4,1) ); // horizontal segment at y=0, touching edge of circle at (1,1)

		CHECK( c1.intersects( s2 )() );
		CHECK( s2.intersects( c1 )() );

		auto int_a = c1.intersects( s2 );
		auto int_b = s2.intersects( c1 );
		CHECK( int_a() );
		CHECK( int_b() );
		CHECK( int_a.size() == 1 );
		CHECK( int_b.size() == 1 );
		CHECK( int_a.get()[0] == Point2d_<NUMTYPE>(2,1) );
	}
}

TEST_CASE( "Circle/FRect intersection", "[int_CF]" )
{
	{
//#include "figures_test/circle_intersect_1.code"
		Circle_<NUMTYPE> r1( Point2d(1,1), 1 );
		FRect_<NUMTYPE> r2( Point2d(3,2), Point2d(4,3) );
		CHECK( r1.intersects(r2)() == false );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 0 );
	}
	{
//#include "figures_test/circle_intersect_2.code"
		Circle_<NUMTYPE> r1( Point2d(3,3), 2 );
		FRect_<NUMTYPE> r2( Point2d(3,2), Point2d(4,3) );
		CHECK( r1.intersects(r2)() == false );
	}
	{
		Circle_<NUMTYPE> r1;
		FRect_<NUMTYPE> r2( 0,0, 3,3 );
		CHECK( r1.intersects(r2)() == true );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 2 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 0,1 ) );
		CHECK( vpts[1] == Point2d( 1,0 ) );
	}
	{
		Circle_<NUMTYPE> r1; // (0,0) with radius=1
		FRect_<NUMTYPE> r2( 1,0, 3,3 );
		CHECK( r1.intersects(r2)() == true );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 1 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 1,0 ) );
	}
}

TEST_CASE( "Circle/Line intersection", "[int_CL]" )
{
	Line2d_<NUMTYPE> lid(1,1); // diagonal line going through (0,0)
	Line2d_<NUMTYPE> liv; // vertical line at x=0
	Line2d_<NUMTYPE> lih( Point2d(-1,0), Point2d(1,0) ); // horizontal line at y=0
	Point2d_<NUMTYPE> pt;
	{
		CHECK( lid.intersects( Point2d_<NUMTYPE>(), 0.5 )() );
		auto ri = lid.intersects( Point2d_<NUMTYPE>(), 0.5 );
		CHECK( ri() == true );
		auto ri2 = lid.intersects( Point2d_<NUMTYPE>(10,5), 1. );
		CHECK( ri2() == false );
		auto ri3 = lid.intersects( Circle_<NUMTYPE>(Point2d_<NUMTYPE>(), 0.5 ) );
		CHECK( ri3() == true );
		auto ri4 = lid.intersects( Circle_<NUMTYPE>(Point2d_<NUMTYPE>(10,5), 1. ) );
		CHECK( ri4() == false );
	}
	{
		auto rih = lih.intersects( pt, 1.0 );
		CHECK( rih() == true );
		CHECK( rih.get().first  == Point2d(-1,0) );
		CHECK( rih.get().second == Point2d(+1,0) );
	}
	{
		auto riv = liv.intersects( pt, 1.0 );
		CHECK( riv() == true );
		CHECK( riv.get().first  == Point2d(0,-1) );
		CHECK( riv.get().second == Point2d(0,+1) );
	}

	Circle_<NUMTYPE> cir2(45);
	CHECK( cir2.radius() == 45. );
	Circle_<NUMTYPE> cir3( Point2d(4,6),7);
	CHECK( cir3.radius() == 7. );
	{
		auto cl1 = cir2.intersects( liv );
		CHECK( cl1() == true );
		CHECK( cl1.get().first  == Point2d(0,-45) );
		CHECK( cl1.get().second == Point2d(0,+45) );
	}
	{
		Circle_<NUMTYPE> c4( Point2d(1,1),1);
		Line2d_<NUMTYPE> l4( Point2d(-5,1), Point2d(5,1));
		auto res1 = c4.intersects( l4 );
		CHECK( res1() == true );
		CHECK( res1.size() == 2 );
		CHECK( res1.get().first  == Point2d(0,1) );
		CHECK( res1.get().second == Point2d(2,1) );

		auto res2 = l4.intersects( c4 );
		CHECK( res2() == true );
		CHECK( res2.size() == 2 );
		CHECK( res2.get().first  == Point2d(0,1) );
		CHECK( res2.get().second == Point2d(2,1) );
	}
}


TEST_CASE( "Line/Segment intersection", "[int_LS]" )
{
	Line2d_<NUMTYPE> li;             // vertical line x=0
	{
		Segment_<NUMTYPE> seg;           // (0,0 -- (1,1)
		CHECK( li.intersects( seg )() );
		CHECK( seg.intersects( li )() );
		auto ri1 = li.intersects( seg );
		auto ri2 = seg.intersects( li );
		CHECK( ri1.size() == 1 );
		CHECK( ri2.size() == 1 );

		CHECK( ri1.get() == Point2d(0,0) );
		CHECK( ri2.get() == Point2d(0,0) );
	}
	{
		Segment_<NUMTYPE> seg( 0,0, 0,2 );  // vertical x=0
		CHECK( !li.intersects( seg )() );
		CHECK( !seg.intersects( li )() );
	}
	{
		Segment_<NUMTYPE> seg( 1,0, 1,2 );
		CHECK( !li.intersects( seg )() );
		CHECK( !seg.intersects( li )() );
	}
}

TEST_CASE( "Line/FRect intersection", "[int_LF]" )
{
	INFO( "with diagonal line" )
	{
		Line2d_<NUMTYPE> li(1,1); // diagonal line going through (0,0)
		Point2d_<NUMTYPE> pt1, pt2; // 0,0

		pt2.set(1,1);
		auto ri = li.intersects( FRect(pt1, pt2) );
		CHECK( ri() == true );
		auto sol = ri.get();
		CHECK( sol.first  == pt1 );
		CHECK( sol.second == pt2 );

		pt1.set( 5,0 );
		pt2.set( 6,1 );
		ri = li.intersects( FRect(pt1, pt2) );
		CHECK( ri() == false );
	}
	INFO( "with H/V line" )
	{
		Point2d_<NUMTYPE> pt1, pt2(1,1);                //  rectangle (0,0) - (1,1)
		FRect r1(pt1, pt2);
		Line2d_<NUMTYPE> li = Point2d_<NUMTYPE>() * Point2d_<NUMTYPE>(0,1); // vertical line at y=x
		auto ri1 = li.intersects( r1 );
		auto ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );

		CHECK( ri1.get().first  == pt1 );
		CHECK( ri1.get().second == Point2d(0,1) );
		CHECK( ri2.get().first  == pt1 );
		CHECK( ri2.get().second == Point2d(0,1) );

		li = Point2d_<NUMTYPE>(1,0) * Point2d_<NUMTYPE>(1,1);     // vertical line at x=1
		ri1 = li.intersects( r1 );
		ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );
		CHECK( ri1.get().first  == Point2d(1,0) );
		CHECK( ri1.get().second == Point2d(1,1) );

		li = Point2d_<NUMTYPE>() * Point2d_<NUMTYPE>(1,0);        // horizontal line at y=0
		ri1 = li.intersects( r1 );
		ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );
		CHECK( ri1.get().first  == pt1 );
		CHECK( ri1.get().second == Point2d(1,0) );

		li = Point2d_<NUMTYPE>(-1,1) * Point2d_<NUMTYPE>(1,1);     // horizontal line at y=1
		ri1 = li.intersects( r1 );
		ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );
		CHECK( ri1.get().first  == Point2d(0,1) );
		CHECK( ri1.get().second == Point2d(1,1) );

		li = Point2d_<NUMTYPE>(-1.,.5) * Point2d_<NUMTYPE>(2.,.5);     // horizontal line at y=0.5
		ri1 = li.intersects( r1 );
		ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );
		CHECK( ri1.get().first  == Point2d(0.,.5) );
		CHECK( ri1.get().second == Point2d(1.,.5) );
	}
}

//////////////////////////////////////////////////////////////
/////               MISC. TESTS                          /////
//////////////////////////////////////////////////////////////

TEST_CASE( "Circle", "[cir1]" )
{
	{
		Circle_<NUMTYPE> c1;       // Default constructor
		CHECK( c1.center() == Point2d(0,0) );
		CHECK( center(c1)  == Point2d(0,0) );
		CHECK( c1.radius() == 1. );
	}
	{
		Circle_<NUMTYPE> c1(444);  // Constructor 2
		CHECK( c1.center() == Point2d(0,0) );
		CHECK( c1.radius() == 444. );
	}
	{
		Point2d pt( 4,5);
		Circle_<NUMTYPE> c1(pt,3);  // Constructor 3
		CHECK( c1.center() == Point2d(4,5) );
		CHECK( c1.radius() == 3 );
	}
	{
		Circle_<NUMTYPE> c1(1,2,3);  // Constructor 4
		CHECK( c1.center() == Point2d(1,2) );
		CHECK( c1.radius() == 3 );
	}
}


TEST_CASE( "Ellipse", "[ell1]" )
{
	{
		Ellipse_<NUMTYPE> el;
		CHECK( el.center() == Point2d(0,0) );
		CHECK( center(el)  == Point2d(0,0) );
		CHECK( el.getMajMin().first == 2.0 );
		CHECK( el.getMajMin().second == 1.0 );
		CHECK( el.angle() == 0.0 );
		CHECK( angle(el)  == 0.0 );
		CHECK( el.isCircle() == false );
		CHECK( isCircle(el) == false );
	//	CHECK( el.getBB() == Point2d(0,0) );
	}
	{
		Circle c(1,2,3);
		Ellipse_<NUMTYPE> el(c);
		CHECK( el.center() == Point2d(1,2) );
		CHECK( center(el)  == Point2d(1,2) );
		CHECK( el.getMajMin().first == 3.0 );
		CHECK( el.getMajMin().second == 3.0 );
		CHECK( el.angle() == 0.0 );
		CHECK( angle(el)  == 0.0 );
		CHECK( el.isCircle() == true );
		CHECK( isCircle(el) == true );
	}

	{
		Ellipse_<NUMTYPE> el(1,2,3.0,3.00001);
		CHECK( el.center() == Point2d(1,2) );
		CHECK( el.isCircle() == false );       // using default threshold
		CHECK( isCircle(el) == false );

		CHECK( el.isCircle(1E-3) == true );    // using arbitrary threshold
		CHECK( isCircle(el,1E-3) == true );
	}

	{
		Ellipse_<NUMTYPE> el(4,5,6,7);
		CHECK( el.center() == Point2d(4,5) );
		CHECK( el.getMajMin().first  == Approx(7.0) );
		CHECK( el.getMajMin().second == Approx(6.0) );
		CHECK( el.angle() == 0.0 );
		CHECK( el.isCircle() == false );
	}
	{
		Ellipse_<NUMTYPE> el(4,5, 6, 7, 1 /*rad.*/ );
		CHECK( el.center() == Point2d(4,5) );
		CHECK( el.getMajMin().first  == Approx(7.0) );
		CHECK( el.getMajMin().second == Approx(6.0) );
		CHECK( el.angle() == Approx(1.0) );
		CHECK( el.isCircle() == false );
	}
}

TEST_CASE( "Segment", "[seg1]" )
{
	{                              // test order of points
		Point2d p1(43,8);
		Point2d p2(43,18);
		Point2d p3(5,55);
		{
			Segment_<NUMTYPE> s( p1, p2 );   // same x value
			CHECK( s.getPts().first == p1 );
			CHECK( s.getPts().second == p2 );
		}
		{
			Segment_<NUMTYPE> s( p2, p1 );   // same x value
			CHECK( s.getPts().first == p1 );
			CHECK( s.getPts().second == p2 );
		}
		{
			Segment_<NUMTYPE> s( p1, p3 );
			CHECK( s.getPts().first == p3 );
			CHECK( s.getPts().second == p1 );
		}
		{
			Segment_<NUMTYPE> s( p3, p1 );
			CHECK( s.getPts().first == p3 );
			CHECK( s.getPts().second == p1 );
		}
	}
	{
		Line2d_<NUMTYPE> li( Point2d(0,0), Point2d(2,2) );
		Segment_<NUMTYPE> s1( Point2d(0,0), Point2d(2,2) );
		Segment_<NUMTYPE> s2( Point2d(2,2), Point2d(0,0) );
		CHECK( s1 == s2 );
		CHECK( s1.isParallelTo(s2) );
		Line2d_<NUMTYPE> l1( Point2d(10,0), Point2d(12,2) );
		CHECK( s1.isParallelTo(l1) );
		CHECK( l1.isParallelTo(s1) );
		CHECK( l1.getAngle( s1 ) == Approx(0) );
		CHECK( s1.getAngle( l1 ) == Approx(0) );
		CHECK( s1.getAngle( s2 ) == Approx(0) );
		CHECK( getAngle( s1, s2 ) == Approx(0) );
		CHECK( getAngle( li, s2 ) == Approx(0) );
		CHECK( getAngle( s2, li ) == Approx(0) );
	}
	{
		Segment_<NUMTYPE> s1( Point2d(0,0), Point2d(3,4) );
		CHECK( s1.length() == 5 );

		Segment_<NUMTYPE> s2( Point2d(9,9), Point2d(8,8) );
		auto pts = s2.getPts();
		CHECK( pts.first  == Point2d_<NUMTYPE>(8,8) );
		CHECK( pts.second == Point2d_<NUMTYPE>(9,9) );
	}
	{
		Segment_<NUMTYPE> s1( Point2d(0,0), Point2d(2,2) );
		Segment_<NUMTYPE> s2( Point2d(2,0), Point2d(0,2) );
		auto si = s1.intersects(s2);
		CHECK( si() == true );
		CHECK( si.get() == Point2d_<NUMTYPE>(1,1) );

		auto pt = s1.getMiddlePoint();
		CHECK( pt == Point2d_<NUMTYPE>(1,1) );
	}
	{  // test that points on a line at equal distance from a point, when
		// transformed in a segment, we get back as middle point the same one.
		Line2d li(9,9);   // diagonal line (0,0) - (9,9)
		auto ppts = li.getPoints( GivenCoord::X, 5, 1 );
		Segment_<NUMTYPE> s1( ppts.first, ppts.second );
		CHECK( s1.getMiddlePoint() == Point2d_<NUMTYPE>(5,5) );
	}
}

TEST_CASE( "FRect", "[frect]" )
{
	{
		FRect_<NUMTYPE> r1;
		CHECK( r1.width()  == 1. );
		CHECK( r1.height() == 1. );
		CHECK( r1.length() == 4 );
		CHECK( r1.area()   == 1 );
		CHECK( r1.center() == Point2d(0.5,0.5) );

		auto p_pts = r1.getPts();
		CHECK( p_pts.first  == Point2d() );
		CHECK( p_pts.second == Point2d(1.,1.) );

		auto pts = r1.get4Pts();
		CHECK( pts[0] == Point2d(0,0) );
		CHECK( pts[1] == Point2d(0,1) );
		CHECK( pts[2] == Point2d(1,1) );
		CHECK( pts[3] == Point2d(1,0) );
	}
	CHECK_THROWS( FRect_<NUMTYPE>( Point2d(4,5), Point2d(4,2) ) );
	{
		FRect_<NUMTYPE> r2a( Point2d(6,5), Point2d(1,2) );
		FRect_<NUMTYPE> r2b( Point2d(6,2), Point2d(1,5) );
		CHECK( r2a == r2b );

		CHECK( r2a.width() == 5. );
		CHECK( r2a.height() == 3. );
		auto p_pts = r2b.getPts();
		CHECK( p_pts.first  == Point2d(1,2) );
		CHECK( p_pts.second == Point2d(6,5) );

		auto pts = r2b.get4Pts();
		CHECK( pts[0] == Point2d(1,2) );
		CHECK( pts[1] == Point2d(1,5) );
		CHECK( pts[2] == Point2d(6,5) );
		CHECK( pts[3] == Point2d(6,2) );
	}
	{
		FRect_<NUMTYPE> r;
		auto s = r.getSegs();
		auto s2 = getSegs( r );
		CHECK( s == s2 );
		CHECK( s[0] == Segment(0,0, 0,1) );
		CHECK( s[1] == Segment(0,1, 1,1) );
		CHECK( s[2] == Segment(1,1, 1,0) );
		CHECK( s[3] == Segment(1,0, 0,0) );
	}
	{
		FRect_<NUMTYPE> r( Point2d(0,0), 100, 50 );
		CHECK( r.width()  == 100 );
		CHECK( r.height() == 50 );
		CHECK( r.length() == 300 );
		CHECK( r.area() == 5000 );
		CHECK( r.center() == Point2d(0,0) );
	}
}

TEST_CASE( "Polyline", "[polyline]" )
{
	{
		Polyline_<NUMTYPE> pl1( 3,4 );
		CHECK( pl1.isClosed() == false );
		CHECK( pl1.isPolygon() == false );
		CHECK( pl1.size() == 1 );
		CHECK( pl1.nbSegs() == 0 );
		CHECK( pl1.length() == 0 );
		CHECK( pl1.area() == 0 );
	}
	{
		Polyline_<NUMTYPE> pl1( 3,4, IsClosed::Yes );
		CHECK( pl1.isClosed() == true );
		CHECK( pl1.isPolygon() == false );
		CHECK( pl1.length() == 0 );
		CHECK( pl1.area() == 0 );
	}
	{
		Polyline_<NUMTYPE> pl1( Point2d(3,4), IsClosed::Yes );
		CHECK( pl1.isClosed() == true );
		CHECK( pl1.isPolygon() == false );
		CHECK( pl1.size() == 1 );
		CHECK( pl1.nbSegs() == 0 );
		CHECK( pl1.length() == 0 );
		CHECK( pl1.area() == 0 );
	}
	{
		FRect r( 5,6, 7,8 );
		Polyline_<NUMTYPE> pl1( r );
		CHECK( pl1.isClosed() == true );
		CHECK( pl1.isPolygon() == true );
		CHECK( pl1.size() == 4 );
		CHECK( pl1.nbSegs() == 4 );
		CHECK( pl1.length() == 8 );
		CHECK( pl1.area() == 4 );
		pl1.translate(1,2);
	}
	{                         // build Polyline from FRect
		FRect r( 5,6, 7,8 );
		Polyline_<NUMTYPE> pl1( r, IsClosed::No );
		CHECK( pl1.isClosed() == false );
		CHECK( pl1.isPolygon() == false );
		CHECK( pl1.size() == 4 );
		CHECK( pl1.nbSegs() == 3 );
		CHECK( pl1.length() == 6 );
		CHECK( pl1.area() == 0 );
	}
	Polyline_<NUMTYPE> pl1;
	pl1.add(
		std::vector<Point2d>{ {0,0}, {1,1.5}, {3,5}, {1,4} }
	);
	CHECK( pl1.size()   == 4 );
	CHECK( pl1.nbSegs() == 3 );
	CHECK( pl1.isPolygon() == false );
	CHECK( isPolygon(pl1)  == false );
	pl1.isClosed() = true;
	CHECK( pl1.nbSegs()    == 4 );
	CHECK( pl1.isPolygon() == true );
	CHECK( isPolygon(pl1)  == true );
	FRect bb1( 0,0, 3,5);
	CHECK( getBB(pl1)  == bb1 );
	CHECK( pl1.getBB() == bb1 );

	pl1.set(
		std::vector<Point2d>{ {0,0}, {0,1}, {1,1}, {1,0} }
	);
	CHECK( pl1.size()   == 4 );
	CHECK( pl1.nbSegs() == 4 );
	CHECK( pl1.length() == 4.);
	CHECK( length(pl1)  == 4.);
	pl1.isClosed() = false;
	CHECK( pl1.size()   == 4 );
	CHECK( pl1.nbSegs() == 3 );
	CHECK( pl1.length() == 3.);
	CHECK( length(pl1)  == 3.);

	FRect bb2( 0,0, 1,1);
	CHECK( getBB(pl1)  == bb2 );
	CHECK( pl1.getBB() == bb2 );
	pl1.translate(2,1.);
	CHECK( pl1.getPoint(0) == Point2d(2,1.) ); // (0,0) translated to (2,1)
}

TEST_CASE( "Polygon area", "[polygon_area]" )
{
	Polyline_<NUMTYPE> pl1(IsClosed::Yes);
	{
		pl1.add( std::vector<Point2d>{ {0,0}, {2,0}, {2,1}, {0,1} } );
		CHECK( pl1.size()   == 4 );
		CHECK( pl1.nbSegs() == 4 );
		CHECK( pl1.isPolygon() == true );
		CHECK( pl1.area() == 2. );
	}
	{
		pl1.set( std::vector<Point2d>{ {0,0}, {2,0}, {2,2}, {1,2}, {1,1}, {0,1} } );
		CHECK( pl1.size()   == 6 );
		CHECK( pl1.nbSegs() == 6 );
		CHECK( pl1.isPolygon() == true );
		CHECK( pl1.area() == 3. );
	}
}

TEST_CASE( "Polyline comparison", "[polyline_comp]" )
{
	Polyline_<NUMTYPE> pl1(IsClosed::No);
	pl1.add( 3,4 );
	pl1.add( 5,6 );
	pl1.add( 7,8 );

	Polyline_<NUMTYPE> pl2(IsClosed::No);
	pl2.add( 7,8 );
	pl2.add( 3,4 );
	pl2.add( 5,6 );

	{
		Polyline_<NUMTYPE> p1 = pl1;
		Polyline_<NUMTYPE> p2 = pl2;

		CHECK( p1.isNormalized() == false );
		CHECK( p2.isNormalized() == false );
		CHECK( (p1==p2) == false );
	}
	{
		Polyline_<NUMTYPE> p1 = pl1;
		Polyline_<NUMTYPE> p2 = pl2;
		p1.isClosed() = true;
		CHECK( p1.isNormalized() == false );
		CHECK( p2.isNormalized() == false );
		p2.isClosed() = true;
		CHECK( (p1==p2) == true );
		CHECK( p1.isNormalized() == true );
		CHECK( p2.isNormalized() == true );
	}
}

TEST_CASE( "general binding", "[gen_bind]" )
{

	struct MyType
	{
		float xxx,y;
	};
	MyType mtpt{3,4};
	Point2d pt( mtpt );
	CHECK( pt.getX() == 3 );
	Line2d li (mtpt); // ???
}

//////////////////////////////////////////////////////////////
/////           OPENCV BINDING TESTS                     /////
//////////////////////////////////////////////////////////////

#ifdef HOMOG2D_USE_OPENCV
TEST_CASE( "Opencv build H", "[test_opencv2]" )
{
	std::vector<Point2d_<NUMTYPE>> v1(4);
	std::vector<Point2d_<NUMTYPE>> v2(4);
	Homogr_<NUMTYPE> H;
	H.buildFrom4Points( v1, v2 );
	buildFrom4Points( v1, v2 );

}

TEST_CASE( "Opencv binding", "[test_opencv]" )
{
	cv::Mat mat_64 = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat mat_32 = cv::Mat::eye(3, 3, CV_32F);
	INFO( "assignment operator()" )
	{
		cv::Mat cvmat = cv::Mat::ones(3,3, CV_32F );
		Homogr H;
		H = cvmat;
		CHECK( H.value(0,0) == 1.);
		CHECK( H.value(1,1) == 1.);
		CHECK( H.value(1,0) == 1.);
		CHECK( H.value(0,1) == 1.);

		cv::Mat cvmat2 = cv::Mat::ones(3,3, CV_32F );
		Homogr H2 = cvmat;
		CHECK( H2.value(0,0) == 1.);
		CHECK( H2.value(1,1) == 1.);
		CHECK( H2.value(1,0) == 1.);
		CHECK( H2.value(0,1) == 1.);

		H = mat_64;
		CHECK( H.value(0,0) == 1.);
		CHECK( H.value(1,1) == 1.);
		CHECK( H.value(1,0) == 0.);
		CHECK( H.value(0,1) == 0.);

		H = mat_32;
		CHECK( H.value(0,0) == 1.);
		CHECK( H.value(1,1) == 1.);
		CHECK( H.value(1,0) == 0.);
		CHECK( H.value(0,1) == 0.);
	}
	INFO( "default copyTo()" )
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
	INFO( "copyTo() with CV_64F" )
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
	INFO( "copyTo() with CV_32F" )
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
	INFO( "Copy to OpenCv points" )
	{
		Point2d_<NUMTYPE> pt(1.,2.);
		{                              // free function
			cv::Point2d cvpt1 = getCvPtd( pt );           // double
			CHECK( (cvpt1.x == 1. && cvpt1.y == 2.) );
			cv::Point2f cvpt2 = getCvPtf( pt );           // float
			CHECK( (cvpt2.x == 1. && cvpt2.y == 2.) );
			cv::Point2i cvpt3 = getCvPti( pt );           // integer point
			CHECK( (cvpt3.x == 1 && cvpt3.y == 2) );

			auto cvpt_1 = getCvPt<cv::Point2d>( pt );
			auto cvpt_2 = getCvPt<cv::Point2f>( pt );
			auto cvpt_3 = getCvPt<cv::Point2i>( pt );
		}
		{
			cv::Point2d cvpt1 = pt.getCvPtd() ;
			CHECK( (cvpt1.x == 1. && cvpt1.y == 2.) );
			cv::Point2f cvpt2 = pt.getCvPtf() ;
			CHECK( (cvpt2.x == 1. && cvpt2.y == 2.) );
			cv::Point2i cvpt3 = pt.getCvPti() ;          // integer point
			CHECK( (cvpt3.x == 1  && cvpt3.y == 2 ) );

			auto cvpt_1 = pt.getCvPt<cv::Point2d>();
			auto cvpt_2 = pt.getCvPt<cv::Point2f>();
			auto cvpt_3 = pt.getCvPt<cv::Point2i>();
		}
		{ // input: vector of "double" points
		  // converted into "float" Opencv points
			std::vector<Point2d> v{ Point2d(1,2), Point2d(5,6), Point2d(3,4) };
			auto vcv1 = getCvPts<cv::Point2d>( v );
			CHECK( vcv1.size() == 3 );
			auto vcv2 = getCvPts<cv::Point2f>( v );
			CHECK( vcv2.size() == 3 );
			auto vcv3 = getCvPts<cv::Point2i>( v );
			CHECK( vcv3.size() == 3 );
		}
	}
	INFO( "Fetch from OpenCv points" )
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
	INFO( "Build line using OpenCv points" )
	{
		Line2d_<NUMTYPE> lia( cv::Point2d(100,200) );
//		Line2d_<NUMTYPE> lib( Point2d(100,200) );
//		CHECK( lia == lib );
	}
}
#endif

