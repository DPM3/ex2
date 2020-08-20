#include<iostream>
#include"Color.h"
#include"BMPImg.cpp"
#include"bmp_tester.hpp"
using namespace testing::bmp;

int main() {
	rotate_image("../lena-color.bmp", "../lenaRotated.bmp");
	convert_to_grayscale("../lena-color.bmp", "../lenaGrayScale.bmp");
	BMPImg img {"../lena-color.bmp"};
}
