/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019-2023 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr

    Licence: MPL v2

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at https://mozilla.org/MPL/2.0/.

**************************************************************************/

/**
\file homog2d_test.cpp
\brief A test file for homog2d, needs Catch2, v2 (single header file version),
see https://github.com/catchorg/Catch2
<br>
Run with <code>$ make test</code>

This file holds mostly "general" tests.

It also holds some tests that are only related to the OpenCv binding
Thus, they are run only if the symbol \c HOMOG2D_USE_OPENCV is defined.<br>
This latter part starts around line 3350.
<br>
Run with <code>$ make test USE_OPENCV=Y</code>

It also holds some tests that are only related to the SVG import feature, that requires
the \c tinyxml2 library, and that the symbol HOMOG2D_USE_SVG_IMPORT is defined.
<br>
Run with <code>$ make test USE_TINYXML2=Y</code>
*/

/// see test [gen_bind]
#define HOMOG2D_BIND_X xxx

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_RUNNER   // alternative: main provided here
#include "catch.hpp"

/// Numerical type for object storage for tests. This is usually defined by makefile.
/// (The internal numerical type for the library is defined by HOMOG2D_INUMTYPE)
#ifndef NUMTYPE
	#define NUMTYPE double
#endif

// define this if there is a need for bugtracking
//#define HOMOG2D_DEBUGMODE

#define HOMOG2D_TEST_MODE
#include "../homog2d.hpp"

#if 0
	#define LOCALLOG(a) std::cout << " - line " << __LINE__ << ": " << a << '\n'
#else
	#define LOCALLOG(a)
#endif

double g_epsilon = std::numeric_limits<NUMTYPE>::epsilon()*10000.;

using namespace h2d;

#define XSTR(s) STR(s)
#define STR(s) #s

int main( int argc, char* argv[] )
{
	std::cout << "START TESTS:"
		<< "\n - homog2d version: " << HOMOG2D_VERSION
		<< "\n - numerical type: " << XSTR(NUMTYPE)
		<< "\n - internal numerical type=" << XSTR(HOMOG2D_INUMTYPE)
		<< "\n - Catch lib version: " << CATCH_VERSION_MAJOR << '.' << CATCH_VERSION_MINOR << '.' << CATCH_VERSION_PATCH
		<< "\n - build options:"

		<< "\n  - HOMOG2D_OPTIMIZE_SPEED: "
#ifdef HOMOG2D_OPTIMIZE_SPEED
		<< "YES"
#else
		<< "NO"
#endif

		<< "\n  - HOMOG2D_USE_OPENCV: "
#ifdef HOMOG2D_USE_OPENCV
		<< "YES"
#else
		<< "NO"
#endif

		<< "\n  - HOMOG2D_USE_SVG_IMPORT: "
#ifdef HOMOG2D_USE_SVG_IMPORT
		<< "YES"
#else
		<< "NO"
#endif

		<< "\n  - HOMOG2D_ENABLE_RTP: "
#ifdef HOMOG2D_ENABLE_RTP
		<< "YES"
#else
		<< "NO"
#endif

		<< '\n';

	Catch::StringMaker<float>::precision = 25;
	Catch::StringMaker<double>::precision = 25;

// removed until PR merged into next release, see https://github.com/catchorg/Catch2/pull/2245
//	Catch::StringMaker<long double>::precision = 25;

// we need to have a bigger threshold value for "float" type, as it has a lot less precision
	if( std::string( XSTR(NUMTYPE) ) == "float" )
		thr::nullDistance() = 1E-6;

	thr::printThresholds( std::cout );

	return Catch::Session().run( argc, argv );
}

TEST_CASE( "numerical types access", "[types-access]" )
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

	FRectF rF;
	FRectD rD;
	FRectL rL;

	SegmentF sF;
	SegmentD sD;
	SegmentL sL;

	CircleF cF;
	CircleD cD;
	CircleL cL;

	OPolylineF poF;
	OPolylineD poD;
	OPolylineL poL;

	CPolylineF pcF;
	CPolylineD pcD;
	CPolylineL pcL;

	EllipseF eF;
	EllipseD eD;
	EllipseL eL;

	CHECK( ptF.dtype() == dtype(ptF) );

	CHECK( ptF.dtype() == Dtype::Float );
	CHECK( liF.dtype() == Dtype::Float );
	CHECK( HF.dtype()  == Dtype::Float );
	CHECK( rF.dtype()  == Dtype::Float );
	CHECK( sF.dtype()  == Dtype::Float );
	CHECK( cF.dtype()  == Dtype::Float );
	CHECK( poF.dtype() == Dtype::Float );
	CHECK( pcF.dtype() == Dtype::Float );
	CHECK( eF.dtype()  == Dtype::Float );

	CHECK( ptD.dtype() == Dtype::Double );
	CHECK( liD.dtype() == Dtype::Double );
	CHECK( HD.dtype()  == Dtype::Double );
	CHECK( rD.dtype()  == Dtype::Double );
	CHECK( sD.dtype()  == Dtype::Double );
	CHECK( cD.dtype()  == Dtype::Double );
	CHECK( poD.dtype() == Dtype::Double );
	CHECK( pcD.dtype() == Dtype::Double );
	CHECK( eD.dtype()  == Dtype::Double );

	CHECK( dtype(ptD) == Dtype::Double );
	CHECK( dtype(liD) == Dtype::Double );
	CHECK( dtype(HF)  == Dtype::Float );
	CHECK( dtype(rF)  == Dtype::Float );

	CHECK( dsize(ptF).first  == ptF.dsize().first );
	CHECK( dsize(ptD).first  == ptD.dsize().first );
	CHECK( dsize(ptL).first  == ptL.dsize().first );

	CHECK( dsize(ptF).second  == ptF.dsize().second );
	CHECK( dsize(ptD).second  == ptD.dsize().second );
	CHECK( dsize(ptL).second  == ptL.dsize().second );

	CHECK( dsize(ptF).first  == 24 );
	CHECK( dsize(ptF).second == 7  );

	CHECK( dsize(ptD).first  == 53 );
	CHECK( dsize(ptD).second == 10 );

	CHECK( dsize(ptL).first  == 64 );
	CHECK( dsize(ptL).second == 63 );
}

TEST_CASE( "comparison testing", "[comparison-test]" )
{
	{
		Segment_<NUMTYPE>
		a1, a2; CHECK( a1 == a2 );
	}
	{
		Circle_<NUMTYPE>
		a1, a2; CHECK( a1 == a2 );
	}
	{
		FRect_<NUMTYPE>
		a1, a2; CHECK( a1 == a2 );
	}
	{
		Line2d_<NUMTYPE>
		a1, a2; CHECK( a1 == a2 );
	}
	{
		Point2d_<NUMTYPE>
		a1, a2; CHECK( a1 == a2 );
	}
	{
		Ellipse_<NUMTYPE>
		a1, a2; CHECK( a1 == a2 );
	}
	{
		CPolyline_<NUMTYPE>
		a1, a2; CHECK( a1 == a2 );
	}
	{
		OPolyline_<NUMTYPE>
		a1, a2; CHECK( a1 == a2 );
	}
}


TEST_CASE( "types testing 1", "[test-types-1]" )
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

		CHECK( ptF.type() == Type::Point2d );
		CHECK( liF.type() == Type::Line2d );
	}

	Point2dD ptD0(1,1);
	Point2dF ptF0(2,2);
	Point2dL ptL0(3,3);

	Line2dD li_D0(1,1);
	Line2dF li_F0(2,2);
	Line2dL li_L0(3,3);
}

TEST_CASE( "types testing 2", "[test-types-2]" )
{
	Point2dD ptD0(1,1);
	Point2dF ptF0(2,2);
	Point2dL ptL0(3,3);

	INFO( "numerical type conversions (assignment)" )
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

		OPolylineF pof;
		OPolylineD pod;
		OPolylineL pol;

 		pol = pod;
		pof = pod;
		pod = pof;
		pol = pof;
		pof = pol;
		pod = pol;
	}
}

TEST_CASE( "normalization", "[normaliz]" )
{
	Point2d_<NUMTYPE> pt1(1,2,3);
	CHECK( pt1.get() == std::array<NUMTYPE,3>{1,2,3} );
	Point2d_<NUMTYPE> pt2(-1,2,3);
	CHECK( pt2.get() == std::array<NUMTYPE,3>{1,-2,-3} );
}

TEST_CASE( "homogeneous coordinates testing", "[homogeneous]" )
{
	{
		Point2d_<NUMTYPE> pt1(2,2,1);
		Point2d_<NUMTYPE> pt2(4,4,2);
		CHECK( pt1 == pt2 );
		pt2.set(8,8,4);
		CHECK( pt1 == pt2 );

		Line2d_<NUMTYPE> li1(0,1,0);
		Line2d_<NUMTYPE> li2(0,2,0);
		CHECK( li1 == li2 );
		li2.set(0,4,0);
		CHECK( li1 == li2 );

		Line2d_<NUMTYPE> li3(3,4,5);
		Line2d_<NUMTYPE> li4(6,8,10);
		CHECK( li3 == li4 );
	}
	{
		INFO( "constructors using a container of values as single argument" );

		std::array<float,3> arr{3,2,1};
		Point2d_<NUMTYPE> pt1(arr);
		CHECK( pt1 == Point2d_<NUMTYPE>(3,2,1) );

		std::vector<float> vec{3,2,1};
		Point2d_<NUMTYPE> pt2(vec);
		CHECK( pt2 == Point2d_<NUMTYPE>(3,2,1) );
	}
}

TEST_CASE( "infinity point", "[points-inf]" )
{
	Line2d liH( LineDir::H, Point2d() ); // horizontal line at y=0
	CHECK_THROWS( Point2d(0,0,0) );
	CHECK_THROWS( Line2d(0,0,0) );

	Point2d pt1(1,0,0);
	Point2d pt2(-1,0,0);
	CHECK( pt1.isInf() );
	CHECK( pt2.isInf() );

	auto li1 = pt1 * Point2d();
	CHECK( li1 == liH );
	auto li2 = pt2 * Point2d();
	CHECK( li2 == liH );

	Point2d pt3(3,3,0);  // infinite points at an angle of 45°
	auto li3 = pt3 * Point2d();
	CHECK( std::abs(li3.getAngle( liH ) - M_PI/4.) < g_epsilon );

	Segment seg( Point2d(), pt1 );
	auto ppts = seg.getPts();

	auto line = seg.getLine();
	CHECK( line == liH );
}

