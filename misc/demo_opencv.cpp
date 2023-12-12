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
\file demo_opencv.cpp
\brief demo of interfacing with Opencv.
Try it with <code>$ make demo</code>.

(requires OpenCv)

To run a specific demo, give its number as first argument, i.e.:

<code>$ BUILD/demo_opencv 4</code>.
*/

#define HOMOG2D_USE_OPENCV
#define HOMOG2D_ENABLE_RTP
//#define HOMOG2D_DEBUGMODE
#include "homog2d.hpp"

// additional Opencv header, needed for GUI stuff
#include "opencv2/highgui.hpp"

using namespace h2d;
//using namespace h2d::img;

// forward declaration
void myMouseCB( int event, int x, int y, int, void* param );

//------------------------------------------------------------------
/// General data struct used in demos, is inherited in each demo
struct Data
{
	friend void myMouseCB( int, int, int, int, void* );

	img::Image<cv::Mat> img;
	int _imWidth = 700;
	int _imHeight = 500;

	std::string win1;   ///< Window name (appears in title)
	int selected = -1;  ///< Mouse selected point
	Point2d pt_mouse;   ///< Mouse coordinates
	std::vector<Point2d> vpt; ///< some points used in demo
	bool leftClicAddPoint = false;
	int tline = 0;          ///< used to draw some lines of text inside window

private:
	std::function<void(void*)> _mouseCB = nullptr;

public:
	explicit Data( int demidx, std::string wname )
	{
		win1 = std::string("Demo ") + std::to_string(demidx) + ": " + wname;
		cv::destroyAllWindows();
		cv::namedWindow( win1 );
		img.setSize( _imWidth, _imHeight );
		img.clear(255);
		vpt.resize(4);
		pt_mouse.set(10,10); // just to avoid it being 0,0
		reset();
		tline = 0;
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
	void setMouseCB( std::function<void(void*)> cb )
	{
		_mouseCB = cb;
		cv::setMouseCallback( win1, myMouseCB, this );
	}

	void addMousePoint()
	{
		vpt.push_back( pt_mouse );
	}

	int nbPts() const
	{
		return (int)vpt.size();
	}
	void clearImage()
	{
		img.clear();
	}
	void showImage()
	{
		img.show( win1 );
	}
	void putTextLine( std::string msg, int lineindex=-1 )
	{
		int lineSize = 22;
		if( lineindex == 0 )
			tline = 0;
		cv::putText( img.getReal(), msg, cv::Point2i( 20, lineSize*++tline), 0, 0.6, cv::Scalar( 150,0,0 ), 1 );
	}
	void drawLines()
	{
		for( int i=0; i<nbPts(); i++ )
		{
			if( selected == i )
				vpt[i].draw( img, img::DrawParams().setColor( 250, 0, 150).setPointStyle( (img::PtStyle)i).selectPoint() );
			else
				vpt[i].draw( img, img::DrawParams().setPointStyle((img::PtStyle)i) );
		}
		Line2d lA( vpt[0], vpt[2] );
		Line2d lB( vpt[0], vpt[3] );
		Line2d lC( vpt[1], vpt[2] );
		Line2d lD( vpt[1], vpt[3] );

		img::DrawParams dp;
		dp.setColor(  250,  50, 100) ;
		lA.draw( img, dp );
		lB.draw( img, dp );
		lC.draw( img, dp );
		lD.draw( img, dp );
		auto rect = getBB( vpt );
		rect.draw( img, img::DrawParams().setColor(0,250,0) );

		auto cbb = rect.getBoundingCircle();
		cbb.draw( img, img::DrawParams().setColor( 0,0,250) );
	}
};

//------------------------------------------------------------------
/// Mouse callback functions, checks if one of the points is selected.
/**
- If so, that point gets moved by the mouse, and the function \c action is called (if it has been assigned)
*/
void
myMouseCB( int event, int x, int y, int, void* param )
{
	auto& data = *reinterpret_cast<Data*>(param);

//	std::cout << __FUNCTION__ << '\n';
	data.setMousePos(x,y);
	bool doSomething = true;

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
			if( data.selected == -1 )
				if( data.leftClicAddPoint )
					data.addMousePoint();
		break;

		case CV_EVENT_MOUSEMOVE:
			if( data.selected != -1 )
			{
				data.vpt[data.selected] = data.pt_mouse;
				data.vpt[data.selected].draw( data.img, img::DrawParams().selectPoint() );
			}
		break;

		case CV_EVENT_RBUTTONDOWN:
			data.selected = -1;
			for( int i=0; i<data.nbPts(); i++ )
				if( data.pt_mouse.distTo( data.vpt[i]) < 10 )  // if mouse is less than 10 pixel away
					data.selected = i;
			if( data.selected != -1 )
			{
				data.vpt.erase( std::begin(data.vpt) + data.selected );
				data.selected = -1;
			}
		break;

		default: doSomething = false; break;
	}
	if( doSomething )
	{
		data.clearImage();
		if( data._mouseCB )
		{
//			std::cout << "call mouse CB\n";
			data._mouseCB( param );
		}
		data.showImage();
	}
}

//------------------------------------------------------------------
/// Generic Keyboard loop, build on top of Opencv's \c cv::waitKey(0)
struct KeyboardLoop
{
	using FuncType = void(void*);
	/// Holds link between key, function, and description
	struct KbLoopAction
	{
		char                    _key;     ///< what key
		std::function<FuncType> _action;  ///< what do do
		std::string             _msg;     ///< message describing key action
		friend std::ostream& operator << ( std::ostream& f, const KbLoopAction& action )
		{
			if( !action._msg.empty() )
				f << " -" << action._key << ": " << action._msg << '\n';
			return f;
		}
	};

private:
	std::vector<KbLoopAction> _actions;
	std::function<FuncType>   _common = nullptr;

public:
	void addKeyAction(
		char key,                              ///< the key
		const std::function<FuncType>& action, ///< the CB, called on key hit
		std::string text=std::string()         ///< the message that will be printed
	)
	{
		auto it = std::find_if(
			_actions.begin(),
			_actions.end(),
			[&]
			( const KbLoopAction& elem )
			{
				return key == elem._key;
			}
		);
		if( it != _actions.end() )
			throw std::runtime_error( std::string("Error, key '") + key + "' already registered" );
		_actions.push_back( KbLoopAction{ key, action, text } );
	}
/// Add common \c action when a valid key is hit
	void addCommonAction( const std::function<FuncType>& action )
	{
		_common = action;
	}
	void showAvailableKeys() const
	{
		if( !_actions.empty() )
		{
			std::cout << "Available keys: " << _actions.size() << '\n';
			for( const auto& elem: _actions )
				std::cout << elem;
		}
		else
			std::cout << "No user keys defined, but 'h' (help), ESC (quit) and SPC (switch to next) available\n";
	}

