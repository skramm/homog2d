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
\file demo_opencv.cpp
\brief demo of interfacing with Opencv
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_USE_EIGEN

#include "homog2d.hpp"

// additional Opencv header, needed for GUI stuff
#include "opencv2/highgui.hpp"

#include <functional>

using namespace homog2d;

/// General data struct used in demos
struct Data
{
	cv::Mat img;
	int width = 700;
	int height = 500;

	std::string win1;   ///< Window name (appears in title)
	int selected = -1;  ///< Mouse selected point
	Point2d pt_mouse;   ///< Mouse coordinates
	std::vector<Point2d> vpt; ///< some points used in demo

	Data( std::string wname ): win1(wname)
	{
		cv::destroyAllWindows();
		cv::namedWindow( win1 );
		img.create( height, width, CV_8UC3 );
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
	void setMouseCallback( cv::MouseCallback mouse_CB )
	{
		cv::setMouseCallback( win1, mouse_CB, this );
	}
	int nbPts() const
	{
		return (int)vpt.size();
	}
	void clearImage()
	{
		img = cv::Scalar(255,255,255);
	}
	void showImage()
	{
		cv::imshow( win1, img );
	}

	void drawLines()
	{
		for( int i=0; i<nbPts(); i++ )
		{
			if( selected == i )
				vpt[i].draw( img, CvDrawParams().setColor( 250, 0, 150).setPointStyle( (PointStyle)i).selectPoint() );
			else
				vpt[i].draw( img, CvDrawParams().setPointStyle((PointStyle)i) );
		}
		Line2d lA( vpt[0], vpt[2] );
		Line2d lB( vpt[0], vpt[3] );
		Line2d lC( vpt[1], vpt[2] );
		Line2d lD( vpt[1], vpt[3] );

		lA.draw( img, CvDrawParams().setColor(   0,  50, 150) );
		lB.draw( img, CvDrawParams().setColor( 150,  50,   0) );
		lC.draw( img, CvDrawParams().setColor(  50, 150,   0) );
		lD.draw( img, CvDrawParams().setColor( 150,   0,  50) );
	}
};


/// Called by mouse callback functions, checks if one of the points is selected.
/**
- If so, that point gets moved by the mouse, and the function \c actionM is called
*/
void
checkSelected( int event, int x, int y, std::function<void(void*)> action, std::function<void(void*)> actionM, void* param )
{
	auto& data = *reinterpret_cast<Data*>(param);

	data.setMousePos(x,y);

	data.clearImage();
	action( param );

	switch( event )
	{
		case CV_EVENT_LBUTTONUP:
			data.selected = -1;
		break;

		case CV_EVENT_LBUTTONDOWN:
			data.selected = -1;
			for( int i=0; i<data.nbPts(); i++ )
				if( data.pt_mouse.distTo( data.vpt[i]) < 10 )  // if mouse is less than 10 pixel away
					data.selected = i;
			if( data.selected != -1 )
				data.vpt[data.selected].draw( data.img, CvDrawParams().selectPoint() );

		break;

		case CV_EVENT_MOUSEMOVE:
			if( data.selected != -1 )
			{
				data.vpt[data.selected] = data.pt_mouse;
				data.vpt[data.selected].draw( data.img, CvDrawParams().selectPoint() );
			}
			actionM( param );
		break;

		default: break;
	}
	data.showImage();
}

void action_1(  void* param );
void action_1M( void* param );
void action_C(  void* param );
void action_CM( void* param );
void action_5(  void* param );
void action_5M( void* param );
void action_6(  void* param );
void action_6M( void* param );
void action_H(  void* param );
void action_HM( void* param );


/// Mouse callback for demo_H
void mouse_CB_H( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_H, action_HM, param );
}

/// Mouse callback for demo_1
void mouse_CB_1( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_1, action_1M, param );
}

/// Mouse callback for demo_C
void mouse_CB_C( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_C, action_CM, param );
}
/// Mouse callback for demo5
void mouse_CB_5( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_5, action_5M, param );
}

/// Mouse callback for demo_6
void mouse_CB_6( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_6, action_6M, param );
}

//------------------------------------------------------------------
void action_1( void* param )
{
	auto& data = *reinterpret_cast<Data*>(param);
	data.drawLines();

	Line2d l( data.vpt[0], data.vpt[2] );
	Line2d la_V( l );
	la_V.addOffset( LineOffset::vert, 60 );
	la_V.draw( data.img, CvDrawParams().setColor(250,0,250) );

	l.addOffset( LineOffset::horiz, 60 );
	l.draw( data.img, CvDrawParams().setColor(250,250,0) );
}

