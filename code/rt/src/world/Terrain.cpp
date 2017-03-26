#include "stdafx.h"
#include "world/Terrain.hpp"
#include "render/dxUtils.hpp"
#include <d3dx11async.h>

#include <iostream>

namespace rt
{

Terrain::Terrain()
	: m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
	, m_dxEffect(nullptr)
	, m_inputLayout(nullptr)
	, m_indicesCount(0)
{
	initGrid(10.0f, 10.0f, 50, 50);
	initShader();
}

Terrain::~Terrain()
{
	ReleaseCOM(m_vertexBuffer);
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_dxEffect);
	ReleaseCOM(m_inputLayout);
}

void Terrain::update(float delta)
{
}

void Terrain::draw(RenderableContext* context)
{
	uint32 stride = sizeof(float32) * 3;
	uint32 offset = 0;

	auto dxContext = Resources::getInstance().getContext();
	dxContext->IASetInputLayout(m_inputLayout);
	dxContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dxContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	ID3DX11EffectTechnique* tech;
	tech = m_dxEffect->GetTechniqueByName("Terrain");
	D3DX11_TECHNIQUE_DESC desc;
	tech->GetDesc(&desc);

	ID3DX11EffectMatrixVariable* fxViewProj = nullptr;
	fxViewProj = m_dxEffect->GetVariableByName("viwewProjection")->AsMatrix();

	fxViewProj->SetMatrix((float*)&context->viewProjectionMatrix);

	for (unsigned int i = 0; i < desc.Passes; i++)
	{
		dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		tech->GetPassByIndex(i)->Apply(0, dxContext);
		dxContext->DrawIndexed(m_indicesCount, 0, 0);
	}
}

void Terrain::initGrid(float32 w, float32 d, uint32 m, uint32 n)
{
	const uint32 vertexCount = m * n;

	float32 halfWidth = 0.5f * w;
	float32 halfDepth = 0.5f * d;
	float32 dx = w / (n - 1);
	float32 dz = d / (m - 1);
	float32 du = 1.0f / (n - 1);
	float32 dv = 1.0f / (m - 1);
	const uint32 faceCount = (m - 1)*(n - 1) * 2;

	struct Vertex
	{
		Vertex() {}
		Vertex(float32 x, float32 y, float32 z)
			: x(x), y(y), z(z)
		{}
		float32 x; float32 y; float32 z;
	};

	auto& resources = Resources::getInstance();
	auto context = resources.getContext();
	auto device = resources.getDxDevice();

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = vertexCount * sizeof(Vertex);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	Vertex* terrainData = new Vertex[vertexCount];

	for (uint32 i = 0; i < m; i++)
	{
		float32 z = halfDepth - i * dz;
		for (uint32 j = 0; j < n; j++)
		{
			float32 x = -halfWidth + j * dx;
			terrainData[i * n + j] = Vertex(x, 0.0f, z);

			/*	normal = (0.0f, 1.0f, 0.0f);
			tangent = (1.0f, 0.0f, 0.0f);

			u = j*du;
			v = i*dv;
			*/
		}
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = terrainData;

	device->CreateBuffer(&desc, &initData, &m_vertexBuffer);
	delete[] terrainData;

	m_indicesCount = faceCount * 3;
	uint32* indices = new uint32[m_indicesCount];

	uint32 k = 0;
	for (uint32 i = 0; i < m - 1; i++)
	{
		for (uint32 j = 0; j < n - 1; j++)
		{
			indices[k] = i*n + j;
			indices[k + 1] = i*n + j + 1;
			indices[k + 2] = (i + 1) * n + j;
			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;
			k += 6;
		}
	}

	desc.ByteWidth = m_indicesCount * sizeof(uint32);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	initData.pSysMem = indices;
	device->CreateBuffer(&desc, &initData, &m_indexBuffer);
	delete[] indices;
}

void Terrain::initShader()
{
	DWORD flags = 0;
	ID3D10Blob* compiledShader = nullptr;
	ID3D10Blob* compilerMsg = nullptr;

	auto& resources = Resources::getInstance();
	auto device = resources.getDxDevice();

	D3DX11CompileFromFile(L"data/shaders/terrain.fx", nullptr, 0, 0, "fx_5_0", flags, 0, nullptr, &compiledShader, &compilerMsg, nullptr);
	
	if (compilerMsg)
	{
		const char* error = (const char*)compilerMsg->GetBufferPointer();
		std::cout << error << std::endl;
		ReleaseCOM(compilerMsg);
	}
	if (compiledShader)
	{
		D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, device, &m_dxEffect);
		ReleaseCOM(compiledShader);
		D3D11_INPUT_ELEMENT_DESC terrainDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		ID3DX11EffectTechnique* technique = nullptr;
		technique = m_dxEffect->GetTechniqueByName("Terrain");
		D3DX11_PASS_DESC pass;
		technique->GetPassByIndex(0)->GetDesc(&pass);

		device->CreateInputLayout(terrainDesc, sizeof(terrainDesc) / sizeof(terrainDesc[0]), pass.pIAInputSignature, pass.IAInputSignatureSize, &m_inputLayout);
	}

}

}