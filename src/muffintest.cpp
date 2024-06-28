#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

namespace MuffinTest {
struct Test {
	std::string name;
	void (*run)();
};

std::vector<Test> tests;
std::vector<std::string> failures;

int add_test(const char* name, void (*run)()) {
	tests.push_back(Test {
		.name = name,
		.run = run
	});
	return 0;
}

void fail(std::string reason) {
	failures.push_back(reason);
}

bool run(Test &test, int offset_width) {
	std::cout << "running " << test.name << std::flush;
	std::string indent = std::string(offset_width - test.name.length(), ' ');

	test.run();

	auto pass = failures.empty();
	if (pass) {
		std::cout << indent << "PASS" << std::endl;
	} else {
		std::cout << indent << "FAIL" << std::endl;
		for (auto &failure : failures) {
			std::cout << "\tcause: " << failure << std::endl;
		}
	}
	failures.clear();
	return pass;
}

void run_all() {
	int passes = 0;
	int fails = 0;
	int indent = std::max_element(tests.begin(), tests.end(), [](const Test &a, const Test &b){ return a.name.length() < b.name.length(); })->name.length() + 4;
	std::cout << "running tests (" << tests.size() << ")" << std::endl;
	for (auto &test : tests) {
		run(test, indent);
	}
}
} // namespace MuffinTest

#define TEST(name)                                                             \
  namespace MuffinTest_TestImpl_##name {                                                             \
  void test();                                                                 \
  int dummy = MuffinTest::add_test(#name, test);                               \
  }                                                                            \
  void MuffinTest_TestImpl_##name::test()

#define FAIL(reason) { MuffinTest::fail(reason); }
#define ASSERT_EQ(x, y) { if (x != y) { FAIL("ASSERT_EQ: assertion failed\n\t\t" + std::to_string(x) + " != " + std::to_string(y)) } }

int main() {
	MuffinTest::run_all();
}

