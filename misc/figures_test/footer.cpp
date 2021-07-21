
	Homogr H;
	H.addScale( 100 );
	H.addTranslation( 20,20 );
	auto r1b = H*r1;
	auto r2b = H*r2;
	img = cv::Scalar(255,255,255);
	r1b.draw( img, CvDrawParams().setColor(250,0,20) );
	r2b.draw( img, CvDrawParams().setColor(20,0,250) );
	cv::imwrite( std::string(argv[0])+ ".png", img );
}

