#pragma once

#include<string>

#include"ErrorCode.h"
#include"Matrix.h"
#include"Color.h"

class BMPImg {
public:
	BMPImg() = default;
	BMPImg(std::string const& path);
	void load(std::string const& path);
	void save(std::string const& path);
}
