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
	Segment s1,s2;
	int selected = -1;
	Point2d pt_mouse;
	Data()
	{
		vpt.resize(4);
	}
	void setMousePos(int x, int y)
	{
		pt_mouse.set(x,y);
	}
	int nbPts() const
	{
		return (int)vpt.size();
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
	clearImage();
	for( int i=0; i<g_data.nbPts(); i++ )
	{
		if( selected == i )
			g_data.vpt[i].drawCvMat( g_img, CvDrawParams().setColor( 250, 0, 150).setPointStyle( (PointStyle)i) );
		else
			g_data.vpt[i].drawCvMat( g_img, CvDrawParams().setPointStyle((PointStyle)i) );
	}

	Line2d lA( g_data.vpt[0], g_data.vpt[2] );
	Line2d lB( g_data.vpt[0], g_data.vpt[3] );
	Line2d lC( g_data.vpt[1], g_data.vpt[2] );
	Line2d lD( g_data.vpt[1], g_data.vpt[3] );

	lA.drawCvMat( g_img, CvDrawParams().setColor(   0,  50, 150) );
	lB.drawCvMat( g_img, CvDrawParams().setColor( 150,  50,   0) );
	lC.drawCvMat( g_img, CvDrawParams().setColor(  50, 150,   0) );
	lD.drawCvMat( g_img, CvDrawParams().setColor( 150,   0,  50) );
}


/// Called by mouse callback functions, checks if one of the points is selected.
/**
- If so, that point gets moved by the mouse, and the function \c action is called
*/
void
checkSelected( int event, int x, int y, std::function<void()> action, std::function<void()> actionM )
{
	clearImage();
	static int selected=-1;
	g_data.setMousePos(x,y);
	action();
	switch( event )
	{
		case CV_EVENT_LBUTTONUP:
			selected = -1;
		break;

		case CV_EVENT_LBUTTONDOWN:
			for( int i=0; i<g_data.nbPts(); i++ )
				if( g_data.pt_mouse.distTo( g_data.vpt[i]) < 10 )  // if mouse is less than 10 pixel away
					selected = i;
		break;

		case CV_EVENT_MOUSEMOVE:
		{
			if( selected != -1 )
				g_data.vpt[selected] = g_data.pt_mouse;
			actionM();
		}
		break;

		default: break;
	}
	cv::imshow( g_wndname, g_img );
}

void action_1()
{
	drawLines( g_data.selected );

	Line2d l( g_data.vpt[0], g_data.vpt[2] );
	Line2d la_V( l );
	la_V.addOffset( OD_Vert, 60 );
	la_V.drawCvMat( g_img, CvDrawParams().setColor(250,0,250) );

	l.addOffset( OD_Horiz, 60 );
	l.drawCvMat( g_img, CvDrawParams().setColor(250,250,0) );
}

void action_1M()
{
	Line2d l_mouse = g_data.pt_mouse * Point2d();
	Line2d l_mouse2= l_mouse.getOrthogonalLine( GC_X, 100 );
	l_mouse.drawCvMat( g_img );
	l_mouse2.drawCvMat( g_img );
}

/// Mouse callback for demo1
void mouse_CB_1( int event, int x, int y, int /* flags */, void* /*param*/ )
{
	checkSelected( event, x, y, action_1, action_1M );
}

void demo1()
{
	std::cout << "Demo 1: click on points and move them\n";
	cv::setMouseCallback( g_wndname, mouse_CB_1 );

	int n=5;
	g_data.vpt[0].set( g_width/2,       g_height/n );
	g_data.vpt[1].set( g_width/2,       g_height*(n-1)/n );
	g_data.vpt[2].set( g_width/n,       g_height/2 );
	g_data.vpt[3].set( g_width*(n-1)/n, g_height/2 );

	g_img.create( g_height, g_width, CV_8UC3 );
	action_1();
	cv::imshow( g_wndname, g_img );

	auto k = cv::waitKey(0);
	if( k == 27 )
		std::exit(0);
}
//------------------------------------------------------------------
void draw2()
{
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

void demo2()
{
	std::cout << "Demo 2: Hit a key: scale:[op], angle:[lm], translation:[gh,yb], reset: r\n";
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
	draw2();
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
			draw2();
		}
	}
}
//------------------------------------------------------------------
void demo3()
{
	std::cout << "Demo 3:\n";

	Line2d li;

	Line2d lV;
	lV.addOffset( OD_Horiz, 100.);
	Line2d lH(1,0);
	lH.addOffset( OD_Vert, 100.);

	clearImage();
	lV.drawCvMat( g_img, CvDrawParams().setColor( 250, 50,  50).setThickness(2) );
	lH.drawCvMat( g_img, CvDrawParams().setColor(  50, 50, 250).setThickness(2) );

	CvDrawParams dp;
	dp.setColor( 0, 250, 0 );
	dp.setDefault();

	li = lV;
	li.addOffset( OD_Horiz, 50 );
	li.drawCvMat( g_img ); // use default draw parameters
	li = lV;
	li.addOffset( OD_Horiz, -50 );
	li.drawCvMat( g_img, CvDrawParams().setColor( 100,250,0) );

	CvDrawParams::resetDefault();

	li = lH;
	li.addOffset( OD_Vert, 50 );
	li.drawCvMat( g_img ); //, CvDrawParams().setColor( 100,0,250) );
	li = lH;
	li.addOffset( OD_Vert, -50 );
	li.drawCvMat( g_img ); //, CvDrawParams().setColor( 100,0,250) );

	cv::imshow( g_wndname, g_img );
	cv::waitKey(0);
}
//------------------------------------------------------------------
void drawLine_4()
{
	clearImage();
	for( size_t i=0; i<g_li.size(); i++ )
		g_li[i].drawCvMat( g_img );
}

