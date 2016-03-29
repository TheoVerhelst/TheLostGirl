#include <TheLostGirl/Application.hpp>
#include <TheLostGirl/Context.hpp>

#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) Type* Context::m_attribute{nullptr};
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT

Context::Context(Application& application)
{
#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) m_attribute = &application.m_attribute;
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
}
