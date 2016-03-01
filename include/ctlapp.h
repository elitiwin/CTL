#ifndef __CTLAPP_H__
#define __CTLAPP_H__
#include "ctlutil.h"
namespace CTL
{
	///////////////////////////////////////////////////////////////////////////////
	// CMessageFilter - Interface for message filter support

	class CMessageFilter
	{
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
	};


	///////////////////////////////////////////////////////////////////////////////
	// CIdleHandler - Interface for idle processing

	class CIdleHandler
	{
	public:
		virtual BOOL OnIdle() = 0;
	};
	class CMessageLoop
	{
	public:
		CSimpleArray<CMessageFilter*> m_aMsgFilter;
		CSimpleArray<CIdleHandler*> m_aIdleHandler;
		MSG m_msg;

		// Message filter operations
		BOOL AddMessageFilter(CMessageFilter* pMessageFilter)
		{
			return m_aMsgFilter.Add(pMessageFilter);
		}

		BOOL RemoveMessageFilter(CMessageFilter* pMessageFilter)
		{
			return m_aMsgFilter.Remove(pMessageFilter);
		}

		// Idle handler operations
		BOOL AddIdleHandler(CIdleHandler* pIdleHandler)
		{
			return m_aIdleHandler.Add(pIdleHandler);
		}

		BOOL RemoveIdleHandler(CIdleHandler* pIdleHandler)
		{
			return m_aIdleHandler.Remove(pIdleHandler);
		}

		// message loop
		int Run()
		{
			BOOL bDoIdle = TRUE;
			int nIdleCount = 0;
			BOOL bRet;

			for (;;)
			{
				while (bDoIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
				{
					if (!OnIdle(nIdleCount++))
						bDoIdle = FALSE;
				}

				bRet = ::GetMessage(&m_msg, NULL, 0, 0);

				if (bRet == -1)
				{
					//ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
					continue;   // error, don't process
				}
				else if (!bRet)
				{
					//ATLTRACE2(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
					break;   // WM_QUIT, exit message loop
				}

				if (!PreTranslateMessage(&m_msg))
				{
					::TranslateMessage(&m_msg);
					::DispatchMessage(&m_msg);
				}

				if (IsIdleMessage(&m_msg))
				{
					bDoIdle = TRUE;
					nIdleCount = 0;
				}
			}

			return (int)m_msg.wParam;
		}

		static BOOL IsIdleMessage(MSG* pMsg)
		{
			// These messages should NOT cause idle processing
			switch (pMsg->message)
			{
			case WM_MOUSEMOVE:
			case WM_PAINT:
			case 0x0118:	// WM_SYSTIMER (caret blink)
				return FALSE;
			}

			return TRUE;
		}

		// Overrideables
		// Override to change message filtering
		virtual BOOL PreTranslateMessage(MSG* pMsg)
		{
			// loop backwards
			for (int i = m_aMsgFilter.GetSize() - 1; i >= 0; i--)
			{
				CMessageFilter* pMessageFilter = m_aMsgFilter[i];
				if (pMessageFilter != NULL && pMessageFilter->PreTranslateMessage(pMsg))
					return TRUE;
			}
			return FALSE;   // not translated
		}

		// override to change idle processing
		virtual BOOL OnIdle(int /*nIdleCount*/)
		{
			for (int i = 0; i < m_aIdleHandler.GetSize(); i++)
			{
				CIdleHandler* pIdleHandler = m_aIdleHandler[i];
				if (pIdleHandler != NULL)
					pIdleHandler->OnIdle();
			}
			return FALSE;   // don't continue
		}
	};
	///////////////////////////////////////////////////////////////////////////////
	// CAppModule - module class for an application

	class CAppModule
	{
	public:
		DWORD m_dwMainThreadID;
		HINSTANCE hInst;
		CSimpleMap<DWORD, CMessageLoop*>* m_pMsgLoopMap;
		CSimpleArray<HWND>* m_pSettingChangeNotify;

		// Overrides of CComModule::Init and Term
		HRESULT Init(HINSTANCE hInstance)
		{
			m_dwMainThreadID = ::GetCurrentThreadId();
			typedef CSimpleMap<DWORD, CMessageLoop*>   _mapClass;
			m_pMsgLoopMap = NULL;
			CTLTRY(m_pMsgLoopMap = new _mapClass);
			if (m_pMsgLoopMap == NULL)
				return E_OUTOFMEMORY;
			m_pSettingChangeNotify = NULL;
			hInst = hInstance;
			return S_OK;
		}
		HINSTANCE GetModuleInstance()
		{
			return hInst;
		}

		void Term()
		{
			TermSettingChangeNotify();
			delete m_pMsgLoopMap;
		}

		// Message loop map methods
		BOOL AddMessageLoop(CMessageLoop* pMsgLoop)
		{
			CStaticDataInitCriticalSectionLock lock;
			if (FAILED(lock.Lock()))
			{
				//ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::AddMessageLoop.\n"));
				CTLASSERT(FALSE);
				return FALSE;
			}

			CTLASSERT(pMsgLoop != NULL);
			CTLASSERT(m_pMsgLoopMap->Lookup(::GetCurrentThreadId()) == NULL);   // not in map yet

			BOOL bRet = m_pMsgLoopMap->Add(::GetCurrentThreadId(), pMsgLoop);

			lock.Unlock();

			return bRet;
		}