/// Mouse callback for demo4
void mouse_CB_4( int /* event */, int x, int y, int /* flags */, void* /*param*/ )
{
	drawLine_4();

	g_data.pt_mouse.set( x, y );
	cv::circle( g_img, g_data.pt_mouse.getCvPtd(), g_radius, cv::Scalar(50,100,150) );
	g_data.pt_mouse.drawCvMat( g_img, CvDrawParams().setColor(250,50,20) );

	for( size_t i=0; i<g_li.size(); i++ )
	{
		auto ri = g_li[i].intersectsCircle( g_data.pt_mouse, g_radius );
		if( ri() )
		{
			auto inter = ri.get();
			inter.first.drawCvMat( g_img,  CvDrawParams().setColor(250, 0, 0) );
			inter.second.drawCvMat( g_img, CvDrawParams().setColor(250, 0, 0) );
		}
	}
	cv::imshow( g_wndname, g_img );
}

void demo4()
{
	std::cout << "Demo 4: move circle over line, hit [lm] to change circle radius\n";

	g_li[0] = Point2d() * Point2d(200,100);
	g_li[1] = Point2d(200,0) * Point2d(200,200);
	g_li[2] = Point2d(0,200) * Point2d(200,200);

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
	g_data.s1.set( g_data.vpt[0], g_data.vpt[1] );
	g_data.s2.set( g_data.vpt[2], g_data.vpt[3] );

	g_data.s1.drawCvMat( g_img, CvDrawParams().setColor( 0,0,250).setThickness(2) );
	g_data.s2.drawCvMat( g_img, CvDrawParams().setColor( 250,0,0).setThickness(2) );
	g_data.s1.getLine().drawCvMat( g_img, CvDrawParams().setColor( 100,100,100) );
	g_data.s2.getLine().drawCvMat( g_img, CvDrawParams().setColor( 100,100,100) );
	draw( g_img, g_data.vpt );
}

void action_5M()
{
	auto inters = g_data.s1.intersects( g_data.s2 );
	if( inters() )
	{
		auto pti = inters.get();
		pti.drawCvMat( g_img );
		Line2d l1 = g_data.s1.getLine().getOrthogonalLine( GC_X,  pti.getX() );
		l1.drawCvMat( g_img, CvDrawParams().setColor( 0,0,100) );

		Line2d l2 = g_data.s2.getLine().getOrthogonalLine( GC_X,  pti.getX() );
		l2.drawCvMat( g_img, CvDrawParams().setColor( 100,0,0) );
	}
}

/// Mouse callback for demo5
void mouse_CB_5( int event, int x, int y, int /* flags */, void* /*param*/ )
{
	checkSelected( event, x, y, action_5, action_5M );
}


void demo5()
{
	std::cout << "Demo 5: Segments\n";

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
int main( int argc, const char** argv )
{
	if( argc > 1 )
	{
		int d = std::atoi( argv[1] );
		std:: cout << " - calling demo " << d << "\n";
	}
	std::cout << " - to switch to next demo, hit [SPC]\n - to exit, hit [ESC]\n"
		<< " - installed OpenCV version : " << CV_VERSION << '\n';
	cv::namedWindow( g_wndname );
	demo1();

	cv::destroyAllWindows(); // to disable the mouse callback
	cv::namedWindow( g_wndname );
	demo2();

	demo3();
	demo4();
	demo5();
	std::cout << "Demo end\n";
}

