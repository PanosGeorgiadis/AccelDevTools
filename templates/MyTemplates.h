// MyTemplates.h : Declaration of the templates
// CpgSimpleArray : A simple array created by PG (Panagiotis Georgiadis)

#pragma once

class CObjectCollectionEnumVariant ;
template <class itemclass> class CObjectCollectionEnumVariantImpl ;

#pragma push_macro("new")
#undef new

// *****************************************************************************
// A simple container of elements of type T
// *****************************************************************************
template <class T>
class CpgSimpleArray
{
public:
     T* m_aT;
     int m_nSize;
     int m_nAllocSize;

     // Construction/destruction
     CpgSimpleArray() : m_aT(NULL), m_nSize(0), m_nAllocSize(0)
     { }

     virtual ~CpgSimpleArray()
     {
          RemoveAll();
          m_aT = NULL;
     }

// Operations
     int GetSize() const
     {
          return m_nSize;
     }
     BOOL Add(T& t)
     {
          if(m_nSize == m_nAllocSize)
          {
               T* aT;
               int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);
               aT = (T*)realloc(m_aT, nNewAllocSize * sizeof(T));
               if(aT == NULL)
                    return FALSE;
               memset(aT+m_nAllocSize, 0, (nNewAllocSize-m_nAllocSize)* sizeof(T)) ; 
               m_nAllocSize = nNewAllocSize;
               m_aT = aT;
          }
          m_nSize++;
          SetAtIndex(m_nSize - 1, t);
          return TRUE;
     }
     BOOL InsertAt(int nIndex, T& t)
     {
          if(m_nSize == m_nAllocSize)
          {
               T* aT;
               int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);
               aT = (T*)realloc(m_aT, nNewAllocSize * sizeof(T));
               if(aT == NULL)
                    return FALSE;
               memset(aT+m_nAllocSize, 0, (nNewAllocSize-m_nAllocSize)* sizeof(T)) ; 
               m_nAllocSize = nNewAllocSize;
               m_aT = aT;
          }
          memmove((void*)&m_aT[nIndex + 1], (void*)&m_aT[nIndex], (m_nSize - nIndex) * sizeof(T));
          m_nSize++;
          SetAtIndex(nIndex, t);
          return TRUE;
     }
     BOOL MoveAt(int from, int to)
     {
          _ASSERT(from >= 0 && from < m_nSize);
          _ASSERT(to >= 0 && to < m_nSize);
          if (!((from >= 0 && from < m_nSize) && 
               (to >= 0 && to < m_nSize)))
               return FALSE;
          if(from == to) return TRUE;
          //T tmp = m_aT[from];    //Remember pointer
          T tmp;
          memmove ((void*)&tmp, (void*)&m_aT[from], sizeof(tmp));
           // fill up the gap of the 'from' item removed out of list
          if(from != (m_nSize - 1)) 
               memmove((void*)&m_aT[from], (void*)&m_aT[from + 1], (m_nSize - (from + 1)) * sizeof(T*));
          //Make room for a item at the 'to' position
          if(to != (m_nSize - 1)) 
               memmove((void*)&m_aT[to + 1], (void*)&m_aT[to], (m_nSize - to - 1) * sizeof(T*));
          
          memmove ((void*)&m_aT[to], (void*)&tmp, sizeof(tmp));
          //m_aT[to] = tmp;
          return TRUE;
     }
     BOOL Remove(T& t)
     {
          int nIndex = Find(t);
          if(nIndex == -1)
               return FALSE;
          return RemoveAt(nIndex);
     }
     BOOL RemoveAt(int nIndex)
     {
          if(nIndex != (m_nSize - 1))
          {
               memmove((void*)&m_aT[nIndex], (void*)&m_aT[nIndex + 1], (m_nSize - (nIndex + 1)) * sizeof(T));
          }
          m_nSize--;
          return TRUE;
     }
     void RemoveAll()
     {
          if(m_aT != NULL)
          {
               free(m_aT);
               m_aT = NULL;
          }
          m_nSize = 0;
          m_nAllocSize = 0;
     }
     T& operator[] (int nIndex) const
     {
          return GetAt(nIndex);
     }
     T& GetAt( int nIndex) const
     {
          ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
          return m_aT[nIndex];
     }
     void SetAt( int nIndex, T Value)
     {
          ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
          m_aT[nIndex] = Value ;
     }
     T* GetData() const
     {
          return m_aT;
     }

// Implementation
     class Wrapper
     {
     public:
          Wrapper(T& _t) : t(_t)
          {
          }
          template <class _Ty>
          void *operator new(size_t, _Ty* p)
          {
               return p;
          }
          template <class _Ty>
          void __cdecl operator delete(void* /* pv */, _Ty* /* p */)
          {
          }
          T t;
     };
     void SetAtIndex(int nIndex, T& t)
     {
          ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
          new(&m_aT[nIndex]) Wrapper(t);
     }
     int Find(T& t) const
     {
          for(int i = 0; i < m_nSize; i++)
          {
               if(m_aT[i] == t)
                    return i;
          }
          return -1;  // not found
     }
};

template <class T>
class CRefCountedArray
{
public:
     T* m_aT;
     int m_nSize;
     int m_nAllocSize;

// Construction/destruction
     CRefCountedArray() : m_aT(NULL), m_nSize(0), m_nAllocSize(0)
     { }

