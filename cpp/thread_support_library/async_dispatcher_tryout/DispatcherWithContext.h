#pragma once

#include <memory>

namespace TaskExecution {

template <typename Context, typename DispatcherType>
class DispatcherWithContext : public DispatcherType
{
public:
	template<typename... Args>
	DispatcherWithContext(Args&&... args) :
		DispatcherType(std::forward<Args>(args)...)
	{		
		this->Call([this]() { m_pContext = std::make_unique<Context>(); });
	}

	~DispatcherWithContext()
	{
		this->Call([this]() { m_pContext.reset(); });
	}

private:
	std::unique_ptr<Context> m_pContext;
};

}