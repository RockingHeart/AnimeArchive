#pragma once
#include "Archive.h"

namespace Cfg
{
	namespace UI
	{

		inline std::unordered_map<winrt::hstring, archive> Archives;
		inline winrt::hstring CurrentTag;
		inline std::size_t LogLimit;

		constexpr std::size_t GetLogLimitDefValue() {
			return 10;
		}

		void Load();

		void LoadLogLimitSetValue();
	}
};