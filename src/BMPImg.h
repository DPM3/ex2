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

	BMPImg(BMPImg const& copy);
	BMPImg(BMPImg&& copy);
	~BMPImg();
	BMPImg& operator=(BMPImg const& copy);
	BMPImg& operator=(BMPImg&& copy);

	void load(std::string const& path);
	BMPImg makeCopy(std::string const& path);
	void save();
	Color& operator() (int i, int j);
	int width();
	int height();
};
