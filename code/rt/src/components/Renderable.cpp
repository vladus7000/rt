#include "stdafx.h"
#include "components/Renderable.hpp"
#include <xnamath.h>
#include "teapot.h"
#include <d3dx11async.h>
#include "render/dxUtils.hpp"
#include "system/Resources.h"

namespace rt
{

struct SimpleVertexFormat
{
	float pos[3];
	float tcoord[2];
	float norm[3];
};

Renderable::Renderable()
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

	auto device = Resources::getInstance().getDxDevice();
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

void Renderable::draw(RenderableContext* context)
{
	unsigned int stride = sizeof(SimpleVertexFormat);
	unsigned int offset = 0;

	auto dxContext = Resources::getInstance().getContext();
	dxContext->IASetInputLayout(m_vertexInputLayout);
	dxContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dxContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	ID3DX11EffectTechnique* tech;
	if (!context->depthOnly)
	{
		tech = m_dxEffect->GetTechniqueByName("ColorTech");
	}
	else
	{
		tech = m_dxEffect->GetTechniqueByName("DepthOnly");
	}
	D3DX11_TECHNIQUE_DESC desc;
	tech->GetDesc(&desc);

	ID3DX11EffectMatrixVariable* fxViewProj = nullptr;
	ID3DX11EffectMatrixVariable* fxWorld = nullptr;
	ID3DX11EffectMatrixVariable* fxWorldN = nullptr;
	ID3DX11EffectMatrixVariable* fxTextureM = nullptr;
	ID3DX11EffectShaderResourceVariable* SRV = nullptr;

	fxViewProj = m_dxEffect->GetVariableByName("ViewProj")->AsMatrix();
	fxWorld = m_dxEffect->GetVariableByName("World")->AsMatrix();
	fxWorldN = m_dxEffect->GetVariableByName("WorldN")->AsMatrix();
	fxTextureM = m_dxEffect->GetVariableByName("TextureM")->AsMatrix();
	SRV = m_dxEffect->GetVariableByName("tex")->AsShaderResource();

	XMMATRIX worldNorm = InverseTranspose(context->worldMatrix);

	SRV->SetResource(m_shaderRV);
	fxViewProj->SetMatrix((float*)&context->viewProjectionMatrix);
	fxWorld->SetMatrix((float*)&context->worldMatrix);
	fxWorldN->SetMatrix((float*)&worldNorm);

	for (unsigned int i = 0; i < desc.Passes; i++)
	{
		dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		tech->GetPassByIndex(i)->Apply(0, dxContext);
		dxContext->DrawIndexed(TEAPOT_NUM_INDICES, 0, 0);
	}
}

void RenderableContext::clear()
{
	viewMatrix = XMMatrixIdentity();
	viewProjectionMatrix = XMMatrixIdentity();
	projectionMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixIdentity();
	depthOnly = false;
}

}