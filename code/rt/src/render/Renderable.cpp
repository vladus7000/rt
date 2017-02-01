#include "render/Renderable.hpp"
#include <xnamath.h>
#include "teapot.h"
#include <d3dx11async.h>

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

struct SimpleVertexFormat
{
	float pos[3];
	float tcoord[2];
	float norm[3];
};

Renderable::Renderable()
{
	m_inited = false;
}

Renderable::~Renderable()
{
	ReleaseCOM(m_vertexBuffer);
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_dxEffect);
	ReleaseCOM(m_vertexInputLayout);
	ReleaseCOM(m_shaderRV);
}

void Renderable::update(float delta)
{
}

void Renderable::prepare(ID3D11Device* device)
{
	if (device && !m_inited)
	{
		m_inited = true;

		D3D11_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vbDesc.ByteWidth = TEAPOT_NUM_VERTS * sizeof(SimpleVertexFormat);
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = 0;
		vbDesc.MiscFlags = 0;
		vbDesc.StructureByteStride = 0;

		SimpleVertexFormat* teapot = new SimpleVertexFormat[TEAPOT_NUM_VERTS];

		for (int i = 0; i < TEAPOT_NUM_VERTS; i++)
		{
			teapot[i].pos[0] = teapot_verts[i][0];
			teapot[i].pos[1] = teapot_verts[i][1];
			teapot[i].pos[2] = teapot_verts[i][2];

			teapot[i].tcoord[0] = teapot_verts[i][4];
			teapot[i].tcoord[1] = teapot_verts[i][5];

			teapot[i].norm[0] = teapot_verts[i][6 + 6]; //0, 3, 6
			teapot[i].norm[1] = teapot_verts[i][7 + 6];
			teapot[i].norm[2] = teapot_verts[i][8 + 6];
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = teapot;

		device->CreateBuffer(&vbDesc, &initData, &m_vertexBuffer);

		delete[] teapot;

		D3D11_BUFFER_DESC ibDesc;
		ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
		ibDesc.ByteWidth = TEAPOT_NUM_INDICES * sizeof(unsigned short);
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0;
		ibDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData2;
		initData2.pSysMem = teapot_indices;

		device->CreateBuffer(&ibDesc, &initData2, &m_indexBuffer);

		D3D11_INPUT_ELEMENT_DESC simpleDescr[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		DWORD flags = 0;
		ID3D10Blob* compiledShader = nullptr;
		ID3D10Blob* compiledMsgs = nullptr;

		D3DX11CompileFromFile(L"data/shaders/effect.txt", nullptr, 0, 0, "fx_5_0", flags, 0, nullptr, &compiledShader, &compiledMsgs, nullptr);
		D3DX11CreateShaderResourceViewFromFile(device, L"data/textures/teapot.jpg", nullptr, nullptr, &m_shaderRV, nullptr);

		D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, device, &m_dxEffect);
		ReleaseCOM(compiledShader);

		ID3DX11EffectTechnique* tech;
		tech = m_dxEffect->GetTechniqueByName("ColorTech");
		D3DX11_PASS_DESC pass;

		tech->GetPassByIndex(0)->GetDesc(&pass);

		device->CreateInputLayout(simpleDescr, sizeof(simpleDescr) / sizeof(simpleDescr[0]), pass.pIAInputSignature, pass.IAInputSignatureSize, &m_vertexInputLayout);
	}
}

bool Renderable::fillContext(RenderableContext* context)
{
	bool result = false;
	if (context)
	{
		context->dxEffect = m_dxEffect;
		context->indexBuffer = m_indexBuffer;
		context->vertexBuffer = m_vertexBuffer;
		context->shaderRV = m_shaderRV;
		context->inputLayout = m_vertexInputLayout;
		context->stride = sizeof(SimpleVertexFormat);
		context->offset = 0;
		context->indexCount = TEAPOT_NUM_INDICES;
		context->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		result = true;
	}
	return result;
}

void RenderableContext::clear()
{
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	dxEffect = nullptr;
	inputLayout = nullptr;
	shaderRV = nullptr;
	topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	stride = 0;
	offset = 0;
	indexCount = 0;

}

}