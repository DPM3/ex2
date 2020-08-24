#include<iostream>
#include<vector>
#include"bmp_tester.hpp"
using namespace testing::bmp;
using namespace std;

int main() {
	vector<string> fileNames = { "lena-color", "persona3", "persona5" };
	for (auto&& name : fileNames) {
		rotate_image("../bmpImages/" + name + ".bmp", "../bmpImages/" + name + "Rot.bmp");
		convert_to_grayscale("../bmpImages/" + name + ".bmp", "../bmpImages/" + name + "Gray.bmp");
	}
}