TEST_CASE( "types testing 3", "[test-types-3]" )
{
	Point2dD ptD0(1,1); // double
	Point2dF ptF0(2,2); // float
	Point2dL ptL0(3,3); // long

	INFO( "numerical type conversions (copy-constructor)" )
	{
		Point2dL ptL1 = ptD0; // double to long
		Point2dL ptL2 = ptF0; // float to long
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

		CircleL cl;
		CircleF cf;
		CircleD cd;

		CircleL cl2 = cd;
		CircleF cf2 = cd;
		CircleD cd2 = cd;

		CircleL cl3 = cf;
		CircleF cf3 = cf;
		CircleD cd3 = cf;

		CircleL cl4 = cl;
		CircleF cf4 = cl;
		CircleD cd4 = cl;

		SegmentL sl;
		SegmentF sf;
		SegmentD sd;

		SegmentL sl2 = sd;
		SegmentF sf2 = sd;
		SegmentD sd2 = sd;

		SegmentL sl3 = sf;
		SegmentF sf3 = sf;
		SegmentD sd3 = sf;

		SegmentL sl4 = sl;
		SegmentF sf4 = sl;
		SegmentD sd4 = sl;

		FRectL rl;
		FRectF rf;
		FRectD rd;

		FRectL rl2 = rd;
		FRectF rf2 = rd;
		FRectD rd2 = rd;

		FRectL rl3 = rf;
		FRectF rf3 = rf;
		FRectD rd3 = rf;

		FRectL rl4 = rl;
		FRectF rf4 = rl;
		FRectD rd4 = rl;

		OPolylineL pol;
		OPolylineF pof;
		OPolylineD pod;

		OPolylineL pol2 = pod;
		OPolylineF pof2 = pod;
		OPolylineD pod2 = pod;

		OPolylineL pol3 = pof;
		OPolylineF pof3 = pof;
		OPolylineD pod3 = pof;

		OPolylineL pol4 = pol;
		OPolylineF pof4 = pol;
		OPolylineD pod4 = pol;

		CPolylineL pcl;
		CPolylineF pcf;
		CPolylineD pcd;

		CPolylineL pcl2 = pcd;
		CPolylineF pcf2 = pcd;
		CPolylineD pcd2 = pcd;

		CPolylineL pcl3 = pcf;
		CPolylineF pcf3 = pcf;
		CPolylineD pcd3 = pcf;

		CPolylineL pcl4 = pcl;
		CPolylineF pcf4 = pcl;
		CPolylineD pcd4 = pcl;

		EllipseL elll;
		EllipseF ellf;
		EllipseD elld;

		EllipseL elll2 = elld;
		EllipseF ellf2 = elld;
		EllipseD elld2 = elld;

		EllipseL elll3 = ellf;
		EllipseF ellf3 = ellf;
		EllipseD elld3 = ellf;

		EllipseL elll4 = elll;
		EllipseF ellf4 = elll;
		EllipseD elld4 = elll;

/**
The goal of theses is to make sure that the conversion (float to double, ...)
does not trigger a build failure.
The checking is only there to avoid a warning about "unused variable".
\todo Once we switch to C++17, we can remove the checking and use:
https://en.cppreference.com/w/cpp/language/attributes/maybe_unused
*/
		Line2dD li_D0(1,1);
		Line2dF li_F0(2,2);
		Line2dL li_L0(3,3);

		Line2dD li_D1 = li_F0; CHECK( li_D1.get()[2] == 0. );
		Line2dD li_L1 = li_F0; CHECK( li_L1.get()[2] == 0. );

		Line2dD li_F2 = li_D0; CHECK( li_F2.get()[2] == 0. );
		Line2dD li_L2 = li_D0; CHECK( li_L2.get()[2] == 0. );

		Line2dD li_F3 = li_L0; CHECK( li_F3.get()[2] == 0. );
		Line2dD li_D3 = li_L0; CHECK( li_D3.get()[2] == 0. );
	}
}

TEST_CASE( "stream operator << test", "[streamingop-test]" )
{
	Line2d li;
	Point2d pt;
	Segment seg;
	Circle cir;
	Ellipse ell;
	CPolyline cpol;
	OPolyline opol;
// just to make sure that this builds !
	std::ostringstream oss;
	oss << li << pt << seg << cir << cpol << opol << ell;
	std::ostringstream oss2;
	oss2 << cpol;
	CHECK( oss2.str() == "CPolyline: empty" );
}

TEST_CASE( "line/point distance", "[lp-dist]" )
{
	size_t n = 1E6;  // nb of runs
	size_t c1{};
	size_t c2{};
	long double sum1{};
	long double sum2{};
	float k = 1;
	std::srand( time(0) );
	for( size_t i=0; i<n; i++ )
	{
		Point2d_<NUMTYPE> pt1(
			1.0*rand()/RAND_MAX * k,
			1.0*rand()/RAND_MAX * k
		);
		Point2d_<NUMTYPE> pt2(
			1.0*rand()/RAND_MAX * k,
			1.0*rand()/RAND_MAX * k
		);
		auto li = pt1 * pt2;
		auto d1 = li.distTo(pt1);
		auto d2 = li.distTo(pt2);
		if( d1 > thr::nullDistance() )
		{
//			std::cerr << ": distance check failure 1, value " << d1 << " > thres (" << thr::nullDistance()  << ")\n";
			sum1 += d1;
			c1++;
		}
		if( li.distTo(pt2) > thr::nullDistance() )
		{
//			std::cerr << "distance check failure 2, value " << d2 << " > thres (" << thr::nullDistance()  << ")\n";
			sum2 += d2;
			c2++;
		}
	}
	if( c1 != 0 || c2 != 0  )
	{
		std::cout << "line/point distance test failures with thres=" << thr::nullDistance()
			<< "\n nb1=" << c1 << ", nb2=" << c2  << " failures over total nb of tests=" << n
			<< "\n- mean distance: d1=" << sum1 / c1
			<< "\n- mean distance: d2=" << sum2 / c2
			<< '\n';
	}
	CHECK( c1 == 0 );
	CHECK( c2 == 0 );
}

TEST_CASE( "side-pt-line", "[side-pt-line]" )
{
	Line2d_<NUMTYPE> li; // vertical line at x=0
	{
		Point2d_<NUMTYPE> pt;
		CHECK( side(pt,li) == 0 );
	}
	{
		Point2d_<NUMTYPE> pt(1,0);
		CHECK( side(pt,li) == 1 );
	}
	{
		Point2d_<NUMTYPE> pt(-1,0);
		CHECK( side(pt,li) == -1 );
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
		CHECK( lH2.getAngle(lH3) == 0. );
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

TEST_CASE( "build point line from container", "[build-cont]" )
{
	std::array<double,3> arr{3,2,1};
	Point2d_<NUMTYPE> pt( arr );
	CHECK( pt == Point2d(3,2,1) );
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
		CHECK_THROWS(  Circle_<NUMTYPE> ( pt, thr::nullDistance()/1.1 ) );
		CHECK_NOTHROW( Circle_<NUMTYPE> ( pt, thr::nullDistance()*1.1 ) );
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
		thr::nullAngleValue() = 0.01;
		{
			Line2d_<NUMTYPE> l2a(Point2d_<NUMTYPE>(1.,0.), Point2d_<NUMTYPE>(1.0005,1.) ); // almost vertical line
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

		Line2d_<NUMTYPE> l4( LineDir::H, 1 ); 	 // horizontal line
		CHECK_THROWS( getParallelDistance( l1, l4 ) );
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
	auto r = std::abs( p1.distTo( p2 ) - priv::sqrt(2.) );
	CHECK( r < thr::nullDistance() );
}

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
		CHECK( std::fabs(angle2 - angle) < thr::nullAngleValue() );
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

		std::vector<Point2d> v_pt(3);  // 3 points at (0,0)
		H.applyTo( v_pt );             // translate them all to (+5,+6)
		CHECK( v_pt[2].getX() == 5 );
		CHECK( v_pt[2].getY() == 6 );
		CHECK( v_pt[0].getX() == 5 );
		CHECK( v_pt[0].getY() == 6 );
		auto v_pt2 = H * v_pt;
		CHECK( v_pt2.size() == 3 );
		CHECK( v_pt2[2].getX() == 10 );
		CHECK( v_pt2[2].getY() == 12 );
		CHECK( v_pt2[0].getX() == 10 );
		CHECK( v_pt2[0].getY() == 12 );

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
	Point2d pt1(10,10),pt2(10,10);
	Line2d li1, li2;
	FRect rect1, rect2;
	Circle cir1, cir2;
	Segment seg1, seg2;
	Ellipse ell1(5.,5.), ell2(3,4);
	CPolyline cpol1,cpol2;
	OPolyline opol1,opol2;

	CHECK( !pt2.isInside( pt1 ) );
	CHECK( !pt2.isInside( li1 ) );
	CHECK( !pt2.isInside( rect1 ) );
	CHECK( !pt2.isInside( cir1 ) );
	CHECK( !pt2.isInside( ell1  ) );
	CHECK( !pt2.isInside( cpol1 ) );
	CHECK( !pt2.isInside( opol1 ) );

	CHECK( !li2.isInside( pt1 ) );
	CHECK( !li2.isInside( li1 ) );
	CHECK( !li2.isInside( rect1 ) );
	CHECK( !li2.isInside( cir1 ) );
	CHECK( !li2.isInside( ell1  ) );
	CHECK( !li2.isInside( cpol1 ) );
	CHECK( !li2.isInside( opol1 ) );

	CHECK( !rect2.isInside( pt1 ) );
	CHECK( !rect2.isInside( li1 ) );
	CHECK( !rect2.isInside( rect1 ) );
	CHECK( !rect2.isInside( cir1 ) );
	CHECK( !rect2.isInside( ell1  ) );
	CHECK( !rect2.isInside( cpol1 ) );
	CHECK( !rect2.isInside( opol1 ) );

	CHECK( !cir2.isInside( pt1 ) );
	CHECK( !cir2.isInside( li1 ) );
	CHECK( !cir2.isInside( rect1 ) );
	CHECK( !cir2.isInside( cir1 ) );
	CHECK( !cir2.isInside( ell1  ) );
	CHECK( !cir2.isInside( cpol1 ) );
	CHECK( !cir2.isInside( opol1 ) );

	CHECK( !seg2.isInside( pt1 ) );
	CHECK( !seg2.isInside( li1 ) );
	CHECK( !seg2.isInside( rect1 ) );
	CHECK( !seg2.isInside( cir1 ) );
	CHECK( !seg2.isInside( ell1  ) );
	CHECK( !seg2.isInside( cpol1 ) );
	CHECK( !seg2.isInside( opol1 ) );

	CHECK( !ell2.isInside( pt1 ) );
	CHECK( !ell2.isInside( li1 ) );
	CHECK( !ell2.isInside( rect1 ) );
	CHECK( !ell2.isInside( cir1 ) );
	CHECK( !ell2.isInside( ell1  ) );
	CHECK( !ell2.isInside( cpol1 ) );
	CHECK( !ell2.isInside( opol1 ) );

	CHECK( !cpol2.isInside( pt1 ) );
	CHECK( !cpol2.isInside( li1 ) );
	CHECK( !cpol2.isInside( rect1 ) );
	CHECK( !cpol2.isInside( cir1 ) );
	CHECK( !cpol2.isInside( ell1  ) );
	CHECK( !cpol2.isInside( cpol1 ) );
	CHECK( !cpol2.isInside( opol1 ) );

	CHECK( !opol2.isInside( pt1 ) );
	CHECK( !opol2.isInside( li1 ) );
	CHECK( !opol2.isInside( rect1 ) );
	CHECK( !opol2.isInside( cir1 ) );
	CHECK( !opol2.isInside( ell1  ) );
	CHECK( !opol2.isInside( cpol1 ) );
	CHECK( !opol2.isInside( opol1 ) );
}

TEST_CASE( "Polyline IsInside Polyline", "[tpolipol]" )
{
	{ // checking https://github.com/skramm/homog2d/issues/10
		CPolyline_<HOMOG2D_INUMTYPE> p1(std::vector<Point2d>{
			Point2d(-2, 2),
			Point2d(-2,-2),
			Point2d( 2,-2),
			Point2d( 2, 2),
		});
		CPolyline_<HOMOG2D_INUMTYPE> p2(std::vector<Point2d>{
			Point2d(-5, 5),
			Point2d(-5,-5),
			Point2d( 5,-5),
			Point2d( 5, 5)
		});
		CHECK( p2.isInside( p1 ) == false );
		CHECK( p1.isInside( p2 ) == true );
	}
	{ // one inside the other
		#include "figures_test/polyline_inside_a1.code"
		CHECK( pl2.isInside( pl ) == false );
		CHECK( pl.isInside( pl2 ) == true );
	}
	{ // simple intersection, one point outside
		#include "figures_test/polyline_inside_a2.code"
		CHECK( pl2.isInside( pl ) == false );
		CHECK( pl.isInside( pl2 ) == false );
	}
	{ // nothing common
		#include "figures_test/polyline_inside_a3.code"
		CHECK( pl2.isInside( pl ) == false );
		CHECK( pl.isInside( pl2 ) == false );
	}
	{ // all points inside, but some intersections
		#include "figures_test/polyline_inside_a4.code"
		CHECK( pl2.isInside( pl ) == false );
		CHECK( pl.isInside( pl2 ) == false );
	}

	{ // Open polyline inside a closed polyline
		#include "figures_test/polyline_inside_b1.code"
		CHECK( pl2.isInside( pl ) == true );
		CHECK( pl.isInside( pl2 ) == false );
	}
	{ // Open polyline inside a closed polyline, but one point outside
		#include "figures_test/polyline_inside_b2.code"
		CHECK( pl2.isInside( pl ) == false );
		CHECK( pl.isInside( pl2 ) == false );
	}
	{ // Open polyline with no common with closed polyline
		#include "figures_test/polyline_inside_b3.code"
		CHECK( pl2.isInside( pl ) == false );
		CHECK( pl.isInside( pl2 ) == false );
	}

	{ // two Open polylines
		OPolyline_<HOMOG2D_INUMTYPE> pl(std::vector<Point2d_<HOMOG2D_INUMTYPE>>{
			{1,2},{1,1},{2,2}
		});
		OPolyline_<HOMOG2D_INUMTYPE> pl2(std::vector<Point2d_<HOMOG2D_INUMTYPE>>{
			{3,3},{3,2},{4,2}
		});
		CHECK( pl2.isInside( pl ) == false );
		CHECK( pl.isInside( pl2 ) == false );
	}
	{ // two Open polylines
		OPolyline_<HOMOG2D_INUMTYPE> pl(std::vector<Point2d_<HOMOG2D_INUMTYPE>>{
			{1,1},{5,1},{5,5},{1,5}
		});
		OPolyline_<HOMOG2D_INUMTYPE> pl2(std::vector<Point2d_<HOMOG2D_INUMTYPE>>{
			{3,3},{3,2},{4,2}
		});
		CHECK( pl2.isInside( pl ) == false );
		CHECK( pl.isInside( pl2 ) == false );
	}
}

TEST_CASE( "Point2d IsInside Polyline", "[tptipol]" )
{
	Point2d_<HOMOG2D_INUMTYPE> pt;

	OPolyline_<HOMOG2D_INUMTYPE> opol{ std::vector<Point2d>{ {0,0}, {3,4}, {1,8} } };
	CHECK( !pt.isInside( opol ) );

	{
		CPolyline_<HOMOG2D_INUMTYPE> cpol{ FRect_<HOMOG2D_INUMTYPE>() };  // polygon (0,0)-(1,0)-(1,1)-(0,1)

		pt.set( 0, 0 );
		CHECK( !pt.isInside( cpol ) );  // equal to one of the points
		pt.set( .2, .3 );
		CHECK( pt.isInside( cpol ) );
		pt.set( 1,0 );
		CHECK( !pt.isInside( cpol ) );
		pt.set( .5, 0. );
		CHECK( !pt.isInside( cpol ) );
	}
	{
//		CPolyline_<HOMOG2D_INUMTYPE> cpol{ {1,0},{2,1},{1,2},{0,1} };
		CPolyline_<HOMOG2D_INUMTYPE> cpol{ std::vector<Point2d>{ {1,0},{2,1},{1,2},{0,1} } };

		pt.set( 1,1 );
		CHECK( pt.isInside( cpol ) );
	}
}

// Whatever the primitive, a line is NEVER inside anything
TEST_CASE( "Line2d IsInside something", "[tlir]" )
{
	Point2d_<NUMTYPE> pt1(2,10);
	Point2d_<NUMTYPE> pt2(10,2);
	auto li = pt1*pt2;
	CHECK( li.isInside( pt1, pt2 ) == false );

	Circle_<NUMTYPE> c( 4, 5, 2 );
	CHECK( li.isInside( c ) == false );

	Ellipse_<NUMTYPE> ell(pt1);
	CHECK( li.isInside( ell ) == false );
}

TEST_CASE( "Point2d IsInside Circle", "[tptic]" )
{
	Circle_<NUMTYPE> c1(10.);
	Circle_<NUMTYPE> c2(2.);
	{
		Point2d p1( 3,3 );          // point inside circle
		CHECK( p1.isInside(c1) );
		CHECK( !p1.isInside(c2) );
		CHECK( p1.isInside(  Point2d(0,0), 8 ) );
		CHECK( !p1.isInside( Point2d(0,0), 2 ) );
	}
	{
		Point2d p1( 10, 0 );          // point on the edge of circle
		CHECK( !p1.isInside(c1) );
		Point2d p2( 0, 10 );          // point on the edge of circle
		CHECK( !p2.isInside(c1) );
	}
}

TEST_CASE( "Point2d IsInside FRect", "[tptir]" )
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
}

TEST_CASE( "Segment IsInside FRect", "[tsir]" )
{
	FRect_<NUMTYPE> r(2,3, 10,10);
	CHECK( r.length() == 30 );
	CHECK( r.area()   == 56 );
	CHECK( r.width()  == 8 );
	CHECK( r.height() == 7 );

	CHECK( Segment_<NUMTYPE>( 0,0, 5,0 ).isInside( r )           == false ); // outside
	CHECK( Segment_<NUMTYPE>( 2,5, 4,5 ).isInside( r )           == false ); // on the contour
	CHECK( Segment_<NUMTYPE>( 2.00001, 5., 4.,5. ).isInside( r ) == true );
	CHECK( Segment_<NUMTYPE>( 3,5, 4,5 ).isInside( r )           == true );
}

TEST_CASE( "Circle IsInside FRect", "[tcir]" )
{
	FRect_<NUMTYPE> r(2,3, 10,10);
	CHECK( Circle_<NUMTYPE>( 5,5, 2 ).isInside( r ) == false );   // touches rectangle at (5,3)
	CHECK( Circle_<NUMTYPE>( 5,5, 1 ).isInside( r ) == true );
	CHECK( Circle_<NUMTYPE>( 6,6, 2 ).isInside( r ) == true );
	CHECK( Circle_<NUMTYPE>().isInside( r )         == false );

	CHECK( Circle( 6,6, 22 ).isInside( r ) == false);
	CHECK( r.isInside( Circle( 6,6, 22 ) ) == true );

	std::vector<Point2d> vecpt{ {3,3},{4,4} };
	CPolyline pl(vecpt);
	CHECK( pl.isInside( r ) == false );  // on contour
}

// TODO !!!
#if 0
TEST_CASE( "Ellipse IsInside FRect", "[tellir]" )
{
	FRect_<NUMTYPE> r(2,3, 10,10);
	{
		Ellipse_<NUMTYPE> ell;
		CHECK( ell.isInside( r ) == false );
	}
	{
		Ellipse_<NUMTYPE> ell( Point2d(5,5), 2, 1 );
		CHECK( ell.isInside( r ) == true );
	}
	{
		Ellipse_<NUMTYPE> ell( Point2d(5,5), 6, 2 );
		CHECK( ell.isInside( r ) == false );
	}
}
#endif

TEST_CASE( "Circle IsInside Circle", "[tcic]" )
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
		Circle_<NUMTYPE> cA( Point2d(),   10.);
		CHECK( cA.radius() == 10. );
		cA.radius() = 12;
		CHECK( cA.radius() == 12. );
		CHECK( cA.center() == Point2d(0,0) );

		Circle_<NUMTYPE> cB( Point2d(5,0), 2.);
		auto seg = getSegment( cA, cB );
		CHECK( seg.getPts().first  == Point2d(0,0) );
		CHECK( seg.getPts().second == Point2d(5,0) );
		CHECK( seg.length() == 5 );
	}
}


