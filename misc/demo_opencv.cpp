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



struct Data
{
	static cv::Mat img;
	static std::string wndname; // = "homog2d demo";
	static int width; // = 600;
	static int height; // = 500;

	int selected = -1;
	Point2d pt_mouse;
	std::vector<Point2d> vpt;

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
	void clearImage()
	{
		img = cv::Scalar(255,255,255);
	}
	void showImage()
	{
		cv::imshow( wndname, img );
	}

	void moveSelectedPoint()
	{
		if( selected != -1 )
			vpt.at(selected) = pt_mouse;
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



int         Data::width = 600;
int         Data::height = 500;
cv::Mat     Data::img;
std::string Data::wndname = "homog2d demo";


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
			for( int i=0; i<data.nbPts(); i++ )
				if( data.pt_mouse.distTo( data.vpt[i]) < 10 )  // if mouse is less than 10 pixel away
					data.selected = i;
		break;

		case CV_EVENT_MOUSEMOVE:
			if( data.selected != -1 )
				data.vpt[data.selected] = data.pt_mouse;
			actionM( param );
		break;

		default: break;
	}
	data.showImage();
}

struct Param_1: public Data
{
};

void action_1( void* param )
{
	auto& data = *reinterpret_cast<Param_1*>(param);
//	clearImage();
	data.drawLines();

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

	Line2d l( data.vpt[0], data.vpt[2] );
	Line2d la_V( l );
	la_V.addOffset( LineOffset::vert, 60 );
	la_V.draw( data.img, CvDrawParams().setColor(250,0,250) );

	l.addOffset( LineOffset::horiz, 60 );
	l.draw( data.img, CvDrawParams().setColor(250,250,0) );
}

void action_1M( void* param )
{
	auto& data = *reinterpret_cast<Param_1*>(param);

	Line2d l_mouse  = data.pt_mouse * Point2d();
	Line2d l_mouse2 = l_mouse.getOrthogonalLine( GivenCoord::X, 100 );
	l_mouse.draw( data.img );
	l_mouse2.draw( data.img );
}

/// Mouse callback for demo_1
void mouse_CB_1( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_1, action_1M, param );
}

