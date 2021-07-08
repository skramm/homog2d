
	img = cv::Scalar(255,255,255);
	r1.draw( img, CvDrawParams().setColor(250,0,20) );
	r2.draw( img, CvDrawParams().setColor(20,0,250) );
	cv::imwrite( "frect1.png", img );
}