//////////////////////////////////////////////////////////////
/////           INTERSECTION TESTS                       /////
//////////////////////////////////////////////////////////////

// This test is there just to make sure that every combination does build
TEST_CASE( "Intersection", "[int_all]" )
{
	FRect    r1,r2;
	Circle   c1,c2;
	Segment  s1,s2;
	Line2d   l1,l2;
	OPolyline po1,po2;
	CPolyline pc1,pc2;

	CHECK( !r1.intersects( r2 )() ); // intersection with object of same type
	CHECK( !c1.intersects( c2 )() );
	CHECK( !s1.intersects( s2 )() );
	CHECK( !l1.intersects( l2 )() );
	CHECK( !po1.intersects( po2 )() );
	CHECK( !pc1.intersects( pc2 )() );

	CHECK( r1.intersects( c2 )() );
	CHECK( r1.intersects( s2 )() );
	CHECK( r1.intersects( l2 )() );
	CHECK( !r1.intersects( po2 )() );  // polyline is empty, so no intersection
	CHECK( !r1.intersects( pc2 )() );  // polyline is empty, so no intersection

	CHECK( c1.intersects( r2 )() );
	CHECK( c1.intersects( s2 )() );
	CHECK( c1.intersects( l2 )() );
	CHECK( !c1.intersects( po2 )() );  // polyline is empty, so no intersection
	CHECK( !c1.intersects( pc2 )() );  // polyline is empty, so no intersection

	CHECK( s1.intersects( r2 )() );
	CHECK( s1.intersects( c2 )() );
	CHECK( s1.intersects( l2 )() );
	CHECK( !s1.intersects( po2 )() );  // polyline is empty, so no intersection
	CHECK( !s1.intersects( pc2 )() );  // polyline is empty, so no intersection

	CHECK( l1.intersects( r2 )() );
	CHECK( l1.intersects( c2 )() );
	CHECK( l1.intersects( s2 )() );
	CHECK( !l1.intersects( po2 )() );  // polyline is empty, so no intersection
	CHECK( !l1.intersects( pc2 )() );  // polyline is empty, so no intersection

	CHECK( !po1.intersects( r2 )() );
	CHECK( !po1.intersects( c2 )() );
	CHECK( !po1.intersects( s2 )() );
	CHECK( !po1.intersects( l2 )() );
	CHECK( !po1.intersects( pc2 )() );

	CHECK( !pc1.intersects( r2 )() );
	CHECK( !pc1.intersects( c2 )() );
	CHECK( !pc1.intersects( s2 )() );
	CHECK( !pc1.intersects( l2 )() );
	CHECK( !pc1.intersects( po2 )() );
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
	{
		s1.set( 0., 0., 1., 0. );
		s2.set( 1., 1., 1., -1E-05 );
		{
			auto si = s1.intersects(s2);
			CHECK( si() );
			CHECK( si.size() == 1 );
			CHECK( si.get() == Point2d(1,0) );
		}
		s2.set( 1., 1., 1., +1E-05 );
		{
			CHECK( !s1.intersects(s2)() );
		}
	}
}

TEST_CASE( "Circle/Circle intersection", "[int_CC]" )
{
	{
		Circle_<NUMTYPE> cA, cB;
		CHECK( cA == cB );                 // identical circles do
		CHECK( !cA.intersects(cB)() );     // not intersect
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
		CHECK( cA.intersects(cB).size() == 2 );
	}
	{
		Circle_<NUMTYPE> cA( Point2d(0,0), 1 );
		Circle_<NUMTYPE> cB( Point2d(2,0), 1 );
		CHECK( cA != cB );
		CHECK( cA.intersects(cB)() );
		auto inter = cA.intersects(cB);
		CHECK( inter() == true );
		CHECK( inter.size() == 1 );
//		CHECK( inter.get().first  == Point2d(1,0) );
//		CHECK( inter.get().second == Point2d(1,0) );
		CHECK( inter.get()[0] == Point2d(1,0) );
	}
}