	void start( Data& data )
	{
		showAvailableKeys();

		char key=0;
		do
		{
			key = cv::waitKey(0);
			if( key == 27 )
			{
				std::cout << "ESC => terminate\n";
				std::exit(0);
			}
			if( key == 32 )
			{
				std::cout << "SPC: switch to next\n";
				return;
			}
			if( key == 'H' || key == 'h' )
				showAvailableKeys();
			else
			{
				auto it = std::find_if(
					_actions.begin(),
					_actions.end(),
					[&]
					( const KbLoopAction& elem )
					{
						return key == elem._key;
					}
				);

				if( it != _actions.end() )
				{
					std::cout << "Action";
					if( !it->_msg.empty() )
						std::cout << ": " << it->_msg;
					std::cout << '\n';

					it->_action(&data);
					if( _common )
						_common(&data);
					data.showImage();
				}
			}
		}
		while( key != 32 ); // SPC
	}
};

//------------------------------------------------------------------
void action_1( void* param )
{
	auto& data = *reinterpret_cast<Data*>(param);
	data.drawLines();

	Line2d l_mouse  = data.pt_mouse * Point2d();
	auto p_lines = l_mouse.getParallelLines( 30 );

	auto ppts = l_mouse.getPoints( Point2d(), 200 );
	Line2d l_mouse_o = l_mouse.getOrthogonalLine( ppts.second );
	l_mouse.draw( data.img );
	l_mouse_o.draw( data.img );

	auto p_lines_o = l_mouse_o.getParallelLines( 10 );


	img::DrawParams dp;
	dp.setColor(100,250,100);
	p_lines.first.draw(  data.img, dp );
	p_lines.second.draw( data.img, dp );

	p_lines_o.first.draw(  data.img, dp );
	p_lines_o.second.draw( data.img, dp );
}

void demo_1( int demidx )
{
	Data data( demidx, "lines" );
	std::cout << "Demo " << demidx << ": click on points and move them\n";

	data.setMouseCB( action_1 );

	int n=5;
	data.vpt[0].set( data._imWidth/2,       data._imHeight/n );
	data.vpt[1].set( data._imWidth/2,       data._imHeight*(n-1)/n );
	data.vpt[2].set( data._imWidth/n,       data._imHeight/2 );
	data.vpt[3].set( data._imWidth*(n-1)/n, data._imHeight/2 );

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
	explicit Param_B( int demidx, std::string wname ): Data( demidx, wname )
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
void demo_B( int demidx )
{
	Param_B data( demidx, "build Homography" );
	std::cout << "Demo " << demidx << ": build Homography from Rotation, Translation, Scale\n"
		<< "Hit a key: scale:[op], angle:[lm], translation:[gh,yb], reset: r\n";

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

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'r', [&](void*){ scale = 1.; angle = tx = ty = 0.; }, "reset" );
	kbloop.addKeyAction( 'm', [&](void*){ angle += angle_delta; }, "increment angle" );
	kbloop.addKeyAction( 'l', [&](void*){ angle -= angle_delta; }, "decrement angle" );
	kbloop.addKeyAction( 'z', [&](void*){ tx += trans_delta;    }, "increment tx" );
	kbloop.addKeyAction( 'a', [&](void*){ tx -= trans_delta;    }, "decrement tx" );
	kbloop.addKeyAction( 'b', [&](void*){ ty += trans_delta;    }, "increment ty" );
	kbloop.addKeyAction( 'y', [&](void*){ ty -= trans_delta;    }, "decrement ty" );
	kbloop.addKeyAction( 'p', [&](void*){ scale *= scale_delta; }, "increment scale" );
	kbloop.addKeyAction( 'o', [&](void*){ scale /= scale_delta; }, "reduce scale" );
	kbloop.addCommonAction( [&](void*)
		{
			data.clearImage();
			Homogr H;
			H.addRotation( angle*K ).addTranslation( tx, ty ).addScale( scale );
			data.initPts();
			H.applyTo(data.vpt);
			data.drawLines();
			data.showImage();
		}
	);

	kbloop.start( data );
}
//------------------------------------------------------------------
struct Param_C: public Data
{
	explicit Param_C( int demidx, std::string wname ): Data( demidx, wname )
	{
		rect.set( Point2d( 180,120), Point2d( 380,280) );
		vpt[0] = Point2d( 70,70 );
		vpt[1] = Point2d( 480,380 );
	}

	int radius = 50;
	std::array<Line2d,3> li;
	FRect rect;

	void drawLines()
	{
		for( size_t i=0; i<li.size(); i++ )
			li[i].draw( img );
	}
};

void action_C( void* param )
{
	auto& data = *reinterpret_cast<Param_C*>(param);
	data.clearImage();
	data.drawLines();

	Circle c1( data.vpt[0], data.radius );
	Circle c2( data.vpt[1], 100 );

	img::DrawParams dpc2;
	dpc2.setColor(150,0,150);
	if( c2.isInside( c1 ) )
		dpc2.setColor(250,100,0);
	c2.draw( data.img, dpc2 );

	img::DrawParams dp;
	dp.setColor(150,0,150);
	if( data.rect.isInside( c1 ) )
		dp.setColor(250,100,0);
	data.rect.draw( data.img, dp );

	img::DrawParams dpc1;
	dpc1.setColor(0,250,0);
	if( c1.isInside( data.rect ) )
		dpc1.setColor(250,100,0);
	if( c1.isInside( c2 ) )
		dpc1.setColor(250,100,0);
	c1.draw( data.img, dpc1 );

	data.pt_mouse.draw( data.img, img::DrawParams().setColor(250,50,20) );

// circle - circle intersections
	auto cci = c1.intersects( c2 );
	if( cci() )
		draw( data.img, cci.get(), img::DrawParams().setColor(0,150,0).setPointStyle(img::PtStyle::Diam) );


// circle - rectangle intersections
	auto cr1 = c1.intersects( data.rect );
	auto cr2 = c2.intersects( data.rect );
	if( cr1() )
		draw( data.img, cr1.get(), img::DrawParams().setColor(0,20,220).setPointStyle(img::PtStyle::Diam) );
	if( cr2() )
		draw( data.img, cr2.get(), img::DrawParams().setColor(0,20,220).setPointStyle(img::PtStyle::Diam) );

// circle - lines intersections
	for( size_t i=0; i<data.li.size(); i++ )
	{
		auto ri = data.li[i].intersects( c1 );
		if( ri() )
		{
			auto inter = ri.get();
			inter.first.draw(  data.img, img::DrawParams().setColor(250, 0, 0) );
			inter.second.draw( data.img, img::DrawParams().setColor(250, 0, 0) );
		}
	}

	auto seg = getSegment( c1, c2 );
	seg.draw( data.img, img::DrawParams().setColor(250, 0, 0) );

	auto pseg = getTanSegs( c1, c2 );
	pseg.first.draw(  data.img, img::DrawParams().setColor(250, 250, 0) );
	pseg.second.draw( data.img, img::DrawParams().setColor(0, 250, 250) );
	data.showImage();
}

void demo_C( int demidx )
{
	std::cout << "Demo " << demidx << ": move circle over line, hit [lm] to change circle radius\n";
	Param_C data( demidx, "circle demo" );

	data.li[0] = Point2d() * Point2d(200,100);
	data.li[1] = Point2d(200,0) * Point2d(200,200);
	data.li[2] = Point2d(0,200) * Point2d(200,200);

	data.clearImage();
	data.drawLines();
	action_C( &data );
	data.showImage();

	data.setMouseCB( action_C );

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'l', [&](void*){ data.radius += 10; }, "increment radius" );
	kbloop.addKeyAction( 'm', [&](void*){ data.radius -= 10; }, "decrement radius" );
	kbloop.addKeyAction( 'r', [&](void*){ data.radius = 80;  }, "reset radius" );
	kbloop.start( data );
}

