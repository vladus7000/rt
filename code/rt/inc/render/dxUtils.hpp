#pragma once
#include <d3d11.h>
#include <xnamath.h>

namespace rt
{
	template< class T>
	void ReleaseCOM(T*& comObj)
	{
		if (comObj)
		{
			comObj->Release();
			comObj = nullptr;
		}
	}

	inline XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}
}