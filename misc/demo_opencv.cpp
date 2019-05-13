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

using namespace homog2d;

std::string g_wndname = "homog2d demo";
cv::Mat g_img;
int g_width = 600;
int g_height = 500;
homog2d::Point2d g_pt[4];
homog2d::Point2d g_pt_mouse;

std::array<Line2d,3> g_li;

int g_radius = 50; // for demo4()

void drawLines( int selected )
{
	g_img = cv::Scalar(255,255,255);
	for( int i=0; i<4; i++ )
	{
		if( selected == i )
			g_pt[i].drawCvMat( g_img, CvDrawParams().setColor( 250, 0, 150).setPointStyle( (PointStyle)i) );
		else
			g_pt[i].drawCvMat( g_img, CvDrawParams().setPointStyle((PointStyle)i) );
	}

	Line2d lA( g_pt[0], g_pt[2] );
	Line2d lB( g_pt[0], g_pt[3] );
	Line2d lC( g_pt[1], g_pt[2] );
	Line2d lD( g_pt[1], g_pt[3] );

	lA.drawCvMat( g_img, CvDrawParams().setColor(   0,  50, 150) );
	lB.drawCvMat( g_img, CvDrawParams().setColor( 150,  50,   0) );
	lC.drawCvMat( g_img, CvDrawParams().setColor(  50, 150,   0) );
	lD.drawCvMat( g_img, CvDrawParams().setColor( 150,   0,  50) );
}

void draw1( int selected )
{
	drawLines( selected );

	Line2d l( g_pt[0], g_pt[2] );
	Line2d la_V( l );
	la_V.addOffset( OD_Vert, 25);
	la_V.drawCvMat( g_img, CvDrawParams().setColor(250,0,250) );

	l.addOffset( OD_Horiz, 25);
	l.drawCvMat( g_img, CvDrawParams().setColor(250,250,0) );
}

/// Mouse callback for demo1
void mouse_CB_1( int event, int x, int y, int /* flags */, void* /*param*/ )
{
	static int selected=-1;
	draw1( selected );
	g_pt_mouse.set( x, y );
	switch( event )
	{
		case CV_EVENT_LBUTTONUP:
			selected = -1;
		break;

		case CV_EVENT_LBUTTONDOWN:
			for( int i=0; i<4; i++ )
				if( g_pt_mouse.distToPoint( g_pt[i]) < 10 )
					selected = i;
		break;

		case CV_EVENT_MOUSEMOVE:
		{
			if( selected != -1 )
				g_pt[selected] = g_pt_mouse;
			Line2d l_mouse = g_pt_mouse * Point2d();
			Line2d l_mouse2= l_mouse.getOrthogonalLine( GC_X, 100 );
			l_mouse.drawCvMat( g_img );
			l_mouse2.drawCvMat( g_img );
		}
		break;

		default: break;
	}
	cv::imshow( g_wndname, g_img );
}

void demo1()
{
	std::cout << "Demo 1: click on points and move them\n";
	cv::setMouseCallback( g_wndname, mouse_CB_1 );

	int n=5;
	g_pt[0].set( g_width/2,       g_height/n );
	g_pt[1].set( g_width/2,       g_height*(n-1)/n );
	g_pt[2].set( g_width/n,       g_height/2 );
	g_pt[3].set( g_width*(n-1)/n, g_height/2 );

	g_img.create( g_height, g_width, CV_8UC3 );
	draw1(-1);
	cv::imshow( g_wndname, g_img );

	cv::waitKey(0);
}

void draw2()
{
	drawLines(-1);
	cv::imshow( g_wndname, g_img );
}
void initPts()
{
	int a=50;
	int b=150;
	g_pt[0].set( a, a );
	g_pt[1].set( b, b );
	g_pt[2].set( b, a );
	g_pt[3].set( a, b );
}

void demo2()
{
	std::cout << "Demo 2: Hit a key: scale:[op], angle:[lm], translation:[gh,yb], reset: r, quit:ESC\n";
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
	while( key != 27 ) // ESC
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
			default: change = false; break;
		}
		if( change )
		{
			H.clear();
			H.addRotation( angle*K ).addTranslation( tx, ty ).addScale( scale );
			initPts();
			H.applyTo(g_pt);
			draw2();
		}
	}
}

void demo3()
{
	std::cout << "Demo 3:\n";

	Line2d li;

	Line2d lV;
	lV.addOffset( OD_Horiz, 100.);
	Line2d lH(1,0);
	lH.addOffset( OD_Vert, 100.);

	g_img = cv::Scalar(255,255,255);
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

void drawLine_4()
{
	g_img = cv::Scalar(255,255,255);
	for( int i=0; i<g_li.size(); i++ )
		g_li[i].drawCvMat( g_img );
}

/// Mouse callback for demo4
void mouse_CB_4( int event, int x, int y, int /* flags */, void* /*param*/ )
{
	drawLine_4();

	g_pt_mouse.set( x, y );
	cv::circle( g_img, g_pt_mouse.getCvPtd(), g_radius, cv::Scalar(50,100,150) );
	g_pt_mouse.drawCvMat( g_img, CvDrawParams().setColor(250,50,20) );

	for( int i=0; i<g_li.size(); i++ )
	{
		RectIntersect ri = g_li[i].intersectsCircle( g_pt_mouse, g_radius );
		if( ri() )
		{
			auto inter = ri.get();
			std::cout << "INTER, pt=" << inter.first << ", " << inter.second << "\n";
			inter.first.drawCvMat( g_img );
			inter.second.drawCvMat( g_img );

			ri._tempB.drawCvMat( g_img, CvDrawParams().setColor(20,50,250) );
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
	while( key != 27 ) // ESC
	{
		bool change = true;
		switch( key = cv::waitKey(0) )
		{
			case 'r': // reset
				g_radius = 50;
			break;
			case 'l':
				g_radius += 10;
			break;
			case 'm':
				g_radius -= 10;
			break;
			default: break;
		}
		std::cout << "radius=" << g_radius << '\n';

		cv::imshow( g_wndname, g_img );
	}
}

int main()
{
	std::cout << "Installed OpenCV version : " << CV_VERSION << '\n';
	cv::namedWindow( g_wndname );
	demo1();

	cv::destroyAllWindows(); // to disable the mouse callback
	cv::namedWindow( g_wndname );
	demo2();

	demo3();
	demo4();
	std::cout << "Demo end\n";
}