//------------------------------------------------------------------
struct Param_SI: Data
{
	explicit Param_SI( int demidx, std::string wname): Data( demidx, wname )
	{}

	Segment seg1,seg2;
};

double action_SI_drawDist( const Segment& seg, void* param )
{
	auto& data = *reinterpret_cast<Param_SI*>(param);

	int segDistCase;
	auto segDist = seg.distTo( data.pt_mouse, &segDistCase );

	auto col_A = img::DrawParams().setColor( 0,200,200 );
	auto col_B = img::DrawParams().setColor( 200,200,0 );
	switch( segDistCase )
	{
		case -1:
			draw( data.img, Segment( data.pt_mouse, seg.getPts().first ), col_B );
		break;
		case +1:
			draw( data.img, Segment( data.pt_mouse, seg.getPts().second ), col_B );
		break;
		default:
			if( data.pt_mouse.distTo( seg.getLine() ) > 3. )
			{
				auto orthog_seg = seg.getLine().getOrthogSegment( data.pt_mouse );
				orthog_seg.draw( data.img, col_A );
			}
	}
	return segDist;
}
void action_SI( void* param )
{
	auto& data = *reinterpret_cast<Param_SI*>(param);

	data.clearImage();
	data.seg1.set( data.vpt[0], data.vpt[1] );
	data.seg2.set( data.vpt[2], data.vpt[3] );

	data.seg1.draw( data.img, img::DrawParams().setColor( 0,0,250).setThickness(2) );
	data.seg2.draw( data.img, img::DrawParams().setColor( 250,0,0).setThickness(2) );
	data.seg1.getLine().draw( data.img, img::DrawParams().setColor( 200,200,200) );
	data.seg2.getLine().draw( data.img, img::DrawParams().setColor( 200,200,200) );
	draw( data.img, data.vpt );

	auto psegs = data.seg1.getParallelSegs( 40 );
	draw( data.img, psegs.first,  img::DrawParams().setColor( 0,250,200) );
	draw( data.img, psegs.second, img::DrawParams().setColor( 200,0,250) );


	if( data.selected != -1 )
		data.vpt[data.selected].draw( data.img, img::DrawParams().selectPoint() );

	auto inters = data.seg1.intersects( data.seg2 );
	if( inters() )
	{
		auto pti = inters.get();
		pti.draw( data.img );
		Line2d l1 = data.seg1.getLine().getOrthogonalLine( pti );
		l1.draw( data.img, img::DrawParams().setColor( 0,0,100) );

		Line2d l2 = data.seg2.getLine().getOrthogonalLine( pti );
		l2.draw( data.img, img::DrawParams().setColor( 100,0,0) );
	}

	Line2d li2( Point2d(350,120),Point2d(20,50));
	li2.draw( data.img );

	auto inters1 = data.seg1.intersects( li2 );
	if( inters1() )
		 inters1.get().draw( data.img, img::DrawParams().setPointStyle(img::PtStyle::Diam).setColor( 250,0,0));
	auto inters2 = data.seg2.intersects( li2 );
	if( inters2() )
		 inters2.get().draw( data.img, img::DrawParams().setPointStyle(img::PtStyle::Diam).setColor( 250,0,0));

	auto segDist1 = action_SI_drawDist( data.seg1, param );
	auto segDist2 = action_SI_drawDist( data.seg2, param );
	data.putTextLine( "distance mouse/s1=" + std::to_string(segDist1) + " mouse/s2=" + std::to_string(segDist2) ,0 );
	data.showImage();
}

/// Segment intersection demo
void demo_SI( int demidx )
{
	Param_SI data( demidx, "segment_intersection" );
	std::cout << "Demo " << demidx << ": intersection of segments\n Select a point and move it around. "
		<< "When they intersect, you get the orthogonal lines of the two segments, at the intersection point.\n"
		<< "Also shows parallel segments\n";

	data.vpt[0] = Point2d(100,200);
	data.vpt[1] = Point2d(200,300);
	data.vpt[2] = Point2d(150,50);
	data.vpt[3] = Point2d(300,250);

	action_SI( &data );
	data.setMouseCB( action_SI );

	if( 27 == cv::waitKey(0) )
		std::exit(0);
}

//------------------------------------------------------------------
struct Param_6 : Data
{
	explicit Param_6( int demidx, std::string wname): Data( demidx, wname )
	{}

	float angle = 20;
};

void action_6( void* param )
{
	auto& data = *reinterpret_cast<Param_6*>(param);

	data.clearImage();
	double K = M_PI / 180.;
	auto tx = data.pt_mouse.getX();
	auto ty = data.pt_mouse.getY();

	auto mouse_pos = std::make_pair(
		Line2d( LineDir::H, ty ),
		Line2d( LineDir::V, tx )
	);
	draw( data.img, mouse_pos, img::DrawParams().setColor( 200,200,200) );

/*	auto org = std::make_pair(
		Line2d( LineDir::H, ty ),
		Line2d( LineDir::V, tx )
	);
	draw( data.img, org );
*/
	Homogr H = Homogr().addTranslation(-tx,-ty).addRotation( data.angle * K ).addTranslation(tx,ty);

	draw( data.img, data.vpt[0] );
	draw( data.img, data.vpt[1] );
	Line2d l1( data.vpt[0], data.vpt[1] );
	Line2d l2 = l1.getRotatedLine( data.vpt[0], data.angle * K );

	auto dpar = img::DrawParams();
	l1.draw( data.img, dpar.setColor( 250,0,0) );
	l2.draw( data.img, dpar.setColor( 0,250,0) );

	Segment s1( data.vpt[2], data.vpt[3] );
	Segment s2 = H*s1;
	s1.draw( data.img, dpar.setColor( 250,0,0) );
	s2.draw( data.img, dpar.setColor( 0,0,250) );
	s1.getPts().first.draw( data.img, dpar.selectPoint() );
	s1.getPts().second.draw( data.img, dpar.selectPoint() );
}

void demo_6( int demidx )
{
	std::cout << "Demo " << demidx << ": apply homography to lines and segments\n Hit [lm] to change angle, "
		<< "and select points of blue segment with mouse\n";
	Param_6 data( demidx, "homography_lines_seg" );
	double angle_delta = 5.; // degrees

	data.setMouseCB( action_6 );
	action_6( &data );

	data.showImage();

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'm', [&](void*){ data.angle += angle_delta; std::cout << "val=" <<data.angle<<'\n'; }, "increment angle" );
	kbloop.addKeyAction( 'l', [&](void*){ data.angle -= angle_delta; std::cout << "val=" <<data.angle<<'\n'; }, "decrement angle" );
	kbloop.addCommonAction( [&](void*){ action_6(&data);} );
	kbloop.start( data );
}
//------------------------------------------------------------------
/// This one has two windows
struct Param_H: public Data
{
	int hmethod = 1;
	img::Image<cv::Mat> img2;
	std::string win2 = "Computed projection";

