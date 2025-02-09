
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
