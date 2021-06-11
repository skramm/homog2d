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
\file demo_opencv.cpp
\brief demo of interfacing with Opencv
*/

#define HOMOG2D_USE_OPENCV
#include "homog2d.hpp"

// additional Opencv header, needed for GUI stuff
#include "opencv2/highgui.hpp"

#include <functional>

using namespace homog2d;

std::string g_wndname = "homog2d demo";
cv::Mat g_img;
int g_width = 600;
int g_height = 500;

struct Data
{
	std::vector<Point2d> vpt;
	std::vector<Point2d> vpt2;
	Segment s1,s2;
	int selected = -1;
	Point2d pt_mouse;
	double angle = 20.;
	Data()
	{
		vpt.resize(4);
		reset();

	}
	void reset()
	{
		vpt[0] = Point2d(100,200);
		vpt[1] = Point2d(200,300);
		vpt[2] = Point2d(150,50);
		vpt[3] = Point2d(300,250);
	}

	void setMousePos(int x, int y)
	{
		pt_mouse.set(x,y);
	}
	int nbPts() const
	{
		return (int)vpt.size();
	}
	void moveSelectedPoint()
	{
		if( selected != -1 )
			vpt.at(selected) = pt_mouse;
	}
};

Data g_data;


std::array<Line2d,3> g_li; // for demo4()
int g_radius = 80;         // for demo4()

void clearImage()
{
	g_img = cv::Scalar(255,255,255);
}

void drawLines( int selected )
{
//	clearImage();
	for( int i=0; i<g_data.nbPts(); i++ )
	{
		if( selected == i )
			g_data.vpt[i].draw( g_img, CvDrawParams().setColor( 250, 0, 150).setPointStyle( (PointStyle)i).selectPoint() );
		else
			g_data.vpt[i].draw( g_img, CvDrawParams().setPointStyle((PointStyle)i) );
	}
	Line2d lA( g_data.vpt[0], g_data.vpt[2] );
	Line2d lB( g_data.vpt[0], g_data.vpt[3] );
	Line2d lC( g_data.vpt[1], g_data.vpt[2] );
	Line2d lD( g_data.vpt[1], g_data.vpt[3] );

	lA.draw( g_img, CvDrawParams().setColor(   0,  50, 150) );
	lB.draw( g_img, CvDrawParams().setColor( 150,  50,   0) );
	lC.draw( g_img, CvDrawParams().setColor(  50, 150,   0) );
	lD.draw( g_img, CvDrawParams().setColor( 150,   0,  50) );
}


/// Called by mouse callback functions, checks if one of the points is selected.
/**
- If so, that point gets moved by the mouse, and the function \c actionM is called
*/
void
checkSelected( int event, int x, int y, std::function<void()> action, std::function<void()> actionM )
{
//	clearImage();
	g_data.setMousePos(x,y);
	action();
//	cv::imshow( g_wndname, g_img );

	switch( event )
	{
		case CV_EVENT_LBUTTONUP:
			g_data.selected = -1;
		break;

		case CV_EVENT_LBUTTONDOWN:
			for( int i=0; i<g_data.nbPts(); i++ )
				if( g_data.pt_mouse.distTo( g_data.vpt[i]) < 10 )  // if mouse is less than 10 pixel away
					g_data.selected = i;
		break;

		case CV_EVENT_MOUSEMOVE:
			if( g_data.selected != -1 )
				g_data.vpt[g_data.selected] = g_data.pt_mouse;
			actionM();
		break;

		default: break;
	}
	cv::imshow( g_wndname, g_img );
}

void action_1()
{
	clearImage();
	drawLines( g_data.selected );

/*	for( int i=0; i<g_data.nbPts(); i++ )
	{
		if( g_data.selected == i )
			g_data.vpt[i].draw( g_img, CvDrawParams().setColor( 250, 0, 150).setPointStyle( (PointStyle)i) );
		else
			g_data.vpt[i].draw( g_img, CvDrawParams().setPointStyle((PointStyle)i) );
	}

	Line2d lA( g_data.vpt[0], g_data.vpt[2] );
	Line2d lB( g_data.vpt[0], g_data.vpt[3] );
	Line2d lC( g_data.vpt[1], g_data.vpt[2] );
	Line2d lD( g_data.vpt[1], g_data.vpt[3] );

	lA.draw( g_img, CvDrawParams().setColor(   0,  50, 150) );
	lB.draw( g_img, CvDrawParams().setColor( 150,  50,   0) );
	lC.draw( g_img, CvDrawParams().setColor(  50, 150,   0) );
	lD.draw( g_img, CvDrawParams().setColor( 150,   0,  50) );
*/

	Line2d l( g_data.vpt[0], g_data.vpt[2] );
	Line2d la_V( l );
	la_V.addOffset( LineOffset::vert, 60 );
	la_V.draw( g_img, CvDrawParams().setColor(250,0,250) );

	l.addOffset( LineOffset::horiz, 60 );
	l.draw( g_img, CvDrawParams().setColor(250,250,0) );
}

