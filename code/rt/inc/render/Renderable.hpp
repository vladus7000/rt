#pragma once
#include <d3dx11effect.h>
#include <d3d11.h>

namespace rt
{
	struct RenderableContext
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		ID3DX11Effect* dxEffect;
		ID3D11InputLayout* inputLayout;
		ID3D11ShaderResourceView* shaderRV;
		D3D_PRIMITIVE_TOPOLOGY topology;
		unsigned int stride;
		unsigned int offset;
		unsigned int indexCount;

		void clear();
	};

	class Renderable
	{
	public:
		Renderable();
		~Renderable();

		void update(float delta);

		void prepare(ID3D11Device* device);
		bool fillContext(RenderableContext* context);

	private:
		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;
		ID3DX11Effect* m_dxEffect = nullptr;
		ID3D11InputLayout* m_vertexInputLayout = nullptr;
		ID3D11ShaderResourceView* m_shaderRV = nullptr;
	};
}