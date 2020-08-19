#include"BMPImg.h"
#include<fstream>
#include<cmath>
#include"file_reading.cpp"

struct BMPImg::Loader {
	//exceptions:
	class CorruptBMPFile { };
	class BadFile { };

	Loader() = default;

	template<typename T>
	static T readVar(const char* *content) {
		T& result = *( (T*) *content );
		*content += sizeof(T) / sizeof(const char);
		return result;
	}

	static std::string loadFile(std::string const& path) {
		return readFileContent(path);
	}

	static void parseHeader(const char* content, int *fileSize, int *pixelArrOffset) {
		if (readVar<char>(&content) != 'B' || readVar<char>(&content) != 'M') {
			throw CorruptBMPFile{};
		}

		if (fileSize != nullptr) {
			*fileSize = readVar<int>(&content);
		}

		//reserved bytes:
		readVar<short>(&content);
		readVar<short>(&content);

		if (pixelArrOffset != nullptr) {
			*pixelArrOffset = readVar<int>(&content);
		}
	}

	static void parseDIBHeader(const char* content, int *bmapWidth, int *bmapHeight,
	                    short *bits4Pixel, int *colPltSize) {
		int DIBHeaderSize = readVar<int>(&content);
		if (DIBHeaderSize != 40) {
			throw CorruptBMPFile{};
		}

		if (bmapWidth != nullptr) {
			*bmapWidth = readVar<int>(&content);
		}
		if (bmapHeight != nullptr) {
			*bmapHeight = readVar<int>(&content);
		}

		if (readVar<short>(&content) != 1) {
			throw CorruptBMPFile{};
		}

		if (bits4Pixel != nullptr) {
			*bits4Pixel = readVar<short>(&content);
		}

		content += 16; //skip compression and real-world scale data

		if (colPltSize != nullptr) {
			*colPltSize = readVar<int>(&content);
			if (*colPltSize == 0) {
				*colPltSize = std::pow(2, *bits4Pixel);
			}
		}
	}

	static std::vector<Color> parseColPlt(const char* content, int pltSize) {
		std::vector<Color> result (pltSize);

		for (int i = 0; i < pltSize; ++i) {
			result[i] = Color{readVar<char>(&content), readVar<char>(&content), readVar<char>(&content)};
			++content;//skip the redundant 0 every 4'th element
		}
		return result;
	}

	static std::vector<char> parsePixelArr8(const char* content, int width, int height) {
		int padding = 4 - ((width) % 4);
		int rowWidth = width + padding;
		std::vector<char> result (rowWidth * height);

		for (int i = 0, j = 0; i < rowWidth * height; ++i, ++j) {
			if (i % rowWidth == width) {
				i += padding;
			}
			result[j] = content[i];
		}
		return result;
	}

	static std::vector<Color> parsePixelArr24(const char* content, int width, int height) {
		int padding = 4 - ((width * 3) % 4);
		int rowWidth = width + padding;
		std::vector<Color> result (rowWidth * height);

		for (int i = 0, j = 0; i < rowWidth * height; i += 3, ++j) {
			if (i % rowWidth == width) {
				i += padding;
			}
			result[j] = Color{content[i], content[i + 1], content[i + 2]};
		}
		return result;
	}
};

struct BMPImg::Writer {
	template<typename T>
	static void writeVar(std::string* s, T t) {
		const char* contentOfT = (const char*) &t;
		for (int i = 0; (long unsigned int) i < sizeof(T) / sizeof(char); ++i) {
			*s += contentOfT[i];
		}
	}
	static void writeToFile(std::string const& path, std::string const& content) {
		writeFileContent(path, content);
	}
	static std::string encodeHeader(int fileSize, int pixelArrOffset) {
		std::string s  = "BM";
		writeVar(&s, fileSize);
		writeVar<short>(&s, 0); //reserved bytes
		writeVar<short>(&s, 0); //reserved bytes
		writeVar(&s, pixelArrOffset);
		return s;
	}
	static std::string encodeDIBHeader(short bits4pixel, int width, int height) {
		std::string s = "";
		writeVar(&s, 40); //size of this header
		writeVar(&s, width); //width
		writeVar(&s, height); //height
		writeVar<short>(&s, 1); //contant - must be 1
		writeVar(&s, bits4pixel); //bit for each pixel
		writeVar(&s, 0); //compression stuff - 0 means no compression
		writeVar(&s, 0); //compression stuff - 0 means no compression
		writeVar(&s, width * 8000); //real world scale
		writeVar(&s, height * 8000); //real world scale
		writeVar(&s, 0); //color palette size
		writeVar(&s, 0); //numer of important colors
		return s;
	}
	static std::string encodePixelArr(std::vector<Color> pixels, int width) {
		std::string s = "";
		int padding = 4 - ((width * 3) % 4);
		int height = pixels.size() / width;
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				Color& pixel = pixels[width * i + j];
				writeVar<char>(&s, pixel.r());
				writeVar<char>(&s, pixel.g());
				writeVar<char>(&s, pixel.b());
			}
			for (int it = 0; it < padding; ++it) {
				writeVar(&s, '\0');
			}
		}
		return s;
	}
};

BMPImg::BMPImg(std::string const& path) : BMPImg{} {
	load(path);
}

BMPImg::BMPImg(int width, int height) 
	: m_width{width}, m_height{height}, m_data(width * height) { }


void BMPImg::load(std::string const& path) {
	std::string s = Loader::loadFile(path);
	const char* contentp = s.c_str();

	//////////-- The Header --//////////
	int fileSize, arrayOffset;
	Loader::parseHeader(contentp, &fileSize, &arrayOffset);
	contentp += 14;

	//////////-- The DIB Header --//////////
	short bits4Pixel;
	int paletteSize;
	Loader::parseDIBHeader(contentp, &m_width, &m_height, &bits4Pixel, &paletteSize);
	bool usingColPlt = bits4Pixel == 8;
	contentp += 40;

	//////////-- The Color Palette & Pixel Array --//////////
	if (usingColPlt) {
		std::vector<Color> colPlt = Loader::parseColPlt(contentp, paletteSize);

		contentp = s.c_str() + arrayOffset;
		std::vector<char> pixelVals = Loader::parsePixelArr8(contentp, m_width, m_height);

		m_data = std::vector<Color>(pixelVals.size());

		for (int i = 0; i < (int) m_data.size(); ++i) {
			m_data[i] = colPlt[pixelVals[i]];
		}
	} else {
		contentp = s.c_str() + arrayOffset;
		m_data = Loader::parsePixelArr24(contentp, m_width, m_height);
	}
}

void BMPImg::save(std::string const& path) {
	std::string content = "";
	content += Writer::encodeHeader(14 + 40 + (m_width + 4 - ((m_width * 3) % 4)), 14 + 40);
	content += Writer::encodeDIBHeader(24, m_width, m_height);
	content += Writer::encodePixelArr(m_data, m_width);
	Writer::writeToFile(path, content);
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
