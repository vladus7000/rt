#pragma once

#include <d3dx11effect.h>
#include <d3d11.h>
#include <xnamath.h>

#include "render/MeshContainer.hpp"
#include "utils/BBOX.hpp"
#include "components/Components.hpp"

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

	class Renderable : public Component
	{
	public:
		Renderable();
		virtual ~Renderable();

		virtual void update(float delta) override;

		void prepare(ID3D11Device* device);
		bool fillContext(RenderableContext* context);

	private:
		BBOX m_bbox;
		MeshContainer m_meshContainer;

		bool m_inited;

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;
		ID3DX11Effect* m_dxEffect = nullptr;
		ID3D11InputLayout* m_vertexInputLayout = nullptr;
		ID3D11ShaderResourceView* m_shaderRV = nullptr;
	};
}