// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	Circle c1( Point2d(120,100), 80 );
	Circle c2( Point2d(300,200), 45 );
	auto seg  = getSegment( c1, c2 );
	auto pseg = getTanSegs( c1, c2 );


	img::Image<SvgImage> im( 450,300 );

	c1.draw( im,  DrawParams().setColor(0,0,100).setAttrString("fill=\"rgb(100,0,0)\"") );
	c2.draw( im,  DrawParams().setColor(0,0,100) );
	seg.draw( im, DrawParams().setColor(250,0,0) );
//	auto lp1 = seg.getOrthonalLine( c1.center() );
	pseg.first.draw( im,  DrawParams().setColor(0,200,100) );

	pseg.second.draw( im, DrawParams().setColor(0,100,200) );
	im.write( "circles1.svg" );
}