		BOOL RemoveMessageLoop()
		{
			CStaticDataInitCriticalSectionLock lock;
			if (FAILED(lock.Lock()))
			{
				//ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::RemoveMessageLoop.\n"));
				CTLASSERT(FALSE);
				return FALSE;
			}

			BOOL bRet = m_pMsgLoopMap->Remove(::GetCurrentThreadId());

			lock.Unlock();

			return bRet;
		}

		CMessageLoop* GetMessageLoop(DWORD dwThreadID = ::GetCurrentThreadId()) const
		{
			CStaticDataInitCriticalSectionLock lock;
			if (FAILED(lock.Lock()))
			{
				//CTLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::GetMessageLoop.\n"));
				CTLASSERT(FALSE);
				return NULL;
			}

			CMessageLoop* pLoop = m_pMsgLoopMap->Lookup(dwThreadID);

			lock.Unlock();

			return pLoop;
		}

		// Setting change notify methods
		// Note: Call this from the main thread for MSDI apps
		BOOL InitSettingChangeNotify(DLGPROC pfnDlgProc = _SettingChangeDlgProc)
		{
			CStaticDataInitCriticalSectionLock lock;
			if (FAILED(lock.Lock()))
			{
				//CTLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::InitSettingChangeNotify.\n"));
				CTLASSERT(FALSE);
				return FALSE;
			}

			if (m_pSettingChangeNotify == NULL)
			{
				typedef CSimpleArray<HWND>   _notifyClass;
				CTLTRY(m_pSettingChangeNotify = new _notifyClass);
				CTLASSERT(m_pSettingChangeNotify != NULL);
			}

			BOOL bRet = (m_pSettingChangeNotify != NULL);
			if (bRet && m_pSettingChangeNotify->GetSize() == 0)
			{
				// init everything
				_ATL_EMPTY_DLGTEMPLATE templ;
				HWND hNtfWnd = ::CreateDialogIndirect(GetModuleInstance(), &templ, NULL, pfnDlgProc);
				CTLASSERT(::IsWindow(hNtfWnd));
				if (::IsWindow(hNtfWnd))
				{
					// need conditional code because types don't match in winuser.h
#ifdef _WIN64
					::SetWindowLongPtr(hNtfWnd, GWLP_USERDATA, (LONG_PTR)this);
#else
					::SetWindowLongPtr(hNtfWnd, GWLP_USERDATA, PtrToLong(this));
#endif
					bRet = m_pSettingChangeNotify->Add(hNtfWnd);
				}
				else
				{
					bRet = FALSE;
				}
			}

			lock.Unlock();

			return bRet;
		}

		void TermSettingChangeNotify()
		{
			CStaticDataInitCriticalSectionLock lock;
			if (FAILED(lock.Lock()))
			{
				//ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::TermSettingChangeNotify.\n"));
				CTLASSERT(FALSE);
				return;
			}

			if (m_pSettingChangeNotify != NULL && m_pSettingChangeNotify->GetSize() > 0)
				::DestroyWindow((*m_pSettingChangeNotify)[0]);
			delete m_pSettingChangeNotify;
			m_pSettingChangeNotify = NULL;

			lock.Unlock();
		}

		BOOL AddSettingChangeNotify(HWND hWnd)
		{
			CStaticDataInitCriticalSectionLock lock;
			if (FAILED(lock.Lock()))
			{
				//ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::AddSettingChangeNotify.\n"));
				CTLASSERT(FALSE);
				return FALSE;
			}

			CTLASSERT(::IsWindow(hWnd));
			BOOL bRet = FALSE;
			if (InitSettingChangeNotify() != FALSE)
				bRet = m_pSettingChangeNotify->Add(hWnd);

			lock.Unlock();

			return bRet;
		}

		BOOL RemoveSettingChangeNotify(HWND hWnd)
		{
			CStaticDataInitCriticalSectionLock lock;
			if (FAILED(lock.Lock()))
			{
				//ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::RemoveSettingChangeNotify.\n"));
				CTLASSERT(FALSE);
				return FALSE;
			}

			BOOL bRet = FALSE;
			if (m_pSettingChangeNotify != NULL)
				bRet = m_pSettingChangeNotify->Remove(hWnd);

			lock.Unlock();

			return bRet;
		}

		// Implementation - setting change notify dialog template and dialog procedure
		struct _ATL_EMPTY_DLGTEMPLATE : DLGTEMPLATE
		{
			_ATL_EMPTY_DLGTEMPLATE()
			{
				memset(this, 0, sizeof(_ATL_EMPTY_DLGTEMPLATE));
				style = WS_POPUP;
			}
			WORD wMenu, wClass, wTitle;
		};

#ifdef _WIN64
		static INT_PTR CALLBACK _SettingChangeDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
#else
		static BOOL CALLBACK _SettingChangeDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
#endif
		{
			if (uMsg == WM_SETTINGCHANGE)
			{
				// need conditional code because types don't match in winuser.h
#ifdef _WIN64
				CAppModule* pModule = (CAppModule*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
#else
				CAppModule* pModule = (CAppModule*)LongToPtr(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
#endif
				CTLASSERT(pModule != NULL);
				CTLASSERT(pModule->m_pSettingChangeNotify != NULL);
				const UINT uTimeout = 1500;   // ms
				for (int i = 1; i < pModule->m_pSettingChangeNotify->GetSize(); i++)
				{
					::SendMessageTimeout((*pModule->m_pSettingChangeNotify)[i], uMsg, wParam, lParam, SMTO_ABORTIFHUNG, uTimeout, NULL);
				}
				return TRUE;
			}
			return FALSE;
		}
	};
	extern CAppModule _CTLBaseModule;
};


#endif