void action_1M()
{
	Line2d l_mouse = g_data.pt_mouse * Point2d();
	Line2d l_mouse2= l_mouse.getOrthogonalLine( GivenCoord::X, 100 );
	l_mouse.draw( g_img );
	l_mouse2.draw( g_img );
}

/// Mouse callback for demo_1
void mouse_CB_1( int event, int x, int y, int /* flags */, void* /*param*/ )
{
	checkSelected( event, x, y, action_1, action_1M );
}

void demo_1( int nd )
{
	std::cout << "Demo " << nd << ": click on points and move them\n";
	cv::setMouseCallback( g_wndname, mouse_CB_1 );

	int n=5;
	g_data.vpt[0].set( g_width/2,       g_height/n );
	g_data.vpt[1].set( g_width/2,       g_height*(n-1)/n );
	g_data.vpt[2].set( g_width/n,       g_height/2 );
	g_data.vpt[3].set( g_width*(n-1)/n, g_height/2 );

	action_1();
	cv::imshow( g_wndname, g_img );

	auto k = cv::waitKey(0);
	if( k == 27 )
		std::exit(0);
}
//------------------------------------------------------------------
void draw_2()
{
	clearImage();
	drawLines(-1);
	cv::imshow( g_wndname, g_img );
}
void initPts()
{
	int a=50;
	int b=150;
	g_data.vpt[0].set( a, a );
	g_data.vpt[1].set( b, b );
	g_data.vpt[2].set( b, a );
	g_data.vpt[3].set( a, b );
}

void demo_2( int n )
{
	std::cout << "Demo " << n << ": Hit a key: scale:[op], angle:[lm], translation:[gh,yb], reset: r\n";
	char key = 0;
	Homogr H;
	double angle = 0.;
	double angle_delta = 5.;
	double scale = 1.;
	double scale_delta = 1.2;
	double tx = 0;
	double ty = 0;
	double trans_delta = 20;
	double K = M_PI/180.;
	initPts();
	draw_2();
	while( key != 32 ) // SPC
	{
		bool change = true;
		switch( key = cv::waitKey(0) )
		{
			case 'r': // reset
				scale = 1.;
				angle = tx = ty = 0.;
			break;
			case 'm': angle += angle_delta; break;
			case 'l': angle -= angle_delta; break;

			case 'h': tx += trans_delta; break;
			case 'g': tx -= trans_delta; break;
			case 'b': ty += trans_delta; break;
			case 'y': ty -= trans_delta; break;

			case 'p': scale *= scale_delta; break;
			case 'o': scale /= scale_delta; break;
			case 27: std::exit(0);
			default: change = false; break;
		}
		if( change )
		{
			H.init();
			H.addRotation( angle*K ).addTranslation( tx, ty ).addScale( scale );
			initPts();
			H.applyTo(g_data.vpt);
			draw_2();
		}
	}
}
//------------------------------------------------------------------
void demo_3( int n )
{
	std::cout << "Demo " << n << ": offset and drawing parameters demo (check code)\n";

	Line2d li;

	Line2d lV;
	lV.addOffset( LineOffset::horiz, 100.);
	Line2d lH(1,0);
	lH.addOffset( LineOffset::vert, 100.);

	clearImage();
	lV.draw( g_img, CvDrawParams().setColor( 250, 50,  50).setThickness(2) );
	lH.draw( g_img, CvDrawParams().setColor(  50, 50, 250).setThickness(2) );

	CvDrawParams dp;
	dp.setColor( 0, 250, 0 );
	dp.setDefault();

	li = lV;
	li.addOffset( LineOffset::horiz, 50 );
	li.draw( g_img ); // use default draw parameters
	li = lV;
	li.addOffset( LineOffset::horiz, -50 );
	li.draw( g_img, CvDrawParams().setColor( 100,250,0) );

	CvDrawParams::resetDefault();

	li = lH;
	li.addOffset( LineOffset::vert, 50 );
	li.draw( g_img ); //, CvDrawParams().setColor( 100,0,250) );
	li = lH;
	li.addOffset( LineOffset::vert, -50 );
	li.draw( g_img ); //, CvDrawParams().setColor( 100,0,250) );

	cv::imshow( g_wndname, g_img );
	cv::waitKey(0);
}
//------------------------------------------------------------------
void drawLine_4()
{
	for( size_t i=0; i<g_li.size(); i++ )
		g_li[i].draw( g_img );
}

