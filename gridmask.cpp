#include <iostream>
#include <stdlib.h>
#include "url.h"
#include "gridmask.h"
#include "tool.h"

using namespace cv;
using namespace std;




void GridMask::build(Size frameSize)
{
	if (size.height == 0 || size.width == 0) {
		LOG.error("Empty mask");
		return;
	}

	mask = Mat::zeros(frameSize, CV_8UC1);
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
