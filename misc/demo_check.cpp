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