     virtual ~CRefCountedArray()
     {
          RemoveAll();
     }

// Operations
     int GetSize() const
     {
          return m_nSize;
     }
     BOOL Add(T& t)
     {
          if(m_nSize == m_nAllocSize)
          {
               T* aT;
               int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);
               aT = (T*)realloc(m_aT, nNewAllocSize * sizeof(T));
               if(aT == NULL)
                    return FALSE;
               memset(aT+m_nAllocSize, 0, (nNewAllocSize-m_nAllocSize)* sizeof(T)) ; 
               m_nAllocSize = nNewAllocSize;
               m_aT = aT;
          }
          m_nSize++;
          SetAtIndex(m_nSize - 1, t);
          return TRUE;
     }
     BOOL InsertAt(int nIndex, T& t)
     {
          if(m_nSize == m_nAllocSize)
          {
               T* aT;
               int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);
               aT = (T*)realloc(m_aT, nNewAllocSize * sizeof(T));
               if(aT == NULL)
                    return FALSE;
               memset(aT+m_nAllocSize, 0, (nNewAllocSize-m_nAllocSize)* sizeof(T)) ; 
               m_nAllocSize = nNewAllocSize;
               m_aT = aT;
          }
          memmove((void*)&m_aT[nIndex + 1], (void*)&m_aT[nIndex], (m_nSize - nIndex) * sizeof(T));
          m_nSize++;
          SetAtIndex(nIndex, t);
          return TRUE;
     }

     BOOL Remove(T& t)
     {
          int nIndex = Find(t);
          if(nIndex == -1)
               return FALSE;
          return RemoveAt(nIndex);
     }
     BOOL RemoveAt(int nIndex)
     {
          if(nIndex != (m_nSize - 1))
          {
               if (m_aT[nIndex] != NULL)
                    m_aT[nIndex]->Release() ;
               memmove((void*)&m_aT[nIndex], (void*)&m_aT[nIndex + 1], (m_nSize - (nIndex + 1)) * sizeof(T));
          }
          m_nSize--;
          return TRUE;
     }
     void RemoveAll()
     {
          for (int nInd = 0 ; nInd < m_nSize ; nInd++)
          {
               if (m_aT[nInd] != NULL)
                    m_aT[nInd]->Release() ;
          }
          if(m_aT != NULL)
          {
               free(m_aT);
               m_aT = NULL;
          }
          m_nSize = 0;
          m_nAllocSize = 0;
     }
     T& operator[] (int nIndex) const
     {
          return GetAt(nIndex);
     }
     T& GetAt( int nIndex) const
     {
          ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
          return m_aT[nIndex];
     }

     T* GetData() const
     {
          return m_aT;
     }

// Implementation
     class Wrapper
     {
     public:
          Wrapper(T& _t) : t(_t)
          {
          }
          template <class _Ty>
          void *operator new(size_t, _Ty* p)
          {
               return p;
          }
          template <class _Ty>
          void __cdecl operator delete(void* /* pv */, _Ty* /* p */)
          {
          }
          T t;
     };
     void SetAtIndex(int nIndex, T& t)
     {
          ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
          new(&m_aT[nIndex]) Wrapper(t);

          if (m_aT[nIndex] != NULL)
               m_aT[nIndex]->AddRef() ;

     }
     int Find(T& t) const
     {
          for(int i = 0; i < m_nSize; i++)
          {
               if(m_aT[i] == t)
                    return i;
          }
          return -1;  // not found
     }
};

#pragma pop_macro("new")

class CRefCountedObject
{
public:
     CRefCountedObject()
     {
          m_lRefCount = 0 ;
     }
protected:
     virtual ~CRefCountedObject()
     {
     }
     virtual void FinalRelease()
     {
     }
public:
     void REFCOUNT_SHOULD_BE_ONE()
     {
#ifdef _DEBUG
          if (1 != m_lRefCount)
               DebugBreak() ;
#endif
     }
     //
     // Reference counting functions
     //
     LONG Release()
     {
          LONG lResult = InterlockedDecrement(&m_lRefCount) ;
          ATLASSERT(lResult >= 0) ;
          if (lResult == 0)
          {
               FinalRelease() ;
               delete this ;
          }
          return lResult ;
     }
     LONG AddRef()
     {
          return InterlockedIncrement(&m_lRefCount) ;
     }
protected:
     LONG m_lRefCount;
};

template <class Item>
class CAutoRelease
{
private:
     Item* m_pObject ;
public:
     CAutoRelease(Item* pObject)
     {
          m_pObject = pObject ;
     }
     virtual ~CAutoRelease()
     {
          if (m_pObject != NULL)
          {
               m_pObject->Release() ;
               m_pObject = NULL;
          }
     }
};

class CAutoReleaseRefCounted : public CAutoRelease<CRefCountedObject>
{
public:
     CAutoReleaseRefCounted(CRefCountedObject* pObject) : CAutoRelease<CRefCountedObject>(pObject)
     {
     }

     virtual ~CAutoReleaseRefCounted()
     {
     }
};

// *****************************************************************************
// reference-counting object that holds a reference to its parent instance
// *****************************************************************************
template<class TParent> class ParentChildDef 
{
private:
     void Init(TParent* pParent)
     {
          _pAtlModule->Lock();
          m_pParent = pParent ;
     }
public:
     ParentChildDef()
     {
          Init(NULL) ;
     }
     ParentChildDef(TParent* pParent) : m_pParent(pParent)
     {
          Init(pParent) ;
     }
     virtual ~ParentChildDef()
     {
          _pAtlModule->Unlock();
          m_pParent = NULL ;
     }
     TParent* GetParent()
     {
          return m_pParent ;
     }
protected:
     LONG ParentAddRef()
     {
          //
          // If we have a parent call the AddRef method
          //
          //NICASSERT(m_pParent != NULL);
          if (m_pParent)
               return m_pParent->AddRef() ;

          return 1 ;
     }
     void ParentRelease(TParent* pParent)
     {
          if (pParent)
               pParent->Release() ;
     }
     LONG ParentRelease()
     {
          //
          // If we have a parent call the Release method
          //
          //NICASSERT(m_pParent != NULL);
          ParentRelease(m_pParent) ;
          return 1 ;
     }
     TParent* m_pParent ;
public:
     virtual void DisconnectParent(){} ;
     virtual void ConnectParent(){} ;
};

