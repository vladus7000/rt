#include "RTProxy.hpp"

public ref class D3DWrap
{
public:
	void  Initialize(HWND hWnd)
	{
		m_engine = new EngineProxy(hWnd);
	}
	void  Shutdown() {}
	void  ProcessFrame() {}
	void  Resize() {}

protected:
	EngineProxy* m_engine;
};