// This part uses externally defined rectangles, check the files
// figures_test/frect_intersect_*.code
// this is done so we can, for a single defined rectangle pair,
// have both the test code here, and a graphical representation of the situation.
// The corresponding svg images are built with `$ make test_fig` (no external dependency)
TEST_CASE( "FRect/FRect intersection", "[int_FF]" )
{
	{                                   // identical rectangles
		FRect_<NUMTYPE> r1, r2;
		CHECK( r1 == r2 );
		CHECK( !r1.intersects(r2)() ); // no intersection !
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 0 );

		auto i1 = intersectArea(r1,r2);
		CHECK( i1() == true );
		CHECK( i1.get() == r1 );

		auto u1 = unionArea(r1,r2);
		CHECK( u1.size() == 4 );      // 4 points
		CHECK( u1 == CPolyline(r1) );  // same
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

		auto u1 = unionArea(r1,r2);
		CHECK( u1.size() == 0 );     // empty
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

		auto u1 = unionArea(r1,r2);
//		Polyline p{ [0,0],[1,1] };
		CHECK( u1.size() == 8 );
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

		auto u1 = unionArea(r1,r2);
//		Polyline p{ [0,0],[1,1] };
		CHECK( u1.size() == 12 );
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
		CHECK( vpts[0] == Point2d(3,1) );
		CHECK( vpts[1] == Point2d(3,3) );
		CHECK( vpts[2] == Point2d(4,1) );
		CHECK( vpts[3] == Point2d(4,3) );

		auto rect_inter = r1.intersectArea(r2);
		CHECK( rect_inter() == true );
		CHECK( rect_inter.get() == FRect(3,1, 4,3) );

		auto u1 = unionArea(r1,r2);
//		Polyline p{ [0,0],[1,1] };
		CHECK( u1.size() == 4 );
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
		CHECK( r1.intersectArea(r2)() == false ); // no area intersection

		r2.translate( 0.000001, 0 );         // move it a bit left
		inters = r1.intersects(r2);          // => no more intersection
		CHECK( inters() == false );
		CHECK( inters.size() == 0 );
		CHECK( r1.intersectArea(r2)() == false ); // still no intersection

		auto u1 = unionArea(r1,r2);
		CHECK( u1.size() == 0 );
	}
	{     // two rectangles joined by corner at 3,2
#include "figures_test/frect_intersect_6.code"
		CHECK( r1.intersects(r2)() );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 1 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 3,2 ) );
		CHECK( r1.intersectArea(r2)() == false ); // only one point !

		auto u1 = unionArea(r1,r2);
		CHECK( u1.size() == 8 );
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
		auto inters2 = r1.intersectArea(r2);
		CHECK( inters2() == true );
		CHECK( inters2.get() == r2 );
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

// this below needs to be expanded
	{
#include "figures_test/frect_intersect_12.code"
		CHECK( r1.intersects(r2)() == true );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 3 );
		auto vpts = inters.get();
//		priv::printVector( vpts, "intersection points demo12" );
		CHECK( vpts[0] == Point2d( 2,0 ) );
		CHECK( vpts[1] == Point2d( 2,2 ) );
		CHECK( vpts[2] == Point2d( 3,0 ) );
		CHECK( r1.intersectArea(r2)() == true );
//		auto inter=r1.intersectArea(r2);
//		CHECK( inter.get() == r2 );
	}

// this below needs to be expanded
	{
#include "figures_test/frect_intersect_13.code"
		CHECK( r1.intersects(r2)() == true );
		auto inters = r1.intersects(r2);
		CHECK( inters.size() == 2 );
		auto vpts = inters.get();
		CHECK( vpts[0] == Point2d( 3,0 ) );
		CHECK( vpts[1] == Point2d( 3,2 ) );
		CHECK( r1.intersectArea(r2)() == false );
//		auto inter=r1.intersectArea(r2);
//		CHECK( inter.get() == r2 );
	}
}

TEST_CASE( "IoU of 2 rectangles", "[IOU_RECT]" )
{
	FRect_<NUMTYPE> r1( 0,0,1,1 );
	FRect_<NUMTYPE> r2( 2,2,3,3 );
	CHECK( IoU(r1,r2) == 0. );
	r1.set(0,0,2,2);
	CHECK( IoU(r1,r2) == 0. );

	r2.set(1,0,3,2);
	CHECK( IoU(r1,r2) == Approx(1./3.) );
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
		CHECK( ri.size() == 2 );
		auto pts = ri.get();
		CHECK( pts[0] == pt1 );
		CHECK( pts[1] == pt2 );

		pt1.set( 5,0 );
		pt2.set( 6,1 );
		ri = li.intersects( FRect(pt1, pt2) );
		CHECK( ri() == false );
	}
	INFO( "with H/V line" )
	{
		Point2d_<NUMTYPE> pt1, pt2(1,1);                //  rectangle (0,0) - (1,1)
		FRect_<NUMTYPE> r1(pt1, pt2);
		Line2d_<NUMTYPE> li = Point2d_<NUMTYPE>() * Point2d_<NUMTYPE>(0,1); // vertical line at y=x
		auto ri1 = li.intersects( r1 );
		auto ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );

		CHECK( ri1.size() == 2 );
		CHECK( ri2.size() == 2 );
		auto pts1 = ri1.get();
		auto pts2 = ri2.get();
		CHECK( pts1[0] == pt1 );
		CHECK( pts1[1] == Point2d(0,1) );

		CHECK( pts2[0] == pt1 );
		CHECK( pts2[1] == Point2d(0,1) );

		li = Point2d_<NUMTYPE>(1,0) * Point2d_<NUMTYPE>(1,1);     // vertical line at x=1
		ri1 = li.intersects( r1 );
		ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );
		CHECK( ri1.size() == 2 );
		CHECK( ri2.size() == 2 );

		pts1 = ri1.get();
		CHECK( pts1[0] == Point2d(1,0) );
		CHECK( pts1[1] == Point2d(1,1) );

		li = Point2d_<NUMTYPE>() * Point2d_<NUMTYPE>(1,0);        // horizontal line at y=0
		ri1 = li.intersects( r1 );
		ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );
		CHECK( ri1.size() == 2 );
		CHECK( ri2.size() == 2 );

		pts1 = ri1.get();
		CHECK( pts1[0] == pt1 );
		CHECK( pts1[1] == Point2d(1,0) );

		li = Point2d_<NUMTYPE>(-1,1) * Point2d_<NUMTYPE>(1,1);     // horizontal line at y=1
		ri1 = li.intersects( r1 );
		ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );
		CHECK( ri1.size() == 2 );
		CHECK( ri2.size() == 2 );

		pts1 = ri1.get();
		CHECK( pts1[0] == Point2d(0,1) );
		CHECK( pts1[1] == Point2d(1,1) );

		li = Point2d_<NUMTYPE>(-1.,.5) * Point2d_<NUMTYPE>(2.,.5);     // horizontal line at y=0.5
		ri1 = li.intersects( r1 );
		ri2 = r1.intersects( li );
		CHECK( ri1() == true );
		CHECK( ri2() == true );
		CHECK( ri1.size() == 2 );
		CHECK( ri2.size() == 2 );

		pts1 = ri1.get();
		CHECK( pts1[0] == Point2d(0.,.5) );
		CHECK( pts1[1] == Point2d(1.,.5) );
	}
	INFO( "single point intersection" )
	{
		FRect_<NUMTYPE> r1( 0,0,1,1 );
		Line2d_<NUMTYPE> li( -1,+1, +1,-1 );
		auto inter = r1.intersects( li );
		CHECK( inter.size() == 1 );
		auto pts = inter.get();
		CHECK( pts[0] == Point2d(0,0) );
	}
}

//////////////////////////////////////////////////////////////
/////               MISC. TESTS                          /////
//////////////////////////////////////////////////////////////


TEST_CASE( "Colinearity", "[colinearity]" )
{
	Point2d p00(0,0), p01(0,1), p30(3,0);
	Point2d pt1, pt2, pt3;

	{
		pt1 = p01; pt2 = p00; pt3 = p30;   // case A
		auto arr = priv::getLargestDistancePoints( pt1, pt2, pt3 );
		CHECK( arr[0]==pt3 );
		CHECK( arr[1]==pt1 );
		CHECK( arr[2]==pt2 );
	}
	{
		pt1 = p01; pt2 = p30; pt3 = p00;   // case B
		auto arr = priv::getLargestDistancePoints( pt1, pt2, pt3 );
		CHECK( arr[0]==pt2 );
		CHECK( arr[1]==pt1 );
		CHECK( arr[2]==pt3 );
	}
	{
		pt1 = p00; pt2 = p01; pt3 = p30;   // case C
		auto arr = priv::getLargestDistancePoints( pt1, pt2, pt3 );
		CHECK( arr[0]==pt3 );
		CHECK( arr[1]==pt2 );
		CHECK( arr[2]==pt1 );
	}
	{
		pt1 = p30; pt2 = p01; pt3 = p00;   // case D
		auto arr = priv::getLargestDistancePoints( pt1, pt2, pt3 );
		CHECK( arr[0]==pt1 );
		CHECK( arr[1]==pt2 );
		CHECK( arr[2]==pt3 );
	}
	{
		pt1 = p00; pt2 = p30; pt3 = p01;   // case E
		auto arr = priv::getLargestDistancePoints( pt1, pt2, pt3 );
		CHECK( arr[0]==pt2 );
		CHECK( arr[1]==pt3 );
		CHECK( arr[2]==pt1 );
	}
	{
		pt1 = p30; pt2 = p00; pt3 = p01;   // case F
		auto arr = priv::getLargestDistancePoints( pt1, pt2, pt3 );
		CHECK( arr[0]==pt1 );
		CHECK( arr[1]==pt3 );
		CHECK( arr[2]==pt2 );
	}
	{
		Point2d p1(0,0), p2(1,0), p3(4,0);
		CHECK( areCollinear( p1, p2, p3 ) );
	}
	{
		Point2d p1(1,0), p2(0,0), p3(4,0);
		CHECK( areCollinear( p1, p2, p3 ) );
	}
	{
		Point2d p1(1,0), p2(0,0), p3(4,1E-3);
		CHECK( !areCollinear( p1, p2, p3 ) );
	}
}

TEST_CASE( "Circle", "[cir1]" )
{
	{
		Circle_<NUMTYPE> c1;       // Default constructor
		CHECK( c1.center() == Point2d(0,0) );
		CHECK( center(c1)  == Point2d(0,0) );
		CHECK( c1.radius() == 1. );
		CHECK( radius(c1)  == 1. );
		CHECK( c1.getBB() == FRect(-1,-1,1,1 ) );
		CHECK( getBB(c1)  == FRect(-1,-1,1,1 ) );

		CHECK( c1.area()   == area(c1) );
		CHECK( c1.length() == length(c1) );
	}
	{
		int i = 44;
		Circle_<NUMTYPE> c1(i);  // 1-arg Constructor - radius
		CHECK( c1.center() == Point2d(0,0) );
		CHECK( c1.radius() == i );
		CHECK( c1.getBB() == FRect(-i,-i,i,i) );
		CHECK( getBB(c1)  == FRect(-i,-i,i,i) );
		c1.set( 454 );
		CHECK( c1.center() == Point2d(0,0) );
		CHECK( c1.radius() == 454 );
	}
	{
		Point2d_<NUMTYPE> pt( 4,5);
		Circle_<NUMTYPE> c2(pt);  // 1-arg Constructor - center
		CHECK( c2.center() == Point2d(4,5) );
		CHECK( c2.radius() == 1 );

		CHECK( center(c2) == Point2d(4,5) );  // free functions call
		CHECK( radius(c2) == 1 );

		c2.set( Point2d_<NUMTYPE>(18,22) );
		CHECK( c2.center() == Point2d(18,22) );
		CHECK( c2.radius() == 1 );
	}
	{
		Point2d_<NUMTYPE> pt1(0,0), pt2(4,0);
		Circle_<NUMTYPE> c1(pt1,pt2);  // 2-args constructor: 2 points
		CHECK( c1.center() == Point2d(2,0) );
		CHECK( c1.radius() == 2. );
		c1.set( Point2d(1,1), Point2d(1,5) );
		CHECK( c1.center() == Point2d(1,3) );
		CHECK( c1.radius() == 2. );
	}
	{
		Point2d_<NUMTYPE> pt1(4,5);
		Circle_<NUMTYPE> c2(pt1,3);  // 2-args constructor - center and radius
		CHECK( c2.center() == pt1 );
		CHECK( c2.radius() == 3 );

		c2.set( Point2d_<NUMTYPE>(42,24), 18 );
		CHECK( c2.center() == Point2d_<NUMTYPE>(42,24) );
		CHECK( c2.radius() == 18 );
	}
	{
		Circle_<NUMTYPE> c1(1,2,3);  // 3-args constructor: x0, y0, radius
		CHECK( c1.center() == Point2d(1,2) );
		CHECK( c1.radius() == 3 );

		c1.set( 11, 22, 33 );
		CHECK( c1.center() == Point2d(11,22) );
		CHECK( c1.radius() == 33 );
	}
	{
		CHECK_THROWS( Circle_<NUMTYPE>(1,2,0.) ); /// ?
		CHECK_THROWS( Circle_<NUMTYPE>(1,2,-1.) );
	}
	{
		Point2d_<NUMTYPE> pt1(4,5), pt2(6,7), pt3; // 3-args constructor: 3 points
		Circle_<NUMTYPE> c1(pt1,pt2,pt3);  // 2-args constructor: 2 points

		c1.set( Point2d(4,5), Point2d(6,7), Point2d(2,2) );

		CHECK_THROWS( c1.set( Point2d(4,5), Point2d(4,5) ) );               // points must be different !
		CHECK_THROWS( c1.set( Point2d(4,5), Point2d(4,5), Point2d(2,2) ) );
		CHECK_THROWS( Circle( Point2d(4,5), Point2d(4,5), Point2d(2,2) ) );
		CHECK_THROWS( Circle( Point2d(4,5), Point2d(4,5) ) );
	}
	{
		Circle_<NUMTYPE> c1;

		CHECK( c1.radius() == 1 );
		c1.radius() = 2;                // member function call
		CHECK( c1.radius() == 2 );
		radius(c1) = 3;                 // free function call
		CHECK( radius(c1) == 3 );

		CHECK( center(c1) == Point2d() );
		c1.center() = Point2d(3,4);            // member function call
		CHECK( center(c1) == Point2d(3,4) );
		center(c1) = Point2d(5,6);             // free function call
		CHECK( center(c1) == Point2d(5,6) );
	}
}

