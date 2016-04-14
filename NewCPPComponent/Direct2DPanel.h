#pragma once
#include "pch.h"
namespace NewCPPComponent {
	ref class Direct2DPanel sealed : Windows::UI::Xaml::Controls::SwapChainPanel
	{
Direct2DPanel();
	void BeginDraw();
	void DrawSmiley();
	void EndDraw();

private:
	void CreateDeviceRessources();
	void CreateD2DFactory();
	void CreateD3DDevice();
	void CreateDXGIDevice();
	void CreateD2DDevice();
	void CreateD2DContext();
	void SetDxgiDeviceAsNativeImageSource();


	Microsoft::WRL::ComPtr<ID2D1Factory3>		m_d2dFactory;
	Microsoft::WRL::ComPtr<ID3D11Device>                m_d3dDevice;

	// Direct2D objects
	Microsoft::WRL::ComPtr<ID2D1Device>                 m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>          m_d2dContext;
	//D3D_FEATURE_LEVEL								m_d3dFeatureLevel;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext3>	m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGIDevice> m_dxgiDevice;



	int                                                 m_width;
	int                                                 m_height;
};

}

