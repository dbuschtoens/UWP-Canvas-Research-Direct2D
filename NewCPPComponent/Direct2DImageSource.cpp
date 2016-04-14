#include "pch.h"
#include "Direct2DImageSource.h"

using namespace NewCPPComponent;
using namespace Platform;
using namespace Microsoft::WRL;

Direct2DImageSource::Direct2DImageSource(int width, int height, bool isOpaque) :
	SurfaceImageSource(width, height, isOpaque)
{
	m_width = width;
	m_height = height;
	CreateDeviceRessources();
}

void NewCPPComponent::Direct2DImageSource::CreateDeviceRessources()
{
	CreateD2DFactory();
	CreateD3DDevice();
	CreateDXGIDevice();
	CreateD2DDevice();
	CreateD2DContext();
	SetDxgiDeviceAsNativeImageSource();
}

void NewCPPComponent::Direct2DImageSource::CreateD2DFactory()
{

	// Initialize Direct2D resources.
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	// Initialize the Direct2D Factory.
	DX::ThrowIfFailed(
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory3),
			&options,
			&m_d2dFactory
		)
	);
}

void NewCPPComponent::Direct2DImageSource::CreateD3DDevice()
{
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// Create the Direct3D 11 API device object.
	DX::ThrowIfFailed(
		D3D11CreateDevice(
			nullptr,                        // Specify nullptr to use the default adapter.
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags,                  // Set debug and Direct2D compatibility flags.
			featureLevels,                  // List of feature levels this app can support.
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,              // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			&m_d3dDevice,                   // Returns the Direct3D device created.
			nullptr,
			nullptr
		)
	);
}

void NewCPPComponent::Direct2DImageSource::CreateDXGIDevice()
{

	// Obtain the underlying DXGI device of the Direct3D11 device.
	DX::ThrowIfFailed(
		m_d3dDevice.As(&m_dxgiDevice)
	);

}

void NewCPPComponent::Direct2DImageSource::CreateD2DDevice()
{
	// Obtain the Direct2D device for 2-D rendering.
	//DX::ThrowIfFailed(
	//	m_d2dFactory->CreateDevice(m_dxgiDevice.Get(), &m_d2dDevice)
	//);
	DX::ThrowIfFailed(
		D2D1CreateDevice(m_dxgiDevice.Get(), nullptr, &m_d2dDevice)
	);
}

void NewCPPComponent::Direct2DImageSource::CreateD2DContext()
{

	// Get Direct2D device's corresponding device context object.
	DX::ThrowIfFailed(
		m_d2dDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&m_d2dContext
		)
	);

}

void NewCPPComponent::Direct2DImageSource::SetDxgiDeviceAsNativeImageSource()
{
	//Query for ISurfaceImageSourceNative interface;
	ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IInspectable*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
	);

	DX::ThrowIfFailed(
		sisNative->SetDevice(m_dxgiDevice.Get())
	);

}

void NewCPPComponent::Direct2DImageSource::BeginDraw()
{
	//Query for ISurfaceImageSourceNative interface;
	ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IInspectable*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
	);
	RECT updateRect;
	updateRect.top = 0;
	updateRect.left = 0;
	updateRect.right = m_width;
	updateRect.bottom = m_height;


	POINT offset;
	ComPtr<IDXGISurface> surface;
	HRESULT beginDrawResult = sisNative->BeginDraw(updateRect, &surface, &offset);
	if (SUCCEEDED(beginDrawResult)) {
		// Create render target.
		ComPtr<ID2D1Bitmap1> bitmap;
		DX::ThrowIfFailed(
			m_d2dContext->CreateBitmapFromDxgiSurface(
				surface.Get(),
				nullptr,
				&bitmap
			)
		);

		// Set context's render target.
		m_d2dContext->SetTarget(bitmap.Get());

		// Begin drawing using D2D context.
		m_d2dContext->BeginDraw();

		// Apply a clip and transform to constrain updates to the target update area.
		// This is required to ensure coordinates within the target surface remain
		// consistent by taking into account the offset returned by BeginDraw, and
		// can also improve performance by optimizing the area that is drawn by D2D.
		// Apps should always account for the offset output parameter returned by
		// BeginDraw, since it may not match the passed updateRect input parameter's location.
		m_d2dContext->PushAxisAlignedClip(
			D2D1::RectF(
				static_cast<float>(offset.x),
				static_cast<float>(offset.y),
				static_cast<float>(offset.x + updateRect.right - updateRect.left),
				static_cast<float>(offset.y + updateRect.bottom - updateRect.top)
			),
			D2D1_ANTIALIAS_MODE_ALIASED
		);

		m_d2dContext->SetTransform(
			D2D1::Matrix3x2F::Translation(
				static_cast<float>(offset.x),
				static_cast<float>(offset.y)
			)
		);
	}
	else if (beginDrawResult == DXGI_ERROR_DEVICE_REMOVED || beginDrawResult == DXGI_ERROR_DEVICE_RESET) {
		CreateD2DFactory();
		CreateD3DDevice();
		CreateDXGIDevice();
		CreateD2DDevice();
		CreateD2DContext();
		DrawSmiley();
	}
	else {
		DX::ThrowIfFailed(beginDrawResult);
	}
}

void NewCPPComponent::Direct2DImageSource::EndDraw()
{

	// Remove the transform and clip applied in BeginDraw since
	// the target area can change on every update.
	m_d2dContext->SetTransform(D2D1::IdentityMatrix());
	m_d2dContext->PopAxisAlignedClip();

	// Remove the render target and end drawing.
	DX::ThrowIfFailed(
		m_d2dContext->EndDraw()
	);

	m_d2dContext->SetTarget(nullptr);

	// Query for ISurfaceImageSourceNative interface.
	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
	);

	DX::ThrowIfFailed(
		sisNative->EndDraw()
	);

}

void NewCPPComponent::Direct2DImageSource::DrawSmiley()
{

		// draw Face
		ID2D1SolidColorBrush *outlineBrush;
		ID2D1SolidColorBrush *faceBrush;
		m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &outlineBrush);
		m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &faceBrush);
		D2D1_ELLIPSE face = D2D1::Ellipse(D2D1::Point2F(150, 150), 100, 100);
		m_d2dContext->FillEllipse(face, faceBrush);
		m_d2dContext->DrawEllipse(face, outlineBrush);

		// draw Eyes
		D2D1_ELLIPSE leftEye = D2D1::Ellipse(D2D1::Point2F(120, 100), 10, 30);
		D2D1_ELLIPSE rightEye = D2D1::Ellipse(D2D1::Point2F(180, 100), 10, 30);
		m_d2dContext->FillEllipse(leftEye, outlineBrush);
		m_d2dContext->FillEllipse(rightEye, outlineBrush);


}
