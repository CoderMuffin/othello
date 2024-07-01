#pragma once

#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <string>

#ifndef MUFFINTEST_STARTUP
	#define MUFFINTEST_STARTUP
#endif
#ifndef MUTTINTEST_TEARDOWN
	#define MUFFINTEST_TEARDOWN
#endif

namespace MuffinTest {
	struct Test {
		std::string name;
		void (*run)();

		Test(std::string name, void (*run)()) : name(name), run(run) { }
	};

	std::vector<Test> tests;
	std::vector<std::string> failures;

	static int add_test(const char* name, void (*run)()) {
		tests.push_back(Test(name, run));
		return 0;
	}

	static void fail(std::string reason) {
		failures.push_back(reason);
	}

	static bool run(Test& test, int offset_width) {
		std::cout << "running " << test.name << std::flush;
		std::string indent = std::string(offset_width - test.name.length(), ' ');

		test.run();

		auto pass = failures.empty();
		if (pass) {
			std::cout << indent << "\x1b[32mPASS\x1b[0m" << std::endl;
		} else {
			std::cout << indent << "\x1b[31mFAIL\x1b[0m" << std::endl;
			for (auto& failure : failures) {
				std::cout << "\tcause: " << failure << std::endl;
			}
		}
		failures.clear();
		return pass;
	}

	static void run_all() {
		int passes = 0;
		int fails = 0;
		int indent = std::max_element(tests.begin(), tests.end(), [](const Test& a, const Test& b) { return a.name.length() < b.name.length(); })->name.length() + 4;

		std::cout << "running tests (" << tests.size() << ")..." << std::endl;
		for (auto& test : tests) {
			if (run(test, indent)) {
				passes++;
			} else {
				fails++;
			}
		}

		std::cout << std::endl << "done. overall: " << (fails == 0 ? "\x1b[32mPASS\x1b[0m" : "\x1b[31mFAIL\x1b[0m") << std::endl;
		std::cout << "\t" << passes << " successes" << std::endl;
		std::cout << "\t" << fails << " failures" << std::endl;
	}
} // namespace MuffinTest

#define TEST(name)                                                             \
  namespace MuffinTest_TestImpl_##name {                                       \
  static void test();                                                          \
  int dummy = MuffinTest::add_test(#name, test);                               \
  }                                                                            \
  static void MuffinTest_TestImpl_##name::test()

#define FAIL(reason) { MuffinTest::fail(reason); }                             

#define ASSERT_EQ(x, y) { if (x != y) {                                        \
std::ostringstream s;                                                          \
s << x;                                                                        \
std::string x_str = s.str(); s.str(""); /* clear the stream */                 \
s << y;                                                                        \
std::string y_str = s.str(); s.str(""); /* clear the stream */                 \
s << "ASSERT_EQ: assertion failed" << std::endl;                               \
if (x_str.find_first_not_of("\n") == std::string::npos && y_str.find_first_not_of("\n") == std::string::npos) {\
	s << "\t\t" << x << " != " << y;                                           \
} else {                                                                       \
	s << x << "\tdoes not equal:" << std::endl << y;                           \
}                                                                              \
FAIL(s.str());                                                                 \
} }

int main() {
	MUFFINTEST_STARTUP
	MuffinTest::run_all();
	MUFFINTEST_TEARDOWN
}
