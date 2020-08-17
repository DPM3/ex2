#include"BMPImg.h"
#include<fstream>

BMPImg::BMPImg(std::string const& path) : BMPImg{} {
	load(path);
}

BMPImg::BMPImg(int width, int height) 
	: m_width{width}, m_height{height}, m_data{new Color[width * height]} { }

BMPImg::BMPImg(BMPImg const& copy) {
	*this = copy;
}

BMPImg::BMPImg(BMPImg&& copy) {
	*this = copy;
}
BMPImg::~BMPImg() {
	if (m_data == nullptr) {
		return;
	}
	delete[] m_data;
}

BMPImg& operator=(BMPImg const& copy) {
	load(copy.m_filePath);
}

BMPImg& operator=(BMPImg&& copy) {
	m_width = copy.m_width;
	m_height = copy.m_height;
	copy.m_width = copy.m_height = 0;

	m_data = copy.m_data;
	copy.m_data = nullptr;

	m_filePath = std::move(copy.m_filePath);
}

void load(std::string const& path) {
	m_filePath = path;
	std::ifstream file {path};
	if (!file) {
		//TODO: throw something here.
	}

	//after these 3 lines, s.c_str() brings an array identical to the file,
	//and contentp is a pointer moving around that array.
	std::string s;
	file >> s;
	char* contentp = s.c_str();

	//this function reads the char pointed by contentp, and increments contentp to the next char.
	auto readChar = [](char* *contentp) -> char {
		return *( (*contentp)++ );
	};
	//this function reads the int pointed by contentp, and increments contentp to the next int.
	auto readInt = [](char* *contentp) -> int {
		int& result = *( (int*) *contentp );
		*contentp += sizeof(int) / sizeof(char);
		return result;
	};

	//////////-- The Header --//////////
	if (readChar(&contentp) != 'B' || readChar(&contentp) != 'M') {
		//TODO: throw something here.
	}
	int fileSize = readInt(&contentp);//get file size(array size also)
	contentp += 4; //move past the 4 bytes of reserved content
	int arrayOffset = readInt(&contentp);//get pixel array offset in array

	//////////-- The DIB Header --//////////
	
}

void save(std::string const& path);

Color& operator() (int i, int j) {
	return data[width * i + j];
}
