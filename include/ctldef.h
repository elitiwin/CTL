#ifndef __CTLDEF_H__
#define __CTLDEF_H__

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE         // UNICODE is used by Windows headers
#endif
#endif

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE        // _UNICODE is used by C-runtime/MFC headers
#endif
#endif

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#include <windows.h>
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>

#define CTL_NO_VTABLE __declspec(novtable)

#ifndef CTLASSERT
#define CTLASSERT(expr) assert(expr)
#endif // CTLASSERT

#ifndef CTLENSURE_THROW
#define CTLENSURE_THROW(expr)			   \
do {                                       \
	int __ctl_condVal=!!(expr);            \
	CTLASSERT(__ctl_condVal);              \
} while (0)
#endif // CTLENSURE_THROW

#ifndef CTLENSURE
#define CTLENSURE(expr) CTLENSURE_THROW(expr)
#endif // CTLENSURE

#define CTLTRYALLOC(x) __pragma(warning(push)) __pragma(warning(disable: 4571)) try{x;} catch(...) {} __pragma(warning(pop))
#ifndef CTLTRY
#define CTLTRY(x) CTLTRYALLOC(x)
#endif	//ATLTRY

namespace CTL
{
	#if defined UNICODE || defined _UNICODE
		#define _T(x)      L ## x
	#else
		#define _T(x)      x
	#endif
	// Exception raise (for functions that cannot return an error code)
	inline void __declspec(noreturn) _CTLRaiseException( DWORD dwExceptionCode, DWORD dwExceptionFlags = EXCEPTION_NONCONTINUABLE )
	{
		::RaiseException( dwExceptionCode, dwExceptionFlags, 0, NULL );
	}
	class CCTLException
	{
	public:
		CCTLException() throw() :
			m_hr(E_FAIL)
		{
		}

		CCTLException(_In_ HRESULT hr) throw() :
			m_hr(hr)
		{
		}

		operator HRESULT() const throw()
		{
			return(m_hr);
		}

	public:
		HRESULT m_hr;
	};

}; // namespace CTL


#endif