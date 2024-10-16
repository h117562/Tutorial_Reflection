#ifndef _MIRROR_H
#define _MIRROR_H

#pragma comment(lib, "d3d11.lib")

#include <d3d11.h>

#include "GraphicStructures.h"

class Mirror
{
public:
	Mirror();
	~Mirror();
	Mirror(const Mirror&);

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

private:
	VertexColor* m_vertices;
	UINT* m_indices;
	UINT m_indexCount;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;


};

#endif