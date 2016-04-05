#include "TestsFixture.hpp"

void TestsFixture::logStream(std::ostream& stream)
{
	m_loggedStream = &stream;
	m_originalStreamBuffer = stream.rdbuf(m_stringStream.rdbuf());
}

std::string TestsFixture::getStreamLog()
{
	std::string res{m_stringStream.str()};
	m_stringStream.str("");
	m_loggedStream->rdbuf(m_originalStreamBuffer);
	return res;
}
