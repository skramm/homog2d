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
\brief This program will demonstrate the variant-based runtime polymorphism capabilities.
Is included in test suite.

Build and run with <code>$ make test-rtp</code>
*/

#define HOMOG2D_ENABLE_VRTP
#include "../../homog2d.hpp"

using namespace h2d;
#include "rtp_v.hpp"

/// see homog2d_test_rtp_2.cpp
int main( int, char** argv )
{
	std::cout << "START " << argv[0] << '\n';

	auto vecpts = std::vector<Point2d>(
		{ {0,0},{3,2},{1,4} }
	);

	img::Image<img::SvgImage> im;
	do_vrtp( vecpts, im );
	im.write( "BUILD/dummy_rtp_2.svg" );
}
