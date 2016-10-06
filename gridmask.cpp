#include <iostream>
#include <stdlib.h>
#include "gridmask.h"

using namespace cv;
using namespace std;
using namespace libconfig;


Matrix GridMask::readEnabled(Config& cfg, const Size& gridSize)
{
	Matrix enabled = Matrix(gridSize.width, vector<bool>(gridSize.height, true));
	if (cfg.exists("grid.blocked")) {
		Setting& values = cfg.lookup("grid.blocked");
		int length = values.getLength();

		for (int i = 0; i < length; i++) {
			Setting& cell = values[i];
			int x = cell[0];
			int y = cell[1];
			if (x < gridSize.width && y < gridSize.height) {
				enabled[x][y] = false;
			}
		}
	}
	return enabled;
}

GridMask::GridMask(Size frameSize)
{
	mask = Mat::zeros(frameSize, CV_8UC1);

	Config& cfg = MDConfig::get();
	Size gridSize(1, 1);
	cfg.lookupValue("grid.size.w", gridSize.width);
	cfg.lookupValue("grid.size.h", gridSize.height);

	Matrix enabled = readEnabled(cfg, gridSize);

	int cellHeight = frameSize.height / gridSize.height;
	int cellWidth = frameSize.width / gridSize.width;
	LOG.debug("Mask cell is %d x %d pixels", cellHeight, cellWidth);
	for (size_t x = 0; x < enabled.size(); x++) {
		for (size_t y = 0; y < enabled[x].size(); y++) {
			if (enabled[x][y]) {

				LOG.debug("Mask enabled cell = %d, %d", x, y);
				Mat roi(mask, Rect(cellWidth*x, cellHeight*y, cellWidth, cellHeight));
				roi = Scalar(255);
			}
		}
	}
}
