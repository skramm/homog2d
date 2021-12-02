// file t_footer.cxx
// used in makefile target test_fig

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 20,20 );

// grid
	int g=220, g0=g/2;
	auto colgrid  = img::DrawParams().setColor(g,g,g);
	auto colgrid0 = img::DrawParams().setColor(g0,g0,g0).setThickness(2);
	for (int i=0; i<6; i++ )
	{
		Line2d li( LineDir::H, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( img2, colgrid0 );
		else
			li2.draw( img2, colgrid );
	}
	for (int i=0; i<7; i++ )
	{
		Line2d li( LineDir::V, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( img2, colgrid0 );
		else
			li2.draw( img2, colgrid );
	}

	auto r1b = H*r1;
	auto r2b = H*r2;

	cv::putText( img2.getReal() , "r1", getBB(r1b).getPts().second.getCvPti(), 1, 2., cv::Scalar(20,0,250) );
	cv::putText( img2.getReal() , "r2", getBB(r2b).getPts().second.getCvPti(), 1, 2., cv::Scalar(250,0,20) );

	r1b.draw( img2, img::DrawParams().setColor(250,0,20) );
	r2b.draw( img2, img::DrawParams().setColor(20,0,250) );

	cv::imwrite( std::string(argv[0])+ ".png", img2.getReal() );
}

