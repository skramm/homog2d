// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig

#include "fig_src.header"

int main()
{
	FRect r1( 20,20, 150,100 );
	FRect r2( 80,70, 180,130 );
	img::Image<SvgImage> im( 250,160 );

	r1.draw( im, DrawParams().setColor(250,0,20).setAttrString("fill=\"rgb(100,200,150)\"") );
	r2.draw( im, DrawParams().setColor(20,0,250) );
	im.write( "demo_svg_attrib.svg" );
}