void action_1M( void* param )
{
	auto& data = *reinterpret_cast<Data*>(param);

	Line2d l_mouse  = data.pt_mouse * Point2d();
	Line2d l_mouse2 = l_mouse.getOrthogonalLine( GivenCoord::X, 100 );
	l_mouse.draw( data.img );
	l_mouse2.draw( data.img );
}


void demo_1( int nd )
{
	Data data("lines");
	std::cout << "Demo " << nd << ": click on points and move them\n";

	data.setMouseCallback( mouse_CB_1 );

	int n=5;
	data.vpt[0].set( data.width/2,       data.height/n );
	data.vpt[1].set( data.width/2,       data.height*(n-1)/n );
	data.vpt[2].set( data.width/n,       data.height/2 );
	data.vpt[3].set( data.width*(n-1)/n, data.height/2 );

	data.clearImage();
	action_1( &data );
	data.showImage();

	auto k = cv::waitKey(0);
	if( k == 27 )
		std::exit(0);
}
//------------------------------------------------------------------
struct Param_B: public Data
{
	Param_B(std::string wname): Data(wname)
	{}
	void initPts()
	{
		int a=50;
		int b=150;
		vpt[0].set( a, a );
		vpt[1].set( b, b );
		vpt[2].set( b, a );
		vpt[3].set( a, b );
	}
};

/// Build H from R,T,S (no mouse)
void demo_B( int n )
{
	Param_B data( "build_H" );
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

	data.clearImage();
	data.initPts();
	data.drawLines();
	data.showImage();

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
			data.clearImage();
			H.init();
			H.addRotation( angle*K ).addTranslation( tx, ty ).addScale( scale );
			data.initPts();
			H.applyTo(data.vpt);
			data.drawLines();
			data.showImage();
		}
	}
}
//------------------------------------------------------------------
#if 0

void demo_3( int n )
{
	std::cout << "Demo " << n << ": offset and drawing parameters demo (check code)\n";

	Line2d li;

	Line2d lV;
	lV.addOffset( LineOffset::horiz, 100.);
	Line2d lH(1,0);
	lH.addOffset( LineOffset::vert, 100.);

	data.clearImage();
	lV.draw( data.img, CvDrawParams().setColor( 250, 50,  50).setThickness(2) );
	lH.draw( data.img, CvDrawParams().setColor(  50, 50, 250).setThickness(2) );

	CvDrawParams dp;
	dp.setColor( 0, 250, 0 );
	dp.setDefault();

	li = lV;
	li.addOffset( LineOffset::horiz, 50 );
	li.draw( data.img ); // use default draw parameters
	li = lV;
	li.addOffset( LineOffset::horiz, -50 );
	li.draw( data.img, CvDrawParams().setColor( 100,250,0) );

	CvDrawParams::resetDefault();

	li = lH;
	li.addOffset( LineOffset::vert, 50 );
	li.draw( data.img ); //, CvDrawParams().setColor( 100,0,250) );
	li = lH;
	li.addOffset( LineOffset::vert, -50 );
	li.draw( data.img ); //, CvDrawParams().setColor( 100,0,250) );

	data.showImage();
	cv::waitKey(0);
}
#endif
//------------------------------------------------------------------
struct Param_C: public Data
{
	Param_C(std::string wname): Data(wname)
	{}

	int radius = 80;
	std::array<Line2d,3> li;
	void drawLines()
	{
		for( size_t i=0; i<li.size(); i++ )
			li[i].draw( img );
	}
};

void action_C( void* /*param*/ )
{
}

void action_CM( void* param )
{
	auto& data = *reinterpret_cast<Param_C*>(param);

	data.clearImage();
	data.drawLines();

	cv::circle( data.img, data.pt_mouse.getCvPtd(), data.radius, cv::Scalar(50,100,150) );
	data.pt_mouse.draw( data.img, CvDrawParams().setColor(250,50,20) );

	for( size_t i=0; i<data.li.size(); i++ )
	{
		auto ri = data.li[i].intersectsCircle( data.pt_mouse, data.radius );
		if( ri() )
		{
			auto inter = ri.get();
			inter.first.draw( data.img,  CvDrawParams().setColor(250, 0, 0) );
			inter.second.draw( data.img, CvDrawParams().setColor(250, 0, 0) );
		}
	}
	data.showImage();
}


void demo_C( int n )
{
	std::cout << "Demo " << n << ": move circle over line, hit [lm] to change circle radius\n";
	Param_C data("circle");

	data.li[0] = Point2d() * Point2d(200,100);
	data.li[1] = Point2d(200,0) * Point2d(200,200);
	data.li[2] = Point2d(0,200) * Point2d(200,200);

	data.clearImage();
	data.drawLines();
	data.showImage();

	data.setMouseCallback( mouse_CB_C );

	char key=0;
	while( key != 32 ) // SPC
	{
		switch( key = cv::waitKey(0) )
		{
			case 'r': // reset
				data.radius = 80;
			break;
			case 'l':
				data.radius += 10;
			break;
			case 'm':
				data.radius -= 10;
			break;
			case 27: std::exit(0);
			default: break;
		}
		std::cout << "radius=" << data.radius << '\n';
		data.showImage();
	}
}

