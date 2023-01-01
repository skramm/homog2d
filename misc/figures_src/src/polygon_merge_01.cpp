// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

void process( Point2d p0, Point2d p1, Point2d p2, Point2d p3, Point2d p4, std::string fn )
{

	Segment s1(p1,p0);
	Segment s2(p2,p0);

	Segment s3(p3,p0);
	Segment s4(p4,p0);

	img::Image<cv::Mat> im( 400,250 );

	uint8_t g = 180;
	s1.getLine().draw( im, DrawParams().setColor(g,g,g) );
	s2.getLine().draw( im, DrawParams().setColor(g,g,g) );
	s3.getLine().draw( im, DrawParams().setColor(g,g,g) );
	s4.getLine().draw( im, DrawParams().setColor(g,g,g) );

	auto col1 = DrawParams().setColor(250,0,20);
	s1.draw( im, col1 );
	s2.draw( im, col1 );
	p1.draw( im, col1 );
	p2.draw( im, col1 );
	p0.draw( im, col1 );

	auto col2 = DrawParams().setColor(0,250,0);
	s3.draw( im, col2 );
	s4.draw( im, col2 );
	p3.draw( im, col2.setPointStyle( PtStyle::Dot) );
	p4.draw( im, col2.setPointStyle( PtStyle::Dot) );
	p0.draw( im, col2 );

	cv::putText( im.getReal(), "p1", getCvPti(p1), 0,0.6, cv::Scalar(0,0,250));
	cv::putText( im.getReal(), "p2", getCvPti(p2), 0,0.6, cv::Scalar(0,0,250));

	cv::putText( im.getReal(), "p3", getCvPti(p3), 0,0.6, cv::Scalar(0,250,0));
	cv::putText( im.getReal(), "p4", getCvPti(p4), 0,0.6, cv::Scalar(0,250,0));
	im.write( fn );
}

int main()
{
	auto x1=50;
	auto x2=250;

	Point2d p0(x2,100);
	{
		Point2d p1(x1,100);
		Point2d p2(x2+80,30);
		Point2d p3(80,25);
		Point2d p4(210,45);
		process( p0, p1, p2, p3, p4, "polygon_merge_N_01.png" );
	}
	{
		Point2d p1(x1,100);
		Point2d p2(x2-20,220);
		Point2d p3(80,25);
		Point2d p4(210,45);
		process( p0, p1, p2, p3, p4, "polygon_merge_N_02.png" );
	}
	{
		Point2d p1(x1,100);
		Point2d p2(x2-20,220);
		Point2d p3(80,25);
		Point2d p4(290,45);
		process( p0, p1, p2, p3, p4, "polygon_merge_N_03.png" );
	}
	{
		Point2d p1(x1,100);
		Point2d p2(x2+80,30);
		Point2d p3(80,25);
		Point2d p4(310,85);
		process( p0, p1, p2, p3, p4, "polygon_merge_Y_01.png" );
	}
	{
		Point2d p1(x1,100);
		Point2d p2(x2+80,30);
		Point2d p3(130,30);
		Point2d p4(310,120);
		process( p0, p1, p2, p3, p4, "polygon_merge_Y_02.png" );
	}
}
