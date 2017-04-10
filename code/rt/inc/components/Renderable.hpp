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
		XMMATRIX viewMatrix;
		XMMATRIX viewProjectionMatrix;
		XMMATRIX projectionMatrix;
		XMMATRIX lightMatrix;
		XMMATRIX worldMatrix;

		void clear();
		bool depthOnly;
	};

	class Renderable : public Component
	{
	public:
		Renderable();
		virtual ~Renderable();

		virtual void update(float delta) override;
		virtual void draw(RenderableContext* context);

		void overrideBuffers(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int32 indexCount);
		void overrideVertexBuffer(ID3D11Buffer* buffer, int32 count);
		void overrideNormalBuffer(ID3D11Buffer* buffer, int32 count);
		void overrideTCoordBuffer(ID3D11Buffer* buffer, int32 count);
		void overrideColorBuffer(ID3D11Buffer* buffer, int32 count);
		void overrideTangentBuffer(ID3D11Buffer* buffer, int32 count);
		void overrideBitangentBuffer(ID3D11Buffer* buffer, int32 count);
		void overrideIndexBuffer(ID3D11Buffer* buffer, int32 count);

	private:
		BBOX m_bbox;
		MeshContainer m_meshContainer;

		bool m_inited;

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;
		ID3D11Buffer* m_TCoordBuffer = nullptr;
		ID3D11Buffer* m_NormalBuffer = nullptr;
		ID3D11Buffer* m_ColorBuffer = nullptr;
		ID3DX11Effect* m_dxEffect = nullptr;
		ID3D11InputLayout* m_vertexInputLayout = nullptr;
		ID3D11ShaderResourceView* m_shaderRV = nullptr;
		int32 m_indexCount;
	};
}