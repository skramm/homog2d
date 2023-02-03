// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

struct LocalData
{
	Circle    c1;
	Ellipse   e1;
	Segment   seg;
	FRect     r1;
	OPolyline opl;
	CPolyline cpl;
	Line2d    line;

	template<typename T>
	void drawStuff( T& im )
	{
		cpl.draw(  im, DrawParams().setColor(0,150,150) );
		opl.draw(  im, DrawParams().setColor(150,150,0).setPointStyle(PtStyle::Dot) );
		c1.draw(   im, DrawParams().setColor(0,0,100) );
		e1.draw(   im, DrawParams().setColor(200,0,0) );
		seg.draw(  im, DrawParams().setColor(150,0,150) );
		r1.draw(   im, DrawParams().setColor(150,200,0) );
		line.draw( im, DrawParams().setColor(0,250,0) );
		drawText( im, "Some Text, default size (20)", Point2d(50,50) );
		drawText( im, "color, size=30px", Point2d(160,100), DrawParams().setFontSize(30).setColor(250,120,0) );

		DrawParams dp_pts;
		auto it1 = c1.intersects( line );
		draw( im, it1.get() );

		auto it2 = c1.intersects( seg );
		draw( im, it2.get() );
	}
};

int main()
{
	img::DrawParams dp;
//	dp.setThickness(1).showPoints().showIndex();
	dp.setThickness(1).showPoints();
	dp.setDefault();

	LocalData data;
	data.c1   = Circle( Point2d(280,200), 60 );
	data.e1   = Ellipse( Point2d(150,100), 60, 15, 20.*M_PI/180. );
	data.seg  = getSegment( data.c1, data.e1 );
	data.r1   = FRect( 40, 30, 220, 112 );
	data.line = Line2d( 22,238,280,178);

	img::Image<SvgImage> im1( 400,300 );
	img::Image<cv::Mat>  im2( 400,300 );

	std::vector<Point2d> vpts{
		{50,10},
		{70,140},
		{180, 110},
		{150, 85},
		{120,190}
	};

	data.opl.set(vpts);
	data.cpl.set(vpts);
	data.opl.translate( 140, 105 );
	data.opl.rotate( Rotate::CCW, Point2d(220,180) );

	data.drawStuff( im1 );
	data.drawStuff( im2 );

	Point2d pt( 40, 280 );
	dp.setColor( 250,0,0);
	img::PtStyle newPointStyle = img::PtStyle::Plus;
	for( int i=0;i<=(int)img::PtStyle::Dot; i++ )
	{
//		std::cout << i << " color=" << dp.color() << '\n';
		pt.draw( im1, dp.setPointStyle( newPointStyle ) );
		pt.draw( im2, dp.setPointStyle( newPointStyle ) );
		newPointStyle = dp._dpValues.nextPointStyle();
		pt.translate(30,0);
	}

	im1.write( "comparison_1.svg" );
	im2.write( "comparison_2.png" );
}
