/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019-2025 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr

    Licence: MPL v2

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at https://mozilla.org/MPL/2.0/.

**************************************************************************/

/**
\file
\brief Demo of reading svg files. Build with `$ make demo-import`

Then run with
`$ BUILD/demo_svg_import misc/test_files/France.svg`
or
`$ BUILD/demo_svg_import misc/test_files/France_Normandie.svg`

This will generate a svg file in current folder that is a copy of what was read in the file.
*/

//#define HOMOG2D_DEBUGMODE
#define HOMOG2D_USE_SVG_IMPORT
#include "../../homog2d.hpp"

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

	std::pair<double,double> imSize(500.,500.);
	try
	{
		imSize = svg::getImgSize( doc );
		std::cout << "Input image size: " << imSize.first << " x " << imSize.second << '\n';
	}
	catch( const std::exception& error )
	{
		std::cout << "input file has no size, size set to 500x500\n -msg=" << error.what() << '\n';
	}

	h2d::svg::Visitor visitor;
	doc.Accept( &visitor );
	const auto& data = visitor.get();
	std::cout << argv[0] << ": Read " << data.size() << " shapes in file '" << argv[1] << "'\n";
	if( !data.size() )
	{
		std::cout << "...exiting, no data!\n";
		return 1;
	}

	img::Image<img::SvgImage> out( imSize.first, imSize.second );
	fct::DrawFunct<img::SvgImage> dfunc( out );

	auto vcolors = img::genRandomColors( data.size() );
	PointPair pp_all;
	size_t c = 0;
	for( const auto& e: data )
	{
		std::visit( dfunc, e );   // draw element

		auto ptpair = std::visit( fct::PtPairFunct{}, e );
		pp_all = getMinMax( pp_all, ptpair );

		std::cout << "Shape " << c << ": " << getString( type(e) )
			<< ", size " << size(e) << "\n";

		if( type(e) != Type::Segment ) // because no BB is defined
		{
			auto bb = std::visit( fct::BBFunct{}, e );
			bb.draw( out, img::DrawParams().setColor(vcolors[c]) );
		}
		c++;
	}
	auto s = FRect(pp_all);
	out.setSize( s.width(), s.height() );
	std::cout << "min/max=" << s << '\n';

	out.write( "demo_import.svg" );
}
