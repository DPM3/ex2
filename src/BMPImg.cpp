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

		std::string content;
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
		std::vector<Color> result (pltSize);

		for (int i = 0; i < pltSize; ++i) {
			result[i] = Color{readChar(&content), readChar(&content), readChar(&content)};
			++content;//skip the redundant 0 every 4'th element
		}
		return result;
	}

	static std::vector<char> parsePixelArr8(char* content, int width, int height) {
		int padding = 4 - ((width) % 4);
		int rowWidth = width + padding;
		std::vector<char> result (rowWidth * height);

		for (int i = 0; i < rowWidth * height; ++i) {
			if (i % rowWidth == width) {
				i += padding;
			}
			result[i] = content[i];
		}
		return result;
	}

	static std::vector<Color> parsePixelArr24(char* content, int width, int height) {
		int padding = 4 - ((width * 3) % 4);
		int rowWidth = width + padding;
		std::vector<Color> result (rowWidth * height);

		for (int i = 0; i < rowWidth * height; i += 4) {
			if (i % rowWidth == width) {
				i += padding;
			}
			result[i] = Color{content[i], content[i + 1], content[i + 2]};
		}
		return result;
	}
};

BMPImg::BMPImg(std::string const& path) : BMPImg{} {
	load(path);
}

BMPImg::BMPImg(int width, int height) 
	: m_width{width}, m_height{height}, m_usingColPlt{false}, m_data(width * height) { }


void BMPImg::load(std::string const& path) {
	std::string s = Loader::loadFile(path);
	char* contentp = const_cast<char*>(s.c_str());

	//////////-- The Header --//////////
	int fileSize, arrayOffset;
	Loader::parseHeader(contentp, &fileSize, &arrayOffset);
	contentp += 14;

	//////////-- The DIB Header --//////////
	m_usingColPlt = true;
	short bits4Pixel;
	int paletteSize;
	Loader::parseDIBHeader(contentp, &m_width, &m_height, &bits4Pixel, &paletteSize);
	m_usingColPlt = bits4Pixel == 8;
	contentp += 40;

	//////////-- The Color Palette & Pixel Array --//////////
	if (m_usingColPlt) {
		std::vector<Color> colPlt = Loader::parseColPlt(contentp, paletteSize);
		contentp += 4 * paletteSize;

		std::vector<char> pixelVals = Loader::parsePixelArr8(contentp, m_width, m_height);

		m_data = std::vector<Color>(pixelVals.size());

		for (int i = 0; i < (int) m_data.size(); ++i) {
			m_data[i] = colPlt[pixelVals[i]];
		}
	} else {
		m_data = Loader::parsePixelArr24(contentp, m_width, m_height);
	}
}


Color& BMPImg::operator() (int i, int j) {
	return m_data[m_width * i + j];
}

int BMPImg::width() {
	return m_width;
}
int BMPImg::height() {
	return m_height;
}
