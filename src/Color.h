#pragma once

class Color {
public:
	typedef unsigned char component_t;

	Color(component_t r = 0, component_t g = 0, component_t b = 0);

	//supports notation such as:
	//
	//Color::component_t comp = myCol.r();
	//myCol.g() = comp;
	//myCol.b() = myCol.r();
	//
	//which is much easier to write and read in complex lines,
	//rather than normal java-like getters and setters in my opinion.
	component_t& r();
	component_t& g();
	component_t& b();

	component_t brightness();

private:
	component_t m_r, m_g, m_b;
};
