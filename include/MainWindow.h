#pragma once

#include <Windows.h>

#include <QWidget>

class QSlider;
class QPushButton;

class MainWindow : public QWidget {
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK mouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
	void onPaint();
	void onHotkey();
	static HWND hWnd;
	HHOOK mouseHook;
	RECT mainRect;

private:
	void createGUI();
	void connectGUI();
	void createOverlay();
	QSlider* transparencySlider = nullptr;
	int transparency;
	QSlider* spotlightSizeSlider = nullptr;
	int spotlightSize;
	QPushButton* toggleButton = nullptr;
	const COLORREF rgbBlack = 0x00000000;
	const COLORREF rgbWhite = 0x00FFFFFF;
	HBRUSH whiteBrush = CreateSolidBrush(rgbWhite);
	bool overlayActive;
};

MainWindow* instanceFromWndProc(HWND hWnd, UINT uMsg, LPARAM lParam);

