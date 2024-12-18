#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

#pragma comment(lib, "winmm.lib")

#include <DirectXMath.h>

#include "D3DClass.h"
#include "textclass.h"
#include "CameraClass.h"

class DebugInfo
{
public:
	DebugInfo();
	~DebugInfo();
	DebugInfo(const DebugInfo&);

	bool Initialize(D3DClass*);
	void Render(TextClass*, CameraClass*);
	void Shutdown();
private:
	void GetFps();

private:
	int m_fps, m_count;
	wchar_t m_fpsString[20];
	wchar_t m_videoNameString[144];
	wchar_t m_memoryString[32];
	wchar_t m_tempString[16];
	unsigned long m_startTime;

	DirectX::XMINT3 m_position, m_rotation;
	DirectX::XMINT3 m_previousPos, m_previousRot;
	wchar_t m_stringPX[16], m_stringPY[16], m_stringPZ[16];
	wchar_t m_stringRX[16], m_stringRY[16], m_stringRZ[16];

};

#endif