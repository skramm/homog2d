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
\brief precision evaluation, using Opencv
*/

#define HOMOG2D_USE_OPENCV
#include "homog2d.hpp"

// additional Opencv header, needed for GUI stuff
#include "opencv2/highgui.hpp"

//#define NUMTYPE double
#define NUMTYPE long double

#include <functional>

using namespace homog2d;

std::string g_wndname = "homog2d demo";

int g_width = 600;
int g_height = 500;

enum class Order
{
	RST,RTS,TSR,TRS,STR,SRT, Dummy
};

const char* getString( Order order )
{
	switch (order )
	{
		case Order::RST: return "RST";
		case Order::RTS: return "RRS";
		case Order::TSR: return "TSR";
		case Order::TRS: return "TRS";
		case Order::STR: return "STR";
		case Order::SRT: return "SRT";
		default: assert(0);
	}
}

struct Data
{
	cv::Mat img;
	std::vector<Point2d_<NUMTYPE>> vpt;
	Point2d_<NUMTYPE> pt; // projected point
	int selected = -1;
	Point2d_<NUMTYPE> pt_mouse;
	long double max_dist = -25.;
	double _tx = 0.;
	double _ty = 0.;
	double _sx = 1.;
	double _sy = 1.;
	double _angle = 20.;

	double _angle_delta = 5;
	double _translate_delta = 50;
	double _scale_delta = 2;

	Order order;
	Line2d_<NUMTYPE> line1, line2; // source and projected lines

	Hmatrix_<type::IsHomogr,NUMTYPE> H;
	Hmatrix_<type::IsHomogr,NUMTYPE> HMT;

/*	std::array<char,3> order;
	order[0] = 'R';
	order[1] = 'S';
	order[2] = 'T';*/
	long double computeDistTransformedLined();

	void changeOrder()
	{
		auto i = static_cast<int>(order);
		i++;
		order = static_cast<Order>(i);

		if( order ==Order::Dummy )
			order = static_cast<Order>(0);
	}

	void angle( bool a )
	{
		_angle += a ? _angle_delta : -_angle_delta;
	}

	void translate( bool a )
	{
		_tx += a ? _translate_delta : -_translate_delta;
	}
	void scale( bool a )
	{
		_sx *= a ? _scale_delta : 1./_scale_delta;
	}

	void computeH()
	{
		H.init();
		auto str = getString( order );
		for( int i=0; i<3; i++ )
			switch( str[i] )
			{
				case 'R': H.addRotation( _angle*M_PI/180. ); break;
				case 'T': H.addTranslation( _tx, _ty );
				case 'S': H.addScale( _sx, _sy );
			}
		HMT = H;
		HMT.inverse().transpose();
	}


	Data()
	{
		vpt.resize(2);
		reset();

	}
	void reset()
	{
		vpt[0] = Point2d(100,200);
		vpt[1] = Point2d(200,300);
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
	void process();
	friend std::ostream& operator << ( std::ostream& f, const Data& );
};


std::ostream& operator << ( std::ostream& f, const Data& data )
{
	f	<< "order=" << getString( data.order )
		<< " rotation=" << data._angle
		<< " scale=(" << data._sx << "," << data._sy << ") "
		<< " translation=(" << data._tx << "," << data._ty << ") "
		<< '\n';
	return f;
}



/// Computation of the line passed through H^{-T} and computation of
/// the distance between the resulting line and the transformed point.
/// Should be 0, always.
long double
Data::computeDistTransformedLined()
{
	line1 = vpt[0] * vpt[1]; // line from pt1 to pt2
	pt = H * vpt[0]; // move the point with H
	line2 = HMT * line1; // move the line with H^{-T}
	return line2.distTo( pt ); // should be 0 !
}

void draw( Data& data )
{
	data.img = cv::Scalar(255,255,255);

	data.line1.draw( data.img, CvDrawParams().setColor(   0,  0, 250) );
	data.line2.draw( data.img, CvDrawParams().setColor(   0,  250, 0) );
	Segment seg( data.vpt[0], data.pt );
	seg.draw( data.img, CvDrawParams().setColor( 50,  50, 50) );
	data.vpt[0].draw( data.img, CvDrawParams().setColor( 250,  50, 0) );
	data.vpt[1].draw( data.img, CvDrawParams().setColor( 0,  50, 250) );

	cv::imshow( g_wndname, data.img );
}

void Data::process()
{
	auto d = computeDistTransformedLined();
	if( d != 0 )
		d = std::log10(d);
	std::cout << "d=" << d
		<<  " max=" << max_dist
		<< " ratio to eps=" << max_dist / std::numeric_limits<NUMTYPE>::epsilon()
		<< '\n';

	if ( d != 0. && d > max_dist )
		max_dist = d;
	draw( *this );
}


/// Mouse callback function, checks if one of the points is selected.
/**
- If so, that point gets moved by the mouse
*/
void mouse_CB( int event, int x, int y, int /* flags */, void* params )
{
	Data* p_data = reinterpret_cast<Data*>(params);
	draw( *p_data );

	p_data->setMousePos(x,y);

	switch( event )
	{
		case CV_EVENT_LBUTTONUP:
			p_data->selected = -1;
		break;

		case CV_EVENT_LBUTTONDOWN:
			for( int i=0; i<2; i++ )
				if( p_data->pt_mouse.distTo( p_data->vpt[i]) < 10 )  // if mouse is less than 10 pixel away
					p_data->selected = i;
		break;

		case CV_EVENT_MOUSEMOVE:
		{
			if( p_data->selected != -1 )
			{
				p_data->vpt[p_data->selected] = p_data->pt_mouse;
				p_data->process();
//				auto d = p_data->computeDistTransformedLined();
//				std::cout << "d=" << (d==0. ? d : std::log10(d)) << '\n';
			}
		}
		break;

		default: break;
	}
	cv::imshow( g_wndname, p_data->img );
}


//==================================================================
int main( int argc, const char** argv )
{
	Data data;
	cv::namedWindow( g_wndname );
	data.img.create( g_height, g_width, CV_8UC3 );

	data.computeH();
	data.process();

	cv::imshow( g_wndname, data.img );
	cv::setMouseCallback( g_wndname, mouse_CB, &data );

	char key=0;
	while( key != 27 ) // SPC
	{
		bool changed = true;
		switch( key = cv::waitKey(0) )
		{
			case 'm': data.angle(1); break;
			case 'l': data.angle(0); break;

			case 'o': data.scale(1); break;
			case 'p': data.scale(0); break;

			case 'g': data.translate(0); break;
			case 'h': data.translate(1); break;
			case ' ': data.changeOrder(); break;
			default: changed = false; break;
		}
		if( changed )
		{
			data.computeH();
			data.process();
			std::cout << data << '\n';
		}
		cv::imshow( g_wndname, data.img );
	}
}
