
#pragma once

#include <Seamless/Export.hpp>

#include <chrono>

namespace cmls
{
	class CMLS_API Timer
	{
	public:
		Timer();
		virtual ~Timer();

		void reset();

		double seconds() const;

	protected:
		std::chrono::high_resolution_clock::time_point m_origin;
	};
}