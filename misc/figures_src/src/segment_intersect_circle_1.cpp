// this file is part of homog2d
// used to build a figure that is included in manual
// See makefile target doc_fig
// Requires OpenCv

#include "fig_src.header"

Point2d pa1;
Point2d pa2;
Segment seg;
Circle cir;

void drawit( std::string fname )
{
	img::Image<cv::Mat> img( 350,250 );
	seg.draw( img, DrawParams().setColor(250,50,80).setThickness(2) );
	cir.draw( img, DrawParams().setColor(50,250,80).setThickness(2) );

	seg.getLine().draw( img );

	pa1.draw( img, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(250,50,80).setThickness(4) );
	pa2.draw( img, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(250,50,80).setThickness(4) );

	auto inter = seg.intersects( cir );
	if( inter() )
	{
		auto pts = inter.get();
		for( const auto& pt: pts )
			pt.draw( img, DrawParams().setPointStyle(img::PtStyle::Diam).setColor(20,50,250).setThickness(2) );
	}

	img.write( fname );
}


int main()
{
	pa1.set( 60,40 );
	pa2.set( 210,120 );
	seg.set( pa1, pa2 );
	cir.set( 200,140,65 );
	drawit( "segment_intersect_circle_1a.png" );

	pa1.set( 60,90 );
	pa2.set( 290,120 );
	seg.set( pa1, pa2 );
//	cir.set( 200,140,65 );
	drawit( "segment_intersect_circle_1b.png" );

	pa1.set( 160,120 );
	pa2.set( 240,160 );
	seg.set( pa1, pa2 );
	cir.set( 200,140,85 );
	drawit( "segment_intersect_circle_1c.png" );


}


