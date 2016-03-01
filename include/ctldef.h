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

#ifndef CTLASSERT
#define CTLASSERT(expr) assert(expr)
#endif // CTLASSERT

#ifndef CTLENSURE_THROW
#define CTLENSURE_THROW(expr)			   \
do {                                       \
	int __ctl_condVal=!!(expr);            \
	CTLASSERT(__ctl_condVal);              \
} while (0)
#endif // ATLENSURE_THROW

#ifndef CTLENSURE
#define CTLENSURE(expr) CTLENSURE_THROW(expr)
#endif // ATLENSURE

namespace CTL
{
	// Exception raise (for functions that cannot return an error code)
	inline void __declspec(noreturn) _CTLRaiseException( DWORD dwExceptionCode, DWORD dwExceptionFlags = EXCEPTION_NONCONTINUABLE )
	{
		::RaiseException( dwExceptionCode, dwExceptionFlags, 0, NULL );
	}

}; // namespace CTL


#endif