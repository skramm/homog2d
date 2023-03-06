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
\brief Demo of reading svg files. Build with `$ make demo_import`
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
	auto err = doc.LoadFile( argv[1] );
	if( err != tinyxml2::XML_SUCCESS )
	{
		std::cout << "...exiting, failed to read file '" << argv[1]  << "'\n";
		return 1;
	}

	svg::printFileAttrib( doc );

	h2d::svg::Visitor visitor;
	doc.Accept( &visitor );
	const auto& data = visitor.get();
	std::cout << argv[0] << ": Read " << data.size() << " shapes in file '" << argv[1] << "'\n";
	if( !data.size() )
	{
		std::cout << "...exiting, no data!\n";
		return 1;
	}

	img::Image<img::SvgImage> out( 500, 500 );

	for( const auto& e: data )
		e->draw( out );
	out.write( "test.svg" );

	auto c = 0;
	for( const auto& p: data )
	{
		std::cout << ++c << ": " << getString( p->type() )
			<<", length=" << p->length() << ", area=" << p->area() << '\n';
		if( p->type() == Type::Circle )
		{
			const Circle* cir = static_cast<Circle*>( p.get() );
			std::cout << " - Circle radius=" << cir->radius() << '\n';
		}
		if( p->type() == Type::CPolyline )
		{
			const CPolyline* pl = static_cast<CPolyline*>( p.get() );
			std::cout << " - CPolyline: is polygon=" << (pl->isPolygon()?'Y':'N') << '\n';
		}
		std::cout << *p << '\n';
	}
}