// *****************************************************************************
// TODO: documentation
// *****************************************************************************
template<class T, class TParent> class ParentChildImpl : public T
{
public:
     ParentChildImpl(TParent* pParent) : T(pParent)
     {
          //NICASSERT(pParent) ;
          //
          // Increment the count because our parent is holding. Don't
          // increment the parent count so use the base AddRef
          //
          InternalAddRef() ;
     }
     virtual ~ParentChildImpl()
     {
          m_dwRef = 1L;
          //FinalRelease();
     }
     virtual void DisconnectParent()
     {
          //
          // Now call the base class so that it can do some
          // stuff there when the parent disconnects
          // 
          T::DisconnectParent() ;
          //
          // Parent wants to get rid of the object so it should
          // decrement the count without decrementing the
          // parent object. We must destroy the object when
          // the refcount goes to 0.
          //
          MyInternalRelease() ;
     }
     virtual void ConnectParent()
     {
          InternalAddRef() ;
          T::ConnectParent() ;
     }
     static T* Create (TParent* pParent)
     {
          T* pObj = new ParentChildImpl<T,TParent>(pParent) ;
          if (!pObj)
               return NULL;

          //
          // Initialize the atlCOMObject
          //

          HRESULT hr = S_OK ;
          __if_exists(T::_AtlInitialConstruct)
          {
               hr = pObj->_AtlInitialConstruct();
          }

          hr = pObj->FinalConstruct ();
          if (FAILED(hr))
          {
               delete pObj;
               return NULL;
          }
          return pObj;
     }
private:
     STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject)
     {
          __if_exists(_InternalQueryInterface)
          {
               return _InternalQueryInterface(iid, ppvObject);
          }
          __if_not_exists(_InternalQueryInterface)
          {
               iid ;
               *ppvObject = 0 ;
               return E_NOINTERFACE ;
          }
     }
     STDMETHOD_(ULONG, AddRef)()
     {
          //
          // Each time someone increments our refcount we must
          // also increment the parents count
          //
          ParentAddRef();
          //
          // Now call the base class
          //
          return InternalAddRef() ;
     }
     STDMETHOD_(ULONG, Release)()
     {
          //
          // Each time someone decrements our refcount we must
          // also decrement the parents count
          //
          TParent* pParent = dynamic_cast<TParent*>(m_pParent);

          LONG lResult = MyInternalRelease();
          ParentRelease (pParent);

          return lResult ;
     }
private:
     //
     // We have our own InternalRelease so we can destroy the object
     // when it is no longer needed
     //
     LONG MyInternalRelease()
     {
          ULONG l = InternalRelease();
          if (l == 0)
          {
               FinalRelease() ;
               delete this;
          }
          return l;
     }
};

// *****************************************************************************
// generic reference-counting object
// *****************************************************************************
class ParentChildBase
{
public:
     ParentChildBase()
     {
          m_dwRef = 0 ;
     }
     virtual ~ParentChildBase()
     {
     }
     STDMETHOD_(ULONG, AddRef)() = 0 ;
     STDMETHOD_(ULONG, Release)() = 0 ;
public:
     long m_dwRef ;

     LONG InternalAddRef()
     {
          return InterlockedIncrement(&m_dwRef) ;
     }
     LONG InternalRelease()
     {
          return InterlockedDecrement(&m_dwRef) ;
     }
     HRESULT _InternalQueryInterface(REFIID /*iid*/, void ** ppvObject)
     {
          *ppvObject = NULL ;
          return E_NOINTERFACE ;
     }
};

// *****************************************************************************
// TODO: documentation
// *****************************************************************************
class ObjectCollectionBase
{
public:
     virtual ~ObjectCollectionBase()
     {
     }
     //
     // GetItem returns the interface to the specified object
     // The GetItem method is 1-based
     //
     virtual HRESULT GetItemByIndex(LONG lIndex, IUnknown** itfUnk) = 0 ;
     virtual LONG GetCount() = 0 ;
protected:
     CComAutoCriticalSection m_csCollectionLock ;
};

template<class TItem, class TParent> class ObjectCollectionDef :  public ParentChildDef<TParent>, 
                                                                  public ObjectCollectionBase
{
public:
     ObjectCollectionDef(TParent* pParent) : ParentChildDef<TParent>(pParent)
     {
     }
protected:
     CpgSimpleArray<TItem*> m_Items ;

public:
     LONG GetCount() 
     { 
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          return m_Items.GetSize();  
     }
     //
     // GetItem returns the interface to the specified object
     // The GetItem method is 1-based
     //
     HRESULT GetItemByIndex(LONG lIndex, IUnknown** itfUnk)
     { 
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          if( (lIndex <= 0) || (lIndex > GetCount()) )
          {
               *itfUnk = NULL;
               return S_OK ;
          }

          return m_Items[lIndex-1]->QueryInterface(IID_IUnknown, (void**)itfUnk) ; 
     }
     //
     // GetItem returns the pointer to the specified object
     // The GetItem method is 1-based
     //
     TItem* GetItemByIndex(LONG Index)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          if( (Index <= 0) || (Index > GetCount()) )
               return NULL;

          return m_Items[Index-1] ; 
     }

     //
     // GetItemByName returns the pointer to the specified object     
     //
     TItem* GetItemByName(BSTR Name, bool bSortedList = true)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          CComVariant vIndex = Name ;
          bool bMatch = false;
          
          LONG lIndex = GetIndex(&vIndex, &bMatch, bSortedList) ;
     
          if(bMatch)
               //Get index returns a zero based index, GetItemByIndex requires a one based index
               return GetItemByIndex(lIndex+1) ;
          else
               return NULL;
     }

     //
     // Use this function if the collection could contain the item being renamed!!!
     //
     void NameChanging(TItem* pSrcItem, const BSTR Name, const int nMaxLength)
     {
          __if_exists(TItem::put_Name)
          {
               CComBSTR bstrUniqueName ;
               CComBSTR bstrNewName ;
               TCHAR* szBuf ;

               szBuf = new TCHAR[nMaxLength+10] ;
                  // TODO: check why there are less characters copied than nMaxLength
                  //for now just copy an extra character
               StrNCpy(szBuf, Name, nMaxLength+1) ;
               StrTrim(szBuf, TEXT(" "));

               bstrNewName = szBuf ;
               StrNCpy(szBuf, Name, nMaxLength-3) ;
               StrTrim(szBuf, TEXT(" "));

               int nUniqueIndexValue = 1;
               for (long lIndex = 1 ; lIndex <= GetCount() ; lIndex++)
               {
                    TItem* pItem = GetItemByIndex(lIndex) ;
                    if (pItem == pSrcItem)
                         continue ;
                    ItemNameChanging(pItem, bstrNewName, szBuf, nUniqueIndexValue);
               }
               delete [] szBuf ;
          }
     }

     //
     // use this function only if you are certain the collection does not contain the item being renamed!!!
     //
     void NameChanging(const BSTR Name, const int nMaxLength)
     {
          __if_exists(TItem::put_Name)
          {
               CComBSTR bstrUniqueName ;
               CComBSTR bstrNewName ;
               TCHAR* szBuf ;

               szBuf = new TCHAR[nMaxLength+10] ;
                  // TODO: check why there are less characters copied than nMaxLength
                  //for now just copy an extra character
               StrNCpy(szBuf, Name, nMaxLength+1) ;
               StrTrim(szBuf, TEXT(" "));

               bstrNewName = szBuf ;
               StrNCpy(szBuf, Name, nMaxLength-3) ;
               StrTrim(szBuf, TEXT(" "));

               int nUniqueIndexValue = 1;
               for (long lIndex = 1 ; lIndex <= GetCount() ; lIndex++)
               {
                    TItem* pItem = GetItemByIndex(lIndex) ;
                    ItemNameChanging(pItem, bstrNewName, szBuf, nUniqueIndexValue);
               }
               delete [] szBuf ;
          }
     }

