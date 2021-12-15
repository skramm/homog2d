// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{

	auto H = Homogr().setScale(30).addTranslation(10,30);

	img::Image<cv::Mat> im( 350,250 );

	Circle c( 2,3,1.5);
	auto c2 = H*c;
	c2.draw( im, DrawParams().setColor(250,0,20) );
	getBB(c2).draw( im );

	Segment seg( 4., 3., 5., 1.5 );
	seg = H*seg;
	seg.draw( im, DrawParams().setColor(250,0,20) );
	getBB(seg).draw( im );

	Ellipse e( 6.,2., 2., 1.2 );
	e = H*e;
	e.draw( im, DrawParams().setColor(250,0,20) );
	getBB(e).draw( im );
	cv::imwrite( "getbb1.png", im.getReal() );
}
