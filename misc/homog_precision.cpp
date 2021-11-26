/**
WIP: evaluate precision of homography on lines
*/


/// Numerical type for object storage
#ifndef NUMTYPE
	#define NUMTYPE double
#endif

#include "../homog2d.hpp"

long double
computeDistTransformedLined( Hmatrix_<type::IsHomogr,NUMTYPE>& H, Point2d_<NUMTYPE> pt1, Point2d_<NUMTYPE> pt2 )
{
	Line2d_<NUMTYPE> line1( pt1, pt2 );
	Point2d_<NUMTYPE> pt = H * pt1; // move the point with H
	Line2d_<NUMTYPE> line2 = H * line1; // move the line with H^{-T}
	return line2.distTo( pt ); // should be 0 !
}

Homogr buildMatrix( int i )
{
	Homogr H;
	return H;
}


int main( int argc, const char** argv )
{
	char sep = ' ';
	int nb_A = 10;
	int nb_B = 15;
	for( int i=0; i<nb_A; i++ )
	{
		auto H = buildMatrix( i );
		auto Hi = H;
		Hi.inverse().transpose();
		for( int j=0; j<nb_B; j++ )
		{
			Point2d_<NUMTYPE> pt1(x,y):

			auto d = computeDistTransformedLined( Hi, pt1, pt2 );
			std::cout << i << sep << j << sep << d << '\n';
		}
		std::cout << '\n';
	}
}
