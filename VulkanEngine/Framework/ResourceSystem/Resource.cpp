#include "Precomp.h"
#include "Resource.h"

#include <fstream>
#include <iostream>

namespace Engine
{
	namespace Resource
	{
		Resource::Resource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator) :
			m_HashID(a_HashID), r_ResourceAllocator(a_ResourceAllocator)
		{}

        bool Resource::Load(const char* a_FilePath)
        {
#ifdef _DEBUG
            m_ResourcePath = a_FilePath;
#endif _DEBUG

            return true;
        }

        const HashIndex Resource::GetHashIndex()
        {
            return m_HashID;
        }

        Buffer Resource::ReadFile(const char* a_FilePath)
		{
            std::ifstream t_File(a_FilePath, std::ios::ate | std::ios::binary);

            if (!t_File.is_open())
            {
                Logger::Assert(false, "failed to open file!");
            }

            size_t t_FileSize = static_cast<size_t>(t_File.tellg());
            char* t_ReadFile = nullptr;

            t_File.seekg(0);
            t_File.read(t_ReadFile, t_FileSize);

            t_File.close();

            
            return Buffer(reinterpret_cast<const unsigned char*>(t_ReadFile), t_FileSize);;
		}
	}
}