#ifndef __CTLUTIL_H__
#define __CTLUTIL_H__

#include "ctldef.h"
#include <cstring>

#pragma push_macro("new")
#undef new
#pragma push_macro("malloc")
#undef malloc
#pragma push_macro("calloc")
#undef calloc
#pragma push_macro("realloc")
#undef realloc
#pragma push_macro("_recalloc")
#undef _recalloc
#pragma push_macro("free")
#undef free

namespace CTL
{
	/////////////////////////////////////////////////////////////////////////////
	// Collection helpers - CSimpleArray & CSimpleMap

	// template class helpers with functions for comparing elements
	// override if using complex types without operator==
	template <class T>
	class CSimpleArrayEqualHelper
	{
	public:
		static bool IsEqual(const T& t1, const T& t2)
		{
			return (t1 == t2);
		}
	};
	template <class T>
	class CSimpleArrayEqualHelperFalse
	{
	public:
		static bool IsEqual(const T&, const T&)
		{
			CTLASSERT(false);
			return false;
		}
	};
	template <class TKey, class TVal>
	class CSimpleMapEqualHelper
	{
	public:
		static bool IsEqualKey(const TKey& k1, const TKey& k2)
		{
			return CSimpleArrayEqualHelper<TKey>::IsEqual(k1, k2);
		}

		static bool IsEqualValue(const TVal& v1, const TVal& v2)
		{
			return CSimpleArrayEqualHelper<TVal>::IsEqual(v1, v2);
		}
	};

	template <class TKey, class TVal>
	class CSimpleMapEqualHelperFalse
	{
	public:
		static bool IsEqualKey(const TKey& k1, const TKey& k2)
		{
			return CSimpleArrayEqualHelper<TKey>::IsEqual(k1, k2);
		}

		static bool IsEqualValue(const TVal&, const TVal&)
		{
			CTLASSERT(FALSE);
			return false;
		}
	};
	template <class T, class TEqual = CSimpleArrayEqualHelper< T > >
	class CSimpleArray
	{
	public:
		// Construction/destruction
		CSimpleArray() : m_aT(NULL), m_nSize(0), m_nAllocSize(0)
		{ }

		~CSimpleArray(){RemoveAll();}

		CSimpleArray(const CSimpleArray< T, TEqual >& src) : m_aT(NULL), m_nSize(0), m_nAllocSize(0)
		{
			if (src.GetSize())
			{
				m_aT = (T*)calloc(src.GetSize(), sizeof(T));
				if (m_aT != NULL)
				{
					m_nAllocSize = src.GetSize();
					for (int i=0; i<src.GetSize(); i++)
						Add(src[i]);
				}
			}
		}
		CSimpleArray< T, TEqual >& operator=(const CSimpleArray< T, TEqual >& src)
		{
			if (GetSize() != src.GetSize())
			{
				RemoveAll();
				m_aT = (T*)calloc(src.GetSize(), sizeof(T));
				if (m_aT != NULL)
					m_nAllocSize = src.GetSize();
			}
			else
			{
				for (int i = GetSize(); i > 0; i--)
					RemoveAt(i - 1);
			}
			for (int i=0; i<src.GetSize(); i++)
				Add(src[i]);
			return *this;
		}

