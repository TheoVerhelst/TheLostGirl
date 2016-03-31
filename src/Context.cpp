#include <TheLostGirl/Application.hpp>
#include <TheLostGirl/Context.hpp>

#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) Type* ContextImpl::m_attribute{nullptr};
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT

ContextImpl::ContextImpl(
#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) Type& Name,
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
		std::nullptr_t dummy)
{
#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) m_attribute = &Name;
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
}
