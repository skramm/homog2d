// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	auto H = Homogr().setScale(30).addTranslation(10,30);

	img::Image<cv::Mat> im( 450,200 );

	Circle c( 1.4, 1., 1.5 );
	auto c2 = H*c;
	getBB(c2).draw( im );
	c2.draw( im, DrawParams().setColor(250,0,20) );

	CPolyline cp(
		std::vector<Point2d>{
			{4,1},{6,.2},{5,3.8}
		}
	);
	auto cp2 = H*cp;
	cp2.getBB().draw( im );
	cp2.draw( im, DrawParams().setColor(250,0,20) );

	Ellipse e( 10.,2., 2., 1.2, 25*M_PI/180. );
	e = H*e;
	getBB(e).draw(  im, DrawParams().setColor(10,0,250) );
	getOBB(e).draw( im, DrawParams().setColor(10,250,0) );
	e.draw( im, DrawParams().setColor(250,0,20) );

	cv::imwrite( "getbb1a.png", im.getReal() );

	im.clear();

	getBB( c2,cp2).draw( im, DrawParams().setColor(40,200,40) );
	getBB( c2,e).draw( im, DrawParams().setColor(40,40,200) );
	getBB( cp2,e).draw( im, DrawParams().setColor(200,40,40) );
	e.draw( im, DrawParams().setColor(30,0,20) );
	cp2.draw( im, DrawParams().setColor(30,0,20) );
	c2.draw( im, DrawParams().setColor(30,0,20) );
	cv::imwrite( "getbb1b.png", im.getReal() );
}
