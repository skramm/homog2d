// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"


int main()
{
	img::DrawParams dp;
//	dp.setThickness(1).showPoints().showIndex();
	dp.setThickness(1).showPoints();
	dp.setDefault();

	Circle c1( Point2d(300,200), 60 );
	Ellipse e1( Point2d(150,100), 60, 15, 20.*M_PI/180. );
	auto seg = getSegment( c1, e1 );
	FRect r1( 40, 30, 220, 112 );

	img::Image<SvgImage> im1( 450,300 );
	img::Image<cv::Mat>  im2( 450,300 );

	std::vector<Point2d> vpts{
		{50,10},
		{70,140},
		{180, 110},
		{150, 85},
		{120,190}
	};
	OPolyline opl(vpts);
	CPolyline cpl(vpts);
	opl.translate( 140, 105 );
	opl.rotate( Rotate::CCW, Point2d(220,180) );

	cpl.draw( im1, DrawParams().setColor(0,150,150) );
	opl.draw( im1, DrawParams().setColor(150,150,0) );
	c1.draw(  im1, DrawParams().setColor(0,0,100) );
	e1.draw(  im1, DrawParams().setColor(200,0,0) );
	seg.draw( im1, DrawParams().setColor(150,0,150) );
	r1.draw(  im1, DrawParams().setColor(150,200,0) );

	cpl.draw( im2, DrawParams().setColor(0,150,150) );
	opl.draw( im2, DrawParams().setColor(150,150,0) );
	c1.draw(  im2, DrawParams().setColor(0,0,100) );
	e1.draw(  im2, DrawParams().setColor(200,0,0) );
	seg.draw( im2, DrawParams().setColor(150,0,150) );
	r1.draw(  im2, DrawParams().setColor(150,200,0) );

	Point2d pt( 40, 280 );
	dp.setColor( 250,0,0);
	img::PtStyle newPointStyle = img::PtStyle::Plus;
	for( int i=0;i<=(int)img::PtStyle::Dot; i++ )
	{
		std::cout << i << " color=" << dp.color() << '\n';
		pt.draw( im1, dp.setPointStyle( newPointStyle ) );
		pt.draw( im2, dp.setPointStyle( newPointStyle ) );
		newPointStyle = dp._dpValues.nextPointStyle();
		pt.translate(30,0);
	}

	Line2d( 22,238,280,178).draw( im1 );
	Line2d( 22,238,280,178).draw( im2 );

	im1.write( "comparison_1.svg" );
	im2.write( "comparison_2.png" );
}