	explicit Param_H( int demidx, std::string wname ): Data( demidx, wname )
	{
		cv::namedWindow( win2 );
		cv::moveWindow( win2, _imWidth, 50 );
		img2.setSize( _imHeight, _imWidth );
	}
	void showImage()
	{
		img.show( win1 );
		img2.show( win2 );
	}
	void clearImage()
	{
		img.clear();
		img2.clear();
	}
	void reset()
	{
		auto pa1 = Point2d(100,100);
		auto pa2 = Point2d(400,300);
		auto v1 = get4Pts( FRect(pa1, pa2) );

		auto pb1 = Point2d(80,150);
		auto pb2 = Point2d(450,350);
		auto v2 = get4Pts( FRect(pb1, pb2) );
		std::copy( v1.begin(), v1.end(), vpt.begin() );
		std::copy( v2.begin(), v2.end(), vpt.begin()+4 );
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
		s1.draw( data.img, img::DrawParams().setColor( 0,0,250) );
		s2.draw( data.img, img::DrawParams().setColor( 250,0,0) );

		s1.draw( data.img2, img::DrawParams().setColor( 0,0,250) );

		Segment( v1[i], v2[i] ).draw( data.img );

		v1[i].draw( data.img );
		v2[i].draw( data.img );
	}

	std::vector<Point2d> vseg;
	auto center_x = 160;
	auto center_y = 220;
	auto size_v = 40;
	auto size_h = 60;
	vseg.emplace_back( Point2d(center_x+size_h, center_y) );
	vseg.emplace_back( Point2d(center_x-size_h, center_y) );
	vseg.emplace_back( Point2d(center_x,        center_y+size_v) );
	vseg.emplace_back( Point2d(center_x,        center_y-size_v) );

	Segment sa1( vseg[0], vseg[1] );
	Segment sb1( vseg[2], vseg[3] );

	sa1.draw( data.img, img::DrawParams().setColor( 0,100,100) );
	sb1.draw( data.img, img::DrawParams().setColor( 0,100,100) );

	cv::putText( data.img.getReal(), "source points", cv::Point2i( v1[0].getX(), v1[0].getY() ), 0, 0.8, cv::Scalar( 250,0,0 ), 2 );
	cv::putText( data.img.getReal(), "dest points",   cv::Point2i( v2[0].getX(), v2[0].getY() ), 0, 0.8, cv::Scalar( 0,0,250 ), 2 );

	Homogr H;
	H.buildFrom4Points( v1, v2, data.hmethod );
	std::cout << "Computed Homography:\n" << H << '\n';

	std::vector<Point2d> vpt3;
	for( int i=0; i<4; i++ )
		vpt3.push_back( H * data.vpt[i] );

	for( int i=0; i<4; i++ )
	{
		Segment s1( vpt3[i], vpt3[i==3?0:i+1] );
		s1.draw( data.img2, img::DrawParams().setColor( 0,250,0) );
	}

	auto vsegH  = H * vseg;
	Segment sa2( vsegH[0], vsegH[1] );
	Segment sb2( vsegH[2], vsegH[3] );

	sa2.draw( data.img2, img::DrawParams().setColor( 0,100,100) );
	sb2.draw( data.img2, img::DrawParams().setColor( 0,100,100) );


	FRect rect( Point2d(200,160), Point2d(330,250));
	rect.draw( data.img );
	auto rect2 = H * rect;
	rect2.draw( data.img2 );

	auto e_x = 320.;
	auto e_y = 360.;
	auto e_h = 70.;

	Circle c_ell( e_x, e_y, e_h );
	c_ell.draw( data.img );
	auto ell = H * c_ell;
	ell.draw( data.img2 );
	auto ecenter = ell.getCenter();
	ecenter.draw( data.img2 );

	auto ell_bb = ell.getBB();
	ell_bb.draw( data.img2 );

	data.showImage();
}

/// Demo of computing a homography from two sets of 4 points
void demo_H( int demidx )
{
	Param_H data( demidx, "Compute Homography from 4 points" );
	data.vpt.resize(8);
	data.reset();
	std::cout << "Demo " << demidx << ": compute homography from two sets of 4 points\n"
		<< " - usage: move points with mouse in left window, right window will show source rectangle (blue)\n"
		<< "and computed projected rectangle (green)\n"
		<< " - keys:\n  -a: switch backend computing library\n  -r: reset points\n";

	data.setMouseCB( action_H );
	action_H( &data );

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'r', [&](void*){ data.reset(); } );
	kbloop.addKeyAction( 'a', [&](void*)
		{
			data.hmethod = data.hmethod?0:1;
#if !defined(HOMOG2D_USE_EIGEN)
			if( data.hmethod == 0 )
			{
				std::cout << "Unable, build without Eigen support, see manual, switch to Opencv\n";
				data.hmethod = 1;
			}
#endif
		}
	);
	kbloop.start( data );
}
//------------------------------------------------------------------
/// This datatype holds two Polyline objects, one closed, one open. They are both edited the same way and
/// we switch drawing
struct Param_PL : Data
{
	explicit Param_PL( int demidx, std::string title ):Data( demidx, title )
	{
		v_po.push_back( &polyline_o );
		v_po.push_back( &polyline_c );
	}
	OPolyline polyline_o;
	CPolyline polyline_c;
	std::vector<detail::Common<double>*> v_po;
	bool showClosedPoly = false;
};

void action_PL( void* param )
{
	auto& data = *reinterpret_cast<Param_PL*>(param);

	data.clearImage();
	data.polyline_o.set( data.vpt );
	data.polyline_c.set( data.vpt );

	auto color = img::DrawParams().setColor( 0,10,200);
	if( data.polyline_c.isPolygon() )
		color = img::DrawParams().setColor( 250,10,20);

	auto len = data.showClosedPoly ? data.polyline_c.length() : data.polyline_o.length();
	if( data.showClosedPoly )
		data.polyline_c.draw( data.img, color );
	else
		data.polyline_o.draw( data.img, color );

	auto col_green = img::DrawParams().setColor(10,250,10);
	Line2d li( Point2d( 10,60), Point2d( 400,270) );
	li.draw( data.img, col_green );

	data.putTextLine( std::string("Nb pts=") + std::to_string( data.polyline_c.size() ), 0 );
	data.putTextLine( std::string("length=") + std::to_string(len)                         );

	auto intersPts_o = li.intersects(data.polyline_o).get();
	auto intersPts_c = li.intersects(data.polyline_c).get();;

	auto intersPts = ( data.showClosedPoly ? intersPts_c : intersPts_o );
	for( const auto& pt: intersPts )
		pt.draw( data.img  );

	Circle cir( 350,180,85);
	cir.draw( data.img, col_green );
	auto i_cir_o = cir.intersects( data.polyline_o );
	auto i_cir_c = cir.intersects( data.polyline_c );

	FRect rect( 90,160,205,245);
	rect.draw( data.img, col_green );
	auto i_rect_o = rect.intersects( data.polyline_o );
	auto i_rect_c = rect.intersects( data.polyline_c );

	std::string str_ispoly{"Polygon: N"};
	if( data.showClosedPoly )
	{
		draw( data.img, i_cir_c.get() );
		draw( data.img, i_rect_c.get() );
		if( data.polyline_c.isPolygon() )
			str_ispoly = "Polygon: Y";
	}
	else
	{
		draw( data.img, i_cir_o.get() );
		draw( data.img, i_rect_o.get() );
	}
	data.putTextLine( str_ispoly );
	auto bb = data.polyline_c.getBB();
	bb.draw( data.img );

	if( data.showClosedPoly && data.polyline_c.isPolygon() )
	{
		auto centroid = data.polyline_c.centroid();
		centroid.draw( data.img, img::DrawParams().setColor(40,20,250) );
		cv::putText(
			data.img.getReal(),
			"centroid",
			centroid.getCvPti(),
			0, 0.6,
			cv::Scalar( 250,0,0 )
		);

		data.putTextLine( std::string("area=") + std::to_string(data.polyline_c.area()) );

		auto isC = "Convex: Y";
		if( !data.polyline_c.isConvex() )
			isC = "Convex: N";
		data.putTextLine( isC );

//		std::cout << "mouse=" << data.pt_mouse << '\n';
		auto YN = data.pt_mouse.isInside(data.polyline_c)?"Y":"N";
		data.putTextLine( std::string("IsInside=") + std::string( YN ) );

/*		auto idx = base::sub::getFarthestSegment( data.pt_mouse, bb );
		auto seg_f = bb.getSegs()[idx];
		seg_f.draw( data.img, img::DrawParams().setColor(250,20,120) );
*/
	}
	data.showImage();
}

