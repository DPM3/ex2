#pragma once

#include<string>
#include<vector>

#include"Color.h"

class BMPImg {
	int m_width, m_height;
	bool m_usingColPlt;
	std::vector<Color> m_colors;
	std::vector<char> m_vals;
	struct Loader;
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