private:
     void ItemNameChanging(TItem* pItem, const BSTR NewName, const BSTR BaseName, int &nUniqueIndexValue)
     {
          if (NULL == pItem)
               return ;
                    
          CComBSTR bstrName ;          
          CComBSTR bstrUniqueName;

          pItem->get_Name(&bstrName) ;

          int nCompare = CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                              NewName, ::SysStringLen(NewName), 
                              bstrName, bstrName.Length()) ;

          if (CSTR_EQUAL == nCompare)
          {
               //
               // Find unique name
               //
               for ( ; nUniqueIndexValue <= 99 ; nUniqueIndexValue++)
               {
                    TCHAR szNum[20] ;

                    StringCchPrintf(szNum, 20, TEXT("$%d"), nUniqueIndexValue) ;

                    bstrUniqueName = BaseName ;
                    bstrUniqueName += szNum ;

                    if (NULL == GetItemByName(bstrUniqueName, false))
                         break ;
               }
               //
               // Have an item with the same name
               // so rename the item
               //

               pItem->put_Name(bstrUniqueName) ;
          }
     }


public:
     virtual ~ObjectCollectionDef()
     {
          //NICASSERT(m_Items.GetSize() == 0) ;
          CleanUp() ;
     }
     void CleanUp()
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;

          int nIndex ;
          for (nIndex = 0 ; nIndex < m_Items.GetSize() ; nIndex++)
          {
               TItem* pItem = m_Items[nIndex] ;
               m_Items[nIndex] = NULL ;
               if (pItem)
                    pItem->DisconnectParent() ;
          }
          m_Items.RemoveAll() ;
     }
     // Performs a binary search to find the item with the specified name or index
     int GetIndex(VARIANT* NameOrIndex, bool* bExactMatch, bool bSortedList = true)
     {
          //
          // Lock this function to prevent the collection from being changed while we are looking for the index.
          //
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;

          int nResult = -10 ;
          *bExactMatch = false ;
          switch(NameOrIndex->vt)
          {
               case VT_I1 :
               case VT_I2 :
               case VT_I4 :
               case VT_I8 :
               case VT_UI1 :
               case VT_UI2 :
               case VT_UI4 :
               case VT_UI8 :
                    {

                         bSortedList ;
                         //
                         // These values should be converted to a int
                         //
                         if ((NameOrIndex->vt == VT_I8) || (NameOrIndex->vt == VT_UI8))
                              nResult = (int)NameOrIndex->llVal-1 ;
                         else
                         {
                              CComVariant vtTmp = *NameOrIndex ;
                              vtTmp.ChangeType(VT_I4) ;
                              nResult = vtTmp.iVal-1 ;
                         }
                         *bExactMatch = true ;
                    }
                    break ;
               case VT_BSTR :
                    __if_exists(TItem::get_Name)
                    {
                         {
                              if (0 == m_Items.GetSize())
                                   return 0 ;
                              //
                              // Do a binary search
                              //
                              int nLeft = 0 ;
                              int nRight = m_Items.GetSize()-1 ;
                              int nMid = (nLeft+nRight)/2 ;
                              CComBSTR bstrSearch = NameOrIndex->bstrVal ;
                              int nNameLength = bstrSearch.Length() ;
                              TItem* pEntry ;
                              int nCompare ;

                              //
                              // If we have no items we should return 0
                              //
                              nResult = 0 ;
                              //
                              // Binary search is of no use when having just
                              // a few elements
                              //
                              CComBSTR bstrTmp ;

                              if(bSortedList)
                              {
                                   while (nRight-nLeft > 5)
                                   {
                                        pEntry = m_Items[nMid] ;
                                        bstrTmp.Empty() ;
                                        pEntry->get_Name(&bstrTmp) ;

                                        nCompare = CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                                                                 bstrSearch, nNameLength, 
                                                                 bstrTmp, bstrTmp.Length()) ;
                                        if (nCompare == CSTR_GREATER_THAN)
                                             nLeft = nMid ;
                                        else nRight = nMid ;
                                        nMid = (nLeft+nRight)/2 ;
                                   }
                                   //
                                   // Now do a linear search on the last few items
                                   //
                                   for (nResult = nLeft ; nResult <= nRight ; nResult++)
                                   {
                                        pEntry = m_Items[nResult] ;
                                        bstrTmp.Empty() ;
                                        pEntry->get_Name(&bstrTmp) ;

                                        nCompare = CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                                                                 bstrSearch, nNameLength, 
                                                                 bstrTmp, bstrTmp.Length()) ;
                                        if (nCompare == CSTR_LESS_THAN)
                                        {
                                             //
                                             // Found insertion point
                                             //
                                             *bExactMatch = false ;
                                             break ;
                                        }
                                        if (nCompare == CSTR_EQUAL)
                                        {
                                             //
                                             // Found exact match
                                             //
                                             *bExactMatch = true ;
                                             break ;
                                        }
                                   }
                              }
                              else
                              {
                                   //not a sorted list, we should do a lineair search until we
                                   //find the item, or the end of the list
                                   for (nResult = nLeft ; nResult <= nRight ; nResult++)
                                   {
                                        pEntry = m_Items[nResult] ;
                                        bstrTmp.Empty() ;
                                        pEntry->get_Name(&bstrTmp) ;

                                        nCompare = CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                                                                 bstrSearch, nNameLength, 
                                                                 bstrTmp, bstrTmp.Length()) ;
                                        if (nCompare == CSTR_EQUAL)
                                        {
                                             //
                                             // Found exact match
                                             //
                                             *bExactMatch = true ;
                                             break ;
                                        }
                                   }
                              }
                         }
                    }
                    break ;
               default :
                    __if_exists(TItem::CompareValue)
                    {
                         bSortedList ;
                         if (0 == m_Items.GetSize())
                              return 0 ;
                         //
                         // Do a binary search
                         //
                         int nLeft = 0 ;
                         int nRight = m_Items.GetSize()-1 ;
                         int nMid = (nLeft+nRight)/2 ;
                         TItem* pEntry ;
                         int nCompare ;

                         //
                         // If we have no items we should return 0
                         //
                         nResult = 0 ;
                         //
                         // Binary search is of no use when having just
                         // a few elements
                         //
                         CComBSTR bstrTmp ;

                         if(bSortedList)
                         {
                              while (nRight-nLeft > 5)
                              {
                                   pEntry = m_Items[nMid] ;
                                   //
                                   // CompareValue returns > 0 if pEntry > NameOrIndex
                                   //
                                   nCompare = pEntry->CompareValue(NameOrIndex) ;

                                   if (nCompare < 0)
                                        nLeft = nMid ;
                                   else nRight = nMid ;
                                   nMid = (nLeft+nRight)/2 ;
                              }
                              //
                              // Now do a linear search on the last few items
                              //
                              for (nResult = nLeft ; nResult <= nRight ; nResult++)
                              {
                                   pEntry = m_Items[nResult] ;
                                   //
                                   // CompareValue returns > 0 if pEntry > NameOrIndex
                                   //
                                   nCompare = pEntry->CompareValue(NameOrIndex) ;

                                   if (nCompare > 0)
                                   {
                                        //
                                        // Found insertion point
                                        //
                                        *bExactMatch = false ;
                                        break ;
                                   }
                                   if (nCompare == 0)
                                   {
                                        //
                                        // Found exact match
                                        //
                                        *bExactMatch = true ;
                                        break ;
                                   }
                              }
                         }
                         else
                         {
                              //not a sorted list, we should do a lineair search until we
                              //find the item, or the end of the list
                              for (nResult = nLeft ; nResult <= nRight ; nResult++)
                              {
                                   pEntry = m_Items[nResult] ;
                                   nCompare = pEntry->CompareValue(NameOrIndex) ;

                                   if (nCompare == 0)
                                   {
                                        //
                                        // Found exact match
                                        //
                                        *bExactMatch = true ;
                                        break ;
                                   }
                              }
                         }
                    }
                    break ;

          }
          //
          // Now check the result value to see if it is valid
          //
          if (nResult >= 0 && nResult <= m_Items.GetSize())
               return nResult ;
          return -1 ;
     }
     void Insert(int nIndex, TItem* pItem)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          //
          // We become a parent of the object so
          // tell the object we are holding a reference
          //
          pItem->ConnectParent() ;
          //
          // Insert the item into the list
          //
          m_Items.InsertAt(nIndex, pItem) ;
     }
     void Add(TItem* pItem)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          //
          // We become a parent of the object so
          // tell the object we are holding a reference
          //
          pItem->ConnectParent() ;
          //
          // Add the item to the list
          //
          m_Items.Add(pItem) ;
     }
     void AddSorted(TItem* pItem)
     {
          //
          // Lock the list
          //
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          if (!pItem)
               return ;
          CComBSTR bstrNewName ;
          pItem->get_Name(&bstrNewName) ;
          //
          // Do binary search
          //
          bool bExact = false ;
          CComVariant vtName = bstrNewName ;
          int nIndex = GetIndex(&vtName, &bExact) ;

          if (nIndex < 0)
               return ;
          Insert(nIndex, pItem) ;
     }
     // 
     // GetIndex returns the 1-based index of the item in the collection
     //
     int GetIndex(TItem* pItem)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          //
          // Try to find it in the list
          //
          int nIndex = m_Items.Find(pItem) ;
          if (nIndex >= 0)
               return nIndex+1 ;
          return 0 ;
     }
     BOOL Remove(TItem* pItem)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          //
          // Try to find it in the list
          //
          int nIndex = m_Items.Find(pItem) ;
          if (nIndex >= 0)
          {
               //
               // We found it so remove it and
               // tell it one of it's parents no
               // longer holds a reference
               //
               m_Items.RemoveAt(nIndex) ;
               pItem->DisconnectParent() ;
               return TRUE ;
          }
          return FALSE ;
     }

     BOOL MoveFromTo(int from, int to)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          //
          // Move the items in the list
          //
          if (m_Items.MoveAt( from, to) )
          {
               return TRUE;
          }
          return FALSE;
     }

     TItem* ItemFromInterface(IUnknownPtr itfUnk)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;

          int nIndex ;
          for (nIndex = 0 ; nIndex < m_Items.GetSize() ; nIndex++)
          {
               IUnknownPtr itfTmp ;

               itfTmp = m_Items[nIndex] ;
               if (itfUnk == itfTmp)
                    return m_Items[nIndex] ;
          }
          return NULL ;
     }
};

