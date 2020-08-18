#include"BMPImg.h"
#include<fstream>
#include<cmath>

struct BMPImg::Loader {
	//exceptions:
	class CorruptBMPFile { };

	Loader() = default;

	static char readChar(char* *content) {
		return *( (*content)++ );
	}
	static int readInt(char* *content) {
		int& result = *( (int*) *content );
		*content += sizeof(int) / sizeof(char);
		return result;
	}
	static short readShort(char* *content) {
		short& result = *( (short*) *content );
		*content += sizeof(short) / sizeof(char);
		return result;
	}

	static std::string loadFile(std::string const& path) {
		std::ifstream file {path};
		if (!file) {
			//TODO: throw something here.
		}

		file >> content;
		return content;
	}

	static void parseHeader(char* content, int *fileSize, int *pixelArrOffset) {
		if (readChar(&content) != 'B' || readChar(&content) != 'M') {
			throw CorruptBMPFile{};
		}

		if (fileSize != nullptr) {
			*fileSize = readInt(&content);
		}

		//reserved bytes:
		readShort(&content);
		readShort(&content);

		if (pixelArrOffset != nullptr) {
			*pixelArrOffset = readInt(&content);
		}
	}

	static void parseDIBHeader(char* content, int *bmapWidth, int *bmapHeight,
	                    short *bits4Pixel, int *colPltSize) {
		int DIBHeaderSize = readInt(&content);
		if (DIBHeaderSize != 40) {
			throw CorruptBMPFile{};
		}

		if (bmapWidth != nullptr) {
			*bmapWidth = readInt(&content);
		}
		if (bmapHeight != nullptr) {
			*bmapHeight = readInt(&content);
		}

		if (readShort(&content) != 1) {
			throw CorruptBMPFile{};
		}

		if (bits4Pixel != nullptr) {
			*bits4Pixel = readShort(&content);
		}

		content += 16; //skip compression and real-world scale data

		if (colPltSize != nullptr) {
			*colPltSize = readInt(&content);
			if (*colPltSize == 0) {
				*colPltSize = std::pow(2, *bits4Pixel);
			}
		}
	}

	static std::vector<Color> parseColPlt(char* content, int pltSize) {
		if (colPlt == nullptr) { return; }

		std::vector<Color> result {pltSize};

		for (int i = 0; i < pltSize; ++i) {
			result.push_back(Color{readChar(&content), readChar(&content), readChar(&content)});
			++content;//skip the redundant 0 every 4'th element
		}
		return result;
	}

	static vector<char> parsePixelArr8(char* content, int width, int height) {
		int padding = 4 - ((width) % 4);
		int rowWidth = width + padding;
		vector<char> result {rowWidth * height};

		for (int i = 0; i < rowWidth * height; ++i) {
			if (i % rowWidth == width) {
				i += padding;
			}
			result.push_back(content[i]);
		}
		return result;
	}

	static vector<Color> parsePixelArr24(char* content, int width, int height) {
		int padding = 4 - ((width * 3) % 4);
		int rowWidth = width + padding;
		vector<Color> result {rowWidth * height};

		for (int i = 0; i < rowWidth * height; i += 4) {
			if (i % rowWidth == width) {
				i += padding;
			}
			result.push_back(Color{content[i], content[i + 1], content[i + 2]});
		}
		return result;
	}
};

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
	std::string s = Loader::loadFile(path);
	char* contentp = s.c_str();

	//////////-- The Header --//////////
	int fileSize, arrayOffset;
	Loader::parseHeader(contentp, &fileSize, &arrayOffset);
	contentp += 14;

	//////////-- The DIB Header --//////////
	m_usingColPlt = true;
	int bits4pixel, paletteSize;
	Loader::parseDIBHeader(contentp, &m_width, &m_height, &bits4pixel, &paletteSize);
	m_usingColPlt = bits4pixel == 8;
	contentp += 40;

	//////////-- The Color Palette --//////////
	if (m_usingColPlt) {
		m_colors = Loader::parseColPlt(contentp, paletteSize);
		contentp += 4 * paletteSize;
	}

	//////////-- The Pixel Array --//////////
	if (m_usingColPlt) {
		m_vals = Loader::parsePixelArr8(contentp, m_width, m_height);
	} else {
		m_colors = Loader::parsePixelArr24(contentp, m_width, m_height);
	}
}

void save(std::string const& path);

Color& operator() (int i, int j) {
	return data[width * i + j];
}

int width() {
	return m_width;
}
int height() {
	return m_height;
}