void demo_PL( int demidx )
{
	Param_PL data( demidx, "Polyline demo" );
	std::cout << "Demo " << demidx
		<< ": polyline\n-Colors\n -Red: polygon (needs to be closed)\n -Blue: intersections\n"
		<< "Lclick to add point, Rclick to remove\n";
	data.leftClicAddPoint=true;

	data.setMouseCB( action_PL );

	action_PL( &data );

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'a', [&](void*)
		{
			data.showClosedPoly = !data.showClosedPoly;
		},
		"switch open/close"
	);
//	kbloop.addCommonAction( [&] { action_PL(&data); } );
	kbloop.start( data );
}

//------------------------------------------------------------------
struct Param_ELL : Data
{
	explicit Param_ELL(  int demidx, std::string title ):Data( demidx, title )
	{}
	void draw()
	{
		clearImage();
		ell = Ellipse_<float>( x0, y0, major, major*ratio_mm, 0. ) ;
		auto ell2 = H * ell;
		ell2.draw( img );

		auto bb1 = ell2.getOBB();
		bb1.draw( img, img::DrawParams().setColor(0,0,250) );

		auto bb2 = ell2.getBB();
		bb2.draw( img, img::DrawParams().setColor(0,250,0) );

		auto axis = ell2.getAxisLines();
		h2d::draw( img, axis );

		putTextLine( std::string("Major length=")       + std::to_string( major ),       0 );
		putTextLine( std::string("ratio Mm=")           + std::to_string( ratio_mm )       );
		putTextLine( std::string("Ellipse area=")       + std::to_string( ell.area()   )   );
		putTextLine( std::string("Ellipse perimeter=")  + std::to_string( ell.length() )   );
		putTextLine( std::string("Green BB area=")      + std::to_string( bb2.area()   )   );
		putTextLine( std::string("Green BB perimeter=") + std::to_string( bb2.length() )   );
		putTextLine( std::string("Blue BB area=")       + std::to_string( bb1.area()   )   );
		putTextLine( std::string("Blue BB perimeter=")  + std::to_string( bb1.length() )   );

		showImage();
	}
	Ellipse_<double> ell;
	Homogr H;
	double angle = 5.;
	double tx = 0;
	double ty = 0;
	double x0=200;
	double y0=250;
	double major = 120.;
	double ratio_mm = .5;
};

/// action for Ellipse demo (run on keyboard hit)
void action_ELL( void* param )
{
	auto& data = *reinterpret_cast<Param_ELL*>(param);
	data.H.init();
	data.H.addTranslation(-data.x0, -data.y0).addRotation( data.angle * M_PI/180.).addTranslation(data.x0+data.tx, data.y0+data.ty);
	data.draw();
}

/// Ellipse demo
void demo_ELL( int demidx )
{
	Param_ELL data( demidx, "Ellipse demo" );
	std::cout << "Demo " << demidx
		<< ": Ellipse (no mouse, enter 'h' for valid keys)\n"
		<< " -blue rectangle: ellipse bounding box\n"
		<< " -green rectangle: blue rectangle bounding box\n";

	double trans_delta = 20;
	double angle_delta = 5.;

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'z', [&](void*){ data.tx += trans_delta;    }, "increment tx" );
	kbloop.addKeyAction( 'a', [&](void*){ data.tx -= trans_delta;    }, "decrement tx" );
	kbloop.addKeyAction( 'b', [&](void*){ data.ty += trans_delta;    }, "increment ty" );
	kbloop.addKeyAction( 'y', [&](void*){ data.ty -= trans_delta;    }, "decrement ty" );
	kbloop.addKeyAction( 'm', [&](void*){ data.angle += angle_delta; }, "increment angle" );
	kbloop.addKeyAction( 'l', [&](void*){ data.angle -= angle_delta; }, "decrement angle" );
	kbloop.addKeyAction( 'o', [&](void*){ data.ratio_mm = std::min(data.ratio_mm*1.1, 1.00); }, "inc ratio" );
	kbloop.addKeyAction( 'p', [&](void*){ data.ratio_mm = std::max(data.ratio_mm/1.1, 0.05); }, "dec ratio" );

	kbloop.addCommonAction( action_ELL );
	action_ELL( &data );
	kbloop.start( data );
}

//------------------------------------------------------------------
/// Circle demo
struct Param_CIR : Data
{
	explicit Param_CIR( int demidx, std::string title ): Data( demidx, title )
	{
		vpt = std::vector<Point2d>{
			{150,120}, {220,240},            // initial rectangle
			{100,100}, {300,100}, {300,200}  // initial circle
		};
		ptr_cr = &cir2;
	}
	void setAndDraw()
	{
		if( buildFrom3Pts )
		{
			try
			{
				cir.set( vpt[2], vpt[3], vpt[4] );
			}
			catch( std::exception& err )
			{
				std::cout << "unable to build circle from the 3 points given:\n=> " << err.what() << '\n';
			}
			CPolyline pol;
			pol.setParallelogram( vpt[2], vpt[3], vpt[4] );
			pol.draw( img, img::DrawParams().setColor(120,200,0) );
		}
		else
			cir.set( vpt[2], vpt[3] );
		rect.set( vpt[0], vpt[1] );
		cir2.set( vpt[0], vpt[1] );

		ptr_cr = &cir2;
		if( drawRect )
			ptr_cr = &rect;

		auto par_c = img::DrawParams().setColor(0,120,250);
		auto par_r = img::DrawParams().setColor(120,0,250);
		if( cir.isInside( rect ) )
			par_c.setColor( 0,250,0);
		if( rect.isInside( cir ) )
			par_r.setColor( 0,250,0);

		cir.draw( img, par_c );
//		rect.draw( img, par_r );
		ptr_cr->draw( img, par_r );

		auto par_pt = img::DrawParams().setColor(250,20,50).setPointSize(2).setPointStyle(img::PtStyle::Dot);
		if( buildFrom3Pts )
			draw( img, vpt, par_pt );
		else                                  // draw only 4 points
			for( int i=0; i<4; i++ )
				vpt[i].draw( img, par_pt );

// intersection points
// TODO: replace this by: 		auto it = cir.intersects( *ptr_cr );
// once we have set a unique function for all intersections types
		auto it_c = cir.intersects( cir2 );
		auto it_r = cir.intersects( rect );
		if( drawRect )
		{
			if( it_r() )
				draw( img, it_r.get(), img::DrawParams().setColor(120,0,0) );
		}
		else
			if( it_c() )
				draw( img, it_c.get(), img::DrawParams().setColor(120,0,0) );
	}

// DATA SECTION
	const detail::Root* ptr_cr;
	Circle cir;
	Circle cir2;
	FRect rect;
	bool buildFrom3Pts = true;
	bool drawRect  = true;

};

