#include "MainWindow.h"

#include <QSettings>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

HWND MainWindow::hWnd = nullptr;

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
	QSettings settings("stablesteady", "spotlight");
	transparency = settings.value("transparency", 50).toInt();
	spotlightSize = settings.value("spotlightSize", 100).toInt();

	createOverlay();
	createGUI();
	connectGUI();
}

MainWindow::~MainWindow() {
	UnregisterHotKey(hWnd, 0);

	UnhookWindowsHookEx(mouseHook);

	QSettings settings("stablesteady", "spotlight");
	settings.setValue("transparency", transparency);
	settings.setValue("spotlightSize", spotlightSize);
}

LRESULT MainWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	MainWindow* window = instanceFromWndProc(hWnd, uMsg, lParam);
	if (window) {
		switch (uMsg) {
		case WM_PAINT: window->onPaint(); break;
		case WM_HOTKEY: switch (wParam) {
		case 0:
			window->onHotkey();
		}  break;
		case WM_DESTROY: PostQuitMessage(0); return 0;
		}
	}
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT MainWindow::mouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	MainWindow* window = instanceFromWndProc(hWnd, 0, 0);
	if (nCode == HC_ACTION) {
		if (wParam == WM_MOUSEMOVE) {
			InvalidateRect(hWnd, &(window->mainRect), FALSE);
		}
	}

	return CallNextHookEx(window->mouseHook, nCode, wParam, lParam);
}

void MainWindow::onPaint() {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memBitmap = CreateCompatibleBitmap(hdc, mainRect.right - mainRect.left, mainRect.bottom - mainRect.top);
	HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBitmap);

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);

	HRGN hrgn = CreateEllipticRgn(cursorPos.x - spotlightSize, cursorPos.y - spotlightSize, cursorPos.x + spotlightSize, cursorPos.y + spotlightSize);
	FillRgn(memDC, hrgn, whiteBrush);

	BitBlt(hdc, 0, 0, mainRect.right - mainRect.left, mainRect.bottom - mainRect.top, memDC, 0, 0, SRCCOPY);

	EndPaint(hWnd, &ps);

	SelectObject(memDC, oldBmp);
	DeleteDC(memDC);
	DeleteObject(memBitmap);
}

void MainWindow::onHotkey() {
	if (overlayActive) {
		ShowWindow(hWnd, SW_HIDE);
		overlayActive = false;
	}
	else {
		ShowWindow(hWnd, SW_SHOW);
		overlayActive = true;
	}
}

void MainWindow::createGUI() {
	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* transparencyLayout = new QHBoxLayout();
	QLabel* transparencyLabel = new QLabel("Transparency: ");
	transparencySlider = new QSlider(Qt::Horizontal);
	transparencySlider->setRange(0, 100);
	transparencySlider->setValue(transparency);
	transparencyLayout->addWidget(transparencyLabel);
	transparencyLayout->addWidget(transparencySlider);

	QHBoxLayout* spotlightSizeLayout = new QHBoxLayout();
	QLabel* spotlightSizeLabel = new QLabel("Spotlight Size (pixels): ");
	spotlightSizeSlider = new QSlider(Qt::Horizontal);
	spotlightSizeSlider->setRange(10, 1000);
	spotlightSizeSlider->setValue(spotlightSize);
	spotlightSizeLayout->addWidget(spotlightSizeLabel);
	spotlightSizeLayout->addWidget(spotlightSizeSlider);

	toggleButton = new QPushButton("Toggle");

	layout->addLayout(transparencyLayout);
	layout->addLayout(spotlightSizeLayout);
	layout->addWidget(toggleButton);
	this->setLayout(layout);
}

void MainWindow::connectGUI() {
	connect(transparencySlider, &QSlider::sliderPressed, [=]() {
		ShowWindow(hWnd, SW_SHOW);
		});
	connect(transparencySlider, &QSlider::sliderReleased, [=]() {
		if (!overlayActive) ShowWindow(hWnd, SW_HIDE);
		});
	connect(transparencySlider, &QSlider::valueChanged, [=](int value) {
		transparency = value;
		SetLayeredWindowAttributes(hWnd, rgbWhite, (transparency * 255) / 100, LWA_COLORKEY | LWA_ALPHA);
		});
	connect(spotlightSizeSlider, &QSlider::sliderPressed, [=]() {
		ShowWindow(hWnd, SW_SHOW);
		});
	connect(spotlightSizeSlider, &QSlider::sliderReleased, [=]() {
		if (!overlayActive) ShowWindow(hWnd, SW_HIDE);
		});
	connect(spotlightSizeSlider, &QSlider::valueChanged, [=](int value) {
		spotlightSize = value;
		});
	connect(toggleButton, &QPushButton::pressed, [=]() {
		if (overlayActive) {
			ShowWindow(hWnd, SW_HIDE);
			overlayActive = false;
		}
		else {
			ShowWindow(hWnd, SW_SHOW);
			overlayActive = true;
		}
		});
}

void MainWindow::createOverlay() {
	HINSTANCE hInstance = static_cast<HINSTANCE>(GetModuleHandle(nullptr));
	WNDCLASS wc = {};
	wc.lpfnWndProc = MainWindow::WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(rgbBlack);
	wc.lpszClassName = L"overlayClass";
	RegisterClassW(&wc);

	hWnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		L"overlayClass",
		L"spotlightOverlay",
		WS_POPUPWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr,
		hInstance,
		this
	);

	SetLayeredWindowAttributes(hWnd, rgbWhite, (transparency * 255) / 100, LWA_COLORKEY | LWA_ALPHA);

	GetWindowRect(hWnd, &mainRect);

	ShowWindow(hWnd, SW_HIDE);
	overlayActive = false;

	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookCallback, hInstance, 0);

	RegisterHotKey(hWnd, 0, MOD_ALT, 0x53); // 0x53 is S
}

MainWindow* instanceFromWndProc(HWND hWnd, UINT uMsg, LPARAM lParam) {
	MainWindow* pInstance;
	if (uMsg == WM_NCCREATE) {
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pInstance = reinterpret_cast<MainWindow*>(pCreateStruct->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pInstance));

		pInstance->hWnd = hWnd;
	}
	else {
		pInstance = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}
	return pInstance;
}
