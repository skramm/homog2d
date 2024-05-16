/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019-2024 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr

    Licence: MPL v2

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at https://mozilla.org/MPL/2.0/.

**************************************************************************/

/**
\file
\brief This program will demonstrate the variant-based runtime polymorphism capabilities.
Is included in test suite.

Build and run with <code>$ make test_rtp</code>
*/

#define HOMOG2D_ENABLE_VRTP
#include "../homog2d.hpp"

using namespace h2d;

/// see homog2d_test_rtp_2.cpp
int main( int, char** argv )
{
	std::cout << "START " << argv[0] << '\n';

	auto vecpts = std::vector<Point2d>(
		{ {0,0},{3,2},{1,4} }
	);

	CPolyline cpol( vecpts );
	OPolyline opol( vecpts );

	std::vector<CommonType> vec;

	vec.push_back( Circle()  );
	vec.push_back( Segment() );
	vec.push_back( FRect()   );
	vec.push_back( Line2d()  );
	vec.push_back( Point2d() );
	vec.push_back( Ellipse() );
	vec.push_back( cpol      );
	vec.push_back( opol      );

	img::Image<img::SvgImage> im(200,200);

	fct::DrawFunct vde( im );
	auto h = Homogr().addTranslation(3,3).addScale(15);
	for( auto& e: vec )
	{
		std::cout << getString(type(e))
			<< "\n -area=" << area(e);
		if( type(e) != Type::Line2d )
			std::cout <<  "\n -length=" << length(e);
		else
		{
			Line2d li = fct::VariantUnwrapper{e};
			std::cout << "\n -length=N/A (li=" << li << ')';
		}

		std::cout << "\n- data type=" << getString(dtype(e)) << '\n';

		e = transform( h, e );
		std::visit( vde, e );              // then draw

	}
	im.write( "BUILD/dummy2.svg" );
}
