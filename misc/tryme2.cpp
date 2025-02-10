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

#include "homog2d.hpp"
using namespace h2d;
using namespace h2d::img;

template<typename T1,typename T2>
void
drawPts( Image<SvgImage>& im, const T1& shape1, const T2& shape2 )
{
	auto x = shape1.intersects(shape2);
	if( x() )
		draw( im, x.get() );
}

int main()
{
	Image<SvgImage> im(300,250);

	Line2d li( Point2d(30,50), Point2d(100,80) );
	li.draw( im, DrawParams().setColor(0,0,250) );

	FRect r( 40,30,200,180 );
	auto r2 = Homogr(-5.*M_PI/180.) * r;
	r2.draw( im, DrawParams().setColor(200,0,0) );

	Circle cir( 162, 141, 65 );
	cir.draw( im, DrawParams().setColor(0,250,0) );

	drawPts( im, li, cir );
	drawPts( im, li, r2 );
	drawPts( im, r2, cir );

	std::cout << im;
}
