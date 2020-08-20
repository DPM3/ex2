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
Color::component_t const& Color::r() const {
	return m_r;
}
Color::component_t const& Color::g() const {
	return m_g;
}
Color::component_t const& Color::b() const {
	return m_b;
}
Color::component_t Color::brightness() {
	return 0.2126 * m_r + 0.7152 * m_g + 0.0722 * m_b;
}
