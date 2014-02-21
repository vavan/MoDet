#ifndef _GRIDMASK_H_
#define _GRIDMASK_H_
#include <list>
#include <string>
#include <opencv/cv.h>

typedef std::list< std::pair<int,int> > MaskInput;

class GridMask
{
private:
    cv::Size size;
    MaskInput mi;
    cv::Mat mask;

public:
    GridMask() {};
    GridMask(cv::Size size, MaskInput mi)
    {
        this->size = size;
        this->mi = mi;
    }
    cv::Mat get() 
    {
        return this->mask;
    };
    void build(cv::Size frameSize);
    static GridMask create(std::string json);
};


#endif //_GRIDMASK_H_
