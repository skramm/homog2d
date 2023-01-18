// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	Segment s1(50,50, 250,50 );
	Segment s2(150,150, 150,50 );
	img::Image<SvgImage> im( 300,200 );
	img::Image<cv::Mat> im2( 300,200 );

	auto style = DrawParams().setColor(250,0,0).setThickness(2);

	Line2d li1( 0,80, 200,120 );
	Point2d pt( 160,50);
	li1.draw( im, style );
	li1.draw( im2, style );
	pt.draw( im, DrawParams().setColor(0,0,250) );
	pt.draw( im2, DrawParams().setColor(0,0,250) );
	drawText( im, "side="+ std::to_string(side( pt, li1 )), Point2d(120,80) );
	drawText( im2, "side="+ std::to_string(side( pt, li1 )), Point2d(120,80) );

	im.write( "side_point_line_1.svg" );
	im2.write( "side_point_line_1.png" );

	im.clear();
	im2.clear();

	Line2d li2( 0,100, 200,90 );
	li2.draw( im, style );
	li2.draw( im2, style );
	pt.draw( im, DrawParams().setColor(0,0,250) );
	pt.draw( im2, DrawParams().setColor(0,0,250) );

	drawText( im, "side="+ std::to_string(side( pt, li2 )), Point2d(120,140) );
	drawText( im2, "side="+ std::to_string(side( pt, li2 )), Point2d(120,140) );

	im.write( "side_point_line_2.svg" );
	im2.write( "side_point_line_2.png" );

	std::cout << "SIDE1=" << side( pt, li1 ) << '\n';
	std::cout << "SIDE2=" << side( pt, li2 ) << '\n';
}
