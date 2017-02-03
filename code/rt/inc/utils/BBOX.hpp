#pragma once

#include <xnamath.h>
#include <float.h>

namespace rt
{
	struct BBOX
	{
		BBOX()
			: minCoord(XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f))
			, maxCoord(XMVectorSet(FLT_MIN, FLT_MIN, FLT_MIN, 1.0f))
			, valid(false)
		{}

		BBOX(const BBOX& other)
			: minCoord(other.minCoord)
			, maxCoord(other.maxCoord)
			, valid(other.valid)
		{}

		bool contains(const XMVECTOR& p)
		{
			bool res = false;
			if (valid)
			{
				res = true;
				for (int i = 0; i < 3; i++)
					res &= (p.m128_f32[i] >= minCoord.m128_f32[i]);
				for (int i = 0; i < 3; i++)
					res &= (p.m128_f32[i] <= maxCoord.m128_f32[i]);
			}
			return res;
		}

		void addPoint(const XMVECTOR& p)
		{
			for (int i = 0; i < 3; i++)
			{
				if (p.m128_f32[i] < minCoord.m128_f32[i])
				{
					minCoord.m128_f32[i] = p.m128_f32[i];
				}
				else if (p.m128_f32[i] > maxCoord.m128_f32[i])
				{
					maxCoord.m128_f32[i] = p.m128_f32[i];
				}
			}

			// check valid
			valid = true;
			for (int i = 0; i < 3; i++)
				valid &= (minCoord.m128_f32[i] < FLT_MAX);
			for (int i = 0; i < 3; i++)
				valid &= (maxCoord.m128_f32[i] > FLT_MIN);
		}

		XMVECTOR minCoord;
		XMVECTOR maxCoord;
		bool valid;
	};
}