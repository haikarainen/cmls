
#include "Seamless/Timer.hpp"

cmls::Timer::Timer()
{
	reset();
}

cmls::Timer::~Timer()
{

}

void cmls::Timer::reset()
{
	m_origin = std::chrono::high_resolution_clock::now();
}

double cmls::Timer::seconds() const
{
	auto now = std::chrono::high_resolution_clock::now();
	auto difference = std::chrono::duration_cast<std::chrono::microseconds>(now - m_origin);
	return double(difference.count()) / 1000000.0;
}