void action_CIR( void* param )
{
	auto& data = *reinterpret_cast<Param_CIR*>(param);
	data.clearImage();
	data.setAndDraw();
	data.showImage();
}

void demo_CIR( int demidx )
{
	Param_CIR data( demidx, "Circle demo" );
	std::cout << "Demo " << demidx << ": Compute circle from 3 points/2 points\n"
		<< "Colors: green if inside, blue if not\n"
		<< "if 3 points, also computes the corresponding parallelogram\n";

	action_CIR( &data );
	data.setMouseCB( action_CIR );

	KeyboardLoop kbloop;
	kbloop.addCommonAction( action_CIR );

	kbloop.addKeyAction( 'a', [&](void*)
		{
			data.buildFrom3Pts = !data.buildFrom3Pts;
		},
		"switch circle from 2 pts / 3 pts"
	);
	kbloop.addKeyAction( 'w', [&](void*)
		{
			data.drawRect = !data.drawRect;
		},
		"switch circle/rectangle"
	);

	kbloop.start( data ); // blocking function
}

//------------------------------------------------------------------
/// Convex hull + Minimum Enclosing Circle demo
struct Param_CH : Data
{
	explicit Param_CH( int demidx, std::string title ): Data( demidx, title )
	{
		vpt = std::vector<Point2d>{ {100,100}, {300,80}, {270,400}, {100,420},{150,250} };
	}
	std::vector<img::Color> vcol;
	bool _mode = false;  ///< drawing mode: convex hull or hull lines
};

void action_CH( void* param )
{
	static size_t old_size = 0;
	auto& data = *reinterpret_cast<Param_CH*>(param);

	data.clearImage();
	draw( data.img, data.vpt, img::DrawParams().showIndex() );

	auto chull = convexHull( data.vpt );
	Circle cir;
	cir.set(  data.vpt );


	auto vlines = getLines( chull.getSegs() );
	if( old_size != vlines.size() )
	{
		data.vcol = img::genRandomColors( vlines.size() );
		old_size = vlines.size();
	}

	auto func = [&](int i)   // lambda, needed to fetch color from index
		{
			return img::DrawParams().setColor(data.vcol[i]);
		};
	std::function<img::DrawParams(int)> f(func);

	if( data._mode )
		draw( data.img, vlines, f );
	if( !data._mode )
		chull.draw( data.img, img::DrawParams().setColor(250,0,0) );
	cir.draw( data.img, img::DrawParams().setColor(0,0,250) );

	auto dp = img::DrawParams().setColor(0,0,0).setPointStyle( img::PtStyle::Dot ).setPointSize(4).setThickness(2);
	chull.getLmPoint().draw( data.img, dp );
	chull.getRmPoint().draw( data.img, dp );
	chull.getTmPoint().draw( data.img, dp );
	chull.getBmPoint().draw( data.img, dp );
	data.showImage();
}

void demo_CH( int demidx )
{
	Param_CH data( demidx, "Convex Hull + MEC demo" );
	std::cout << "Demo " << demidx << ": Convex hull + Minimum Enclosing Circle. Lclick to add points, Rclick to remove\n";
	action_CH( &data );
	data.setMouseCB( action_CH );

	data.leftClicAddPoint=true;

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'a', [&](void*){ data._mode  = !data._mode; }, "Toggle drawing mode (Hull, or hull lines)" );
	kbloop.addCommonAction( action_CH );
	action_CH( &data );
	kbloop.start( data );
}

//------------------------------------------------------------------
/// Rectangle intersection demo
struct Param_RI : Data
{
	explicit Param_RI( int demidx, std::string title ):Data( demidx, title )
	{}
	bool doUnion = true;
};

void action_RI( void* param )
{
	auto& data = *reinterpret_cast<Param_RI*>(param);

	data.clearImage();
	draw( data.img, data.vpt );
	FRect r1( data.vpt[0], data.vpt[1] );
	FRect r2( data.vpt[2], data.vpt[3] );
	r1.draw( data.img, img::DrawParams().setColor(250,0,0) );
	r2.draw( data.img, img::DrawParams().setColor(0,250,0) );
	auto c1a = r1.getBoundingCircle();
	auto c1b = r1.getInscribedCircle();
	c1a.draw( data.img );
	c1b.draw( data.img );
	if( data.doUnion )
	{
		auto res = r1 & r2;
		if( res() )
			res.get().draw( data.img, img::DrawParams().setColor(0,0,250) );
	}
	else
	{
		auto res = r1 | r2;
		res.draw( data.img, img::DrawParams().setColor(0,0,250) );
	}

	data.showImage();
}

void demo_RI( int demidx )
{
	Param_RI data( demidx, "Rectangle intersection demo" );
	std::cout << "Demo " << demidx << ": RI demo\n(Move rectangle with mouse)\n";

	data.setMouseCB( action_RI );
	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'a', [&](void*){ data.doUnion  = !data.doUnion; }, "Toggle union/intersection" );
	kbloop.addCommonAction( action_RI );
	action_RI( &data );
	kbloop.start( data );
}

//------------------------------------------------------------------
/// Segments demo
struct Param_SEG : Data
{
	explicit Param_SEG( int demidx, std::string title ):Data( demidx, title )
	{}
	bool showIndexes      = false;
	bool showIntersection = false;
	bool showMiddlePoint  = false;
	bool showBisector     = false;
	bool regen = false;
	std::vector<Segment>          vseg;
	std::vector<img::Color> vcol;

