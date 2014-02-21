#include <iostream>
#include <stdlib.h>
#include <opencv/cv.h>
#include "json/json.h"
#include "gridmask.h"

using namespace cv;
using namespace std;


Json::Value json_parse(std::string raw_data)
{
    Json::Value root;   
    Json::Reader reader;

    if ( !reader.parse( raw_data, root ) )
    {
        std::cout  << "MotDet Error: Json parsing:" << std::endl
                   << reader.getFormattedErrorMessages();
        exit(1);
    }
    return root;
}


void GridMask::build(Size frameSize) 
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

GridMask GridMask::create(string json)
{
    Json::Value jvalue = json_parse(json);
    Size size(jvalue.get("cols", 0).asInt(), jvalue.get("rows", 0).asInt());   
        
    Json::Value selections = jvalue["selections"];
    MaskInput mi;
    pair<int, int> item;
    for ( int i = 0; i < selections.size(); ++i )
    {
        item.first = selections[i].get("cols", 0).asInt();
        item.second = selections[i].get("rows", 0).asInt();
        mi.push_back(item);
    }

    return GridMask( size, mi ); 
}


