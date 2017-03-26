#pragma once
#include "components/Renderable.hpp"

namespace rt
{
	class Terrain : public Renderable
	{
	public:
		Terrain();
		virtual ~Terrain();

		virtual void update(float delta) override;
		virtual void draw(RenderableContext* context) override;

	private:
		void initGrid(float32 w, float32 d, uint32 m, uint32 n);
		void initShader();

	private:
		ID3D11Buffer* m_vertexBuffer;
		ID3D11Buffer* m_indexBuffer;
		ID3DX11Effect* m_dxEffect;
		ID3D11InputLayout* m_inputLayout;
		uint32 m_indicesCount;
	};
}