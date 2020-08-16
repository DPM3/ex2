#pragma once

#include<string>

#include"Color.h"

class BMPImg {
	unsigned int m_width, m_height;
	Color* m_data;
public:
	BMPImg() = default;
	BMPImg(unsigned int width, unsigned int height);
	BMPImg(std::string const& path);
	void load(std::string const& path);
	void save(std::string const& path);
	Color& operator() (int i, int j);
};
