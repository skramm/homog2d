// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	img::Image<cv::Mat> im( 380,200 );
	FRect r1( 110,80, 180,110 );

	for( auto seg: r1.getSegs() )
		seg.getLine().draw( im, DrawParams().setColor(220,220,220) );

	r1.draw( im, DrawParams().setColor(250,0,20) );
	r1.getExtended().draw( im, DrawParams().setColor(20,0,250) );

	draw( im, r1.getDiagonals(), DrawParams().setColor(20,240,20) );
	im.write( "frect_extended.png" );
}
