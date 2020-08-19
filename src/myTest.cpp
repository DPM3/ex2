#include<iostream>
#include"Color.h"
#include"BMPImg.cpp"
#include"bmp_tester.hpp"
using namespace testing::bmp;

int main() {
	rotate_image("../lena-color.bmp", "../lenaRotated.bmp");
	convert_to_grayscale("../lena-color.bmp", "../lenaGrayScale.bmp");
	BMPImg img {"../lena-color.bmp"};
	std::cout << (int)img(50,50).r() << (int)img(50,50).g() << std::endl;
	std::cout << img.width() << img.height() << std::endl;
	img.save("../lenaSimpleTest.bmp");
	std::string dibHeader = BMPImg::Writer::encodeDIBHeader(24, 512, 512);
	int width, height;
	short bits4pixel;
	int colPltSize;
	BMPImg::Loader::parseDIBHeader((const BMPImg::byte*) dibHeader.c_str(), &width, &height,
	                               &bits4pixel, &colPltSize);
	std::cout << width << height << bits4pixel << colPltSize << std::endl;
	for (int i = 0; i < img.height(); ++i) {
	for (int j = 0; j < img.width(); ++j) {
		img(i,j) = Color(255,255,255);
	}
	}
	img.save("../white.bmp");
}
