#ifndef __CTLWIN_H__
#define __CTLWIN_H__
#include "ctlapp.h"

// protect template members from windowsx.h macros
#ifdef _INC_WINDOWSX
#undef SubclassWindow
#endif //_INC_WINDOWSX

#ifdef SetWindowLongPtrA
#undef SetWindowLongPtrA
inline LONG_PTR SetWindowLongPtrA(
	HWND hWnd,
	int nIndex,
	LONG_PTR dwNewLong)
{
	return(::SetWindowLongA(hWnd, nIndex, LONG(dwNewLong)));
}
#endif

#ifdef SetWindowLongPtrW
#undef SetWindowLongPtrW
inline LONG_PTR SetWindowLongPtrW(
	HWND hWnd,
	int nIndex,
	LONG_PTR dwNewLong)
{
	return(::SetWindowLongW(hWnd, nIndex, LONG(dwNewLong)));
}
#endif

#ifdef GetWindowLongPtrA
#undef GetWindowLongPtrA
inline LONG_PTR GetWindowLongPtrA(
	HWND hWnd,
	int nIndex)
{
	return(::GetWindowLongA(hWnd, nIndex));
}
#endif

#ifdef GetWindowLongPtrW
#undef GetWindowLongPtrW
inline LONG_PTR GetWindowLongPtrW(
	HWND hWnd,
	int nIndex)
{
	return(::GetWindowLongW(hWnd, nIndex));
}
#endif

namespace CTL
{
	/////////////////////////////////////////////////////////////////////////////
	// Dual argument helper classes

	class _U_RECT
	{
	public:
		_U_RECT(LPRECT lpRect) : m_lpRect(lpRect)
		{
		}
		_U_RECT(RECT& rc) : m_lpRect(&rc)
		{
		}
		LPRECT m_lpRect;
	};

	class _U_MENUorID
	{
	public:
		_U_MENUorID(HMENU hMenu) : m_hMenu(hMenu)
		{
		}
		_U_MENUorID(UINT nID) : m_hMenu((HMENU)(UINT_PTR)nID)
		{
		}
		HMENU m_hMenu;
	};

	class _U_STRINGorID
	{
	public:
		_U_STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{
		}
		_U_STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{
		}
		LPCTSTR m_lpstr;
	};
	/////////////////////////////////////////////////////////////////////////////
	// CWindow - client side for a Windows window

	class CWindow
	{
	public:
		static RECT rcDefault;
		HWND m_hWnd;

		CWindow(HWND hWnd = NULL) throw() :
			m_hWnd(hWnd)
		{
		}

		CWindow& operator=(HWND hWnd) throw()
		{
			m_hWnd = hWnd;
			return *this;
		}

		static LPCTSTR GetWndClassName() throw()
		{
			return NULL;
		}

		void Attach(_In_opt_ HWND hWndNew) throw()
		{
			CTLASSERT((hWndNew == NULL) || ::IsWindow(hWndNew));
			m_hWnd = hWndNew;
		}

		HWND Detach() throw()
		{
			HWND hWnd = m_hWnd;
			m_hWnd = NULL;
			return hWnd;
		}

		HWND Create(
			LPCTSTR lpstrWndClass,
			HWND hWndParent,
		    _U_RECT rect = NULL,
			LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0,
			DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U,
			LPVOID lpCreateParam = NULL) throw()
		{
			if (rect.m_lpRect == NULL)
				rect.m_lpRect = &rcDefault;
			m_hWnd = ::CreateWindowEx(dwExStyle, lpstrWndClass, szWindowName,
				dwStyle, rect.m_lpRect->left, rect.m_lpRect->top, rect.m_lpRect->right - rect.m_lpRect->left,
				rect.m_lpRect->bottom - rect.m_lpRect->top, hWndParent, MenuOrID.m_hMenu,
				_CTLBaseModule.GetModuleInstance(), lpCreateParam);
			return m_hWnd;
		}