/// Mouse callback for demo4
void mouse_CB_4( int /* event */, int x, int y, int /* flags */, void* /*param*/ )
{
	clearImage();
	drawLine_4();

	g_data.pt_mouse.set( x, y );
	cv::circle( g_img, g_data.pt_mouse.getCvPtd(), g_radius, cv::Scalar(50,100,150) );
	g_data.pt_mouse.draw( g_img, CvDrawParams().setColor(250,50,20) );

	for( size_t i=0; i<g_li.size(); i++ )
	{
		auto ri = g_li[i].intersectsCircle( g_data.pt_mouse, g_radius );
		if( ri() )
		{
			auto inter = ri.get();
			inter.first.draw( g_img,  CvDrawParams().setColor(250, 0, 0) );
			inter.second.draw( g_img, CvDrawParams().setColor(250, 0, 0) );
		}
	}
	cv::imshow( g_wndname, g_img );
}

void demo_4( int n )
{
	std::cout << "Demo " << n << ": move circle over line, hit [lm] to change circle radius\n";

	g_li[0] = Point2d() * Point2d(200,100);
	g_li[1] = Point2d(200,0) * Point2d(200,200);
	g_li[2] = Point2d(0,200) * Point2d(200,200);

	clearImage();
	drawLine_4();
	cv::imshow( g_wndname, g_img );

	cv::setMouseCallback( g_wndname, mouse_CB_4 );

	char key=0;
	while( key != 32 ) // SPC
	{
		switch( key = cv::waitKey(0) )
		{
			case 'r': // reset
				g_radius = 80;
			break;
			case 'l':
				g_radius += 10;
			break;
			case 'm':
				g_radius -= 10;
			break;
			case 27: std::exit(0);
			default: break;
		}
		std::cout << "radius=" << g_radius << '\n';
		cv::imshow( g_wndname, g_img );
	}
}

//------------------------------------------------------------------
void action_5()
{
	clearImage();
	g_data.s1.set( g_data.vpt[0], g_data.vpt[1] );
	g_data.s2.set( g_data.vpt[2], g_data.vpt[3] );

	g_data.s1.draw( g_img, CvDrawParams().setColor( 0,0,250).setThickness(2) );
	g_data.s2.draw( g_img, CvDrawParams().setColor( 250,0,0).setThickness(2) );
	g_data.s1.getLine().draw( g_img, CvDrawParams().setColor( 100,100,100) );
	g_data.s2.getLine().draw( g_img, CvDrawParams().setColor( 100,100,100) );
	draw( g_img, g_data.vpt );
	cv::imshow( g_wndname, g_img );
}

void action_5M()
{
	if( g_data.selected != -1 )
		g_data.vpt[g_data.selected].draw( g_img, CvDrawParams().selectPoint() );
	auto inters = g_data.s1.intersects( g_data.s2 );
	if( inters() ) //&& g_data.selected != -1 )
	{
		auto pti = inters.get();
		pti.draw( g_img );
		Line2d l1 = g_data.s1.getLine().getOrthogonalLine( GivenCoord::X,  pti.getX() );
		l1.draw( g_img, CvDrawParams().setColor( 0,0,100) );

		Line2d l2 = g_data.s2.getLine().getOrthogonalLine( GivenCoord::X,  pti.getX() );
		l2.draw( g_img, CvDrawParams().setColor( 100,0,0) );
	}
	cv::imshow( g_wndname, g_img );
}

/// Mouse callback for demo5
void mouse_CB_5( int event, int x, int y, int /* flags */, void* /*param*/ )
{
	checkSelected( event, x, y, action_5, action_5M );
}


void demo_5( int n )
{
	std::cout << "Demo " << n << ": intersection of segments\n Select a point and move it around. "
		<< "When they intersect, you get the orthogonal lines of the two segments, at the intersection point.\n";

	g_data.vpt[0] = Point2d(100,200);
	g_data.vpt[1] = Point2d(200,300);
	g_data.vpt[2] = Point2d(150,50);
	g_data.vpt[3] = Point2d(300,250);

	action_5();
	cv::setMouseCallback( g_wndname, mouse_CB_5 );
	if( 27 == cv::waitKey(0) )
		std::exit(0);
}

//------------------------------------------------------------------
void action_6()
{
	clearImage();
	double K = M_PI / 180.;

	Homogr H( g_data.angle * K );
	H.transpose();
	Line2d l1( g_data.vpt[0], g_data.vpt[1] );
	Line2d l2 = H*l1;

	l1.draw( g_img, CvDrawParams().setColor( 250,0,0) );
	l2.draw( g_img, CvDrawParams().setColor( 0,250,0) );

	Segment s1( g_data.vpt[2], g_data.vpt[3] );
	Segment s2 = H*s1;
	s1.draw( g_img, CvDrawParams().setColor( 0,0,250) );
	s2.draw( g_img, CvDrawParams().setColor( 250,250,0) );
	s1.get().first.draw( g_img, CvDrawParams().selectPoint() );
	s1.get().second.draw( g_img, CvDrawParams().selectPoint() );
}

