#ifndef TESTSFIXTURE_HPP
#define TESTSFIXTURE_HPP

#include <iostream>
#include <sstream>

class TestsFixture
{
	protected:
		void logStream(std::ostream& stream);
		std::string getStreamLog();

	private:
		std::ostream* m_loggedStream;
		std::streambuf* m_originalStreamBuffer;
		std::stringstream m_stringStream;
};

#endif//TESTSFIXTURE_HPP