		BOOL DestroyWindow() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));

			if (!::DestroyWindow(m_hWnd))
				return FALSE;

			m_hWnd = NULL;
			return TRUE;
		}

		// Attributes

		operator HWND() const throw()
		{
			return m_hWnd;
		}

		DWORD GetStyle() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (DWORD)::GetWindowLong(m_hWnd, GWL_STYLE);
		}

		DWORD GetExStyle() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (DWORD)::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		}

		LONG GetWindowLong(int nIndex) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowLong(m_hWnd, nIndex);
		}

		LONG_PTR GetWindowLongPtr(int nIndex) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowLongPtr(m_hWnd, nIndex);
		}

		LONG SetWindowLong(
			_In_ int nIndex,
			_In_ LONG dwNewLong) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowLong(m_hWnd, nIndex, dwNewLong);
		}

		LONG_PTR SetWindowLongPtr(
			int nIndex,
			LONG_PTR dwNewLong) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowLongPtr(m_hWnd, nIndex, dwNewLong);
		}

		WORD GetWindowWord(_In_ int nIndex) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowWord(m_hWnd, nIndex);
		}

		WORD SetWindowWord(
			_In_ int nIndex,
			_In_ WORD wNewWord) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowWord(m_hWnd, nIndex, wNewWord);
		}

		// Message Functions

		LRESULT SendMessage(
			_In_ UINT message,
			_In_ WPARAM wParam = 0,
			_In_ LPARAM lParam = 0) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SendMessage(m_hWnd, message, wParam, lParam);
		}

		BOOL PostMessage(
			_In_ UINT message,
			_In_ WPARAM wParam = 0,
			_In_ LPARAM lParam = 0) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::PostMessage(m_hWnd, message, wParam, lParam);
		}

		BOOL SendNotifyMessage(
			_In_ UINT message,
			_In_ WPARAM wParam = 0,
			_In_ LPARAM lParam = 0) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SendNotifyMessage(m_hWnd, message, wParam, lParam);
		}

		// support for C style macros
		static LRESULT SendMessage(
			_In_ HWND hWnd,
			_In_ UINT message,
			_In_ WPARAM wParam,
			_In_ LPARAM lParam) throw()
		{
			CTLASSERT(::IsWindow(hWnd));
			return ::SendMessage(hWnd, message, wParam, lParam);
		}

		// Window Text Functions

		BOOL SetWindowText(_In_z_ LPCTSTR lpszString) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowText(m_hWnd, lpszString);
		}

		int GetWindowText(
			 LPTSTR lpszStringBuf,
			 int nMaxCount) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowText(m_hWnd, lpszStringBuf, nMaxCount);
		}

		int GetWindowTextLength() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowTextLength(m_hWnd);
		}

		// Font Functions

		void SetFont(
			_In_ HFONT hFont,
			_In_ BOOL bRedraw = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(bRedraw, 0));
		}

		HFONT GetFont() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0);
		}

		// Menu Functions (non-child windows only)

		HMENU GetMenu() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (HMENU)::GetMenu(m_hWnd);
		}

		BOOL SetMenu(HMENU hMenu) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetMenu(m_hWnd, hMenu);
		}

		BOOL DrawMenuBar() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::DrawMenuBar(m_hWnd);
		}

		HMENU GetSystemMenu(BOOL bRevert) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (HMENU)::GetSystemMenu(m_hWnd, bRevert);
		}

		BOOL HiliteMenuItem(
			HMENU hMenu,
			UINT uItemHilite,
			UINT uHilite) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::HiliteMenuItem(m_hWnd, hMenu, uItemHilite, uHilite);
		}

		// Window Size and Position Functions

		BOOL IsIconic() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::IsIconic(m_hWnd);
		}

		BOOL IsZoomed() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::IsZoomed(m_hWnd);
		}

		BOOL MoveWindow(
			int x,
			int y,
			int nWidth,
			int nHeight,
			BOOL bRepaint = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint);
		}

		BOOL MoveWindow(
			LPCRECT lpRect,
			BOOL bRepaint = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::MoveWindow(m_hWnd, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
		}

		BOOL SetWindowPos(
			HWND hWndInsertAfter,
			int x,
			int y,
			int cx,
			int cy,
			UINT nFlags) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowPos(m_hWnd, hWndInsertAfter, x, y, cx, cy, nFlags);
		}

		BOOL SetWindowPos(
			HWND hWndInsertAfter,
			LPCRECT lpRect,
			UINT nFlags) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowPos(m_hWnd, hWndInsertAfter, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, nFlags);
		}

		UINT ArrangeIconicWindows() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ArrangeIconicWindows(m_hWnd);
		}

		BOOL BringWindowToTop() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::BringWindowToTop(m_hWnd);
		}

		BOOL GetWindowRect(LPRECT lpRect) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowRect(m_hWnd, lpRect);
		}

		BOOL GetClientRect(LPRECT lpRect) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetClientRect(m_hWnd, lpRect);
		}

		BOOL GetWindowPlacement(WINDOWPLACEMENT FAR* lpwndpl) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowPlacement(m_hWnd, lpwndpl);
		}

		BOOL SetWindowPlacement(const WINDOWPLACEMENT FAR* lpwndpl) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowPlacement(m_hWnd, lpwndpl);
		}

		// Coordinate Mapping Functions

		BOOL ClientToScreen(LPPOINT lpPoint) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ClientToScreen(m_hWnd, lpPoint);
		}

		BOOL ClientToScreen(LPRECT lpRect) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			if (!::ClientToScreen(m_hWnd, (LPPOINT)lpRect))
				return FALSE;
			return ::ClientToScreen(m_hWnd, ((LPPOINT)lpRect) + 1);
		}

		BOOL ScreenToClient(LPPOINT lpPoint) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ScreenToClient(m_hWnd, lpPoint);
		}

		BOOL ScreenToClient(LPRECT lpRect) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			if (!::ScreenToClient(m_hWnd, (LPPOINT)lpRect))
				return FALSE;
			return ::ScreenToClient(m_hWnd, ((LPPOINT)lpRect) + 1);
		}

		int MapWindowPoints(
			HWND hWndTo,
			LPPOINT lpPoint,
			UINT nCount) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::MapWindowPoints(m_hWnd, hWndTo, lpPoint, nCount);
		}

		int MapWindowPoints(
			HWND hWndTo,
			LPRECT lpRect) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::MapWindowPoints(m_hWnd, hWndTo, (LPPOINT)lpRect, 2);
		}

		// Update and Painting Functions

		HDC BeginPaint(LPPAINTSTRUCT lpPaint) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::BeginPaint(m_hWnd, lpPaint);
		}

		void EndPaint(LPPAINTSTRUCT lpPaint) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::EndPaint(m_hWnd, lpPaint);
		}

		HDC GetDC() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetDC(m_hWnd);
		}

		HDC GetWindowDC() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowDC(m_hWnd);
		}

		int ReleaseDC(HDC hDC) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ReleaseDC(m_hWnd, hDC);
		}

		void Print(
			HDC hDC,
			DWORD dwFlags) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_PRINT, (WPARAM)hDC, dwFlags);
		}

		void PrintClient(
			HDC hDC,
			DWORD dwFlags) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_PRINTCLIENT, (WPARAM)hDC, dwFlags);
		}

		BOOL UpdateWindow() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::UpdateWindow(m_hWnd);
		}

		void SetRedraw(BOOL bRedraw = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_SETREDRAW, (WPARAM)bRedraw, 0);
		}

		BOOL GetUpdateRect(
			LPRECT lpRect,
			BOOL bErase = FALSE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetUpdateRect(m_hWnd, lpRect, bErase);
		}

		int GetUpdateRgn(
			HRGN hRgn,
			BOOL bErase = FALSE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetUpdateRgn(m_hWnd, hRgn, bErase);
		}

		BOOL Invalidate(BOOL bErase = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::InvalidateRect(m_hWnd, NULL, bErase);
		}

		BOOL InvalidateRect(
			LPCRECT lpRect,
			BOOL bErase = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::InvalidateRect(m_hWnd, lpRect, bErase);
		}

		BOOL ValidateRect(LPCRECT lpRect) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ValidateRect(m_hWnd, lpRect);
		}

		void InvalidateRgn(
			HRGN hRgn,
			BOOL bErase = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::InvalidateRgn(m_hWnd, hRgn, bErase);
		}

		BOOL ValidateRgn(HRGN hRgn) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ValidateRgn(m_hWnd, hRgn);
		}

		BOOL ShowWindow(int nCmdShow) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ShowWindow(m_hWnd, nCmdShow);
		}

		BOOL IsWindowVisible() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::IsWindowVisible(m_hWnd);
		}

		BOOL ShowOwnedPopups(BOOL bShow = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ShowOwnedPopups(m_hWnd, bShow);
		}

		HDC GetDCEx(
			HRGN hRgnClip,
			DWORD flags) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetDCEx(m_hWnd, hRgnClip, flags);
		}

		BOOL LockWindowUpdate(BOOL bLock = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::LockWindowUpdate(bLock ? m_hWnd : NULL);
		}

		BOOL RedrawWindow(
			LPCRECT lpRectUpdate = NULL,
			HRGN hRgnUpdate = NULL,
			UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::RedrawWindow(m_hWnd, lpRectUpdate, hRgnUpdate, flags);
		}

		// Timer Functions

		UINT_PTR SetTimer(
			UINT_PTR nIDEvent,
			UINT nElapse,
			void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) = NULL) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetTimer(m_hWnd, nIDEvent, nElapse, (TIMERPROC)lpfnTimer);
		}

		BOOL KillTimer(UINT_PTR nIDEvent) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::KillTimer(m_hWnd, nIDEvent);
		}

		// Window State Functions

		BOOL IsWindowEnabled() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::IsWindowEnabled(m_hWnd);
		}

		BOOL EnableWindow(BOOL bEnable = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::EnableWindow(m_hWnd, bEnable);
		}

		HWND SetActiveWindow() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetActiveWindow(m_hWnd);
		}

		HWND SetCapture() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetCapture(m_hWnd);
		}

		HWND SetFocus() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetFocus(m_hWnd);
		}

		// Dialog-Box Item Functions

		BOOL CheckDlgButton(
			int nIDButton,
			UINT nCheck) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::CheckDlgButton(m_hWnd, nIDButton, nCheck);
		}

		BOOL CheckRadioButton(
			int nIDFirstButton,
			int nIDLastButton,
			int nIDCheckButton) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::CheckRadioButton(m_hWnd, nIDFirstButton, nIDLastButton, nIDCheckButton);
		}

		int DlgDirList(
			LPTSTR lpPathSpec,
			int nIDListBox,
			int nIDStaticPath,
			UINT nFileType) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::DlgDirList(m_hWnd, lpPathSpec, nIDListBox, nIDStaticPath, nFileType);
		}

		int DlgDirListComboBox(
			LPTSTR lpPathSpec,
			int nIDComboBox,
			int nIDStaticPath,
			UINT nFileType) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::DlgDirListComboBox(m_hWnd, lpPathSpec, nIDComboBox, nIDStaticPath, nFileType);
		}

		BOOL DlgDirSelect(
		    LPTSTR lpString,
			int nCount,
			int nIDListBox) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::DlgDirSelectEx(m_hWnd, lpString, nCount, nIDListBox);
		}

		BOOL DlgDirSelectComboBox(
			LPTSTR lpString,
			int nCount,
			int nIDComboBox) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::DlgDirSelectComboBoxEx(m_hWnd, lpString, nCount, nIDComboBox);
		}

		UINT GetDlgItemInt(
			int nID,
			BOOL* lpTrans = NULL,
			BOOL bSigned = TRUE) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetDlgItemInt(m_hWnd, nID, lpTrans, bSigned);
		}

		UINT GetDlgItemText(
			int nID,
			LPTSTR lpStr,
			int nMaxCount) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetDlgItemText(m_hWnd, nID, lpStr, nMaxCount);
		}

		CWindow GetNextDlgGroupItem(
			HWND hWndCtl,
			BOOL bPrevious = FALSE) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::GetNextDlgGroupItem(m_hWnd, hWndCtl, bPrevious));
		}

		CWindow GetNextDlgTabItem(
			HWND hWndCtl,
			BOOL bPrevious = FALSE) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::GetNextDlgTabItem(m_hWnd, hWndCtl, bPrevious));
		}

		UINT IsDlgButtonChecked(int nIDButton) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::IsDlgButtonChecked(m_hWnd, nIDButton);
		}

		LRESULT SendDlgItemMessage(
			int nID,
			UINT message,
			WPARAM wParam = 0,
			LPARAM lParam = 0) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SendDlgItemMessage(m_hWnd, nID, message, wParam, lParam);
		}

		BOOL SetDlgItemInt(
			int nID,
			UINT nValue,
			BOOL bSigned = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetDlgItemInt(m_hWnd, nID, nValue, bSigned);
		}

		BOOL SetDlgItemText(
			int nID,
			LPCTSTR lpszString) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetDlgItemText(m_hWnd, nID, lpszString);
		}

			// Scrolling Functions

		int GetScrollPos(int nBar) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetScrollPos(m_hWnd, nBar);
		}

		BOOL GetScrollRange(
			int nBar,
			LPINT lpMinPos,
			LPINT lpMaxPos) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetScrollRange(m_hWnd, nBar, lpMinPos, lpMaxPos);
		}

		BOOL ScrollWindow(
			int xAmount,
			int yAmount,
			LPCRECT lpRect = NULL,
			LPCRECT lpClipRect = NULL) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ScrollWindow(m_hWnd, xAmount, yAmount, lpRect, lpClipRect);
		}

		int ScrollWindowEx(
			int dx,
			int dy,
			LPCRECT lpRectScroll,
			LPCRECT lpRectClip,
			HRGN hRgnUpdate,
			LPRECT lpRectUpdate,
			UINT uFlags) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ScrollWindowEx(m_hWnd, dx, dy, lpRectScroll, lpRectClip, hRgnUpdate, lpRectUpdate, uFlags);
		}

		int ScrollWindowEx(
			int dx,
			int dy,
			UINT uFlags,
			LPCRECT lpRectScroll = NULL,
			LPCRECT lpRectClip = NULL,
			HRGN hRgnUpdate = NULL,
			LPRECT lpRectUpdate = NULL) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ScrollWindowEx(m_hWnd, dx, dy, lpRectScroll, lpRectClip, hRgnUpdate, lpRectUpdate, uFlags);
		}

		int SetScrollPos(
			int nBar,
			int nPos,
			BOOL bRedraw = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetScrollPos(m_hWnd, nBar, nPos, bRedraw);
		}

		BOOL SetScrollRange(
			int nBar,
			int nMinPos,
			int nMaxPos,
			BOOL bRedraw = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetScrollRange(m_hWnd, nBar, nMinPos, nMaxPos, bRedraw);
		}

		BOOL ShowScrollBar(
			UINT nBar,
			BOOL bShow = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ShowScrollBar(m_hWnd, nBar, bShow);
		}

		BOOL EnableScrollBar(
			UINT uSBFlags,
			UINT uArrowFlags = ESB_ENABLE_BOTH) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::EnableScrollBar(m_hWnd, uSBFlags, uArrowFlags);
		}

		// Window Access Functions

		CWindow ChildWindowFromPoint(POINT point) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::ChildWindowFromPoint(m_hWnd, point));
		}

		CWindow ChildWindowFromPointEx(
			POINT point,
			UINT uFlags) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::ChildWindowFromPointEx(m_hWnd, point, uFlags));
		}

		CWindow GetTopWindow() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::GetTopWindow(m_hWnd));
		}

		CWindow GetWindow(UINT nCmd) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::GetWindow(m_hWnd, nCmd));
		}

		CWindow GetLastActivePopup() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::GetLastActivePopup(m_hWnd));
		}

		BOOL IsChild(HWND hWnd) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::IsChild(m_hWnd, hWnd);
		}

		CWindow GetParent() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::GetParent(m_hWnd));
		}

		CWindow SetParent(HWND hWndNewParent) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::SetParent(m_hWnd, hWndNewParent));
		}

		// Window Tree Access

		int GetDlgCtrlID() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetDlgCtrlID(m_hWnd);
		}

		int SetDlgCtrlID(int nID) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (int)::SetWindowLong(m_hWnd, GWL_ID, nID);
		}

		CWindow GetDlgItem(int nID) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return CWindow(::GetDlgItem(m_hWnd, nID));
		}

		// Alert Functions

		BOOL FlashWindow(BOOL bInvert) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::FlashWindow(m_hWnd, bInvert);
		}

		int MessageBox(
			LPCTSTR lpszText,
			LPCTSTR lpszCaption = _T(""),
			UINT nType = MB_OK) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::MessageBox(m_hWnd, lpszText, lpszCaption, nType);
		}

		// Clipboard Functions

		BOOL ChangeClipboardChain(HWND hWndNewNext) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ChangeClipboardChain(m_hWnd, hWndNewNext);
		}

		HWND SetClipboardViewer() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetClipboardViewer(m_hWnd);
		}

		BOOL OpenClipboard() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::OpenClipboard(m_hWnd);
		}

		// Caret Functions

		BOOL CreateCaret(HBITMAP hBitmap) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::CreateCaret(m_hWnd, hBitmap, 0, 0);
		}

		BOOL CreateSolidCaret(int nWidth, int nHeight) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::CreateCaret(m_hWnd, (HBITMAP)0, nWidth, nHeight);
		}

		BOOL CreateGrayCaret(int nWidth, int nHeight) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::CreateCaret(m_hWnd, (HBITMAP)1, nWidth, nHeight);
		}

		BOOL HideCaret() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::HideCaret(m_hWnd);
		}

		BOOL ShowCaret() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ShowCaret(m_hWnd);
		}

