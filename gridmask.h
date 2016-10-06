#ifndef _GRIDMASK_H_
#define _GRIDMASK_H_
#include <list>
#include <string>
#include <opencv2/core/core.hpp>
#include "tool.h"


typedef std::vector<std::vector<bool> > Matrix;


/*
 * Grid mask for selection. Only motion inside selected area is detected
 */
class GridMask
{
private:
    cv::Mat mask;

    Matrix readEnabled(libconfig::Config& cfg, const cv::Size& gridSize);

public:
    GridMask(cv::Size frameSize);
    GridMask() {};

    cv::Mat get() 
    {
        return this->mask;
    };
};


#endif //_GRIDMASK_H_
