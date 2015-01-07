#include <opencv2/opencv.hpp>
#include <stdio.h>

#ifdef WIN32
	// unix to win porting
	#define	  __func__   __FUNCTION__
	#define	  __PRETTY_FUNCTION__   __FUNCTION__
	#define   snprintf   _snprintf
	#define	  sleep(n_seconds)   Sleep((n_seconds)*1000)
	#define   round(d)   floor((d) + 0.5)
#endif

// includes section

// functions section

void difference_absolue( const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dest)
{
	int r;
	int c;

	int row_count;
	int col_count;

	uchar val1;
	uchar val2;

	for (r = 0; r < src1.rows; ++r)
	{
		for (c = 0; c < src1.cols; ++c)
		{
			val1 = src1.at<uchar>(r, c);
			val2 = src2.at<uchar>(r, c);

			dest.at<uchar>(r, c) = (val1 > val2) ? (val1 - val2) : (val2 - val1);
		}
	}
}
float somme_normalisee(const cv::Mat& src)
{
	int r;
	int c;

	int row_count;
	int col_count;

	uchar val1;
	uchar val2;

	float sum;

	sum = 0;

	for (r = 0; r < src.rows; ++r)
		for (c = 0; c < src.cols; ++c)
			sum += (float)src.at<uchar>(r,c) / 256;

	return (sum / (src.rows * src.cols));
}

bool readVideo( cv::VideoCapture *capture, cv::Mat *out)
{
	bool bOk = capture->grab();
	capture->retrieve(*out);

	return bOk;
}

void showImage( const char* windowName, const cv::Mat *in)
{
	if( in == NULL || ( in->cols == 0 && in->rows == 0 ) )
	{
		// invalid image, display empty image
		const int w = 200;
		const int h = 100;
		cv::Mat img = cv::Mat( h, w, CV_8UC3, cv::Scalar(0));
		cv::line( img, cv::Point( 0, 0), cv::Point( w-1, h-1), cv::Scalar(0,0,255), 2);
		cv::line( img, cv::Point( 0, h-1), cv::Point( w-1, 0), cv::Scalar(0,0,255), 2);
		cv::imshow( windowName, img);
		return;
	}
	else if( in->depth() == CV_32F  ||  in->depth() == CV_64F )
	{
		// float image must be normalized in [0,1] to be displayed
		cv::Mat img;
		cv::normalize( *in, img, 1.0, 0.0, cv::NORM_MINMAX);
		cv::imshow( windowName, img);
		return;
	}

	cv::imshow( windowName, *in);
}

void usage(char* binName)
{
	fprintf(stderr, "%s threshold\r\n", binName);
	exit(-1);
}

int main(int argc, char** argv)
{
	// disable buffer on stdout to make 'printf' outputs
	// immediately displayed in GUI-console
	setbuf(stdout, NULL);

	// initializations section

	cv::VideoCapture capture_block1("/Shared/TP_VTDVR/resources/video.avi");
	if( ! capture_block1.isOpened() )
	{
		printf( "Failed to open %s video file.\n", "/Shared/TP_VTDVR/resources/video.avi");
		return -1;
	}

	int key = 0;
	bool paused = false;
	bool goOn = true;

	float threshold;

	if (argc < 2)
		usage(argv[0]);
	if (sscanf(argv[1], "%f", &threshold) < 0)
		usage(argv[0]);

	cv::Mat prevGrayFrame;
	cv::Mat currentGrayFrame;

	cv::Mat diffFrame;
	cv::Mat currentSceneFirstFrame;

	while (goOn)
	{
		// processings section

		cv::Mat block1_out1;
		if( ! readVideo( &capture_block1, &block1_out1) )
		{
			printf("End of video file.\n");
			break;
		}

		if (currentSceneFirstFrame.empty())
			currentSceneFirstFrame = block1_out1.clone();

		showImage( "block2 (ESC to stop, SPACE to pause)", &block1_out1);

		cv::cvtColor( *(&block1_out1), *(&currentGrayFrame), CV_BGR2GRAY, 0);

		if (diffFrame.empty())
			diffFrame = cv::Mat(block1_out1.rows, block1_out1.cols, CV_8UC1);

		if (!prevGrayFrame.empty())
		{
			difference_absolue(prevGrayFrame, currentGrayFrame, diffFrame);
			if (somme_normalisee(diffFrame) > threshold)
				currentSceneFirstFrame = block1_out1.clone();
		}

		prevGrayFrame = currentGrayFrame.clone();

		showImage( "Current scene first frame (ESC to stop, SPACE to pause)", &currentSceneFirstFrame);

		if( paused )
			key = cv::waitKey(0);
		else
			key = cv::waitKey(25);
		if( (key & 255) == ' ' )
			paused = ! paused;
		else if( key != -1 )
			goOn = false;
	}

	// cleanings section

	return 0;
}

