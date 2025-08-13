#pragma once

#include <atlcom.h>
#include <cpplib/com/checked_hresult.h>

namespace cpp
{
namespace com
{

template <typename T>
class ComGitPtr
{
public:
	ComGitPtr()
	{
	}

    ComGitPtr(const ATL::CComPtr<T>& p) : 
        m_p{p}
    {
    }

    ATL::CComPtr<T> get() const
	{
		ATL::CComPtr<T> p;
		CheckedHResult chr = m_p.CopyTo(&p);
		return p;
	}

	ATL::CComPtr<T> operator->() const
	{
		return get();
	}

private:
	ATL::CComGITPtr<T> m_p;
};

} // com 
} // cpp