		// Operations
		int GetSize() const
		{
			return m_nSize;
		}
		BOOL Add(const T& t)
		{
			if(m_nSize == m_nAllocSize)
			{
				// Make sure newElement is not a reference to an element in the array.
				// Or else, it will be invalidated by the reallocation.
				CTLENSURE(	(&t < m_aT) ||
					(&t >= (m_aT + m_nAllocSize) ) );

				T* aT;
				int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);

				if (nNewAllocSize<0||nNewAllocSize>INT_MAX/sizeof(T))
				{
					return FALSE;
				}

				aT = (T*)_recalloc(m_aT, nNewAllocSize, sizeof(T));
				if(aT == NULL)
					return FALSE;
				m_nAllocSize = nNewAllocSize;
				m_aT = aT;
			}
			InternalSetAtIndex(m_nSize, t);
			m_nSize++;
			return TRUE;
		}
		BOOL Remove(const T& t)
		{
			int nIndex = Find(t);
			if(nIndex == -1)
				return FALSE;
			return RemoveAt(nIndex);
		}
		BOOL RemoveAt(int nIndex)
		{
			CTLASSERT(nIndex >= 0 && nIndex < m_nSize);
			if (nIndex < 0 || nIndex >= m_nSize)
				return FALSE;
			m_aT[nIndex].~T();
			if(nIndex != (m_nSize - 1))
				Checked::memmove_s((void*)(m_aT + nIndex), (m_nSize - nIndex) * sizeof(T), (void*)(m_aT + nIndex + 1), (m_nSize - (nIndex + 1)) * sizeof(T));
			m_nSize--;
			return TRUE;
		}
		void RemoveAll()
		{
			if(m_aT != NULL)
			{
				for(int i = 0; i < m_nSize; i++)
					m_aT[i].~T();
				free(m_aT);
				m_aT = NULL;
			}
			m_nSize = 0;
			m_nAllocSize = 0;
		}   
		const T& operator[] (int nIndex) const
		{
			CTLASSERT(nIndex >= 0 && nIndex < m_nSize);
			if(nIndex < 0 || nIndex >= m_nSize)
			{
				_CTLRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);					
			}
			return m_aT[nIndex];
		}
		T& operator[] (int nIndex)
		{
			CTLASSERT(nIndex >= 0 && nIndex < m_nSize);
			if(nIndex < 0 || nIndex >= m_nSize)
			{
				_CTLRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);					
			}
			return m_aT[nIndex];
		}
		T* GetData() const
		{
			return m_aT;
		}

		int Find(const T& t) const
		{
			for(int i = 0; i < m_nSize; i++)
			{
				if(TEqual::IsEqual(m_aT[i], t))
					return i;
			}
			return -1;  // not found
		}

		BOOL SetAtIndex(int nIndex, const T& t)
		{
			if (nIndex < 0 || nIndex >= m_nSize)
				return FALSE;
			InternalSetAtIndex(nIndex, t);
			return TRUE;
		}

		// Implementation
		class Wrapper
		{
		public:
			Wrapper(const T& _t) : t(_t)
			{
			}
			template <class _Ty>
			void * __cdecl operator new(size_t, _Ty* p)
			{
				return p;
			}
			template <class _Ty>
			void __cdecl operator delete(void* /* pv */, _Ty* /* p */)
			{
			}
			T t;
		};

		// Implementation
		void InternalSetAtIndex(int nIndex, const T& t)
		{
			new(m_aT + nIndex) Wrapper(t);
		}

		typedef T _ArrayElementType;
		T* m_aT;
		int m_nSize;
		int m_nAllocSize;

	};
	// intended for small number of simple types or pointers
	template <class TKey, class TVal, class TEqual = CSimpleMapEqualHelper< TKey, TVal > >
	class CSimpleMap
	{
	public:
		TKey* m_aKey;
		TVal* m_aVal;
		int m_nSize;

		typedef TKey _ArrayKeyType;
		typedef TVal _ArrayElementType;

		// Construction/destruction
		CSimpleMap() : m_aKey(NULL), m_aVal(NULL), m_nSize(0)
		{ }

		~CSimpleMap()
		{
			RemoveAll();	
		}

		// Operations
		int GetSize() const
		{
			return m_nSize;
		}
		BOOL Add(const TKey& key, const TVal& val)
		{
			TKey* pKey;
			pKey = (TKey*)_recalloc(m_aKey, (m_nSize + 1), sizeof(TKey));
			if(pKey == NULL)
				return FALSE;
			m_aKey = pKey;
			TVal* pVal;
			pVal = (TVal*)_recalloc(m_aVal, (m_nSize + 1), sizeof(TVal));
			if(pVal == NULL)
				return FALSE;
			m_aVal = pVal;
			InternalSetAtIndex(m_nSize, key, val);
			m_nSize++;
			return TRUE;
		}
		BOOL Remove(const TKey& key)
		{
			int nIndex = FindKey(key);
			if(nIndex == -1)
				return FALSE;
			return RemoveAt(nIndex);
		}
		BOOL RemoveAt(int nIndex)
		{
			CTLASSERT(nIndex >= 0 && nIndex < m_nSize);
			if (nIndex < 0 || nIndex >= m_nSize)
				return FALSE;
			m_aKey[nIndex].~TKey();
			m_aVal[nIndex].~TVal();
			if(nIndex != (m_nSize - 1))
			{
				memmove_s((void*)(m_aKey + nIndex), (m_nSize - nIndex) * sizeof(TKey), (void*)(m_aKey + nIndex + 1), (m_nSize - (nIndex + 1)) * sizeof(TKey));
				memmove_s((void*)(m_aVal + nIndex), (m_nSize - nIndex) * sizeof(TVal), (void*)(m_aVal + nIndex + 1), (m_nSize - (nIndex + 1)) * sizeof(TVal));
			}
			TKey* pKey;
			pKey = (TKey*)_recalloc(m_aKey, (m_nSize - 1), sizeof(TKey));
			if(pKey != NULL || m_nSize == 1)
				m_aKey = pKey;
			TVal* pVal;
			pVal = (TVal*)_recalloc(m_aVal, (m_nSize - 1), sizeof(TVal));
			if(pVal != NULL || m_nSize == 1)
				m_aVal = pVal;
			m_nSize--;
			return TRUE;
		}
		void RemoveAll()
		{
			if(m_aKey != NULL)
			{
				for(int i = 0; i < m_nSize; i++)
				{
					m_aKey[i].~TKey();
					m_aVal[i].~TVal();
				}
				free(m_aKey);
				m_aKey = NULL;
			}
			if(m_aVal != NULL)
			{
				free(m_aVal);
				m_aVal = NULL;
			}

			m_nSize = 0;
		}
		BOOL SetAt(const TKey& key, const TVal& val)
		{
			int nIndex = FindKey(key);
			if(nIndex == -1)
				return FALSE;
			CTLASSERT(nIndex >= 0 && nIndex < m_nSize);
			m_aKey[nIndex].~TKey();
			m_aVal[nIndex].~TVal();
			InternalSetAtIndex(nIndex, key, val);
			return TRUE;
		}
		TVal Lookup(const TKey& key) const
		{
			int nIndex = FindKey(key);
			if(nIndex == -1)
				return NULL;    // must be able to convert
			return GetValueAt(nIndex);
		}
		TKey ReverseLookup(const TVal& val) const
		{
			int nIndex = FindVal(val);
			if(nIndex == -1)
				return NULL;    // must be able to convert
			return GetKeyAt(nIndex);
		}
		TKey& GetKeyAt(int nIndex) const
		{
			CTLASSERT(nIndex >= 0 && nIndex < m_nSize);
			if(nIndex < 0 || nIndex >= m_nSize)
				CTLRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);

			return m_aKey[nIndex];
		}
		TVal& GetValueAt(int nIndex) const
		{
			CTLASSERT(nIndex >= 0 && nIndex < m_nSize);
			if(nIndex < 0 || nIndex >= m_nSize)
				CTLRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);	

			return m_aVal[nIndex];
		}

		int FindKey(const TKey& key) const
		{
			for(int i = 0; i < m_nSize; i++)
			{
				if(TEqual::IsEqualKey(m_aKey[i], key))
					return i;
			}
			return -1;  // not found
		}
		int FindVal(const TVal& val) const
		{
			for(int i = 0; i < m_nSize; i++)
			{
				if(TEqual::IsEqualValue(m_aVal[i], val))
					return i;
			}
			return -1;  // not found
		}

		BOOL SetAtIndex(int nIndex, const TKey& key, const TVal& val)
		{
			if (nIndex < 0 || nIndex >= m_nSize)
				return FALSE;
			InternalSetAtIndex(nIndex, key, val);
			return TRUE;
		}


		// Implementation

		template <typename T>
		class Wrapper
		{
		public:
			Wrapper(const T& _t) : t(_t)
			{
			}
			template <class _Ty>
			void *operator new(size_t, _Ty* p)
			{
				return p;
			}
			template <class _Ty>
			void operator delete(void* /* pv */, _Ty* /* p */)
			{
			}
			T t;
		};
		void InternalSetAtIndex(int nIndex, const TKey& key, const TVal& val)
		{
			new(m_aKey + nIndex) Wrapper<TKey>(key);
			new(m_aVal + nIndex) Wrapper<TVal>(val);
		}
	};

	// Sync Classes
	class CCTLCriticalSection
	{
	public:
		CCTLCriticalSection() throw()
		{
			memset(&m_sec, 0, sizeof(CRITICAL_SECTION));
		}

		~CCTLCriticalSection()
		{
		}

		HRESULT Lock() throw()
		{
			EnterCriticalSection(&m_sec);
			return S_OK;
		}
		HRESULT Unlock() throw()
		{
			LeaveCriticalSection(&m_sec);
			return S_OK;
		}
		HRESULT Init() throw()
		{
			HRESULT hRes = S_OK;
			if (!InitializeCriticalSectionAndSpinCount(&m_sec, 0))
			{
				hRes = HRESULT_FROM_WIN32(GetLastError());
			}

			return hRes;
		}

		HRESULT Term() throw()
		{
			DeleteCriticalSection(&m_sec);
			return S_OK;
		}
		CRITICAL_SECTION m_sec;
	};
	template< class TLock >
	class CCTLCritSecLock
	{
	public:
			CCTLCritSecLock(
				TLock& cs,
				bool bInitialLock = true);

			~CCTLCritSecLock() throw();

			HRESULT Lock() throw();

			void Unlock() throw();

		// Implementation
	private:
		TLock& m_cs;
		bool m_bLocked;

		// Private to avoid accidental use
		CCTLCritSecLock(_In_ const CCTLCritSecLock&) throw();
		CCTLCritSecLock& operator=(_In_ const CCTLCritSecLock&) throw();
	};
	template< class TLock >
	#pragma warning(suppress: 26166 28196) // Constructor throws on failure of Lock() method
	inline CCTLCritSecLock< TLock >::CCTLCritSecLock(
			TLock& cs,
			bool bInitialLock) :m_cs(cs),m_bLocked(false)
	{
		if (bInitialLock)
		{
			HRESULT hr;

			hr = Lock();
			if (FAILED(hr))
			{
				throw CCTLException(hr);
			}
		}
	}

	template< class TLock >
	inline CCTLCritSecLock< TLock >::~CCTLCritSecLock() throw()
	{
		if (m_bLocked)
		{
			Unlock();
		}
	}

	template< class TLock >
	#pragma warning(suppress: 26165) // Lock is acquired by template lock object '(this->m_cs).m_sec'
	inline HRESULT CCTLCritSecLock< TLock >::Lock() throw()
	{
		HRESULT hr;

		CTLASSERT(!m_bLocked);
		hr = m_cs.Lock();
		if (FAILED(hr))
		{
			return(hr);
		}
		m_bLocked = true;

		return(S_OK);
	}

	template< class TLock >
	#pragma warning(suppress: 26167) // Lock is released by template lock object '(this->m_cs).m_sec'
	inline void CCTLCritSecLock< TLock >::Unlock() throw()
	{
		#pragma warning(suppress: 26110) // Template parameter hides lock object
		m_cs.Unlock();
		m_bLocked = false;
	}
	///////////////////////////////////////////////////////////////////////////////
	// CStaticDataInitCriticalSectionLock and CWindowCreateCriticalSectionLock
	// internal classes to manage critical sections for both ATL3 and ATL7
	CCTLCriticalSection m_csStaticDataInitAndTypeInfo;
	class CStaticDataInitCriticalSectionLock
	{
	public:
		CCTLCritSecLock<CCTLCriticalSection> m_cslock;

		CStaticDataInitCriticalSectionLock() : m_cslock(m_csStaticDataInitAndTypeInfo, false)
		{ }

		HRESULT Lock()
		{
			return m_cslock.Lock();
		}

		void Unlock()
		{
			m_cslock.Unlock();
		}
	};
};
#pragma pop_macro("new")
#pragma pop_macro("free")
#pragma pop_macro("realloc")
#pragma pop_macro("_recalloc")
#pragma pop_macro("malloc")
#pragma pop_macro("calloc")

#endif