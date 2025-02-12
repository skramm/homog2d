// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig

#include "fig_src.header"

int main()
{
	FRect r1( 50,20, 200,200 );
	FRect r2( 200,20, 300,200 );
	img::Image<img::SvgImage> im( 400,250 );

	r1.draw( im, DrawParams().setColor(250,0,20) );
	r2.draw( im, DrawParams().setColor(20,0,250) );

	auto inter = r1.intersects( r2 );
	if( inter() )
		for( const auto& pt: inter.get() )
			pt.draw( im, DrawParams().setPointStyle(PtStyle::Dot).setPointSize(7).setColor(0,0,0) );

	im.write( "frect2.svg" );
}
