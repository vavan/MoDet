//https://help.ubuntu.com/community/OpenCV

#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <list>
#include <iostream>
#include <time.h>
#include "url.h"


using namespace cv;
using namespace std;

typedef list< pair<int,int> > MaskInput;




class Mask
{
    private:
    Size size;
    Mat mask;
    MaskInput mi;

    public:
    Mask() {};
    Mask(Size size, MaskInput mi)
    {
        this->size = size;
        this->mi = mi;
    }
    void build(Size frameSize) 
    {
        mask = Mat::zeros(frameSize, CV_8UC1);
 		int row_size = frameSize.height / size.height;
		int col_size = frameSize.width / size.width;
        for(MaskInput::iterator i=mi.begin(); i!=mi.end(); i++)
        {
            int x = (*i).first;
            int y = (*i).second;
            Mat roi(mask, Rect(col_size*x, row_size*y, col_size, row_size));
            roi = Scalar(255);           
        }     
    }
    Mat get() 
    {
        return this->mask;
    };
};

class MotionDetector
{
    private:
        Mat prevFrame;
        Mask mask;
        Mat diff;
        time_t last_detected;


        string streamUrl;
        VideoCapture createCapture();
        void process(InputArray input);

	public:
	    static const int ROWS = 9;
	    static const int COLS = 16;

        void setStream(string streamUrl) {
            this->streamUrl = streamUrl;
        };
        void setMask(Mask & mask)
        {
            this->mask = mask;
        };
        void run();
};


VideoCapture MotionDetector::createCapture()
{
    VideoCapture cap(streamUrl);
    if ( !cap.isOpened() )
    {
        cout << "Cannot open the video stream" << endl;
        exit(2);
    }
    double fps = cap.get(CV_CAP_PROP_FPS); 
    cout << "Frame per seconds : " << fps << endl;
    return cap;
}


void MotionDetector::process(InputArray inputFrame) 
{
    Mat currFrame = inputFrame.getMat();

    GaussianBlur(currFrame, currFrame, Size(9, 9), 2);
    cvtColor(currFrame, currFrame, CV_RGB2GRAY);

    if (prevFrame.empty()) {
        currFrame.copyTo(prevFrame);
        return;
    }

    absdiff(prevFrame, currFrame, diff);
    threshold(diff, diff, 30, 255, THRESH_BINARY);
    currFrame.copyTo(prevFrame);

    Mat filtered;
    diff.copyTo(filtered, mask.get());

    imshow("MD window", filtered);
    int nonZero = countNonZero(filtered);
    if (nonZero > 0) 
    {
        time_t now = time(NULL);
        if (now - last_detected > 5) 
        {
             printf("!!!!\n");
             last_detected = now;
        }
    }
}

void MotionDetector::run() 
{
    try 
    {
        last_detected = time(NULL);
        VideoCapture cap = createCapture();
        namedWindow("MD window", WINDOW_AUTOSIZE); 

        Mat frame;
        int skip = 0;
        while(1)
        {
            if (!cap.grab()) break;
            if (skip++ > 5) 
            {
                if (!cap.retrieve(frame)) break;

                if (mask.get().empty()) 
                {
                    mask.build(frame.size());
                }
                process(frame);
                skip = 0;
            }
            if( waitKey (30) == 27 ) break;
        }
    } 
    catch( cv::Exception& e )
    {
        const char* err_msg = e.what();
        std::cout << "OpenCV exception: " << err_msg << std::endl;
    }
}



MaskInput buildMi()
{
    MaskInput mi;
    pair<int, int> item;
    for (int i = 0; i < 8; i++) 
    {
        for (int j = 0; j < i+5; j++) 
        {
            item.first = j; item.second = i;    mi.push_back(item);
        }
    }
    return mi; 
}


int main() 
{
    Url url;
    string sid = url.get_login("erik100", "pw");  
    cout << "SID:" << sid << endl;

    MotionDetector md = MotionDetector();
    md.setStream("rtmp://192.168.2.100/rtmp/live");
    Mask mask( Size(16, 9), buildMi() );
    md.setMask( mask );
	md.run();
}


