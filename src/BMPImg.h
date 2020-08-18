#pragma once

#include<string>

#include"Color.h"

class BMPImg {
	int m_width, m_height;
	bool m_usingColorPalette;
	Color* m_colors;
	unsigned char* m_pixelArray;
public:
	BMPImg() = default;
	BMPImg(int width, int height);
	BMPImg(std::string const& path);

	BMPImg(BMPImg const& copy);
	BMPImg(BMPImg&& copy);
	~BMPImg();
	BMPImg& operator=(BMPImg const& copy);
	BMPImg& operator=(BMPImg&& copy);

	void load(std::string const& path);
	void save(std::string const& path);
	Color& operator() (int i, int j);
	int width();
	int height();
};
