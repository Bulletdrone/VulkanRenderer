#pragma once
#include <stdint.h>

namespace Engine
{
	namespace Resource
	{
		class Resource
		{
		public:
			Resource(const uint64_t a_HashID);
			virtual ~Resource() {};

			virtual bool IsInitialized() = 0;

			virtual bool Load(const char* a_FilePath) = 0;
			virtual bool Unload() = 0;

		private:
			const uint64_t m_HashID;
		};
	}
}