TEST_CASE( "Circle from points", "[cir2]" )
{
	{
		INFO( "circle from 2 pts");
		std::vector<Point2d_<NUMTYPE>> pts1{ {0,0}, {2,0} };
		std::vector<Point2d_<NUMTYPE>> pts2{ {0,3}, {4,3} };
// check using constructor
		Circle_<NUMTYPE> c1(pts1);
		CHECK( c1.center() == Point2d(1,0) );
		CHECK( c1.radius() == 1. );
		Circle_<NUMTYPE> c2(pts2);
		CHECK( c2.center() == Point2d(2,3) );
		CHECK( c2.radius() == 2. );
// check using "set" function
		c1.set(pts2);
		CHECK( c1.center() == Point2d(2,3) );
		CHECK( c1.radius() == 2. );
		c2.set(pts1);
		CHECK( c2.center() == Point2d(1,0) );
		CHECK( c2.radius() == 1. );
	}
	{
		INFO( "circle from 4 pts");
		std::vector<Point2d_<NUMTYPE>> pts1{ {0,4}, {4,0}, {0,-4}, {0,0} };
		Circle_<NUMTYPE> c1(pts1);
		CHECK( c1.center() == Point2d(0,0) );
		CHECK( c1.radius() == 4. );
		c1.set(pts1);
		CHECK( c1.center() == Point2d(0,0) );
		CHECK( c1.radius() == 4. );

		std::vector<Point2d_<NUMTYPE>> pts2{ {0,4} };
		CHECK_THROWS( Circle_<NUMTYPE>(pts2) ); // 1 point not allowed
		std::vector<Point2d_<NUMTYPE>> pts3;
		CHECK_THROWS( Circle_<NUMTYPE>(pts3) ); // 0 points not allowed
	}
}

TEST_CASE( "Ellipse", "[ell1]" )
{
	{
		Ellipse_<NUMTYPE> el;
		CHECK( el.getCenter() == Point2d(0,0) );
		CHECK( getCenter(el)  == Point2d(0,0) );
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
		CHECK( el.getCenter() == Point2d(1,2) );
		CHECK( getCenter(el)  == Point2d(1,2) );
		CHECK( el.getMajMin().first == 3.0 );
		CHECK( el.getMajMin().second == 3.0 );
		CHECK( el.angle() == 0.0 );
		CHECK( angle(el)  == 0.0 );
		CHECK( el.isCircle() == true );
		CHECK( isCircle(el) == true );
	}

	{
		Ellipse_<NUMTYPE> el(1,2,3.0,3.00001);
		CHECK( el.getCenter() == Point2d(1,2) );
		CHECK( el.isCircle() == false );       // using default threshold
		CHECK( isCircle(el) == false );

		CHECK( el.isCircle(1E-3) == true );    // using arbitrary threshold
		CHECK( isCircle(el,1E-3) == true );
	}

	{
		Ellipse_<NUMTYPE> el(4,5,6,7);
		CHECK( el.getCenter() == Point2d(4,5) );
		CHECK( el.getMajMin().first  == Approx(7.0) );
		CHECK( el.getMajMin().second == Approx(6.0) );
		CHECK( el.angle() == 0.0 );
		CHECK( el.isCircle() == false );
	}
	{
		Ellipse_<NUMTYPE> el(4,5, 6, 7, 1 /*rad.*/ );
		CHECK( el.getCenter() == Point2d(4,5) );
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

			s.translate( 2, 3 );
			CHECK( s.getPts().first == Point2d(45,11) );
			CHECK( s.getPts().second == Point2d(45,21) );
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
		Point2d_<NUMTYPE> p1( 4,5 );
		Point2d_<NUMTYPE> p2( 1,2 );
		auto ppts = std::make_pair( p1, p2 );
		Segment_<NUMTYPE> s( ppts );
		CHECK( s.getPts().first  == p2 );
		CHECK( s.getPts().second == p1 );
		s.set( ppts );
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
		CHECK( s1.area() == 0 );

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

		auto pt = s1.getCenter();
		CHECK( pt == Point2d_<NUMTYPE>(1,1) );
	}
	{
		Segment_<NUMTYPE> s1( Point2d(0,0), Point2d(2,2) );
		Segment_<NUMTYPE> s2( Point2d(2,0), Point2d(0,2) );
		auto bis = s1.getBisector();
		CHECK( bis == s2.getLine() );
	}
	{  // test that points on a line at equal distance from a point, when
		// transformed in a segment, we get back as middle point the same one.
		Line2d li(9,9);   // diagonal line (0,0) - (9,9)
		auto ppts = li.getPoints( GivenCoord::X, 5, 1 );
		Segment_<NUMTYPE> s1( ppts.first, ppts.second );
		CHECK( s1.getCenter() == Point2d_<NUMTYPE>(5,5) );
	}
}

TEST_CASE( "Segment-split", "[seg-split]" )
{
	Segment_<NUMTYPE> s1( Point2d(0,0), Point2d(2,0) );
	auto psegs1 = s1.split();
	auto psegs2 = split(s1);
	CHECK( psegs1 == psegs2 );

	CHECK( psegs1.first  == Segment( Point2d(0,0), Point2d(1,0) ) );
	CHECK( psegs1.second == Segment( Point2d(1,0), Point2d(2,0) ) );
}

TEST_CASE( "Segment 2", "[seg2]" )
{
	{
		Segment_<HOMOG2D_INUMTYPE> seg (0,0,1,0); // horizontal segment
		auto psegs  = seg.getParallelSegs(1.);
		auto psegs2 = seg.getParallelSegs(1.);
		CHECK( psegs == psegs2 );
		CHECK( psegs.first  == Segment(0,-1,1,-1) );
		CHECK( psegs.second == Segment(0,+1,1,+1) );
	}
	{
		Segment_<HOMOG2D_INUMTYPE> seg (0,0,0,1); // vertical segment
		auto psegs = seg.getParallelSegs(1.);
		CHECK( psegs.first  == Segment(-1,0,-1,+1) );
		CHECK( psegs.second == Segment(+1,0,+1,+1) );
	}
}

TEST_CASE( "Segment extended", "[seg_extended]" )
{
	Segment_<HOMOG2D_INUMTYPE> seg(0,0,1,0);
	auto s2 = seg.getExtended();
	CHECK( s2.length() == 3. );
	CHECK( s2.getPts().first == Point2d(-1,0) );
	CHECK( s2.getPts().second == Point2d(2,0) );

	auto s3 = getExtended(seg);
	CHECK( s3.length() == 3. );
	CHECK( s3.getPts().first == Point2d(-1,0) );
	CHECK( s3.getPts().second == Point2d(2,0) );
}

TEST_CASE( "Segment orthogonal", "[seg_orthog]" )
{
	Segment_<HOMOG2D_INUMTYPE> seg(0,0,1,0);
	auto pts  = seg.getOrthogPts();
	auto pts2 = getOrthogPts(seg);
	CHECK( pts2 == pts );
	auto pol = CPolyline(pts);
	CHECK( pol == CPolyline( std::vector<Point2d>{ {0,-1},{1,-1},{1,1},{0,1} } ) );

	auto osegs  = seg.getOrthogSegs();
	auto osegs2 = getOrthogSegs(seg);
	CHECK( osegs2 == osegs );
	std::array<Segment_<HOMOG2D_INUMTYPE>,4> gt;
	gt[0] = Segment( 0,0,0,+1 );
	gt[1] = Segment( 0,0,0,-1 );
	gt[2] = Segment( 1,0,1,+1 );
	gt[3] = Segment( 1,0,1,-1 );
	std::sort( gt.begin(), gt.end() );
	std::sort( osegs.begin(), osegs.end() );
	CHECK( gt == osegs );
}

TEST_CASE( "FRect pair bounding box", "[frect-BB]" )
{
	{                              // two identical rectangles
		FRect_<NUMTYPE> r1, r2;
		CHECK( getBB(r1,r2) == r1 );
	}
	{                                  // one bottom left, the other top right
		FRect_<NUMTYPE> r1(0,0, 1,1);
		FRect_<NUMTYPE> r2(2,2, 4,4);
		CHECK( getBB(r1,r2) == FRect_<NUMTYPE>(0,0,4,4) );
	}
	{                                 // one top left, the other bottom right
		FRect_<NUMTYPE> r1(0,2, 1,3);
		FRect_<NUMTYPE> r2(2,0, 4,1);
		CHECK( getBB(r1,r2) == FRect_<NUMTYPE>(0,0,4,3) );
	}
	{                                 // one inside the other
		FRect_<NUMTYPE> r1(0,0, 5,5);
		FRect_<NUMTYPE> r2(1,1, 3,3);
		CHECK( getBB(r1,r2) == FRect_<NUMTYPE>(0,0,5,5) );
	}
	{                                 // one inside the other, with a common edge
		FRect_<NUMTYPE> r1(0,0, 5,5);
		FRect_<NUMTYPE> r2(1,0, 3,3);
		CHECK( getBB(r1,r2) == FRect_<NUMTYPE>(0,0,5,5) );
	}

// test for rectangle of different types
	{                                 // one inside the other, with a common edge
		FRect_<float> r1(0,0, 5,5);
		FRect_<double> r2(1,0, 3,3);
		auto bb = getBB(r1,r2);
		CHECK( bb == FRect_<long double>(0,0,5,5) );
		CHECK( bb.dtype() == Dtype::Float );
	}
}

