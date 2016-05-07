#ifndef LANGMANAGERTESTS_HPP
#define LANGMANAGERTESTS_HPP

#include <experimental/filesystem>
#include <tests/TestsFixture.hpp>

class LangManagerTestsFixture : public TestsFixture
{
	public:
	/// Constructor.
	LangManagerTestsFixture();

	/// Destructor.
	~LangManagerTestsFixture();

	static inline std::size_t getFileLineNumber(const std::experimental::filesystem::path& filename);

#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) Type* m_attribute{nullptr};
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
	ContextImpl context;///< The context of this test.
	LangManager langs;///< The lang manager.
};

#endif//LANGMANAGERTESTS_HPP
