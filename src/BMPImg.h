#pragma once

#include<string>

#include"Color.h"

class BMPImg {
	int m_width, m_height;
	Color* m_data;
	string m_filePath;
public:
	BMPImg() = default;
	BMPImg(int width, int height);
	BMPImg(std::string const& path);
	void load(std::string const& path);
	void save();
	Color& operator() (int i, int j);
	int width();
	int height();
};
