/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019-2022 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr

    Licence: MPL v2

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at https://mozilla.org/MPL/2.0/.

**************************************************************************/

/**
\file demo_svg_import.cpp
\brief demo of reading svg files
*/

//#define HOMOG2D_DEBUGMODE
#define HOMOG2D_USE_SVG_IMPORT
#include "../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	tinyxml2::XMLDocument doc;
	if( argc < 2 )
	{
		std::cout << "arg missing!\n";
		return 1;
	}
	doc.LoadFile( argv[1] );

	svg::printFileAttrib( doc );

	h2d::svg::Visitor visitor;
	doc.Accept( &visitor );
	auto data = visitor.get();

	img::Image<img::SvgImage> out( 500,500);

	for( const auto e: data )
		e->draw( out );

	out.write( "test.svg" ) ;
}
