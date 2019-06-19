#pragma once

#include "stdafx.h"
#include "SharedImageBuffer.h"

using std::string;

using namespace cv;
using namespace boost::interprocess;
using namespace boost::detail;


void program_thread(std::stringstream & cmd)
{
	std::system(cmd.str().c_str());
}

void _camera(ti &o) {
	while (1) {
		o.cam.vc >> o.cam.image;

		if (!o.od->index) {
			memcpy(o.od->SIBimage[0]._pMem, o.cam.image.data, o.od->dsz);
		}
		else {
			memcpy(o.od->SIBimage[1]._pMem, o.cam.image.data, o.od->dsz);
		}

		o.od->ready_flag = true;
		boost::detail::Sleep(40);
	}
}