template<class T, class TItem, class TItemInterface, class TParent> class ObjectCollection :  public ParentChildImpl<T, TParent>
{
public:
     ObjectCollection(TParent* pParent) : ParentChildImpl<T, TParent>(pParent)
     {
     }
     virtual ~ObjectCollection()
     {
          //OutputDebugString(TEXT("Destructor: ObjectCollection\n")) ;
     }
public:
     static T* Create (TParent* pParent)
     {
          T* pObj = new ObjectCollection<T, TItem, TItemInterface, TParent>(pParent);
          if (!pObj)
               return NULL;

          __if_exists(FinalConstruct)
          {
               pObj->InternalFinalConstructAddRef();

               HRESULT hr = S_OK ;

               __if_exists(T::_AtlInitialConstruct)
               {
                    hr = pObj->_AtlInitialConstruct();
               }

               hr = pObj->FinalConstruct ();
               pObj->InternalFinalConstructRelease();
               if (FAILED(hr))
               {
                    delete pObj;
                    return NULL;
               }
          }
          return pObj;
     }
protected:
     STDMETHOD(get_Count)(LONG* Result)
     {
          if (!Result)
               return E_POINTER ;
          *Result = GetCount() ;
          return S_OK ;
     }
     STDMETHOD(get_Item)(long lIndex, TItemInterface** pVal)
     {
          CComVariant vtIndex = lIndex ;
          return get_Item(vtIndex, pVal) ;
     }
     STDMETHOD(get_Item)(VARIANT NameOrIndex, TItemInterface** pVal)
     {
          __if_exists(T::GetItem)
          {
               return T::GetItem(NameOrIndex, pVal) ;
          }
          __if_not_exists(T::GetItem)
          {
               return GetItem(NameOrIndex, pVal) ;
          }
     }
     HRESULT GetItem(VARIANT NameOrIndex, TItemInterface** pVal)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          int nIndex = 0 ;

          if (NameOrIndex.vt == VT_BSTR)
          {
               CComBSTR bstrSearch(NameOrIndex.bstrVal) ;

               bstrSearch.ToLower() ;
               for (nIndex = 0 ; nIndex < m_Items.GetSize() ; nIndex++)
               {
                    CComBSTR bstrName ;

                    __if_exists(TItem::get_Name)
                    {
                         m_Items[nIndex]->get_Name(&bstrName) ;
                    }
                    bstrName.ToLower() ;
                    if (bstrName == bstrSearch)
                         break ;
               }
          }
          else
          {
               if ((NameOrIndex.vt == VT_I8) || (NameOrIndex.vt == VT_UI8))
                    nIndex = (int)NameOrIndex.llVal-1 ;
               else
               {
                    CComVariant vtTmp ;
                    vtTmp = NameOrIndex ;
                    vtTmp.ChangeType(VT_I4) ;
                    nIndex = vtTmp.lVal-1 ;
               }
          }
          if (nIndex < 0 || nIndex >= m_Items.GetSize())
          {
               *pVal = NULL ;
               return S_OK ;
          }
          return m_Items[nIndex]->QueryInterface(__uuidof(TItemInterface), (void**)pVal) ;
     }
     HRESULT NewEnum(IUnknown * * pVal)
     {
          CComCritSecLock<CComAutoCriticalSection> Lock(m_csCollectionLock) ;
          if (pVal == NULL)
               return E_POINTER;
          *pVal = NULL;

          HRESULT hr ;

          typedef CObjectCollectionEnumVariantImpl< TItem > _CEnum ;

          CObjectCollectionEnumVariantImpl<TItem>* pEnum = new CObjectCollectionEnumVariantImpl<TItem>(&m_Items) ;
          if (!pEnum)
               return E_OUTOFMEMORY ;

          hr = pEnum->QueryInterface(IID_IEnumVARIANT, (void**)pVal) ;
          if (FAILED(hr))
               delete pEnum ;
          return hr ;
     }
     STDMETHOD(get__NewEnum)(IUnknown * * pVal)
     {
          __if_exists(T::NewEnum)
          {
               return T::NewEnum(pVal) ;
          }
          __if_not_exists(T::NewEnum)
          {
               return NewEnum(pVal) ;
          }
     }
};

