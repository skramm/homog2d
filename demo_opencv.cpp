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
\brief demo of interfacing with Opencv (PRELIMINAR)
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
int g_selected = -1;
homog2d::Point2d g_pt[4];
homog2d::Point2d g_pt_mouse;

void drawLines()
{
	g_img = cv::Scalar(255,255,255);
	for( int i=0; i<4; i++ )
	{
		if( g_selected == i )
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

void draw1()
{
	drawLines();

	Line2d l( g_pt[0], g_pt[2] );
	Line2d la_V( l );
	la_V.addOffset( OD_Vert, 25);
	la_V.drawCvMat( g_img, CvDrawParams().setColor(250,0,250) );

	l.addOffset( OD_Horiz, 25);
	l.drawCvMat( g_img, CvDrawParams().setColor(250,250,0) );

	cv::imshow( g_wndname, g_img );
}

/// Mouse callback
void mouse_CB( int event, int x, int y, int flags, void* param )
{
	static int c;
	std::cout << "count:" << c++ << '\n';
	draw1();
	g_pt_mouse.set( x, y );
	g_selected = -1;
	Point2d pt( g_pt_mouse );
	for( int i=0; i<4; i++ )
		if( pt.distToPoint( g_pt[i]) < 10 )
		{
			g_pt[i] = pt;
			g_selected = i;
			break;
		}
}

int demo1()
{
	std::cout << "Demo 1 !\n";
	cv::setMouseCallback( g_wndname, mouse_CB );

	int n=5;
	g_pt[0].set( g_width/2,       g_height/n );
	g_pt[1].set( g_width/2,       g_height*(n-1)/n );
	g_pt[2].set( g_width/n,       g_height/2 );
	g_pt[3].set( g_width*(n-1)/n, g_height/2 );

	g_img.create( g_height, g_width, CV_8UC3 );
	draw1();
	char key = cv::waitKey(0);
}

void applyH( const Homogr& H )
{
	for( int i=0; i<4; i++ )
		g_pt[i] = H * g_pt[i];
}

void draw2()
{
	drawLines();
	cv::imshow( g_wndname, g_img );
}

void demo2()
{
	std::cout << "Demo 2 !\n";
//	cv::setMouseCallback( g_wndname, mouse_CB );
//	cv::namedWindow( g_wndname );
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
	g_pt[0].set( 50, 50);
	g_pt[1].set( 150, 150);
	g_pt[2].set( 150, 50);
	g_pt[3].set( 50, 150);
	draw2();
	std::cout << "Hit a key: scale:[op], angle:[lm], translation:[gh,yb]\n";
	while( key != 27 ) // ESC
	{
		bool change = true;
		switch( key = cv::waitKey(0) )
		{
			case 'r': // reset
				angle = 0.;
				scale = 1.;
				tx = 0;
				ty = 0;
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
			std::cout << "angle=" << angle << " scale=" << scale << "\n";
			H.setRotation( angle*K );
			H.addTranslation( tx, ty );
			H.addScale( scale );
			std::cout << H << '\n';
			for( int i=0; i<4; i++ )
				g_pt[i] = H * g_pt[i];
			draw2();
		}
	}
}

int main()
{
	cv::namedWindow( g_wndname );
	demo1();

	cv::destroyAllWindows(); // to disable the mouse callback
	cv::namedWindow( g_wndname );
	demo2();
}