void demo_1( int nd )
{
	Param_1 data;
	std::cout << "Demo " << nd << ": click on points and move them\n";
	cv::setMouseCallback( data.wndname, mouse_CB_1, &data );

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
struct Param_2: public Data
{
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

void demo_2( int n )
{
	Param_2 data;
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
struct Param_4: public Data
{
	int radius = 80;
	std::array<Line2d,3> li;
	void drawLines()
	{
		for( size_t i=0; i<li.size(); i++ )
			li[i].draw( img );
	}
};

void action_4( void* param )
{
}

void action_4M( void* param )
{
	auto& data = *reinterpret_cast<Param_4*>(param);

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

/// Mouse callback for demo_4
void mouse_CB_4( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_4, action_4M, param );
}

void demo_4( int n )
{
	std::cout << "Demo " << n << ": move circle over line, hit [lm] to change circle radius\n";
	Param_4 data;

	data.li[0] = Point2d() * Point2d(200,100);
	data.li[1] = Point2d(200,0) * Point2d(200,200);
	data.li[2] = Point2d(0,200) * Point2d(200,200);

	data.clearImage();
	data.drawLines();
	data.showImage();

	cv::setMouseCallback( data.wndname, mouse_CB_4, &data );

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

/// Mouse callback for demo5
void mouse_CB_5( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_5, action_5M, param );
}


void demo_5( int n )
{
	Param_5 data;
	std::cout << "Demo " << n << ": intersection of segments\n Select a point and move it around. "
		<< "When they intersect, you get the orthogonal lines of the two segments, at the intersection point.\n";

	data.vpt[0] = Point2d(100,200);
	data.vpt[1] = Point2d(200,300);
	data.vpt[2] = Point2d(150,50);
	data.vpt[3] = Point2d(300,250);

	action_5( &data );
	cv::setMouseCallback( data.wndname, mouse_CB_5, &data );
	if( 27 == cv::waitKey(0) )
		std::exit(0);
}

//------------------------------------------------------------------
struct Param_6 : Data
{
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

void action_6M( void* param )
{
	auto& data = *reinterpret_cast<Param_6*>(param);
	data.moveSelectedPoint();

}
/// Mouse callback for demo_6
void mouse_CB_6( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_6, action_6M, param );
}

void demo_6(int n)
{
	std::cout << "Demo " << n << ": apply homography to lines and segments\n Hit [lm] to change angle, "
		<< "and select points of blue segment with mouse\n";
	Param_6 data;
	cv::setMouseCallback( data.wndname, mouse_CB_6, &data );

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
struct Param_7: public Data
{
	int hmethod = 0;
	cv::Mat img2;

	Param_7()
	{
		cv::namedWindow( "win2" );
		img2.create( height, width, CV_8UC3 );
	}
	void showImage()
	{
		cv::imshow( wndname, img );
		cv::imshow( "win2",  img2 );
	}
	void clearImage()
	{
		img = cv::Scalar(255,255,255);
		img2 = cv::Scalar(255,255,255);
	}

};

void action_7( void* param )
{
	auto& data = *reinterpret_cast<Param_7*>(param);

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

	Homogr H;

	H.buildFromPoints( v1, v2, data.hmethod );
	std::cout << H << '\n';

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

void action_7M( void* param )
{
	auto par = reinterpret_cast<Param_7*>(param);
	par->moveSelectedPoint();
}

/// Mouse callback for demo_7
void mouse_CB_7( int event, int x, int y, int /* flags */, void* param )
{
	checkSelected( event, x, y, action_7, action_7M, param );
}

void demo_7_reset( Param_7& data )
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
void demo_7( int n )
{
	Param_7 data;
	data.vpt.resize(8);
	demo_7_reset(data);
	std::cout << "Demo " << n << ": compute homography from two sets of 4 points\n";

	cv::setMouseCallback( data.wndname, mouse_CB_7, &data );

	action_7( &data );

	char key=0;
	while( key != 32 ) // SPC
	{
		bool d = false;
		switch( key = cv::waitKey(0) )
		{
			case 'A':
			case 'a':
				data.hmethod = data.hmethod?0:1;
				d = true;
			break;
			case 'r':
				demo_7_reset(data);
			break;

			case 27: std::exit(0);
			default: break;
		}
		if( d )
		{
			action_7( &data );
			data.showImage();
		}
	}
}
//------------------------------------------------------------------
int main( int argc, const char** argv )
{
//	std::cout << "float: " << std::numeric_limits<float>::digits10 << "\n";
//	std::cout << "double: " << std::numeric_limits<double>::digits10 << "\n";
//	std::cout << "long double:"  << std::numeric_limits<long double>::digits10 << "\n";
#if 0
{
	float f = M_PI;
	double d = M_PI;
	long double l = M_PI;
	std::cout << "f=" << f << "\nd=" << d << "\nl=" << l << "\n";
	for( int i=6; i<24; i=i+2)
	{
		std::cout << std::setprecision(i) << "- precision:" << i
			<< ":\nf=" << f << "\nd=" << d << "\nl=" << l << "\n";
	}

	std::cout << "epsilon:\n-f=" << std::numeric_limits<float>::epsilon()
<< "\n-d=" << std::numeric_limits<double>::epsilon()
<< "\n-l=" << std::numeric_limits<long double>::epsilon()
<< "\n";


return 0;
}
#endif

	std::cout << "homog2d graphical demo using Opencv"
		<< "\n - homog version: " << HOMOG2D_VERSION
		<< "\n - build with OpenCV version: " << CV_VERSION << '\n';
	std::vector<std::function<void(int)>> v_demo{
		demo_7,
		demo_1,
		demo_2,
//		demo_3,
		demo_4,
		demo_5,
		demo_6
	};
	cv::namedWindow( Data::wndname );
	Data::img.create( Data::height, Data::width, CV_8UC3 );

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
		cv::namedWindow( Data::wndname );
		v_demo[i](i+1);
	}
	std::cout << "Demo end\n";
}

