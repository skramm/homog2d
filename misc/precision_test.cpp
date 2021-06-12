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
\brief precision evaluation

Computation of the line passed through H^{-T} and computation of
the distance between the resulting line and the transformed point.
Should be 0, always.

We do this for multiple random transformation, in random order, and with multiple random points

*/


#include "homog2d.hpp"


#define NUMTYPE double
//#define NUMTYPE long double
#include <random>


using namespace homog2d;

/// Order of transformations
enum class Order
{
	RST,RTS,TSR,TRS,STR,SRT, Dummy
};

const char* getString( Order order )
{
	switch (order )
	{
		case Order::RST: return "RST";
		case Order::RTS: return "RTS";
		case Order::TSR: return "TSR";
		case Order::TRS: return "TRS";
		case Order::STR: return "STR";
		case Order::SRT: return "SRT";
		default: assert(0);
	}
}

struct RandomData
{
	RandomData()
	{
		std::srand( std::time(nullptr) );

	}

	double getRandom( double min=0., double max=1. )
	{
		return (double) std::rand() / RAND_MAX * (max-min+1) + min;
	}
	double getRandomAngle()
	{
		return getRandom( 0., 359. );
	}
	double getRandomTranslation()
	{
		auto t = getRandom() * std::pow( 10., getRandom(1.,15. ) );
		std::cout << "translation=" << t << '\n';
		return t;
	}
	double getRandomScale()
	{
		auto t= std::pow( 2., getRandom(1.,10. ) );
		std::cout << "scale=" << t << '\n';
		return t;
	}
	Point2d_<NUMTYPE> getRandomPt()
	{
		Point2d_<NUMTYPE> pt(
			getRandom() * std::pow( 10., getRandom(0.,10. ) ),
			getRandom() * std::pow( 10., getRandom(0.,10. ) )
		);
		return pt;
	}
};



//==================================================================
int main( int argc, const char** argv )
{
	RandomData rd;

	int nbTransfo = 5;
	int nbPts = 20;

/*	for( int i=0; i<30; i++ )
		std::cout << rd.getRandom() << "\n";
	return 0;*/

	for( int i=0; i<nbTransfo; i++ )
	{
		auto angle = rd.getRandomAngle();
		auto tx = rd.getRandomTranslation();
		auto ty = rd.getRandomTranslation();
		auto sx = rd.getRandomScale();
		auto sy = rd.getRandomScale();
		std::cout << std::scientific << i << ": angle=" << angle
			<< " tx=" << tx << " ty=" << ty
			<< " sx=" << sx << " sy=" << sy
			<< '\n';

		for( int order=0; order<6; order++ )
		{
			Hmatrix_<type::IsHomogr,NUMTYPE> H;

			auto str = getString( static_cast<Order>(order) );
			std::cout << "order = " << str << '\n';
			for( int c=0; c<3; c++ )
				switch( str[c] )
				{
					case 'R': H.addRotation( angle*M_PI/180. ); break;
					case 'T': H.addTranslation( tx, ty );
					case 'S': H.addScale( sx, sy );
				}
//			std::cout << H << '\n';
			Hmatrix_<type::IsHomogr,NUMTYPE> HMT = H;
			HMT.inverse().transpose();
			double dmin = 999.;
			double dmax = 0.;
			for( int j=0; j<nbPts; j++ )
			{
				auto pt1 = rd.getRandomPt();
				auto pt2 = rd.getRandomPt();
				auto lA = pt1 * pt2;
				auto pt = H * pt1;    // project point with H
				auto lB = HMT * lA;   // project line with H^-T
				auto d = pt.distTo( lB );
				auto dlog = d;
//				if( d != 0. )
//					dlog = std::log10(d);
#if 0
				std::cout << j
//					<< std::fixed
					<< std::scientific
					<< std::setprecision(2)
					<< ": pt1=" << pt1
					<< ", pt2=" << pt2
					<< " dist pts=" << pt1.distTo(pt2)
					<< "=> dH=" << dlog
					<< " ratio=" << pt1.distTo(pt2) / dlog
					<< '\n';
#endif
				if( d != 0. )
				{
					dmax = std::max( d, dmax );
					dmin = std::min( d, dmin );
				}

			}
			std::cout << "dist: min=" << dmin <<  " max=" << dmax << '\n';
		}
	}
}
