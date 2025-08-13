// Like ATL::IDispEventImpl<> but with full COM support.

//	class MyEventSink :
//		public CComObjectRootEx<CComMultiThreadModel>,
//		public DispinterfaceImpl<0, MyEventSink, IMyEvents, &__uuidof(IMyEvents), &__uuidof(__MyTypeLib), 1, 0>
//	{
//	public:
//		BEGIN_COM_MAP(MyEventSink)
//			COM_INTERFACE_ENTRY(IMyEvents)
//		END_COM_MAP()
//	 
//		BEGIN_SINK_MAP(MyEventSink)
//			SINK_ENTRY_EX(0, __uuidof(Acquisition::Server::Com::IScanGeneratorEvents), dispid_IMyEvents_OnEvent, OnEvent)
//		END_SINK_MAP()
//	
//		void __stdcall OnEvent()
//	};

#pragma once

namespace TaskExecution {

template <UINT nID, class T, typename IEvents, const IID* pdiid = &__uuidof(IEvents), const GUID* plibid = &GUID_NULL, WORD wMajor = 0, WORD wMinor = 0, class tihclass = CComTypeInfoHolder>
class DispinterfaceImpl :
	public IDispEventImpl<nID, T, pdiid, plibid, wMajor, wMinor, tihclass>,
	public IEvents
{
public:
	typedef IDispEventImpl<nID, T, pdiid, plibid, wMajor, wMinor, tihclass> DispEventImpl;

	STDMETHODIMP GetTypeInfoCount(unsigned int* pctinfo)
	{
		return DispEventImpl::GetTypeInfoCount(pctinfo);
	}

	STDMETHODIMP GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo** ppTInfo)
	{
		return DispEventImpl::GetTypeInfo(iTInfo, lcid, ppTInfo);
	}

	STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR** rgszNames, unsigned int cNames, LCID lcid, DISPID* rgDispId)
	{
		return DispEventImpl::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
	}

	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, unsigned int* puArgErr)
	{
		return DispEventImpl::Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	}
};

}