TEST_CASE( "bounding box of two objects", "[getBB-pair]" )
{
	FRect_<NUMTYPE> r1(0,3, 2,0);
	FRect_<NUMTYPE> r2(4,5, 6,8);
	FRect_<NUMTYPE> bbr(0,0, 6,8);
	CHECK( bbr == getBB( r1,r2 ) );
/*	{                                    // distant segments
		Segment_<NUMTYPE> s1(0,3, 2,0);
		Segment_<NUMTYPE> s2(4,5, 6,8);
		FRect_<NUMTYPE> bbs(0,0, 6,8);
		CHECK( bbs == getBB( s1,s2 ) );
	}
	{                                    // crossing segments
		Segment_<NUMTYPE> s1(0,0, 5,5);
		Segment_<NUMTYPE> s2(0,6, 4,0);
		FRect_<NUMTYPE> bbs(0,0, 5,6);
		CHECK( bbs == getBB( s1,s2 ) );
	}
*/
	{                                  // one circle inside the other
		Circle_<NUMTYPE> c1(0,0, 2);
		Circle_<NUMTYPE> c2(0,1, 4);
		FRect_<NUMTYPE> bbc(-4,-3, +4,5);
		CHECK( bbc == getBB( c1,c2 ) );
	}
	{                                  // two distant circles
		Circle_<NUMTYPE> c1(5,5, 2);
		Circle_<NUMTYPE> c2(0,0, 1);
		FRect_<NUMTYPE> bbc(-1,-1, 7,7);
		CHECK( bbc == getBB( c1,c2 ) );
	}
	{  // CPolyline / Frect
		CPolyline_<NUMTYPE> po( std::vector<Point2d>{ {0,0}, {1,1}, {3,2} } );
		FRect_<NUMTYPE> rect;
		CHECK( getBB(po,rect) == FRect_<NUMTYPE>(0,0,3,2) );
	}
	{  // OPolyline / Frect
		OPolyline_<NUMTYPE> po( std::vector<Point2d>{ {0,0}, {1,1}, {3,2} } );
		FRect_<NUMTYPE> rect;
		CHECK( getBB(po,rect) == FRect_<NUMTYPE>(0,0,3,2) );
	}
/*	{  // OPolyline / Segment
		OPolyline_<NUMTYPE> po( std::vector<Point2d>{ {0,0}, {1,1}, {3,2} } );
		Segment_<NUMTYPE> seg;
		CHECK( getBB(seg,po) == FRect_<NUMTYPE>(0,0,3,2) );
	}*/

	{  // Circle / Frect
		Circle_<NUMTYPE> cir( 5,5,3 ); // center at 5,5, radius=3
		FRect_<NUMTYPE> rect; // (0,0)--(1,1)
		CHECK( getBB(cir,rect) == FRect_<NUMTYPE>(0,0,8,8) );
	}
/*	{  // Circle / Segment
		Circle_<NUMTYPE> cir( 5,5,3 ); // center at 5,5, radius=3
		Segment_<NUMTYPE> seg(10,20,30,40);
		CHECK( getBB(cir,seg) == FRect_<NUMTYPE>(2,2,30,40) );
	}*/
	{  // Circle / Circle (one inside the other)
		Circle_<NUMTYPE> cir1( 5,5,3 ); // center at 5,5, radius=3
		Circle_<NUMTYPE> cir2( 5,5,1 ); // center at 5,5, radius=1
		CHECK( getBB(cir1,cir2) == FRect_<NUMTYPE>(2,2,8,8) );
	}
	{  // two identical Circles
		Circle_<NUMTYPE> cir1,cir2;
		CHECK( getBB(cir1,cir2) == FRect_<NUMTYPE>(-1,-1,1,1) );
	}
	{  // two identical Ellipses
		Ellipse_<NUMTYPE> e1,e2;
		CHECK( getBB(e1,e2) == FRect_<NUMTYPE>(-2,-1,2,1) );
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
		CHECK( r1.getCenter() == Point2d(0.5,0.5) );

// free functions
		CHECK( width(r1)  == 1. );
		CHECK( height(r1) == 1. );
		CHECK( length(r1) == 4 );
		CHECK( area(r1)   == 1 );
		CHECK( getCenter(r1) == Point2d(0.5,0.5) );

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
		CHECK( r.getCenter() == Point2d(0,0) );
	}
	{
		FRect_<NUMTYPE> r1( 0,0, 5, 3 );
		FRect_<NUMTYPE> r2( 2,3, 7, 6 );
		CHECK( r1.size() == r2.size() );
		CHECK( size(r1) == size(r2) );
	}
}

TEST_CASE( "FRect extended", "[frect-ext]" )
{
	FRect_<NUMTYPE> rect;
	auto r1a = rect.getExtended();
	auto r1b = getExtended(rect);
	CHECK( r1a == r1b );
	CHECK( r1a == FRect(-1,-1,2,2 ) );
	CHECK( r1a.area() == 9 * rect.area() );

	FRect_<NUMTYPE> rect2(4,5,6,6);
	CHECK( rect2.getExtended() == FRect(2,4,8,7) );
}

TEST_CASE( "FRect diagonals", "[frect-diags]" )
{
	FRect_<NUMTYPE> r1;
	auto psegs1 = r1.getDiagonals();
	auto psegs2 = getDiagonals(r1);
	CHECK( psegs1 == psegs2 );

	CHECK( psegs1.first  == Segment(0,0,1,1) );
	CHECK( psegs1.second == Segment(0,1,1,0) );
}

TEST_CASE( "Polyline comparison 1", "[polyline-comparison-1]" )
{
	std::vector<Point2d_<NUMTYPE>> vpt1{ {0,0}, {1,0}, {1,1}        };
	std::vector<Point2d_<NUMTYPE>> vpt2{        {1,0}, {1,1}, {0,0} };
	{
		OPolyline opl1, opl2;
		opl1.set( vpt1 );
		opl2.set( vpt2 );
		CHECK( opl1 != opl2 );

		CPolyline cpl1, cpl2;
		cpl1.set( vpt1 );
		cpl2.set( vpt2 );
		CHECK( cpl1 == cpl2 );

		CHECK( opl1 != cpl1 ); // comparing polygons of different types is allowed
		CHECK( opl2 != cpl1 ); // (but will always return false)
	}
}


TEST_CASE( "Polyline minimization", "[polyline-min]" )
{
	INFO( "Polyline pl2( IsClosed::No" );
	{
#include "figures_test/polyline_min_1O.code" // open

		CHECK( pl.size() == 3 );
		CHECK( pl.nbSegs() == 2 );
		pl.minimize();
		CHECK( pl.size() == 2 );
		CHECK( pl.nbSegs() == 1 );
	}
	{
#include "figures_test/polyline_min_1C.code"  // closed
		CHECK( pl.size()   == 3 );
		CHECK( pl.nbSegs() == 3 );
		pl.minimize();
		CHECK( pl.size()   == 2 );
		CHECK( pl.nbSegs() == 2 );
	}
	{
#include "figures_test/polyline_min_2O.code"// open
		CHECK( pl.size()   == 3 );
		CHECK( pl.nbSegs() == 2 );
		pl.minimize();
//		std::cout << "pl:" << pl << '\n';
		CHECK( pl.size()   == 3 ); // no change
		CHECK( pl.nbSegs() == 2 );
	}
	{
#include "figures_test/polyline_min_2C.code" // closed
		CHECK( pl.size()   == 3 );
		CHECK( pl.nbSegs() == 3 );
		pl.minimize();
		CHECK( pl.size()   == 3 );   // no change !
		CHECK( pl.nbSegs() == 3 );
	}
	{
#include "figures_test/polyline_min_3O.code" // open
		CHECK( pl.size()   == 4 );
		CHECK( pl.nbSegs() == 3 );
		pl.minimize();
//		std::cout << "pl:" << pl << '\n';
		CHECK( pl.size()   == 4 );     // no change
		CHECK( pl.nbSegs() == 3 );
	}
	{
#include "figures_test/polyline_min_3C.code" // closed
		CHECK( pl.size()   == 4 );
		CHECK( pl.nbSegs() == 4 );
		pl.minimize();
//		std::cout << "pl:" << pl << '\n';
		CHECK( pl.size()   == 3 );     // no change
		CHECK( pl.nbSegs() == 3 );
	}
}

template<typename T, typename U>
void polytest_1( const base::PolylineBase<T,U>& pl1 )
{
	CHECK( pl1.isPolygon() == false );
	CHECK( isPolygon(pl1)  == false );

	CHECK( pl1.size() == 0 );
	CHECK( size(pl1)  == 0 );

	CHECK( pl1.nbSegs() == 0 );
	CHECK( nbSegs(pl1)  == 0 );

	CHECK( pl1.length() == 0 );
	CHECK( length(pl1)  == 0 );

	CHECK( pl1.area()   == 0 );
	CHECK( area(pl1)    == 0 );

	CHECK_THROWS( centroid(pl1) ); // unable
	CHECK_THROWS( pl1.centroid() ); // unable
}

TEST_CASE( "Polyline", "[polyline]" )
{
	{                                 // default constructor
		OPolyline_<NUMTYPE> pl1;
		polytest_1( pl1 );
		CPolyline_<NUMTYPE> pl2;
		polytest_1( pl2 );
	}
	{
		CPolyline_<NUMTYPE> pc1;
		OPolyline_<NUMTYPE> po1;
		std::vector<Point2d> vpt{ {0,0} };
		CHECK_THROWS( pc1.set( vpt ) );  // can't hold only 1 point
		CHECK_THROWS( po1.set( vpt ) );
	}
	{
		std::vector<Point2d> vpt{ {0,0} };
		CHECK_THROWS( CPolyline_<NUMTYPE>( vpt ) ); // can(t build a polyline with a vector of size=1
		CHECK_THROWS( OPolyline_<NUMTYPE>( vpt ) );
	}
	{                           // build from Rectangle
		FRect r( 5,6, 7,8 );
		CPolyline_<NUMTYPE> pl1( r );

		CHECK( pl1.isPolygon() == true );
		CHECK( isPolygon(pl1)  == true );
		CHECK( pl1.size()   == 4 );
		CHECK( size(pl1)   == 4 );
		CHECK( pl1.nbSegs() == 4 );
		CHECK( nbSegs(pl1)  == 4 );
		CHECK( pl1.length() == 8 );
		CHECK( length(pl1)  == 8 );
		CHECK( pl1.area()   == 4 );
		CHECK( area(pl1)    == 4 );
		CHECK( pl1.centroid() == Point2d(6,7) );
		CHECK( centroid(pl1)  == Point2d(6,7) );

		auto vpts = pl1.getPts();
		CHECK( vpts.size() == 4 );
		CHECK( vpts[0] == Point2d(5,6) );

		pl1.translate(1,2);
		CHECK( pl1 == CPolyline_<NUMTYPE>( FRect( 6,8, 8,10 ) ) );
	}
	{                           // set from Rectangle
		FRect r( 5,6, 7,8 );
		CPolyline_<NUMTYPE> pl1;
		pl1.set(r);

		CHECK( pl1.isPolygon() == true );
		CHECK( pl1.size() == 4 );
		CHECK( pl1.getPoint(0)  == Point2d(5,6) );
	}
	{
		OPolyline_<NUMTYPE> po1;
		CPolyline_<NUMTYPE> pc1;
		std::vector<Point2d> vpt{ {0,0},{2,2}, {2,2}, {4,3} };

		CHECK_THROWS( po1.set( vpt ) ); // cant have two contiguous identical points
		CHECK_THROWS( pc1.set( vpt ) ); // cant have two contiguous identical points
	}
	{
		std::vector<Point2d> vpt{ {0,0}, {1,1.5}, {3,5}, {1,4} };

		OPolyline_<NUMTYPE> po1(vpt);
		CPolyline_<NUMTYPE> pc1(vpt);

		CHECK( po1.isClosed() == false );
		CHECK( pc1.isClosed() == true );

		CHECK( po1.size() == 4 );
		CHECK( pc1.size() == 4 );

		CHECK( po1.nbSegs() == 3 );
		CHECK( pc1.nbSegs() == 4 );

		CHECK( po1.isPolygon() == false );
		CHECK( pc1.isPolygon() == true );

		CPolyline_<NUMTYPE> pc2(po1);
		CHECK( pc2.nbSegs()    == 4 );
		CHECK( pc2.isPolygon() == true );

		FRect bb1( 0,0, 3,5);
		CHECK( getBB(po1) == bb1 );
		CHECK( getBB(pc1) == bb1 );
		CHECK( po1.getBB() == bb1 );
		CHECK( pc1.getBB() == bb1 );
	}
	{
		std::vector<Point2d> vpt{ {0,0}, {1,1}, {0,1}, {1,0} };
		CPolyline_<NUMTYPE> pc1(vpt);
		CHECK( pc1.isClosed() == true );
		CHECK( pc1.size() == 4 );
		CHECK( pc1.nbSegs() == 4 );
		CHECK( pc1.isPolygon() == false ); // crossing segments

		pc1.translate(2,1.);
		CHECK( pc1.getPoint(0) == Point2d(2,1.) ); // (0,0) translated to (2,1)
	}
	{                                        // build from std::array
		std::array<Point2d,3> pts{{ {0,0}, {1,1}, {0,1} }};
		CPolyline_<NUMTYPE> pc(pts);
		OPolyline_<NUMTYPE> po(pts);
		CHECK( pc.isClosed() == true );
		CHECK( po.isClosed() == false );
		CHECK( pc.size() == 3 );
		CHECK( po.size() == 3 );
		CHECK( pc.nbSegs() == 3 );
		CHECK( po.nbSegs() == 2 );
	}
	{                                          // build from std::list
		std::list<Point2d> pts{ {0,0}, {1,1}, {0,1} };
		CPolyline_<NUMTYPE> pc(pts);
		OPolyline_<NUMTYPE> po(pts);
		CHECK( pc.isClosed() == true );
		CHECK( po.isClosed() == false );
		CHECK( pc.size() == 3 );
		CHECK( po.size() == 3 );
		CHECK( pc.nbSegs() == 3 );
		CHECK( po.nbSegs() == 2 );
	}
	{                   // build from segment
		Segment_<NUMTYPE> seg;
		CPolyline_<NUMTYPE> pc( seg );
		OPolyline_<NUMTYPE> po( seg );
		CHECK( pc.size() == 2 );
		CHECK( po.size() == 2 );
		CHECK( pc.getPoint(0) == Point2d(0,0) );
		CHECK( po.getPoint(0) == Point2d(0,0) );
		CHECK( pc.nbSegs() == 2 );
		CHECK( po.nbSegs() == 1 );
	}
	{                   // build from segment - 2
		Segment_<NUMTYPE> seg; // (0,0) - (1,1)
		CPolyline_<NUMTYPE> pc1( seg );
		OPolyline_<NUMTYPE> po1( seg );
		std::list<Point2d> pts{ {1,1},{0,0} };
		CPolyline_<NUMTYPE> pc2(pts);
		OPolyline_<NUMTYPE> po2(pts);
		CHECK( pc1 == pc2 );
		CHECK( po1 == po2 );
	}
}

