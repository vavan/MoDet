#ifndef _GRIDMASK_H_
#define _GRIDMASK_H_
#include <list>
#include <string>
#include <opencv/cv.h>

typedef std::list< std::pair<int,int> > MaskInput;

/*
 * Grid mask for selection. Only motion inside selected area is detected
 */
class GridMask
{
private:
    cv::Size size;
    MaskInput mi;
    cv::Mat mask;

public:
    //Ctors
    GridMask() {};
    GridMask(cv::Size size, MaskInput mi)
    {
        this->size = size;
        this->mi = mi;
    }
    //Mask getter
    cv::Mat get() 
    {
        return this->mask;
    };
    //Build the cv::Mat based on give FRAME SIZE
    void build(cv::Size frameSize);

    //Create GridMask object based on selection received from backend
    static GridMask create(std::string json);
};


#endif //_GRIDMASK_H_