#define OBJECTCOLLECTION(BaseClass,ItemClass,ParentClass) ObjectCollection<BaseClass,ItemClass,I##ItemClass,ParentClass>
#define OBJECTCOLLECTIONEX(BaseClass,ItemClass,ParentClass) ObjectCollectionEx<BaseClass,ItemClass,I##ItemClass,ParentClass>

// *****************************************************************************
// TODO: documentation
// *****************************************************************************
class CObjectCollectionEnumVariant : public CComObject<CComEnum<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy<VARIANT> > >
{
public:
     CObjectCollectionEnumVariant() {} ;
     CObjectCollectionEnumVariant(CComVariant* varArray, LONG lSize)
     {
          Init(&varArray[0], &varArray[lSize], NULL, AtlFlagCopy) ;
     }
     virtual ~CObjectCollectionEnumVariant()
     {
          //OutputDebugString(TEXT("Destructor: CObjectCollectionEnumVariant\n")) ;
     }
} ;

template <class itemclass> class CObjectCollectionEnumVariantImpl : public CObjectCollectionEnumVariant
{
public:
     CObjectCollectionEnumVariantImpl(CpgSimpleArray<itemclass*> *pArray)
     {
          CComVariant* varArray ;

          varArray = new CComVariant[pArray->GetSize()] ;
          for (int nInd = 0 ; nInd < pArray->GetSize() ; nInd++)
          {
               //IDispatch* pUnk ;
               //itemclass* pItem ;

               //pUnk = 
               //pDispatch = NULL;
               //
               varArray[nInd].vt = VT_DISPATCH ;
               ((*pArray)[nInd])->QueryInterface(IID_IDispatch, (void**)&varArray[nInd].pdispVal) ;
               //if (pDispatch)
               //     pDispatch->Release() ;
          }
          Init(&varArray[0], &varArray[pArray->GetSize()], NULL, AtlFlagCopy) ;
          delete [] varArray ;
     }
};