TEST_CASE( "Polyline setParallelogram", "[polyline-setParallelogram]" )
{
	{
		Point2d_<NUMTYPE> pt1(0,0);
		Point2d_<NUMTYPE> pt2(0,2);
		Point2d_<NUMTYPE> pt3(4,5);
		CPolyline_<NUMTYPE> pol;
		pol.setParallelogram( pt1, pt2, pt3 );

		std::vector<Point2d_<NUMTYPE>> vpts{ {0,0}, {0,2}, {4,5}, {4,3} };
		CPolyline_<NUMTYPE> pol2(vpts);
		CHECK( pol == pol2 );
	}
}

TEST_CASE( "Polyline get extreme point", "[polyline-getExtremePoint]" )
{
	{
		CPolyline cp_empty;
		CHECK_THROWS( cp_empty.getLmPoint() );
	}
	{
		Point2d_<NUMTYPE> pt( 3, 4 );
		std::vector<Point2d_<NUMTYPE>> vpt{ pt };

		CHECK( getLmPoint(vpt) == pt );
		CHECK( getRmPoint(vpt) == pt );
		CHECK( getTmPoint(vpt) == pt );
		CHECK( getBmPoint(vpt) == pt );
	}
	{
		std::vector<Point2d_<NUMTYPE>> vpt{ {1,0}, {0,1}, {-1,0}, {0,-1} };
		CPolyline cp( vpt );
		CHECK( cp.size() == 4 );
		CHECK( cp.getLmPoint() == Point2d_<NUMTYPE>(-1,0) );
		CHECK( cp.getRmPoint() == Point2d_<NUMTYPE>(1,0) );
		CHECK( cp.getTmPoint() == Point2d_<NUMTYPE>(0,1) );
		CHECK( cp.getBmPoint() == Point2d_<NUMTYPE>(0,-1) );

		CHECK( getLmPoint(vpt) == Point2d_<NUMTYPE>(-1,0) );
		CHECK( getRmPoint(vpt) == Point2d_<NUMTYPE>(1,0) );
		CHECK( getTmPoint(vpt) == Point2d_<NUMTYPE>(0,1) );
		CHECK( getBmPoint(vpt) == Point2d_<NUMTYPE>(0,-1) );
	}
	{
		std::vector<Point2d_<NUMTYPE>> vpt{ {0,0}, {1,0}, {2,0}, {2,1}, {2,2}, {1,2}, {0,2}, {0,1} };
		CPolyline cp( vpt );
		CHECK( cp.size() == 8 );
		CHECK( cp.getLmPoint() == Point2d_<NUMTYPE>(0,0) );
		CHECK( cp.getRmPoint() == Point2d_<NUMTYPE>(2,0) );
		CHECK( cp.getTmPoint() == Point2d_<NUMTYPE>(0,2) );
		CHECK( cp.getBmPoint() == Point2d_<NUMTYPE>(0,0) );

		CHECK( getLmPoint(vpt) == Point2d_<NUMTYPE>(0,0) );
		CHECK( getRmPoint(vpt) == Point2d_<NUMTYPE>(2,0) );
		CHECK( getTmPoint(vpt) == Point2d_<NUMTYPE>(0,2) );
		CHECK( getBmPoint(vpt) == Point2d_<NUMTYPE>(0,0) );
	}
}

TEST_CASE( "Bounding Box of set of objects", "[BB-cont]" )
{
	std::vector<Point2d> vpts1{ {0,0}, {1,1.5}, {3,5}, {1,4} };
	FRect bb1( 0,0, 3,5 );
	{
		CHECK( getBB(vpts1) == bb1 );

		CPolyline cpol( vpts1 );
		OPolyline opol( vpts1 );
		std::vector<CPolyline> vec_c{ cpol };
		std::vector<OPolyline> vec_o{ opol };
		CHECK( getBB(vec_c) == bb1 );
		CHECK( getBB(vec_o) == bb1 );
	}
	{
		std::vector<Segment> vseg;
		CHECK_THROWS( getBB(vseg) );
		vseg.emplace_back( Segment(0,0,0,1) );
		CHECK_THROWS( getBB(vseg) );     // no area !

		vseg.emplace_back( Segment(4,5,6,7) );
		FRect bb2( 0,0, 6,7 );
		CHECK( getBB(vseg) == bb2 );
	}
	{
		std::list<Circle> vcir;
		CHECK_THROWS( getBB(vcir) );     // empty !
		vcir.emplace_back( Circle(1,1,1) );
		FRect bb3( 0,0, 2,2 );
		CHECK( getBB(vcir) == bb3 );
	}
	{
		std::vector<Point2d> vpt{ {0,11} };
		CHECK_THROWS( getBB(vpt) );         // need at least 2 points !

		std::list<Point2d> lpt{ {2,11} };
		CHECK_THROWS( getBB(lpt) );         // need at least 2 points !

		std::array<Point2d,1> apt;
		apt[0] = Point2d{2,11};
		CHECK_THROWS( getBB(apt) );         // need at least 2 points !
	}
}

TEST_CASE( "getCenters() and getLines()", "[getCenters]" )
{
	{
		std::vector<Segment> vec;
		auto out1 = getCenters( vec );
		CHECK( out1.empty() );

		vec.emplace_back( Segment( 0,0,2,0) );
		vec.emplace_back( Segment( 0,1,2,1) );
		auto out2 = getCenters( vec );
		CHECK( out2.size() == 2 );
		CHECK( out2[0] == Point2d(1,0) );
		CHECK( out2[1] == Point2d(1,1) );

		auto out3 = getLines( vec );
		CHECK( out3.size() == 2 );
		CHECK( out3[0] == Line2d( Point2d(0,0), Point2d(5,0) ) );
		CHECK( out3[1] == Line2d( Point2d(0,1), Point2d(5,1) ) );
	}
	{
		std::list<Circle> vec;
		auto out1 = getCenters( vec );
		CHECK( out1.empty() );

		vec.emplace_back( Circle( 0,0, 2) );
		vec.emplace_back( Circle( 1,1, 3) );
		auto out2 = getCenters( vec );
		CHECK( out2.size() == 2 );
		CHECK( out2[0] == Point2d(0,0) );
		CHECK( out2[1] == Point2d(1,1) );
	}
	{
		std::array<Ellipse,2> vec;
		auto out1 = getCenters( vec );
		CHECK( out1.size() == 2 );    // because input array has size=2

		vec[0] = Ellipse( 0,0, 2, 3 );
		vec[1] = Ellipse( 1,1, 3, 8 );
		auto out2 = getCenters( vec );
		CHECK( out2.size() == 2 );
		CHECK( out2[0] == Point2d(0,0) );
		CHECK( out2[1] == Point2d(1,1) );
	}
}

TEST_CASE( "Polygon convexity", "[polyline-convex]" )
{
	OPolyline_<NUMTYPE> plo;
	CPolyline_<NUMTYPE> plc;
	CHECK( !plo.isConvex() );  // empty !!
	CHECK( !plc.isConvex() );

	CHECK( !isConvex(plc) ); // free function call
	CHECK( !isConvex(plo) );

	plo.set( std::vector<Point2d>{ {0,0}, {2,0} } );
	plc.set( std::vector<Point2d>{ {0,0}, {2,0} } );
	CHECK( !plo.isConvex() );  // 2 pts are fine, but not convex
	CHECK( !plc.isConvex() );

	plo.set( std::vector<Point2d>{ {0,0}, {2,0}, {2,1} } );
	plc.set( std::vector<Point2d>{ {0,0}, {2,0}, {2,1} } );
	CHECK( !plo.isConvex() );  // 3 pts ok, but open Polyline never convex
	CHECK( plc.isConvex() );

	plc.set( std::vector<Point2d>{ {0,0}, {2,0}, {2,2}, {0,2} } );
	CHECK( plc.isConvex() );
	plc.set( std::vector<Point2d>{ {0,0}, {2,0}, {1,1}, {2,2}, {0,2} } );
	CHECK( !plc.isConvex() );

	plc.set( std::vector<Point2d>{ {2,2}, {2,-2}, {-2,-2}, {-2,2} } );
	CHECK( plc.isConvex() );
	plc.set( std::vector<Point2d>{ {2,2}, {2,-2}, {1,1}, {-2,2} } );
	CHECK( !plc.isConvex() );
}

TEST_CASE( "Polygon orientation", "[polyline-orient]" )
{
	{
		CPolyline_<NUMTYPE> pl1;
		CPolyline_<NUMTYPE> pl2;                                             //  +-----+
		pl1.set( std::vector<Point2d>{ {0,0}, {2,0}, {2,1}, {0,1} } );       //  |     |
		pl2.set( std::vector<Point2d>{ {0,0}, {0,1}, {2,1}, {2,0} } );       //  +-----+
		CHECK( pl1 == pl2 );   // is closed, so the points describe the same thing
	}
	{
		OPolyline_<NUMTYPE> pl1;
		OPolyline_<NUMTYPE> pl2;                                          //  +-----+     +-----+
		pl1.set( std::vector<Point2d>{ {0,0}, {2,0}, {2,1}, {0,1} } );    //        | and |     |
		pl2.set( std::vector<Point2d>{ {0,0}, {0,1}, {2,1}, {2,0} } );    //  +-----+     +     +
		CHECK( pl1 != pl2 ); // is open, so they are different

		CPolyline_<NUMTYPE> plc1(pl1);
		CPolyline_<NUMTYPE> plc2(pl2);
		CHECK( plc1 == plc2 );
	}
	{
		std::vector<Point2d> v1{ {0,0}, {1,0}, {1,1} };
		std::vector<Point2d> v2{ {1,1}, {1,0}, {0,0} };

		CPolyline_<NUMTYPE> pl1(v1);
		CPolyline_<NUMTYPE> pl2(v2);
		CHECK( pl1 == pl2 );
		OPolyline_<NUMTYPE> po1(v1);
		OPolyline_<NUMTYPE> po2(v2);
		CHECK( po1 == po2 );
	}
}