//------------------------------------------------------------------
struct Param_5: Data
{
	Param_5(std::string wname): Data(wname)
	{}

	Segment seg1,seg2;
};

void action_5( void* param )
{
	auto& data = *reinterpret_cast<Param_5*>(param);

	data.clearImage();
	data.seg1.set( data.vpt[0], data.vpt[1] );
	data.seg2.set( data.vpt[2], data.vpt[3] );

	data.seg1.draw( data.img, CvDrawParams().setColor( 0,0,250).setThickness(2) );
	data.seg2.draw( data.img, CvDrawParams().setColor( 250,0,0).setThickness(2) );
	data.seg1.getLine().draw( data.img, CvDrawParams().setColor( 100,100,100) );
	data.seg2.getLine().draw( data.img, CvDrawParams().setColor( 100,100,100) );
	draw( data.img, data.vpt );
	data.showImage();
}

void action_5M( void* param )
{
	auto& data = *reinterpret_cast<Param_5*>(param);

	if( data.selected != -1 )
		data.vpt[data.selected].draw( data.img, CvDrawParams().selectPoint() );
	auto inters = data.seg1.intersects( data.seg2 );
	if( inters() ) //&& g_data.selected != -1 )
	{
		auto pti = inters.get();
		pti.draw( data.img );
		Line2d l1 = data.seg1.getLine().getOrthogonalLine( GivenCoord::X, pti.getX() );
		l1.draw( data.img, CvDrawParams().setColor( 0,0,100) );

		Line2d l2 = data.seg2.getLine().getOrthogonalLine( GivenCoord::X, pti.getX() );
		l2.draw( data.img, CvDrawParams().setColor( 100,0,0) );
	}
	data.showImage();
}



void demo_5( int n )
{
	Param_5 data( "segment_intersection" );
	std::cout << "Demo " << n << ": intersection of segments\n Select a point and move it around. "
		<< "When they intersect, you get the orthogonal lines of the two segments, at the intersection point.\n";

	data.vpt[0] = Point2d(100,200);
	data.vpt[1] = Point2d(200,300);
	data.vpt[2] = Point2d(150,50);
	data.vpt[3] = Point2d(300,250);

	action_5( &data );
	data.setMouseCallback( mouse_CB_5 );

	if( 27 == cv::waitKey(0) )
		std::exit(0);
}

//------------------------------------------------------------------
struct Param_6 : Data
{
	Param_6(std::string wname): Data(wname)
	{}

	float angle = 20;
};

void action_6( void* param )
{
	auto& data = *reinterpret_cast<Param_6*>(param);

	data.clearImage();
	double K = M_PI / 180.;

	Homogr H( data.angle * K );
	H.transpose();
	Line2d l1( data.vpt[0], data.vpt[1] );
	Line2d l2 = H*l1;

	l1.draw( data.img, CvDrawParams().setColor( 250,0,0) );
	l2.draw( data.img, CvDrawParams().setColor( 0,250,0) );

	Segment s1( data.vpt[2], data.vpt[3] );
	Segment s2 = H*s1;
	s1.draw( data.img, CvDrawParams().setColor( 0,0,250) );
	s2.draw( data.img, CvDrawParams().setColor( 250,250,0) );
	s1.get().first.draw( data.img, CvDrawParams().selectPoint() );
	s1.get().second.draw( data.img, CvDrawParams().selectPoint() );
}

void action_6M( void* /*param*/ )
{
}

void demo_6(int n)
{
	std::cout << "Demo " << n << ": apply homography to lines and segments\n Hit [lm] to change angle, "
		<< "and select points of blue segment with mouse\n";
	Param_6 data( "homography_lines_seg" );
	data.setMouseCallback( mouse_CB_6 );
	double angle_delta = 5.; // degrees

	action_6( &data );
	data.showImage();

	char key=0;
	while( key != 32 ) // SPC
	{
		bool d = false;
		switch( key = cv::waitKey(0) )
		{
			case 'm': data.angle += angle_delta; d = true; break;
			case 'l': data.angle -= angle_delta; d = true; break;

			case 27: std::exit(0);
			default: break;
		}
		if( d )
		{
			action_6( &data );
			data.showImage();
		}
	}
}
//------------------------------------------------------------------
/// This one has two windows
struct Param_H: public Data
{
	int hmethod = 0;
	cv::Mat img2;
	std::string win2 = "Computed_projection";

