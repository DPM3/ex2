#include"Color.h"

Color::Color(component_t r, component_t g, component_t b)
	: m_r{r}, m_g{g}, m_b{b} { }

Color::component_t& Color::r() {
	return m_r;
}
Color::component_t& Color::g() {
	return m_g;
}
Color::component_t& Color::b() {
	return m_b;
}