// *****************************************************************************
// TODO: documentation
// *****************************************************************************
#pragma warning (disable: 4127)
template <class T, BOOL bAutoDelete>
class CPntrArray
{
public:

// Construction/destruction
     CPntrArray() : m_aT(NULL), m_nSize(0), m_nAllocSize(0),m_nAllocStep(8)
     { }

     virtual ~CPntrArray()
     {
          RemoveAll();
     }

// Operations
     int GetSize() const
     {
          return m_nSize;
     }
     BOOL Add(T* t)
     {
          if (!CheckAndIncreaseSize())
               return FALSE;
          m_aT[m_nSize++] = t;;
          return TRUE;
     }
     BOOL InsertAt(int nIndex, T* t)
     {
          _ASSERT(nIndex >= 0 && nIndex < m_nSize);
          //if (nIndex<0 || nIndex>= m_nSize)
          //   return Add(t);
          if (!CheckAndIncreaseSize())
               return FALSE;
          memmove((void*)&m_aT[nIndex + 1], (void*)&m_aT[nIndex], (m_nSize - nIndex) * sizeof(T*));
          m_nSize++;
          m_aT[nIndex] = t;
          return TRUE;
     }
     BOOL MoveAt(int from, int to)
     {
          _ASSERT(from >= 0 && from < m_nSize);
          _ASSERT(to >= 0 && to < m_nSize);
          if(from == to) return TRUE;
          T* tmp = m_aT[from];     //Remember pointer
           // fill up the gap of the 'from' item removed out of list
          if(from != (m_nSize - 1)) 
               memmove((void*)&m_aT[from], (void*)&m_aT[from + 1], (m_nSize - (from + 1)) * sizeof(T*));
          //Make room for a item at the 'to' position
          if(to != (m_nSize - 1)) 
               memmove((void*)&m_aT[to + 1], (void*)&m_aT[to], (m_nSize - (to + 1)) * sizeof(T*));
          m_aT[to] = tmp;
          return TRUE;
     }
     BOOL RemoveAt(int nIndex)
     {
          _ASSERT(nIndex >= 0 && nIndex < m_nSize);
          if (bAutoDelete)
               delete m_aT[nIndex];
          if(nIndex != (m_nSize - 1))
               memmove((void*)&m_aT[nIndex], (void*)&m_aT[nIndex + 1], (m_nSize - (nIndex + 1)) * sizeof(T*));
          m_nSize--;
          return TRUE;
     }
     void RemoveAll()
     {
          if (bAutoDelete)
          {
               for (int i=0; i<m_nSize; i++)
                    delete m_aT[i];
          }
          if(m_aT != NULL)
          {
               free(m_aT);
               m_aT = NULL;
          }
          m_nSize = 0;
          m_nAllocSize = 0;
     }
     T*& operator[] (int nIndex) const
     {
          _ASSERT(nIndex >= 0 && nIndex < m_nSize);
          return m_aT[nIndex];
     }
     void SetAtIndex(int nIndex, T* t)
     {
          _ASSERT(nIndex >= 0 && nIndex < m_nSize);
          if (bAutoDelete)
               delete m_aT[nIndex];
          m_aT[nIndex] = t;
     }
     void SetIncrement(int Size)
     {
          if (Size>0)
               m_nAllocStep = Size;
     }
     int Find(T* t) const
     {
          for(int i = 0; i < m_nSize; i++)
          {
               if(m_aT[i] == t)
                    return i;
          }
          return -1;  // not found
     }

private:
     BOOL CheckAndIncreaseSize()
     {
          if(m_nSize == m_nAllocSize) //Check if all is in use
          {
               T** aT;
               // increase size with at least m_nAllocStep items
               int nNewAllocSize = (m_nAllocSize == 0) ? m_nAllocStep : m_nSize + m_nAllocStep;
               // reallocate memory to hold more information
               aT = (T**)realloc(m_aT, nNewAllocSize * sizeof(T*));
               if(aT == NULL)
                    return FALSE;
               memset(aT+m_nAllocSize, 0, (nNewAllocSize-m_nAllocSize)* sizeof(T*)) ; 
               m_nAllocSize = nNewAllocSize;
               m_aT = aT;
          }
          return TRUE;
     }

     T** m_aT;
     int m_nSize;
     int m_nAllocSize;
     int m_nAllocStep;

};

