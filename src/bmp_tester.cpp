#include "bmp_tester.hpp"
#include "Color.h"
#include "BMPImg.h"

namespace testing {
namespace bmp {

void rotate_image(const std::string& imagePath, const std::string& outputPath) {
	BMPImg iimg = BMPImg(imagePath);

	//The width and height of the input image, or the height and width of the output image
	const int iwidth_oheight = iimg.width();
	const int iheight_owidth = iimg.height();

	BMPImg oimg = BMPImg(iheight_owidth, iwidth_oheight);

	//First we'll transpose the image
	for (int i = 0; i < iwidth_oheight; i++) {
		for (int j = 0; j < iheight_owidth; j++) {
			oimg(j, i) = iimg(i, j);
		}
	}

	//Then we'll reverse each row of the output image, and that'll make a 90 degrees rotation
	for (int i = 0; i < iwidth_oheight; i++) {
		for (int j = 0; j < iheight_owidth / 2; j++) {
			oimg(i, j) = oimg(i, iheight_owidth - j - 1);
		}
	}
	oimg.save(outputPath);
}

void convert_to_grayscale(const std::string& imagePath,
	const std::string& outputPath) {
	BMPImg iimg = BMPImg(imagePath);
	BMPImg oimg {iimg.width(), iimg.height()};
	const int width = oimg.width();
	const int height = oimg.height();

	//Each pixel in the output image will consist of the brightness of the matching pixel
	//in the input image in all 3 of its RGB components
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Color ic = oimg(i, j);
			Color::component_t b = c.brightness();
			Color oc = Color(b, b, b);
			oimg(i, j) = oc;
		}
	}
	oimg.save(outputPath);
}
}
}
