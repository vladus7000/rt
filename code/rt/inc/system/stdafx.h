#pragma once
#include "Types.hpp"
#include "Assertions.hpp"
#include "Resources.h"
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