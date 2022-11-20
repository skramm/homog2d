// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

template<typename T>
void process( img::Image<img::SvgImage>& im, const T& t, std::string fn )
{
	im.clear();
	draw( im, t, DrawParams().setColor(250,0,20) );
	auto hull = convexHull( t );
	hull.draw( im );
	im.write( fn );
}

int main()
{
	std::vector<Point2d> vpts{
		{1,1},
		{5,0},
		{1,-1},
		{0,-5},
		{-1,-1},
		{-6,0},
		{-1,1},
		{0,6}
	};
	CPolyline poly(vpts);

	auto H = Homogr().addTranslation(7,7).addScale(20);
	poly = H * poly;
	H.applyTo( vpts );
	img::Image<img::SvgImage> im( 300,300 );

	process( im, vpts, "convex-hull-1.svg" );
	process( im, poly, "convex-hull-2.svg" );
}