void action_6M()
{
	g_data.moveSelectedPoint();

}
/// Mouse callback for demo_6
void mouse_CB_6( int event, int x, int y, int /* flags */, void* /*param*/ )
{
	checkSelected( event, x, y, action_6, action_6M );
}

void demo_6(int n)
{
	std::cout << "Demo " << n << ": apply homography to lines and segments\n Hit [lm] to change angle, "
		<< "and select points of blue segment with mouse\n";

	cv::setMouseCallback( g_wndname, mouse_CB_6 );

	double angle_delta = 5.; // degrees

	action_6();
	cv::imshow( g_wndname, g_img );

	char key=0;
	while( key != 32 ) // SPC
	{
		bool d = false;
		switch( key = cv::waitKey(0) )
		{
			case 'm': g_data.angle += angle_delta; d = true; break;
			case 'l': g_data.angle -= angle_delta; d = true; break;

			case 27: std::exit(0);
			default: break;
		}
		if( d )
		{
			action_6();
			cv::imshow( g_wndname, g_img );
		}
	}
}
//------------------------------------------------------------------
void action_7()
{
	clearImage();

	for( int i=0; i<4; i++ )
	{
		Segment s1( g_data.vpt[i], g_data.vpt[i==3?0:i+1] );
		Segment s2( g_data.vpt2[i], g_data.vpt2[i==3?0:i+1] );
		s1.draw( g_img, CvDrawParams().setColor( 0,0,250) );
		s2.draw( g_img, CvDrawParams().setColor( 250,0,0) );

		Segment( g_data.vpt[i], g_data.vpt2[i] ).draw( g_img );
	}
//	std::vector<Point2d> p_in(4);

	Homogr H;
	H.buildFromPoints( g_data.vpt, g_data.vpt2, 1 );
	std::cout << H << '\n';
	cv::imshow( g_wndname, g_img );

}

void action_7M()
{
	g_data.moveSelectedPoint();

}

/// Mouse callback for demo_6
void mouse_CB_7( int event, int x, int y, int /* flags */, void* /*param*/ )
{
	checkSelected( event, x, y, action_7, action_7M );
}

/// Demo of computing a homography from two sets of 4 points
void demo_7( int n )
{
	std::cout << "Demo " << n << ": compute homography from two sets of 4 points\n";
	auto pa1 = Point2d(100,100);
	auto pa2 = Point2d(400,300);
	g_data.vpt = getRectPts( pa1, pa2 );

	auto pb1 = Point2d(80,150);
	auto pb2 = Point2d(450,350);
	g_data.vpt2 = getRectPts( pb1, pb2 );

//	g_data.vpt.resize(8);
//	std::copy( vptrect_a.begin(), vptrect_a.end(), g_data.vpt.begin() );
//	std::copy( vptrect_b.begin(), vptrect_b.end(), g_data.vpt.begin()+4 );
//	for( auto pt: g_data.vpt )
//		std::cout << "-pt=" << pt << '\n';

	action_7();
	cv::setMouseCallback( g_wndname, mouse_CB_7 );
	if( 27 == cv::waitKey(0) )
		std::exit(0);
}
//------------------------------------------------------------------
int main( int argc, const char** argv )
{
	std::cout << "homog2d graphical demo using Opencv"
		<< "\n - homog version: " << HOMOG2D_VERSION
		<< "\n - build with OpenCV version: " << CV_VERSION << '\n';
	std::vector<std::function<void(int)>> v_demo{
		demo_7,
		demo_1,
		demo_2,
		demo_3,
		demo_4,
		demo_5,
		demo_6
	};
	cv::namedWindow( g_wndname );
	g_img.create( g_height, g_width, CV_8UC3 );

	if( argc > 1 )
	{
		int d = std::atoi( argv[1] );
		assert( d>0 && d<=(int)v_demo.size() );
		std:: cout << " - calling demo " << d << "\n";
		v_demo[d-1](d);
		return 0;
	}
	std::cout << " - to switch to next demo, hit [SPC]\n - to exit, hit [ESC]\n";
	for( size_t i=0; i<v_demo.size(); i++ )
	{
		cv::destroyAllWindows(); // to disable the mouse callback
		cv::namedWindow( g_wndname );
		v_demo[i](i+1);
	}
	std::cout << "Demo end\n";
}