TEST_CASE( "Polyline fullstep rotation", "[polyline-rot]" )
{
	OPolyline_<NUMTYPE> plo;
	CPolyline_<NUMTYPE> plc;
	OPolyline_<NUMTYPE> plo_ff;
	CPolyline_<NUMTYPE> plc_ff;
	std::vector<Point2d>  vpts{ {0,0}, {2,0}, {1,1} };
	{
		plo.set( vpts );
		plc.set( vpts );
		plo.rotate( Rotate::CCW );
		plc.rotate( Rotate::CCW );
		std::vector<Point2d> vpts2{ {0,0},{0,2},{-1,1} };
		OPolyline_<NUMTYPE> plo2( vpts2 );
		CPolyline_<NUMTYPE> plc2( vpts2 );
		CHECK( plo == plo2 );
		CHECK( plc == plc2 );

		plo_ff.set( vpts );
		plc_ff.set( vpts );
		rotate( plo_ff, Rotate::CCW );
		rotate( plc_ff, Rotate::CCW );
		CHECK( plo == plo_ff );
		CHECK( plc == plc_ff );
	}
	{
		plo.set( vpts );
		plc.set( vpts );
		plo.rotate( Rotate::VMirror );
		plc.rotate( Rotate::VMirror );
		std::vector<Point2d> vpts2{ {0,0},{-2,0},{-1,1} };
		OPolyline_<NUMTYPE> plo2( vpts2 );
		CPolyline_<NUMTYPE> plc2( vpts2 );
		CHECK( plo == plo2 );
		CHECK( plc == plc2 );
	}
	{
		plo.set( vpts );
		plc.set( vpts );
		plo.rotate( Rotate::HMirror );
		plc.rotate( Rotate::HMirror );
		std::vector<Point2d> vpts2{ {0,0},{2,0},{1,-1} };
		OPolyline_<NUMTYPE> plo2( vpts2 );
		CPolyline_<NUMTYPE> plc2( vpts2 );
		CHECK( plo == plo2 );
		CHECK( plc == plc2 );
	}
}

TEST_CASE( "Polygon area", "[polyline-area]" )
{
	CPolyline_<NUMTYPE> pl1;
	{
		pl1.set( std::vector<Point2d>{ {0,0}, {2,0}, {2,1}, {0,1} } );
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
	OPolyline_<NUMTYPE> plo;
	plo.set( std::vector<Point2d>{ {0,0}, {2,0}, {2,1}, {0,1} } );
	CHECK( plo.isPolygon() == false );
	CHECK( plo.area() == 0. );
}

TEST_CASE( "Polyline comparison 2", "[polyline-comp-2]" )
{
	{
		OPolyline_<NUMTYPE> pa,pb;
		CHECK( pa == pb );
	}
	{
		CPolyline_<NUMTYPE> pa,pb;
		CHECK( pa == pb );
	}

	OPolyline_<NUMTYPE> pl2( std::vector<Point2d>{ {7,8},{3,4},{5,6}       } );
	OPolyline_<NUMTYPE> pl1( std::vector<Point2d>{       {3,4},{5,6},{7,8} } );

	{
		OPolyline_<NUMTYPE> p1 = pl1;
		OPolyline_<NUMTYPE> p2 = pl2;

		CHECK( p1.isClosed()     == false );
		CHECK( p1.isNormalized() == false );
		CHECK( p2.isNormalized() == false );
		CHECK( p1!=p2 );
	}
	{
		CPolyline_<NUMTYPE> p1 = pl1; // build a closed one from an open one
		CPolyline_<NUMTYPE> p2 = pl2;

		CHECK( p1.isClosed()     == true );
		CHECK( p1.isNormalized() == false );
		CHECK( p2.isNormalized() == false );
		CHECK( (p1==p2) == true );
		CHECK( p1.isNormalized() == true );
		CHECK( p2.isNormalized() == true );
	}
	{
		CPolyline_<NUMTYPE> p2( std::vector<Point2d>{ {1,2},{1,1},{2,1}       } );
		CPolyline_<NUMTYPE> p1( std::vector<Point2d>{       {1,1},{2,1},{1,2} } );

		CHECK( p1.getPoint(0) == Point2d_<NUMTYPE>(1,1) );
		CHECK( p2.getPoint(0) == Point2d_<NUMTYPE>(1,2) );
		CHECK( p1 == p2 ); // normalizing
		CHECK( p1.getPoint(0) == Point2d_<NUMTYPE>(1,1) );
		CHECK( p2.getPoint(0) == Point2d_<NUMTYPE>(1,1) );
	}
	{
		CPolyline pa, pb;
		{
#include "figures_test/polyline_comp_1a.code"
			pa = pl;
		}
		{
#include "figures_test/polyline_comp_1b.code"
			pb = pl;
		}
		CHECK( pa.size() == pb.size() );
		CHECK( pa != pb );
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

TEST_CASE( "convex hull", "[conv_hull]" )
{
	{
		CPolyline_<NUMTYPE> pl;

		pl.set( FRect_<NUMTYPE>(1,1,3,3) );
		CHECK( priv::chull::getPivotPoint(pl.getPts() ) == 0 );

		std::vector<Point2d> v1{ {0,0}, {2,0}, {2,2}, {1,2}, {1,1}, {0,1} };
		pl.set( v1 );
		CHECK( priv::chull::getPivotPoint(pl.getPts() ) == 0 );

		std::rotate( v1.begin(), v1.begin()+1, v1.end() );
		pl.set( v1 );
		CHECK( priv::chull::getPivotPoint(pl.getPts() ) == 5 );

		std::rotate( v1.begin(), v1.begin()+1, v1.end() );
		pl.set( v1 );
		CHECK( priv::chull::getPivotPoint(pl.getPts() ) == 4 );
	}
	{
#include "figures_test/polyline_chull_1.code"
		auto vp = pl.getPts();
		CHECK( priv::chull::getPivotPoint(pl.getPts() ) == 0 );
		auto vout = priv::chull::sortPoints( vp, 0 );
		CHECK( vout == std::vector<size_t>{ 0,1,2,3 } );
		auto ch = convexHull( pl );
		CHECK( ch == CPolyline(
				std::vector<Point2d>{
					{0,0},{3,0},{0,3}
				}
			)
		);
	}
	{
		CPolyline pl1;
		auto ch1 = convexHull( pl1 );
		CHECK( ch1.size() == 0 );
		auto ch2 = pl1.convexHull();
		CHECK( ch2.size() == 0 );
	}
	{
		CPolyline pl1( std::vector<Point2d>{ {1,1}, {2,2} });
		auto ch1 = convexHull( pl1 );
		CHECK( ch1.size() == 2 );
		auto ch2 = pl1.convexHull();
		CHECK( ch2.size() == 2 );
	}
}


//////////////////////////////////////////////////////////////
/////               SVG IMPORT TESTS                     /////
//////////////////////////////////////////////////////////////

#ifdef HOMOG2D_USE_SVG_IMPORT
TEST_CASE( "SVG_Import_1", "[svg_import_1]" )
{
/// \note Here, we use only the "double" type, because that is the one used on import
	{
		Circle c( 50,50,20);
		img::Image<img::SvgImage> im(200,200);
		c.draw( im );
		im.write( "BUILD/test_svg_11.svg" );

		tinyxml2::XMLDocument doc;                // load the file that we just created
		doc.LoadFile( "BUILD/test_svg_11.svg" );

		h2d::svg::Visitor visitor;
		doc.Accept( &visitor );
		const auto& data = visitor.get();
		CHECK( data.size() == 1 );
		const auto& elem = data.at(0);
		CHECK( elem->type() == Type::Circle );

		const Circle* pc2 = static_cast<Circle*>( elem.get() );
		CHECK( pc2->radius() == 20 );
	}
	{                               // this test makes sure the <g> element is ignored
		tinyxml2::XMLDocument doc;
		doc.LoadFile( "misc/other/test_svg_import_1.svg" );
		h2d::svg::Visitor visitor;
		doc.Accept( &visitor );
		const auto& data = visitor.get();
		CHECK( data.size() == 3 );
		for( const auto& elem: data )
			CHECK( elem->type() == Type::Circle );
	}
	{                            // read a file with 3 circles, one rect, one segment and a polygon
		tinyxml2::XMLDocument doc;
		doc.LoadFile( "misc/other/test_svg_import_2.svg" );
		h2d::svg::Visitor visitor;
		doc.Accept( &visitor );
		const auto& data = visitor.get();
		CHECK( data.size() == 6 );
		CHECK( data.at(3)->type() == Type::Segment );
		CHECK( data.at(4)->type() == Type::FRect );
		CHECK( data.at(5)->type() == Type::CPolyline );
	}
}

TEST_CASE( "SVG Import Ellipse", "[svg_import_ell]" )
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "misc/other/test_svg_import_3.svg" );
	h2d::svg::Visitor visitor;
	doc.Accept( &visitor );
	const auto& data = visitor.get();
	CHECK( data.size() == 3 );
	for( const auto& p: data )
	{
		std::cout << *p << '\n';
		if( p->type() == Type::Ellipse )
		{
			Ellipse ell( 150, 100, 60, 15, 20*M_PI/180. );
			const Ellipse* pell = static_cast<const Ellipse*>( p.get() );
			CHECK( ell == *pell );
		}
	}
}
#endif

//////////////////////////////////////////////////////////////
/////        BOOST GEOMETRY BINDING TESTS                /////
//////////////////////////////////////////////////////////////

#ifdef HOMOG2D_USE_BOOSTGEOM
TEST_CASE( "boost geometry point import", "[bg-pt-import]" )
{
	namespace bg = boost::geometry;
	bg::model::point<double, 2, boost::geometry::cs::cartesian> ptb1(3,4);
	bg::model::d2::point_xy<double> ptb2(5,6);
	Point2d_<NUMTYPE> pt1(ptb1);
	Point2d_<NUMTYPE> pt2(ptb2);
	CHECK( pt1 == Point2d_<NUMTYPE>(3,4) );
	CHECK( pt2 == Point2d_<NUMTYPE>(5,6) );

	pt1 = ptb1;         // using assignment operator
	pt2 = ptb2;
	CHECK( pt1 == Point2d_<NUMTYPE>(3,4) );
	CHECK( pt2 == Point2d_<NUMTYPE>(5,6) );
}

TEST_CASE( "boost geometry point export", "[bg-pt-export]" )
{
	Point2d_<NUMTYPE> ref1(3,4);
	Point2d_<NUMTYPE> ref2(5,6);
	namespace bg = boost::geometry;
	using point_t1 = bg::model::point<double, 2, bg::cs::cartesian>;
	using point_t2 = bg::model::d2::point_xy<double>;

	auto pt1 = getPt<point_t1>( ref1 );
	auto pt2 = getPt<point_t2>( ref2 );
	CHECK( bg::get<0>(pt1) == ref1.getX() );
	CHECK( bg::get<0>(pt2) == ref2.getX() );
}

TEST_CASE( "boost geometry vector point export", "[bg-vpt-export]" )
{
	namespace bg = boost::geometry;
	std::vector<Point2d> vin;
	vin.push_back( Point2d(0.0, 0.0) );
	vin.push_back( Point2d(0.0, 5.0));
	vin.push_back( Point2d(5.0, 5.0));
	auto vout = getPts<bg::model::d2::point_xy<float>>(vin); // convert to a vector of boost geometry points
	CHECK( vout.size() == 3 );
}

#endif
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

			auto cvpt_1 = getPt<cv::Point2d>( pt );
			auto cvpt_2 = getPt<cv::Point2f>( pt );
			auto cvpt_3 = getPt<cv::Point2i>( pt );
		}
		{
			cv::Point2d cvpt1 = pt.getCvPtd() ;
			CHECK( (cvpt1.x == 1. && cvpt1.y == 2.) );
			cv::Point2f cvpt2 = pt.getCvPtf() ;
			CHECK( (cvpt2.x == 1. && cvpt2.y == 2.) );
			cv::Point2i cvpt3 = pt.getCvPti() ;          // integer point
			CHECK( (cvpt3.x == 1  && cvpt3.y == 2 ) );

			auto cvpt_1 = pt.getPt<cv::Point2d>();
			auto cvpt_2 = pt.getPt<cv::Point2f>();
			auto cvpt_3 = pt.getPt<cv::Point2i>();
		}
		{ // input: vector of "double" points
		  // converted into "float" Opencv points
			std::vector<Point2d> v{ Point2d(1,2), Point2d(5,6), Point2d(3,4) };
			auto vcv1 = getPts<cv::Point2d>( v );
			CHECK( vcv1.size() == 3 );
			auto vcv2 = getPts<cv::Point2f>( v );
			CHECK( vcv2.size() == 3 );
			auto vcv3 = getPts<cv::Point2i>( v );
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

