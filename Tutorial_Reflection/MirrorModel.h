#ifndef _MIRROR_MODEL_H
#define _MIRROR_MODEL_H

#pragma comment(lib, "d3d11.lib")

#include <d3d11.h>

#include "GraphicStructures.h"

class MirrorModel
{
public:
	MirrorModel();
	~MirrorModel();
	MirrorModel(const MirrorModel&);

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*, ID3D11ShaderResourceView*);

private:
	VertexTextureCoord* m_vertices;
	UINT* m_indices;
	UINT m_indexCount;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	UINT stride = sizeof(VertexTextureCoord);
	UINT offset = 0;


};

#endif