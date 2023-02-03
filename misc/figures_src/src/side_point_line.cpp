// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	img::Image<SvgImage> im( 300,200 );
	auto style = DrawParams().setColor(250,0,0).setThickness(2);

	Line2d li1( 0,80, 200,120 );
	Point2d pt( 160,50);
	li1.draw( im, style );
	pt.draw( im, DrawParams().setColor(0,0,250) );
	drawText( im, "side="+ std::to_string(side( pt, li1 )), Point2d(40,80), DrawParams().setFontSize(40) );
	im.write( "side_point_line_1.svg" );

	im.clear();

	Line2d li2( 0,100, 200,90 );
	li2.draw( im, style );
	pt.draw( im, DrawParams().setColor(0,0,250) );

	drawText( im, "side="+ std::to_string(side( pt, li2 )), Point2d(120,140), DrawParams().setFontSize(40) );

	im.write( "side_point_line_2.svg" );
}
