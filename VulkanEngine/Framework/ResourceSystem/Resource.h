#pragma once
#include <stdint.h>

namespace Engine
{
	class ResourceAllocator;

	typedef uint64_t HashIndex;

	namespace Resource
	{
		struct Buffer
		{
			Buffer(const unsigned char* a_Data, const size_t a_Size)
				: data(a_Data), size(a_Size) {}

			const unsigned char* data = nullptr;
			const size_t size = 0;
		};

		class Resource
		{
		public:
			Resource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator);
			virtual ~Resource() {};

			virtual bool IsInitialized() = 0;

			virtual bool Load(const char* a_FilePath) = 0;
			virtual bool Unload() = 0;

			const HashIndex GetHashIndex() { return m_HashID; };

		private:
			const HashIndex m_HashID;

		protected:
			static Buffer ReadFile(const char* a_FilePath);

			ResourceAllocator& r_ResourceAllocator;
		};
	}
}