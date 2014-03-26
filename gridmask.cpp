#include <iostream>
#include <stdlib.h>
#include <opencv/cv.h>
#include "json/json.h"
#include "gridmask.h"
#include "config.h"

using namespace cv;
using namespace std;


Json::Value json_parse(std::string raw_data)
{
    Json::Value root;   
    Json::Reader reader;

    if ( !reader.parse( raw_data, root ) )
    {
        LOG.error("Json parsing error: "+reader.getFormattedErrorMessages());
    }
    return root;
}


void GridMask::build(Size frameSize) 
{
    mask = Mat::zeros(frameSize, CV_8UC1);

	if (size.width == 0 || size.height == 0) {
		LOG.info("Update mask. Select whole frame");
		Mat roi(mask, Rect(0, 0, frameSize.width, frameSize.height));
		roi = Scalar(255);
	} else {
		int row_size = frameSize.height / size.height;
		int col_size = frameSize.width / size.width;
		LOG.info("Update mask. Cell is %d x %d pixels", col_size, row_size);
		for(MaskInput::iterator i=mi.begin(); i!=mi.end(); i++) {
			int x = (*i).first;
			int y = (*i).second;
			Mat roi(mask, Rect(col_size*x, row_size*y, col_size, row_size));
			roi = Scalar(255);
		}
	}
}

GridMask GridMask::create(string sRoot)
{
    Json::Value root = json_parse(sRoot);
    Json::Value device = root["device"];
    //int code = root.get("code", 0).asInt();
    string sDeviceSetup = device.get("deviceSetup", "").asString();
    Json::Value deviceSetup = json_parse(sDeviceSetup);
    Json::Value motionGrid = deviceSetup["motion_grid"];
    Json::Value selections = motionGrid["selections"];
    int width = motionGrid.get("columns", 0).asInt();
    int height = motionGrid.get("rows", 0).asInt();

    LOG.debug("Grid %d x %d. Selected %d cells", width, height, selections.size());

    MaskInput mi;
    pair<int, int> item;
	for ( unsigned int i = 0; i < selections.size(); ++i )
	{
       item.first = selections[i].get("col", 0).asInt();
       item.second = selections[i].get("row", 0).asInt();
       mi.push_back(item);
	}
    return GridMask( Size(width, height), mi );
}


/*
 * {"device":
 * 	{"id":1,"deviceId":"8484","device_type":"camera","deviceSetup":
 *		"{\"motion_grid\" :
 *			{\"selections\":[{\"row\":1,\"col\":1},{\"row\":2,\"col\":1},{\"row\":3,\"col\":1},{\"row\":4,\"col\":1},{\"row\":1,\"col\":2},{\"row\":2,\"col\":2},{\"row\":3,\"col\":2},{\"row\":4,\"col\":2},{\"row\":1,\"col\":3},{\"row\":2,\"col\":3},{\"row\":3,\"col\":3},{\"row\":4,\"col\":3},{\"row\":1,\"col\":4},{\"row\":2,\"col\":4},{\"row\":3,\"col\":4},{\"row\":4,\"col\":4}],
 *			 \"columns\":10,\"rows\":6
*			}
*		}",
*		"status":true
*	},"code":0,"message":"Get Device is success.."}
 *
 */
