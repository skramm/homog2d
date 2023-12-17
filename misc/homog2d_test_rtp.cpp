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
\file homog2d_test_rtp.cpp
\brief This program will test the RunTime Polymorphism capabilities
*/


#define CATCH_CONFIG_RUNNER   // alternative: main provided here
#include "catch.hpp"

#define HOMOG2D_TEST_MODE
#define HOMOG2D_ENABLE_RTP
#include "../homog2d.hpp"

using namespace h2d;

int main( int, char** )
{
	Segment seg;
	CPolyline pol(
		std::vector<Point2d>(
			{ {0,0},{3,2},{1,4} }
		)
	);
	Circle cir;
	FRect rect;
	img::Image<img::SvgImage> im;
	std::vector<std::shared_ptr<rtp::Root>> vec;
	vec.push_back( std::make_shared<Circle>(cir) );
	vec.push_back( std::make_shared<CPolyline>(pol) );
	vec.push_back( std::make_shared<Segment>(seg) );
	vec.push_back( std::make_shared<FRect>(rect) );

	for( auto& e: vec )
	{
		std::cout << getString(e->type()) << ": " << *e
			<< "\n  -area = " << e->area()
			<< "\n  -length = " << e->length()
			<< '\n';
		e->draw( im );

/*		if( e->type() == Type::CPolyline )
		{
			auto pl1 = std::dynamic_pointer_cast<CPolyline>( e );
			std::cout << "pl1 is closed=" << pl1->isClosed() << '\n';

			auto pl2 = std::dynamic_pointer_cast<OPolyline>( e );    // warning: failure here, bad cast!
			std::cout << "pl2 is closed=" << pl2->isClosed() << '\n';
			std::cout << "pl2 area=" << pl2->area() << '\n';

		}
*/
	}
	im.write( "BUILD/dummy.svg" );
}