	int nbSegs = 100;
	int delta = 40;
	int width2  = _imWidth-delta;
	int height2 = _imHeight-delta;
	int k_col  = 200;
	int k_min  = 15;
	void generateSegments()
	{
//		std::cout << "generating " << nbSegs << " segments\n";
		std::srand( time(nullptr) );
		vseg.clear();
		vcol.clear();
		for( auto i=0; i<nbSegs; i++ )
		{
			auto len = 1.0*rand() / RAND_MAX * 40 + 10;
			auto p1x = 1.0*rand() / RAND_MAX * width2 + 20;
			auto p2x = 1.0*rand() / RAND_MAX * width2 + 20;;
			auto p1y = 1.0*rand() / RAND_MAX * height2 + 20;
			auto p2y = 1.0*rand() / RAND_MAX * height2 + 20;
			auto line = Line2d( p1x, p1y, p2x, p2y );
			auto ppts = line.getPoints( Point2d( p1x, p1y) , len );
			vseg.push_back( Segment( ppts ) );
		}
		vcol = img::genRandomColors( nbSegs );
		regen = false;
	}
};
void action_SEG( void* param )
{
	auto& data = *reinterpret_cast<Param_SEG*>(param);

	data.clearImage();
	if( data.regen )
		data.generateSegments();

	auto func = [&](int i)   // lambda, needed to fetch color from index
		{
			return img::DrawParams().showIndex(data.showIndexes).setColor(data.vcol[i]);
		};
	std::function<img::DrawParams(int)> f(func);
	draw( data.img, data.vseg, f );

	if( data.showIntersection )
	{
		size_t c_intersect = 0;
		for( size_t i=0; i<data.vseg.size()-1; i++ )
		{
			auto s1 = data.vseg[i];
			for( size_t j=i+1; j<data.vseg.size(); j++ )
			{
				auto s2 = data.vseg[j];
				auto pi = s1.intersects( s2 );
				if( pi() )
				{
					draw( data.img, pi.get(), img::DrawParams().setColor( 250,0,0) );
					c_intersect++;
				}
			}
		}
		std::cout << "- # intersection points=" << c_intersect << '\n';
	}
	if( data.showMiddlePoint )
	{
		for( const auto& seg: data.vseg )
			seg.getCenter().draw( data.img, img::DrawParams().setColor( 0,0,250) );
	}
	if( data.showBisector )
	{
		std::vector<Line2d> v_bisect( data.vseg.size() );
		std::transform(                               // To draw bisector lines with the same
			data.vseg.begin(),                        //  color as the segments,
			data.vseg.end(),                          //  we need to store them in a vector.
			v_bisect.begin(),
			[](const Segment& s){ return s.getBisector(); }
		);
		draw( data.img, v_bisect, f );
	}
	data.showImage();
}

void demo_SEG( int demidx )
{
	Param_SEG data( demidx, "Segments demo" );
	std::cout << "Demo " << demidx << ": Segments demo\n";
	data.generateSegments();

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'm', [&](void*){ data.showMiddlePoint  = !data.showMiddlePoint; }, "show middle point" );
	kbloop.addKeyAction( 'n', [&](void*){ data.showIndexes      = !data.showIndexes; }, "show indexes" );
	kbloop.addKeyAction( 'i', [&](void*){ data.showIntersection = !data.showIntersection; }, "show intersection points" );
	kbloop.addKeyAction( 'b', [&](void*){ data.showBisector     = !data.showBisector; }, "show bisector lines" );

	kbloop.addKeyAction( 'r', [&](void*){ data.regen = true; }, "Re-generate" );
	kbloop.addKeyAction( 'w', [&](void*){ data.nbSegs = data.nbSegs*2; data.regen = true; }, "double nb points" );
	kbloop.addKeyAction( 'x', [&](void*){ data.nbSegs = data.nbSegs/2; data.regen = true; }, "half nb points" );

	kbloop.addCommonAction( action_SEG );
	action_SEG( &data );

	kbloop.start( data );
}

//------------------------------------------------------------------
/// Polyline full step rotate demo parameters
struct Param_polRot : Data
{
	explicit Param_polRot( int demidx, std::string title ): Data( demidx, title )
	{
		_poly.set(
			std::vector<Point2d>{
				{0,0}, {100,0}, {100,100}, {50,150}, {0,100}
			}
		);
		_poly.translate( 180,180); // so it lies in the window
		_rect.set(0,0,160,100);
		_rect.translate( 220,230); // so it lies in the window

	}
	void nextRefPt()
	{
		if( _item )
		{
			_refPt_p++;
			if( _refPt_p >= _poly.size() )
				_refPt_p = 0;
			std::cout << "move to next ref pt: poly" << _refPt_p << ": " << _poly.getPoint( _refPt_p ) <<  '\n';
		}
/*		else
		{
			_refPt_r++;
			if( _refPt_r >= 4 )
				_refPt_r = 0;
			std::cout << "move to next ref pt: " << _refPt_r <<  '\n';
		}*/
	}
	void doIt( bool b = true )
	{
		_doIt = b;
	}

	CPolyline _poly;
	FRect     _rect;
	Rotate    _rotateType = Rotate::CW;
	size_t    _refPt_p = 0;                 ///< default index of center point (Polyline)
//	size_t    _refPt_r = 0;                 ///< default index of center point (Rectangle)
	bool      _doIt = false;
	bool      _item = true;
};

void action_polRot( void* param )
{
	auto& data = *reinterpret_cast<Param_polRot*>(param);
	data.clearImage();

	if( data._doIt )
	{
		if( data._item )
			data._poly.rotate( data._rotateType, data._poly.getPoint( data._refPt_p ) );
		else
			data._rect.rotate( data._rotateType, data.pt_mouse );
		 data._doIt = false;
	}

	if( data._item )
	{
		data._poly.draw( data.img, img::DrawParams().setColor(250,0,0).showPoints() );
		data._poly.getPoint( data._refPt_p ).draw( data.img, img::DrawParams().setColor(0,0,250).setPointStyle(img::PtStyle::Dot) );
	}
	else
	{
		data._rect.draw( data.img, img::DrawParams().setColor(0,0,250).showPoints() );
		draw( data.img, data.pt_mouse, img::DrawParams().setColor(250,0,0).setPointStyle(img::PtStyle::Dot) );
	}
	data.showImage();
}

void demo_polRot( int demidx )
{
	Param_polRot data( demidx, "Polyline/Rectangle full step rotate demo" );
	std::cout << "Demo " << demidx << ": Polyline/Rectangle full step rotate demo\n"
		<< " - Polyline: center point is one of the points\n"
		<< " - Rectangle: center point is free, use mouse\n"
		<< "Warning: as images as shown here with vertical axis reversed, what appears as a CW is actually a CCW rotation!\n";

	data.setMouseCB( action_polRot );

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'a', [&](void*){ data._rotateType=Rotate::CW;       data.doIt(); }, "rotate CW" );
	kbloop.addKeyAction( 'z', [&](void*){ data._rotateType=Rotate::CCW;      data.doIt(); }, "rotate CCW" );
	kbloop.addKeyAction( 'e', [&](void*){ data._rotateType=Rotate::Full;     data.doIt(); }, "rotate Full" );
	kbloop.addKeyAction( 'q', [&](void*){ data._rotateType=Rotate::VMirror;  data.doIt(); }, "VMirror" );
	kbloop.addKeyAction( 's', [&](void*){ data._rotateType=Rotate::HMirror;  data.doIt(); }, "HMirror" );
	kbloop.addKeyAction( 'w', [&](void*){ data.nextRefPt();                  data.doIt(false); }, "move to next reference point" );
	kbloop.addKeyAction( 'r', [&](void*){ data._item = !data._item;          data.doIt(false); }, "toggle poly/rectangle" );

	kbloop.addCommonAction( action_polRot );
	action_polRot( &data );

	kbloop.start( data );
}
//------------------------------------------------------------------
/// Polyline full step rotate demo parameters
struct Param_NFP : Data
{
	explicit Param_NFP( int demidx, std::string title ): Data( demidx, title )
	{
		genRandomPoints();
	}
	int _mode = 0;

