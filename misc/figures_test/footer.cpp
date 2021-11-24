
	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 20,20 );

	img = cv::Scalar(255,255,255);

// grid
	int g=220;
	auto colgrid = img::DrawParams().setColor(g,g,g);
	for (int i=0; i<6; i++ )
	{
		Line2d li( LineDir::H, i );
		auto li2 = H*li;
		li2.draw( img, colgrid );
	}
	for (int i=0; i<7; i++ )
	{
		Line2d li( LineDir::V, i );
		auto li2 = H*li;
		li2.draw( img, colgrid );
	}

	auto r1b = H*r1;
	auto r2b = H*r2;
	r1b.draw( img, img::DrawParams().setColor(250,0,20) );
	r2b.draw( img, img::DrawParams().setColor(20,0,250) );

	cv::imwrite( std::string(argv[0])+ ".png", img );
}