	Param_H(std::string wname): Data(wname)
	{
		cv::namedWindow( win2 );
		cv::moveWindow( win2, width, 50 );
		img2.create( height, width, CV_8UC3 );
	}
	void showImage()
	{
		cv::imshow( win1, img );
		cv::imshow( win2, img2 );
	}
	void clearImage()
	{
		img = cv::Scalar(255,255,255);
		img2 = cv::Scalar(255,255,255);
	}
};


void action_H( void* param )
{
	auto& data = *reinterpret_cast<Param_H*>(param);

	data.clearImage();

	std::vector<Point2d> v1(4);
	std::vector<Point2d> v2(4);
	std::copy( data.vpt.begin(),   data.vpt.begin()+4, v1.begin() );
	std::copy( data.vpt.begin()+4, data.vpt.end(),     v2.begin() );
	for( int i=0; i<4; i++ )
	{
		Segment s1( v1[i], v1[i==3?0:i+1] );
		Segment s2( v2[i], v2[i==3?0:i+1] );
		s1.draw( data.img, CvDrawParams().setColor( 0,0,250) );
		s2.draw( data.img, CvDrawParams().setColor( 250,0,0) );

		s1.draw( data.img2, CvDrawParams().setColor( 0,0,250) );

		Segment( v1[i], v2[i] ).draw( data.img );

		v1[i].draw( data.img );
		v2[i].draw( data.img );
	}

	cv::putText(  data.img, "source points", cv::Point2i( v1[0].getX(), v1[0].getY() ), 0, 0.8, cv::Scalar( 250,0,0 ), 2 );
	cv::putText(  data.img, "dest points",   cv::Point2i( v2[0].getX(), v2[0].getY() ), 0, 0.8, cv::Scalar( 0,0,250 ), 2 );

	Homogr H;
	H.buildFrom4Points( v1, v2, data.hmethod );
	std::cout << "Computed Homography:\n" << H << '\n';

	std::vector<Point2d> vpt3;
	for( int i=0; i<4; i++ )
		vpt3.push_back( H * data.vpt[i] );

	for( int i=0; i<4; i++ )
	{
		Segment s1( vpt3[i], vpt3[i==3?0:i+1] );
		s1.draw( data.img2, CvDrawParams().setColor( 0,250,0) );
	}
	data.showImage();
}

void action_HM( void* /*param*/ )
{
}

void demo_H_reset( Param_H& data )
{
	auto pa1 = Point2d(100,100);
	auto pa2 = Point2d(400,300);
	auto v1 = getRectPts( pa1, pa2 );

	auto pb1 = Point2d(80,150);
	auto pb2 = Point2d(450,350);
	auto v2 = getRectPts( pb1, pb2 );
	std::copy( v1.begin(), v1.end(), data.vpt.begin() );
	std::copy( v2.begin(), v2.end(), data.vpt.begin()+4 );
}

/// Demo of computing a homography from two sets of 4 points
void demo_H( int n )
{
	Param_H data( "compute_H" );
	data.vpt.resize(8);
	demo_H_reset(data);
	std::cout << "Demo " << n << ": compute homography from two sets of 4 points\n"
		<< " - usage: move points with mouse in left window, right window will show source rectangle and computed projected rectangle (green)\n"
		<< " - keys:\n  -a: switch backend computing library\n  -r: reset points\n";

	data.setMouseCallback( mouse_CB_H );

	action_H( &data );

	char key=0;
	while( key != 32 ) // SPC
	{
		bool d = false;
		switch( key = cv::waitKey(0) )
		{
			case 'A':
			case 'a':
				data.hmethod = data.hmethod?0:1;
				std::cout << "-used library: " << (data.hmethod?"Opencv":"Eigen") << '\n';
#if !defined(HOMOG2D_USE_EIGEN)
				if( data.hmethod == 1 )
				{
					std::cout << "Unable, build without Eigen support, see manual, switch to Opencv\n";
					data.hmethod = 0;
				}
#endif
				d = true;
			break;
			case 'R':
			case 'r':
				d = true;
				demo_H_reset(data);
			break;

			case 27: std::exit(0);
			default: break;
		}
		if( d )
		{
			action_H( &data );
			data.showImage();
		}
	}
}
//------------------------------------------------------------------
int main( int argc, const char** argv )
{
	std::cout << "homog2d graphical demo using Opencv"
		<< "\n - homog version: " << HOMOG2D_VERSION
		<< "\n - build with OpenCV version: " << CV_VERSION << '\n';

	std::vector<std::function<void(int)>> v_demo{
		demo_H,
		demo_1,
		demo_B,
//		demo_3,
		demo_C,
		demo_5,
		demo_6
	};

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
		v_demo[i](i+1);

	std::cout << "Demo end\n";
}