	void genRandomPoints()
	{
		vpt.clear();
/*		std::cout << "generating " << nbSegs << " segments\n";
		vseg.clear();
		vcol.clear();*/
		int nbPts = 1.0*rand() / RAND_MAX * 100 + 10;
		for( auto i=0; i<nbPts; i++ )
		{
			auto x = 1.0*rand() / RAND_MAX * (_imWidth-120) + 50;
			auto y = 1.0*rand() / RAND_MAX * (_imHeight-120) + 50;
			vpt.push_back( Point2d(x,y) );
		}
//		vcol = genRandomColors( nbSegs );
	}
};

void action_NFP( void* param )
{
	auto& data = *reinterpret_cast<Param_NFP*>(param);
	data.clearImage();
	draw( data.img, data.vpt );
	data.pt_mouse.draw( data.img, img::DrawParams().setColor( 250,0,0) );
	switch( data._mode )
	{
		case 0:
		{
			auto idx = findNearestPoint( data.pt_mouse, data.vpt );
			Segment(data.vpt[idx], data.pt_mouse).draw( data.img, img::DrawParams().setColor( 250,0,0) );
		}
		break;
		case 1:
		{
			auto idx = findFarthestPoint( data.pt_mouse, data.vpt );
			Segment(data.vpt[idx], data.pt_mouse).draw( data.img, img::DrawParams().setColor( 0,250,0) );
		}
		break;
		case 2:
		{
			auto pidx = findNearestFarthestPoint( data.pt_mouse, data.vpt );
			Segment(data.vpt[pidx.first], data.pt_mouse).draw( data.img, img::DrawParams().setColor( 250,0,0) );
			Segment(data.vpt[pidx.second], data.pt_mouse).draw( data.img, img::DrawParams().setColor( 0,250,0) );
		}
		break;
		default: assert(0);
	}

	data.showImage();
}

void demo_NFP( int demidx )
{
	Param_NFP data( demidx, "Closest/Farthest Point" );
	std::cout << "Demo " << demidx << ": Closest/Farthest Point\n";

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'a', [&](void*){ data._mode==2? data._mode=0: data._mode++; }, "switch mode (nearest/farthest/both)" );
	kbloop.addKeyAction( 'b', [&](void*){ data.genRandomPoints(); }, "Re-generate random points" );

	kbloop.addCommonAction( action_NFP );
	action_NFP( &data );
	data.setMouseCB( action_NFP );

	kbloop.start( data );
}
//------------------------------------------------------------------
/// Demo of pts/segments perpendicular to a segment
struct Param_ORS : Data
{
	explicit Param_ORS( int demidx, std::string title ): Data( demidx, title )
	{
		_vcol[0]=img::Color(0,250,125);
		_vcol[1]=img::Color(0,125,250);
		_vcol[2]=img::Color(125,0,250);
		_vcol[3]=img::Color(250,0,125);
	}
	bool _ptsOrSegs = false;
	bool _drawPolyg = false;
	std::array<img::Color,4> _vcol;
};

void action_ORS( void* param )
{
	auto& data = *reinterpret_cast<Param_ORS*>(param);
	data.clearImage();

	auto fl = [&](int i)   // lambda
	{
		return img::DrawParams().setPointStyle(img::PtStyle::Dot).setColor( data._vcol[i] );
	};
	std::function<img::DrawParams(int)> style(fl);

	Segment seg( data.vpt[0], data.vpt[1] );
	seg.draw( data.img, img::DrawParams().setColor( 250,0,0) );
	if( data._ptsOrSegs )
		draw( data.img, seg.getOrthogSegs(), style );
	else
	{
		auto opts = seg.getOrthogPts();
		draw( data.img, opts, style );
		if( data._drawPolyg )
			CPolyline( opts ).draw( data.img, img::DrawParams().setColor( 125,125,0) );
	}
	data.pt_mouse.draw( data.img, img::DrawParams().setColor( 250,0,0) );
	data.showImage();
}

void demo_orthSeg( int demidx )
{
	Param_ORS data( demidx, "Orthogonal segments" );
	std::cout << "Demo " << demidx << ": Orthogonal segments\n(Move the segment with mouse)\n";

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'a', [&](void*){ data._ptsOrSegs=!data._ptsOrSegs; }, "switch mode: points or segments" );
	kbloop.addKeyAction( 'w', [&](void*){ data._drawPolyg=!data._drawPolyg; }, "switch mode: draw polygon in points mode" );

	kbloop.addCommonAction( action_ORS );
	action_ORS( &data );
	data.setMouseCB( action_ORS );

	kbloop.start( data );
}

//------------------------------------------------------------------
struct Param_RCP : Data
{
	explicit Param_RCP( int demidx, std::string title ): Data( demidx, title )
	{}
	size_t nbPts = 5;
};

void action_RCP( void* param )
{
	auto& data = *reinterpret_cast<Param_RCP*>(param);
	data.clearImage();
	CPolyline pol(180, data.nbPts );
	std::cout << " -Building Regular Convex Polygon with " << data.nbPts << " points\n";
	pol.translate(250,200);
	pol.draw( data.img );
	data.showImage();
}

void demo_RCP( int demidx )
{
	Param_RCP data( demidx, "Regular Convex Polygon" );
	std::cout << "Demo " << demidx << ": Regular Convex Polygon\n";

	KeyboardLoop kbloop;
	kbloop.addKeyAction( 'a', [&](void*){ data.nbPts++; }, "more points" );
	kbloop.addKeyAction( 'w', [&](void*){ data.nbPts--; }, "less points" );

	kbloop.addCommonAction( action_RCP );
	action_RCP( &data );
//	data.setMouseCB( action_ORS );

	kbloop.start( data );

}

//------------------------------------------------------------------
/// Demo program, using Opencv.
/**
- if called with no arguments, will switch through all the demos, with SPC
- if called with an (integer) argument, will launch only that demo
*/
int main( int argc, const char** argv )
{
	std::cout << "homog2d graphical demo using Opencv"
		<< "\n - homog version: " << HOMOG2D_VERSION
		<< "\n - build with OpenCV version: " << CV_VERSION << '\n';

		Point2dF pt1;
		std::cout << "float: size=" << pt1.dsize().first << "-" << pt1.dsize().second << '\n';

		Point2dL pt2;
		std::cout << "long: size=" << pt2.dsize().first << "-" << pt2.dsize().second << '\n';

		Point2dD pt3;
		std::cout << "double: size=" << pt3.dsize().first << "-" << pt3.dsize().second << '\n';

	std::vector<std::function<void(int)>> v_demo{
		demo_RCP,
		demo_orthSeg,   // Perpendicular segment
		demo_NFP,   // Nearest/Farthest Point
		demo_RI,    // rectangle intersection
		demo_CIR,
		demo_CH,    // Convex Hull + Minimum Enclosing Circle (MEC)
		demo_SEG,
		demo_B,
		demo_ELL,
		demo_H,
		demo_PL,
		demo_1,
		demo_C,
		demo_SI,
		demo_6,
		demo_polRot // full step rotation of Polyline and rectangle
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
	{
		std::cout << "----------------------------------\n";
		v_demo[i](i+1);
	}
	std::cout << "Demo end\n";
}
