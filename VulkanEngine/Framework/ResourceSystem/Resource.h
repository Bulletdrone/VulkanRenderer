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
			~Buffer() { delete[] data; }

			const unsigned char* data = nullptr;
			const size_t size = 0;
		};

		class Resource
		{
		public:
			Resource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator);
			virtual ~Resource() {};

			virtual bool IsInitialized() = 0;

			virtual bool Load(const char* a_FilePath);
			virtual bool Unload() = 0;

			const HashIndex GetHashIndex();

		private:
			const HashIndex m_HashID;

#ifdef _DEBUG
			//For now debug has the ability to reload.
			const char* m_ResourcePath = nullptr;
#endif _DEBUG

		protected:
			static Buffer ReadFile(const char* a_FilePath);

			ResourceAllocator& r_ResourceAllocator;
		};
	}
}