#ifdef _INC_SHELLAPI
		// Drag-Drop Functions
		void DragAcceptFiles(BOOL bAccept = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd)); ::DragAcceptFiles(m_hWnd, bAccept);
		}
#endif

		// Icon Functions

		HICON SetIcon(
			HICON hIcon,
			BOOL bBigIcon = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (HICON)::SendMessage(m_hWnd, WM_SETICON, bBigIcon, (LPARAM)hIcon);
		}

		HICON GetIcon(BOOL bBigIcon = TRUE) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (HICON)::SendMessage(m_hWnd, WM_GETICON, bBigIcon, 0);
		}

		// Help Functions

		BOOL WinHelp(
			LPCTSTR lpszHelp,
			UINT nCmd = HELP_CONTEXT,
			DWORD dwData = 0) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::WinHelp(m_hWnd, lpszHelp, nCmd, dwData);
		}

		BOOL SetWindowContextHelpId(_In_ DWORD dwContextHelpId) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowContextHelpId(m_hWnd, dwContextHelpId);
		}

		DWORD GetWindowContextHelpId() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowContextHelpId(m_hWnd);
		}

		// Hot Key Functions

		int SetHotKey(
			WORD wVirtualKeyCode,
			WORD wModifiers) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, WM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0);
		}

		DWORD GetHotKey() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return (DWORD)::SendMessage(m_hWnd, WM_GETHOTKEY, 0, 0);
		}

		// Misc. Operations

		//N new
		BOOL GetScrollInfo(
			int nBar,
			LPSCROLLINFO lpScrollInfo) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetScrollInfo(m_hWnd, nBar, lpScrollInfo);
		}
		int SetScrollInfo(
			int nBar,
			LPSCROLLINFO lpScrollInfo,
			BOOL bRedraw = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetScrollInfo(m_hWnd, nBar, lpScrollInfo, bRedraw);
		}
		BOOL IsDialogMessage(_In_ LPMSG lpMsg) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::IsDialogMessage(m_hWnd, lpMsg);
		}

		void NextDlgCtrl() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_NEXTDLGCTL, 0, 0L);
		}
		void PrevDlgCtrl() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_NEXTDLGCTL, 1, 0L);
		}
		void GotoDlgCtrl(HWND hWndCtrl) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_NEXTDLGCTL, (WPARAM)hWndCtrl, 1L);
		}

		BOOL ResizeClient(
			int nWidth,
			int nHeight,
			BOOL bRedraw = TRUE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));

			RECT rcWnd;
			if (!GetClientRect(&rcWnd))
				return FALSE;

			if (nWidth != -1)
				rcWnd.right = nWidth;
			if (nHeight != -1)
				rcWnd.bottom = nHeight;

			if (!::AdjustWindowRectEx(&rcWnd, GetStyle(), (!(GetStyle() & WS_CHILD) && (GetMenu() != NULL)), GetExStyle()))
				return FALSE;

			UINT uFlags = SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE;
			if (!bRedraw)
				uFlags |= SWP_NOREDRAW;

			return SetWindowPos(NULL, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, uFlags);
		}

		int GetWindowRgn(HRGN hRgn) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowRgn(m_hWnd, hRgn);
		}
		int SetWindowRgn(
			HRGN hRgn,
			BOOL bRedraw = FALSE) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::SetWindowRgn(m_hWnd, hRgn, bRedraw);
		}
		HDWP DeferWindowPos(
			HDWP hWinPosInfo,
			HWND hWndInsertAfter,
			int x,
			int y,
			int cx,
			int cy,
			UINT uFlags) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::DeferWindowPos(hWinPosInfo, m_hWnd, hWndInsertAfter, x, y, cx, cy, uFlags);
		}
		DWORD GetWindowThreadID() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::GetWindowThreadProcessId(m_hWnd, NULL);
		}
		DWORD GetWindowProcessID() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			DWORD dwProcessID;
			::GetWindowThreadProcessId(m_hWnd, &dwProcessID);
			return dwProcessID;
		}
		BOOL IsWindow() const throw()
		{
			return ::IsWindow(m_hWnd);
		}
		BOOL IsWindowUnicode() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::IsWindowUnicode(m_hWnd);
		}
		BOOL IsParentDialog() throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			TCHAR szBuf[8]; // "#32770" + NUL character
			if (GetClassName(GetParent(), szBuf, sizeof(szBuf) / sizeof(szBuf[0])) == 0)
				return FALSE;
			return lstrcmp(szBuf, _T("#32770")) == 0;
		}
		BOOL ShowWindowAsync(int nCmdShow) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));
			return ::ShowWindowAsync(m_hWnd, nCmdShow);
		}

		CWindow GetDescendantWindow(int nID) const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));

			// GetDlgItem recursive (return first found)
			// breadth-first for 1 level, then depth-first for next level

			// use GetDlgItem since it is a fast USER function
			HWND hWndChild, hWndTmp;
			if ((hWndChild = ::GetDlgItem(m_hWnd, nID)) != NULL)
			{
				if (::GetTopWindow(hWndChild) != NULL)
				{
					// children with the same ID as their parent have priority
					CWindow wnd(hWndChild);
					hWndTmp = wnd.GetDescendantWindow(nID);
					if (hWndTmp != NULL)
						return CWindow(hWndTmp);
				}
				return CWindow(hWndChild);
			}

			// walk each child
			for (hWndChild = ::GetTopWindow(m_hWnd); hWndChild != NULL;
			hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
			{
				CWindow wnd(hWndChild);
				hWndTmp = wnd.GetDescendantWindow(nID);
				if (hWndTmp != NULL)
					return CWindow(hWndTmp);
			}

			return CWindow(NULL);    // not found
		}

		void SendMessageToDescendants(
			UINT message,
			WPARAM wParam = 0,
			LPARAM lParam = 0,
			BOOL bDeep = TRUE) throw()
		{
			for (HWND hWndChild = ::GetTopWindow(m_hWnd); hWndChild != NULL;
			hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
			{
				::SendMessage(hWndChild, message, wParam, lParam);

				if (bDeep && ::GetTopWindow(hWndChild) != NULL)
				{
					// send to child windows after parent
					CWindow wnd(hWndChild);
					wnd.SendMessageToDescendants(message, wParam, lParam, bDeep);
				}
			}
		}

		BOOL CenterWindow(HWND hWndCenter = NULL) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));

			// determine owner window to center against
			DWORD dwStyle = GetStyle();
			if (hWndCenter == NULL)
			{
				if (dwStyle & WS_CHILD)
					hWndCenter = ::GetParent(m_hWnd);
				else
					hWndCenter = ::GetWindow(m_hWnd, GW_OWNER);
			}

			// get coordinates of the window relative to its parent
			RECT rcDlg;
			::GetWindowRect(m_hWnd, &rcDlg);
			RECT rcArea;
			RECT rcCenter;
			HWND hWndParent;
			if (!(dwStyle & WS_CHILD))
			{
				// don't center against invisible or minimized windows
				if (hWndCenter != NULL)
				{
					DWORD dwStyleCenter = ::GetWindowLong(hWndCenter, GWL_STYLE);
					if (!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
						hWndCenter = NULL;
				}

				// center within screen coordinates
				HMONITOR hMonitor = NULL;
				if (hWndCenter != NULL)
				{
					hMonitor = ::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST);
				}
				else
				{
					hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
				}
				//ATLENSURE_RETURN_VAL(hMonitor != NULL, FALSE);

				MONITORINFO minfo;
				minfo.cbSize = sizeof(MONITORINFO);
				BOOL bResult = ::GetMonitorInfo(hMonitor, &minfo);
				//ATLENSURE_RETURN_VAL(bResult, FALSE);

				rcArea = minfo.rcWork;

				if (hWndCenter == NULL)
					rcCenter = rcArea;
				else
					::GetWindowRect(hWndCenter, &rcCenter);
			}
			else
			{
				// center within parent client coordinates
				hWndParent = ::GetParent(m_hWnd);
				CTLASSERT(::IsWindow(hWndParent));

				::GetClientRect(hWndParent, &rcArea);
				CTLASSERT(::IsWindow(hWndCenter));
				::GetClientRect(hWndCenter, &rcCenter);
				::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
			}

			int DlgWidth = rcDlg.right - rcDlg.left;
			int DlgHeight = rcDlg.bottom - rcDlg.top;

			// find dialog's upper left based on rcCenter
			int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
			int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

			// if the dialog is outside the screen, move it inside
			if (xLeft + DlgWidth > rcArea.right)
				xLeft = rcArea.right - DlgWidth;
			if (xLeft < rcArea.left)
				xLeft = rcArea.left;

			if (yTop + DlgHeight > rcArea.bottom)
				yTop = rcArea.bottom - DlgHeight;
			if (yTop < rcArea.top)
				yTop = rcArea.top;

			// map screen coordinates to child coordinates
			return ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}

		BOOL ModifyStyle(
			DWORD dwRemove,
			DWORD dwAdd,
			UINT nFlags = 0) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));

			DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
			DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
			if (dwStyle == dwNewStyle)
				return FALSE;

			::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);
			if (nFlags != 0)
			{
				::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
					SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
			}

			return TRUE;
		}

		BOOL ModifyStyleEx(
			DWORD dwRemove,
			DWORD dwAdd,
			UINT nFlags = 0) throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));

			DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
			DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
			if (dwStyle == dwNewStyle)
				return FALSE;

			::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
			if (nFlags != 0)
			{
				::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
					SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
			}

			return TRUE;
		}

		CWindow GetTopLevelParent() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));

			HWND hWndParent = m_hWnd;
			HWND hWndTmp;
			while ((hWndTmp = ::GetParent(hWndParent)) != NULL)
				hWndParent = hWndTmp;

			return CWindow(hWndParent);
		}

		CWindow GetTopLevelWindow() const throw()
		{
			CTLASSERT(::IsWindow(m_hWnd));

			HWND hWndParent;
			HWND hWndTmp = m_hWnd;

			do
			{
				hWndParent = hWndTmp;
				hWndTmp = (::GetWindowLong(hWndParent, GWL_STYLE) & WS_CHILD) ? ::GetParent(hWndParent) : ::GetWindow(hWndParent, GW_OWNER);
			} while (hWndTmp != NULL);

			return CWindow(hWndParent);
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	// _CTL_MSG - extended MSG structure

	struct _CTL_MSG :
		public MSG
	{
	public:
		// Additional data members
		int cbSize;
		BOOL bHandled;

		// Constructors
		_CTL_MSG() : cbSize(sizeof(_CTL_MSG)), bHandled(TRUE)
		{
			hwnd = NULL;
			message = 0;
			wParam = 0;
			lParam = 0;
			time = 0;
			pt.x = pt.y = 0;
		}
		_CTL_MSG(
			HWND hWnd,
			UINT uMsg,
			WPARAM wParamIn,
			LPARAM lParamIn,
			DWORD dwTime,
			POINT ptIn,
			BOOL bHandledIn) :
			cbSize(sizeof(_CTL_MSG)), bHandled(bHandledIn)
		{
			hwnd = hWnd;
			message = uMsg;
			wParam = wParamIn;
			lParam = lParamIn;
			time = dwTime;
			pt = ptIn;
		}
		_CTL_MSG(
			HWND hWnd,
			UINT uMsg,
			WPARAM wParamIn,
			LPARAM lParamIn,
			BOOL bHandledIn = TRUE) :
			cbSize(sizeof(_CTL_MSG)), bHandled(bHandledIn)
		{
			hwnd = hWnd;
			message = uMsg;
			wParam = wParamIn;
			lParam = lParamIn;
			time = 0;
			pt.x = pt.y = 0;
		}
		_CTL_MSG(
			MSG& msg,
			BOOL bHandledIn = TRUE) :
			cbSize(sizeof(_CTL_MSG)), bHandled(bHandledIn)
		{
			hwnd = msg.hwnd;
			message = msg.message;
			wParam = msg.wParam;
			lParam = msg.lParam;
			time = msg.time;
			pt = msg.pt;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	// CMessageMap - abstract class that provides an interface for message maps

	class CTL_NO_VTABLE CMessageMap
	{
	public:
		virtual BOOL ProcessWindowMessage(
			HWND hWnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam,
			LRESULT& lResult,
			DWORD dwMsgMapID) = 0;
	};

	/////////////////////////////////////////////////////////////////////////////
	// CWindowImpl - Implements a window

	template <class TBase /* = CWindow */>
	class CTL_NO_VTABLE CWindowImplRoot :
		public TBase,
		public CMessageMap
	{
	public:
		CWndProcThunk m_thunk;
		const _CTL_MSG* m_pCurrentMsg;
		DWORD m_dwState;

		enum { WINSTATE_DESTROYED = 0x00000001 };

		// Constructor/destructor
		CWindowImplRoot() : m_pCurrentMsg(NULL), m_dwState(0)
		{
		}

		virtual ~CWindowImplRoot()
		{
#ifdef _DEBUG
			if (m_hWnd != NULL)	// should be cleared in WindowProc
			{
				//ATLTRACE(atlTraceWindowing, 0, _T("ERROR - Object deleted before window was destroyed\n"));
				CTLASSERT(FALSE);
			}
#endif //_DEBUG
		}

		// Current message
		const _CTL_MSG* GetCurrentMessage() const
		{
			return m_pCurrentMsg;
		}

		// "handled" management for cracked handlers
		BOOL IsMsgHandled() const
		{
			const _CTL_MSG* pMsg = GetCurrentMessage();
			CTLASSERT(pMsg->cbSize >= sizeof(_CTL_MSG));
			return pMsg->bHandled;
		}
		void SetMsgHandled(BOOL bHandled)
		{
			_CTL_MSG* pMsg = (_CTL_MSG*)GetCurrentMessage();	// override const
			CTLASSERT(pMsg->cbSize >= sizeof(_CTL_MSG));
			pMsg->bHandled = bHandled;
		}

		// Message forwarding and reflection support
		LRESULT ForwardNotifications(
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam,
			BOOL& bHandled);
		LRESULT ReflectNotifications(
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam,
			BOOL& bHandled);
		BOOL DefaultReflectionHandler(
			HWND hWnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam,
			LRESULT& lResult);
	};

	template <class TBase>
	LRESULT CWindowImplRoot< TBase >::ForwardNotifications(
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam,
		BOOL& bHandled)
	{
		LRESULT lResult = 0;
		switch (uMsg)
		{
		case WM_COMMAND:
		case WM_NOTIFY:
		case WM_PARENTNOTIFY:
		case WM_DRAWITEM:
		case WM_MEASUREITEM:
		case WM_COMPAREITEM:
		case WM_DELETEITEM:
		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			lResult = GetParent().SendMessage(uMsg, wParam, lParam);
			break;
		default:
			bHandled = FALSE;
			break;
		}
		return lResult;
	}

	template <class TBase>
	LRESULT CWindowImplRoot< TBase >::ReflectNotifications(
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam,
		BOOL& bHandled)
	{
		HWND hWndChild = NULL;

		switch (uMsg)
		{
		case WM_COMMAND:
			if (lParam != NULL)	// not from a menu
				hWndChild = (HWND)lParam;
			break;
		case WM_NOTIFY:
			hWndChild = ((LPNMHDR)lParam)->hwndFrom;
			break;
		case WM_PARENTNOTIFY:
			switch (LOWORD(wParam))
			{
			case WM_CREATE:
			case WM_DESTROY:
				hWndChild = (HWND)lParam;
				break;
			default:
				hWndChild = GetDlgItem(HIWORD(wParam));
				break;
			}
			break;
		case WM_DRAWITEM:
			if (wParam)	// not from a menu
				hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
			break;
		case WM_MEASUREITEM:
			if (wParam)	// not from a menu
				hWndChild = GetDlgItem(((LPMEASUREITEMSTRUCT)lParam)->CtlID);
			break;
		case WM_COMPAREITEM:
			if (wParam)	// not from a menu
				hWndChild = ((LPCOMPAREITEMSTRUCT)lParam)->hwndItem;
			break;
		case WM_DELETEITEM:
			if (wParam)	// not from a menu
				hWndChild = ((LPDELETEITEMSTRUCT)lParam)->hwndItem;

			break;
		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL:
		case WM_VSCROLL:
			hWndChild = (HWND)lParam;
			break;
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			hWndChild = (HWND)lParam;
			break;
		default:
			break;
		}

		if (hWndChild == NULL)
		{
			bHandled = FALSE;
			return 1;
		}

		CTLASSERT(::IsWindow(hWndChild));
		return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
	}

	template <class TBase>
	BOOL CWindowImplRoot< TBase >::DefaultReflectionHandler(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam,
		LRESULT& lResult)
	{
		switch (uMsg)
		{
		//case OCM_COMMAND:
		//case OCM_NOTIFY:
		//case OCM_PARENTNOTIFY:
		//case OCM_DRAWITEM:
		//case OCM_MEASUREITEM:
		//case OCM_COMPAREITEM:
		//case OCM_DELETEITEM:
		//case OCM_VKEYTOITEM:
		//case OCM_CHARTOITEM:
		//case OCM_HSCROLL:
		//case OCM_VSCROLL:
		//case OCM_CTLCOLORBTN:
		//case OCM_CTLCOLORDLG:
		//case OCM_CTLCOLOREDIT:
		//case OCM_CTLCOLORLISTBOX:
		//case OCM_CTLCOLORMSGBOX:
		//case OCM_CTLCOLORSCROLLBAR:
		//case OCM_CTLCOLORSTATIC:
		//	lResult = ::DefWindowProc(hWnd, uMsg - OCM__BASE, wParam, lParam);
		//	return TRUE;
		default:
			break;
		}
		return FALSE;
	}

	template <class TBase = CWindow, class TWinTraits = CControlWinTraits>
	class CTL_NO_VTABLE CWindowImplBaseT :
		public CWindowImplRoot< TBase >
	{
	public:
		WNDPROC m_pfnSuperWindowProc;

		CWindowImplBaseT() : m_pfnSuperWindowProc(::DefWindowProc)
		{
		}

		static DWORD GetWndStyle(DWORD dwStyle)
		{
			return TWinTraits::GetWndStyle(dwStyle);
		}
		static DWORD GetWndExStyle(DWORD dwExStyle)
		{
			return TWinTraits::GetWndExStyle(dwExStyle);
		}

		virtual WNDPROC GetWindowProc()
		{
			return WindowProc;
		}
		static LRESULT CALLBACK StartWindowProc(
			HWND hWnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK WindowProc(
			HWND hWnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam);

		HWND Create(
			HWND hWndParent,
			_U_RECT rect,
			LPCTSTR szWindowName,
			DWORD dwStyle,
			DWORD dwExStyle,
			_U_MENUorID MenuOrID,
			ATOM atom,
			LPVOID lpCreateParam = NULL);

		BOOL DestroyWindow()
		{
			if (!::DestroyWindow(m_hWnd))
			{
				return FALSE;
			}

			return TRUE;
		}
		BOOL SubclassWindow(HWND hWnd);
		HWND UnsubclassWindow(BOOL bForce = FALSE);

		LRESULT DefWindowProc()
		{
			const _CTL_MSG* pMsg = m_pCurrentMsg;
			LRESULT lRes = 0;
			if (pMsg != NULL)
				lRes = DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
			return lRes;
		}

		LRESULT DefWindowProc(
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam)
		{
			return ::CallWindowProc(m_pfnSuperWindowProc, m_hWnd, uMsg, wParam, lParam);
		}

		virtual void OnFinalMessage(_In_ HWND /*hWnd*/)
		{
			// override to do something, if needed
		}
	};

	typedef CWindowImplBaseT<CWindow>	CWindowImplBase;

	template <class TBase, class TWinTraits>
	LRESULT CALLBACK CWindowImplBaseT< TBase, TWinTraits >::StartWindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam)
	{
		CWindowImplBaseT< TBase, TWinTraits >* pThis = (CWindowImplBaseT< TBase, TWinTraits >*)_AtlWinModule.ExtractCreateWndData();
		CTLASSERT(pThis != NULL);
		if (!pThis)
		{
			return 0;
		}
		pThis->m_hWnd = hWnd;

		// Initialize the thunk.  This is allocated in CWindowImplBaseT::Create,
		// so failure is unexpected here.

		pThis->m_thunk.Init(pThis->GetWindowProc(), pThis);
		WNDPROC pProc = pThis->m_thunk.GetWNDPROC();
		WNDPROC pOldProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);
#ifdef _DEBUG
		// check if somebody has subclassed us already since we discard it
		if (pOldProc != StartWindowProc)
			;
			//ATLTRACE(atlTraceWindowing, 0, _T("Subclassing through a hook discarded.\n"));
#else
		(pOldProc);	// avoid unused warning
#endif
		return pProc(hWnd, uMsg, wParam, lParam);
	}

	template <class TBase, class TWinTraits>
	LRESULT CALLBACK CWindowImplBaseT< TBase, TWinTraits >::WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam)
	{
		CWindowImplBaseT< TBase, TWinTraits >* pThis = (CWindowImplBaseT< TBase, TWinTraits >*)hWnd;
		// set a ptr to this message and save the old value
		_CTL_MSG msg(pThis->m_hWnd, uMsg, wParam, lParam);
		const _CTL_MSG* pOldMsg = pThis->m_pCurrentMsg;
		pThis->m_pCurrentMsg = &msg;
		// pass to the message map to process
		LRESULT lRes = 0;
		BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);
		// restore saved value for the current message
		ATLASSERT(pThis->m_pCurrentMsg == &msg);

		// do the default processing if message was not handled
		if (!bRet)
		{
			if (uMsg != WM_NCDESTROY)
				lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
			else
			{
				// unsubclass, if needed
				LONG_PTR pfnWndProc = ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC);
				lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
				if (pThis->m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC) == pfnWndProc)
					::SetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC, (LONG_PTR)pThis->m_pfnSuperWindowProc);
				// mark window as destryed
				pThis->m_dwState |= WINSTATE_DESTROYED;
			}
		}
		if ((pThis->m_dwState & WINSTATE_DESTROYED) && pOldMsg == NULL)
		{
			// clear out window handle
			HWND hWndThis = pThis->m_hWnd;
			pThis->m_hWnd = NULL;
			pThis->m_dwState &= ~WINSTATE_DESTROYED;
			// clean up after window is destroyed
			pThis->m_pCurrentMsg = pOldMsg;
			pThis->OnFinalMessage(hWndThis);
		}
		else {
			pThis->m_pCurrentMsg = pOldMsg;
		}
		return lRes;
	}

	template <class TBase, class TWinTraits>
	HWND CWindowImplBaseT< TBase, TWinTraits >::Create(
		HWND hWndParent,
		_U_RECT rect,
		LPCTSTR szWindowName,
		DWORD dwStyle,
		DWORD dwExStyle,
		_U_MENUorID MenuOrID,
		ATOM atom,
		LPVOID lpCreateParam)
	{
		ATLASSUME(m_hWnd == NULL);

		// Allocate the thunk structure here, where we can fail gracefully.
		BOOL result = m_thunk.Init(NULL, NULL);
		if (result == FALSE) {
			SetLastError(ERROR_OUTOFMEMORY);
			return NULL;
		}

		if (atom == 0)
			return NULL;

		_AtlWinModule.AddCreateWndData(&m_thunk.cd, this);

		if (MenuOrID.m_hMenu == NULL && (dwStyle & WS_CHILD))
			MenuOrID.m_hMenu = (HMENU)(UINT_PTR)this;
		if (rect.m_lpRect == NULL)
			rect.m_lpRect = &TBase::rcDefault;

		HWND hWnd = ::CreateWindowEx(dwExStyle, MAKEINTATOM(atom), szWindowName,
			dwStyle, rect.m_lpRect->left, rect.m_lpRect->top, rect.m_lpRect->right - rect.m_lpRect->left,
			rect.m_lpRect->bottom - rect.m_lpRect->top, hWndParent, MenuOrID.m_hMenu,
			_CTLBaseModule.GetModuleInstance(), lpCreateParam);

		ATLASSUME(m_hWnd == hWnd);

		return hWnd;
	}

	template <class TBase, class TWinTraits>
	BOOL CWindowImplBaseT< TBase, TWinTraits >::SubclassWindow(HWND hWnd)
	{
		CTLASSERT(::IsWindow(hWnd));

		// Allocate the thunk structure here, where we can fail gracefully.

		BOOL result = m_thunk.Init(GetWindowProc(), this);
		if (result == FALSE)
		{
			return FALSE;
		}
		WNDPROC pProc = m_thunk.GetWNDPROC();
		WNDPROC pfnWndProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);
		if (pfnWndProc == NULL)
			return FALSE;
		m_pfnSuperWindowProc = pfnWndProc;
		m_hWnd = hWnd;
		return TRUE;
	}

	// Use only if you want to subclass before window is destroyed,
	// WindowProc will automatically subclass when  window goes away
	template <class TBase, class TWinTraits>
	HWND CWindowImplBaseT< TBase, TWinTraits >::UnsubclassWindow(BOOL bForce /*= FALSE*/)
	{

		WNDPROC pOurProc = m_thunk.GetWNDPROC();
		WNDPROC pActiveProc = (WNDPROC)::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);

		HWND hWnd = NULL;
		if (bForce || pOurProc == pActiveProc)
		{
			if (!::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pfnSuperWindowProc))
				return NULL;

			m_pfnSuperWindowProc = ::DefWindowProc;
			hWnd = m_hWnd;
			m_hWnd = NULL;
		}
		return hWnd;
	}

	template <class T, class TBase /* = CWindow */, class TWinTraits /* = CControlWinTraits */>
	class ATL_NO_VTABLE CWindowImpl :
		public CWindowImplBaseT< TBase, TWinTraits >
	{
	public:
		DECLARE_WND_CLASS(NULL)

		static LPCTSTR GetWndCaption()
		{
			return NULL;
		}

		HWND Create(
			HWND hWndParent,
			_U_RECT rect = NULL,
			LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0,
			DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U,
			LPVOID lpCreateParam = NULL)
		{
			if (T::GetWndClassInfo().m_lpszOrigName == NULL)
				T::GetWndClassInfo().m_lpszOrigName = GetWndClassName();
			ATOM atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);

			dwStyle = T::GetWndStyle(dwStyle);
			dwExStyle = T::GetWndExStyle(dwExStyle);

			// set caption
			if (szWindowName == NULL)
				szWindowName = T::GetWndCaption();

			return CWindowImplBaseT< TBase, TWinTraits >::Create(hWndParent, rect, szWindowName,
				dwStyle, dwExStyle, MenuOrID, atom, lpCreateParam);
		}
	};

};

#endif