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
\file
\brief This program will demonstrate the RunTime Polymorphism capabilities using the new std::variant approach
Is included in test suite.

*/


#define CATCH_CONFIG_RUNNER   // alternative: main provided here
#include "catch.hpp"

//#define HOMOG2D_TEST_MODE

#include "../homog2d.hpp"

using namespace h2d;

/// see homog2d_test_rtp_2.cpp
int main( int, char** )
{
	auto vecpts = std::vector<Point2d>(
		{ {0,0},{3,2},{1,4} }
	);

	CPolyline cpol( vecpts );
	OPolyline opol( vecpts );

	std::vector<CommonTyped> vec;

	vec.push_back( Circle()  );
	vec.push_back( Segment() );
	vec.push_back( FRect()   );
//	vec.push_back( Line2d()  );
	vec.push_back( Point2d() );
	vec.push_back( Ellipse() );
	vec.push_back( cpol      );
	vec.push_back( opol      );

	img::Image<img::SvgImage> im(200,200);

	for( auto& e: vec )
	{
		std::cout << getString(getType(e))
			<< "\n -area=" << area(e)
			<< "\n -length=" << length(e)
			<< "\n";
		img::DrawFunct vde( im );
		std::visit( vde, e );
/*		 << *e
			<< "\n  -area = " << e->area();
		if( e->type() != Type::Line2d )
			std::cout << "\n  -length = " << e->length();
		else
			std::cout << "\n  -length = infinite";
		std::cout << '\n';
		e->draw( im );

		if( e->type() == Type::CPolyline )
		{
			auto pl1 = std::dynamic_pointer_cast<CPolyline>( e );
			std::cout << "pl1 is closed=" << pl1->isClosed() << '\n';
*/
	}
	im.write( "BUILD/dummy2.svg" );
}
