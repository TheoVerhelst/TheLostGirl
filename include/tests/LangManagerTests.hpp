#ifndef LANGMANAGERTESTS_HPP
#define LANGMANAGERTESTS_HPP

#include <tests/TestsFixture.hpp>

class LangManagerTestsFixture : public TestsFixture
{
	public:
	/// Constructor.
	LangManagerTestsFixture();

	/// Destructor.
	~LangManagerTestsFixture();

	static inline std::size_t getFileLineNumber(const std::string& filename);

	LangManager langs;///< The lang manager.
#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) Type* m_attribute{nullptr};
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
	ContextImpl context;///< The context of this test.
};

#endif//LANGMANAGERTESTS_HPP
