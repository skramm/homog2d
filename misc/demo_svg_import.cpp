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
\brief demo of reading svg files (polylines only)
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#define HOMOG2D_USE_SVG_IMPORT
#include "../homog2d.hpp"
#include "tinyxml2.h"
// additional Opencv header, needed for GUI stuff
#include "opencv2/highgui.hpp"

using namespace h2d;
//using namespace h2d::img;

int main( int argc, const char** argv )
{
	tinyxml2::XMLDocument doc;
	if( argc < 2 )
	{
		std::cout << "arg missing!\n";
		return 1;
	}
	doc.LoadFile( argv[1] );

	img::Image<cv::Mat> im( 500,500 );
	tinyxml2::XMLElement* e2 = doc.FirstChildElement( "svg" )->FirstChildElement( "g" );

	int i = 0;
	for( tinyxml2::XMLElement* child = e2->FirstChildElement("path"); child != NULL; child = child->NextSiblingElement())
	{
	    const char *d; // = s.data();
        child->QueryAttribute("d", &d );
//        std::cout << "data=" << d << "\n";

		auto v_polyline = svg::parsePath( std::string(d) );
		img::draw( im, v_polyline );
		im.write( "BUILD/poly_" + std::to_string(i++) + ".png" );
	}