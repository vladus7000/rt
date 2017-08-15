#pragma once

#ifdef _EXPORTING
#define CLASS_DECLSPEC    __declspec(dllexport)
#else
#define CLASS_DECLSPEC    __declspec(dllimport)
#endif


#include "Types.hpp"
#include "Assertions.hpp"
#include "Resources.h"
#include "system/FileManager.hpp"
#include <d3d11.h>
#include "render/dxUtils.hpp"

namespace rt
{
	template<class T>
	inline T* construct(int32 alignement)
	{
		void* mem = system::System::allocAllignement(sizeof(T), alignement);
		return new (mem) T();
	}
}