// *****************************************************************************
// TODO: documentation
// *****************************************************************************
template <class T>
class CNicInterfaceArray
{
public:

// Construction/destruction
     CNicInterfaceArray() : m_aT(NULL), m_nSize(0), m_nAllocSize(0),m_nAllocStep(8)
     { }
     virtual ~CNicInterfaceArray()
     {
          RemoveAll();
     }

// Operations
     int GetSize() const 
    { 
        return m_nSize; 
    }
     BOOL Add(T* t)
     {
          t->AddRef();
          if (!CheckAndIncreaseSize())
               return FALSE;
          m_aT[m_nSize++] = t;
          return TRUE;
     }
     BOOL InsertAt(int nIndex, T* t)
     {
          _ASSERT(nIndex >= 0 && nIndex < m_nSize);
          if (!(nIndex >= 0 && nIndex < m_nSize))
               return FALSE;
          //Optional add it to the list
          //if (nIndex<0 || nIndex>= m_nSize)
          //   return Add(t);
          if (!CheckAndIncreaseSize())
               return FALSE;
          memmove((void*)&m_aT[nIndex + 1], (void*)&m_aT[nIndex], (m_nSize - nIndex) * sizeof(T*));
          t->AddRef();
          m_aT[nIndex] = t;
          m_nSize++;
          return TRUE;
     }
     BOOL MoveAt(int from, int to)
     {
          _ASSERT(from >= 0 && from < m_nSize);
          _ASSERT(to >= 0 && to < m_nSize);
          if (!((from >= 0 && from < m_nSize) && 
               (to >= 0 && to < m_nSize)))
               return FALSE;
          if(from == to) return TRUE;
          T* tmp = m_aT[from];     //Remember pointer
           // fill up the gap of the 'from' item removed out of list
          if(from != (m_nSize - 1)) 
               memmove((void*)&m_aT[from], (void*)&m_aT[from + 1], (m_nSize - (from + 1)) * sizeof(T*));
          //Make room for a item at the 'to' position
          if(to != (m_nSize - 1)) 
               memmove((void*)&m_aT[to + 1], (void*)&m_aT[to], (m_nSize - (to + 1)) * sizeof(T*));
          m_aT[to] = tmp;
          return TRUE;
     }
     BOOL RemoveAt(int nIndex)
     {
          _ASSERT(nIndex >= 0 && nIndex < m_nSize);
          if ( !(nIndex >= 0 && nIndex < m_nSize))
               return FALSE;
          m_aT[nIndex]->Release();
          if(nIndex != (m_nSize - 1))   //If not the last, shift up other interfaces
               memmove((void*)&m_aT[nIndex], (void*)&m_aT[nIndex + 1], (m_nSize - (nIndex + 1)) * sizeof(T*));
          m_nSize--;
          return TRUE;
     }
     void RemoveAll()
     {
          for (int i=0; i<m_nSize; i++) //Release interfaces
               m_aT[i]->Release();
          if(m_aT != NULL)    //Free interface pointer array
          {
               free(m_aT);
               m_aT = NULL;
          }
          m_nSize = 0;
          m_nAllocSize = 0;
     }
     T* operator[] (int nIndex) const   // read-only
     {
          _ASSERT(nIndex >= 0 && nIndex < m_nSize);
          if(!(nIndex >= 0 && nIndex < m_nSize))
               return NULL;
          return m_aT[nIndex];
     }
     void SetIncrement(int Size)
     {
          if (Size>0)
               m_nAllocStep = Size;
     }
     //Do not implement SetAtIndex. Or make sure to free occupied interface first
     void SetAtIndex(int nIndex, T* t)
     {
          _ASSERT(nIndex >= 0 && nIndex < m_nSize);
          if (!(nIndex >= 0 && nIndex < m_nSize))
               return;
          if (bAutoDelete)
               m_aT[nIndex]->Release();
          t->AddRef();
          m_aT[nIndex] = t;
     }

     int Find(T* t) const
     {
          for(int i = 0; i < m_nSize; i++)
          {
               if(m_aT[i] == t)
                    return i;
          }
          return -1;  // not found
     }

private:
// Functions
     BOOL CheckAndIncreaseSize()
     {
          if(m_nSize == m_nAllocSize) //Check if all is in use
          {
               T** aT;
               // increase size with at least m_nAllocStep items
               int nNewAllocSize = (m_nAllocSize == 0) ? m_nAllocStep : m_nSize + m_nAllocStep;
               // reallocate memory to hold more information
               aT = (T**)realloc(m_aT, nNewAllocSize * sizeof(T*));
               if(aT == NULL) 
                    return FALSE;
               memset(aT+m_nAllocSize, 0, (nNewAllocSize-m_nAllocSize)* sizeof(T*)) ; 
               m_nAllocSize = nNewAllocSize;
               m_aT = aT;
          }
          return TRUE;
     }
// Properties
     T** m_aT;
     int m_nSize;
     int m_nAllocSize;
     int m_nAllocStep;

};

template <class P, class T>
class CReadOnlyCollection
{
//--------------------------------------------------------------------------------------------------
// Copy constructor and assignment operator.
//--------------------------------------------------------------------------------------------------
private:
    CReadOnlyCollection (const CReadOnlyCollection&);
    CReadOnlyCollection& operator= (const CReadOnlyCollection&);

public:
     CReadOnlyCollection(P* pCollection)
     {
          ATLASSERT(pCollection != NULL) ;
          if (pCollection == NULL)
               return ;

          __if_exists(P::Lock)
          {
               pCollection->Lock() ;
          }
          int nCount = pCollection->GetCount() ;
          for (int nInd = 0 ; nInd < nCount ; nInd++)
          {
               T* pItem = pCollection->GetItemByIndex(nInd+1) ;
               if (pItem != NULL)
               {
                    __if_exists(T::AddRef)
                    {
                         pItem->AddRef() ;
                    }
                    m_Items.Add(pItem);
               }
          }
          // Check for the lock function. If there is a lock
          // the compile of the Unlock should succeed
          // If we can't lock it we should not unlock it
          __if_exists(P::Lock)
          {
               pCollection->Unlock() ;
          }
     }
     virtual ~CReadOnlyCollection()
     {
          for (int nInd = 0 ; nInd < m_Items.GetSize() ; nInd++)
          {
               // Remove item from collection
               T* pItem = m_Items[nInd] ;
               m_Items[nInd] = NULL ;
               // Release item
               // We check for the existence of the AddRef function
               // If we can't AddRef it, we should not release it
               __if_exists(T::AddRef)
               {
                    if (pItem)
                         pItem->Release() ;
               }
          }
     }

     int GetCount()
     {
          return m_Items.GetSize() ;
     }
     T* GetItemByIndex(int nIndex)
     {
          ATLASSERT(nIndex >= 1 && nIndex <= m_Items.GetSize()) ;
          if (nIndex >= 1 && nIndex <= m_Items.GetSize())
               return m_Items[nIndex-1] ;
          return NULL ;
     }

     bool Contains(T* Item)
     {
          for (int nInd = 0 ; nInd < m_Items.GetSize() ; nInd++)
          {
               if(m_Items[nInd] == Item)
                    return true;
          }
          return false;
     }

private:
     CpgSimpleArray<T*> m_Items ;
};

