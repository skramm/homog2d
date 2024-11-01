// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"
int main()
{
	std::vector<Point2d> pts{
		{20,20},{40,50},{60,30},{90,45},{120,30}
	};
	OPolyline opl(pts);
	auto H = Homogr().setScale(3).addTranslation(10,20);
//	opl = H * opl;

	img::Image<img::SvgImage> im( 450,250 );

	draw( im, H*opl );
	draw( im, H*pts[2], DrawParams().setPointStyle(PtStyle::Diam) );
	draw( im, H*pts[1], DrawParams().setPointStyle(PtStyle::Dot) );
	draw( im, H*pts[3], DrawParams().setPointStyle(PtStyle::Dot) );

	Point2d pti(0,35);
	Point2d ptl(140,45);
	draw( im, H*Segment(pti,pts.front()) );
	draw( im, H*Segment(ptl,pts.back()) );

	Segment segA( pts[0], pts[3] );
	Segment segB( pts[1], pts[4] );
	draw( im, H*segA, DrawParams().setColor(250,0,0) );
	draw( im, H*segB, DrawParams().setColor(0,250,0) );

	drawText( im, "n-2", H*pts[0] );
	drawText( im, "n-1", H*pts[1] );
	drawText( im, "n",   H*pts[2] );
	drawText( im, "n+1", H*pts[3] );
	drawText( im, "n+2", H*pts[4] );
	im.write( "polyline_minim_1.svg" );

}
