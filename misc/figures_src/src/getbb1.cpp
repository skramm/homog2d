// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	auto H = Homogr().setScale(30).addTranslation(10,30);

	img::Image<cv::Mat> im( 450,200 );

	Circle c( 1.3, 1., 1.5 );
	auto c2 = H*c;
	c2.draw( im, DrawParams().setColor(250,0,20) );
	getBB(c2).draw( im );

/*	Segment seg( 3.5, 3., 7., 0.5 );
	seg = H*seg;
	seg.draw( im, DrawParams().setColor(250,0,20) );
	getBB(seg).draw( im );
*/
	Ellipse e( 10.,2., 2., 1.2, 25*M_PI/180. );
	e = H*e;
	e.draw( im, DrawParams().setColor(250,0,20) );
	getBB(e).draw( im );
	e.getOBB().draw( im );
	cv::imwrite( "getbb1.png", im.getReal() );
}
