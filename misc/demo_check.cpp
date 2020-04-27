/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr, or http://www.litislab.eu/

    Licence: LGPL v3

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

	See included lgpl.txt and gpl.txt files.

**************************************************************************/

/**
\file demo_check.cpp
\brief Build with test, just there to make sure all required headers are included in header
(because in the test program, including Catch also brings in lots of headers)
*/

#include "homog2d.hpp"

using namespace homog2d;

int main()
{
	Line2d l;
	Point2d pt;
	Homogr h;

	Point2d p1(100,100);
	std::cout << "p1=" << p1 << "\n";
	Point2d p2(200,100);
	std::vector<std::vector<float>> m = {
		{ 1,  0,  0 },
		{ 0,  0, -1 },
		{ 1,  2,  1 }
	};
	Homogr F = m;
	std::cout << "F=\n" << F;
//	Line2d::doLineNormalization(false);


	Line2d lp1 = F * p1;
//	std::cout << "lp1:" << lp1._v[0] << " " << lp1._v[1] << " " << lp1._v[2] << "\n";


	std::cout << "lp1=" << lp1 << "\n";

	Line2d lp2 = F * p2;
	std::cout << "lp2=" << lp2 << "\n";

	Point2d inter = lp1 * lp2;
	std::cout << "inter=" << inter << "\n";
}
