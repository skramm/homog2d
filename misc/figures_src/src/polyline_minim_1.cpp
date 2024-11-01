// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

void drawPoints( img::Image<img::SvgImage>& im, const std::vector<Point2d>& pts, const Homogr& H )
{
	drawText( im, "n-2", H*pts[1] );
	drawText( im, "n-1", H*pts[2] );
	drawText( im, "n",   H*pts[3] );
	drawText( im, "n+1", H*pts[4] );
	drawText( im, "n+2", H*pts[5] );
}

int main()
{
	img::Image<img::SvgImage> im( 500,250 );
	auto H = Homogr().addTranslation(10,5).addScale(3);
	std::vector<Point2d> pts{
		{0,35},{20,20},{40,50},{60,30},{90,45},{120,30},{140,45}
	};
	std::vector<Segment> vsegs;
	for( size_t i=0; i<pts.size()-1; i++ )
		vsegs.emplace_back( H*Segment( pts[i], pts[i+1] ) );

	for( const auto& s: vsegs )
		draw( im, s );

	draw( im, H*pts[3], DrawParams().setPointStyle(PtStyle::Squ) );
	draw( im, H*pts[2], DrawParams().setPointStyle(PtStyle::Dot) );
	draw( im, H*pts[4], DrawParams().setPointStyle(PtStyle::Dot) );

	drawPoints( im, pts, H );
	im.write( "polyline_minim_1a.svg" );

	Segment seg24(H*pts[2], H*pts[4]);
	draw( im, seg24, DrawParams().setColor(150,150,250) );
	auto seg_orth = seg24.getLine().getOrthogSegment( H*pts[3] );
	draw( im, seg_orth, DrawParams().setColor(150,150,250) );
	im.write( "polyline_minim_1b.svg" );

	im.clear();
	for( size_t i=0; i<vsegs.size(); i++ )
		if( i == 2 || i == 3)
			draw( im, vsegs[i], DrawParams().setColor(200,220,200) );
		else
			draw( im, vsegs[i] );

	Segment segA( pts[1], pts[4] );
	Segment segB( pts[2], pts[5] );
	draw( im, H*segA, DrawParams().setColor(250,0,0) );
	draw( im, H*segB, DrawParams().setColor(0,250,0) );
	draw( im, H*Segment( pts[2], pts[4] ), DrawParams().setColor(100,100,250) );

	draw( im, H*pts[2], DrawParams().setPointStyle(PtStyle::Dot) );
	draw( im, H*pts[4], DrawParams().setPointStyle(PtStyle::Dot) );

	drawPoints( im, pts, H );
	im.write( "polyline_minim_1c.svg" );
}
