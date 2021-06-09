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


#include "homog2d.hpp"


//#define NUMTYPE double
#define NUMTYPE long double
#include <random>
//#include <functional>

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
/*
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
*/

struct RandomData
{
	RandomData()
	{
		std::srand( std::time(nullptr) );

	}

	double getRandom( double min=0., double max=1. )
	{
		return ((double) std::rand() / (RAND_MAX+1)) * (max-min+1) + min;
	}
	double getRandomAngle()
	{
		return getRandom( 0., 359. );
	}
	double getRandomTranslation()
	{
		auto t= getRandom() * std::pow( 10., getRandom(1.,15. ) );
		std::cout << "translation=" << t << '\n';
		return t;
	}
	double getRandomScale()
	{
		auto t= std::pow( 2., getRandom(1.,15. ) );
		std::cout << "scale=" << t << '\n';
		return t;

	}
	Point2d_<NUMTYPE> getRandomPt()
	{
		Point2d_<NUMTYPE> pt;
		return pt;
	}


};

RandomData rd;


//==================================================================
int main( int argc, const char** argv )
{
	int nbTransfo = 20;
	int nbPts = 100;



	for( int i=0; i<nbTransfo; i++ )
	{
		auto angle = rd.getRandomAngle();
		auto tx = rd.getRandomTranslation();
		auto ty = rd.getRandomTranslation();
		auto sx = rd.getRandomScale();
		auto sy = rd.getRandomScale();
		std::cout << i << ": angle=" << angle
			<< " tx=" << tx << " ty=" << ty
			<< " sx=" << sx << " sy=" << sy
			<< '\n';

		for( int order=0; order<6; order++ )
		{

			Hmatrix_<type::IsHomogr,NUMTYPE> H,HMT;

			auto str = getString( static_cast<Order>(order) );
			for( int c=0; c<3; c++ )
			switch( str[c] )
			{
				case 'R': H.addRotation( angle*M_PI/180. ); break;
				case 'T': H.addTranslation( tx, ty );
				case 'S': H.addScale( sx, sy );
			}
			HMT = H;
			HMT.inverse().transpose();

			for( int j=0; j<nbPts; j++ )
			{
				auto pt1 = rd.getRandomPt();
				auto pt2 = rd.getRandomPt();
				auto lA = pt1 * pt2;
				auto pt = H * pt1;
				auto lB = HMT * lA;
				auto d = pt.distTo( lB );
				if( d != 0)
					d == std::log10( d );
				std::cout << j << ":" << d << '\n';
			}
		}
	}
}
