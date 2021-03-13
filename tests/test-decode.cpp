#include <iostream>
#include "cpp-can-parser/CANDatabase.h"
#include <exception>

#define ASSERT_EQUAL(expect,actual) \
    if (expect != actual) { \
        std::string error("ASSERT_EQUAL!\n"); \
        error += std::string("expect: ") + #expect + " (" + std::to_string(expect) + ")\n"; \
        error += std::string("actual: ") + #actual + " (" + std::to_string(actual) + ")\n"; \
        throw std::runtime_error(error); \
    }

int main(int argc, char** argv) {
    using namespace CppCAN;
    bool okay = true;
    const std::string file("dbc-files/decode-test.dbc");

    try {
        CANDatabase db = CANDatabase::fromFile(file);
        CANFrame frame = db[1523];
        uint64_t data = 0x4A004A006E002600;
        const CANSignal &batt_sig = frame["batt"];
        CANSignal::raw_t batt_raw = batt_sig.decode(&data);
        ASSERT_EQUAL(0x6e,batt_raw);
        ASSERT_EQUAL(11.0,batt_sig.rawToPhys(batt_raw));
    }
    catch(const CANDatabaseException& e) {
        std::cerr << "Error with file \"" << file << "\": " << e.what() << std::endl;
        okay = false;
    }
    catch(const std::runtime_error& re) {
        std::cerr << re.what() << std::endl;
        okay = false;
    }
    catch(...) {
        std::cerr << "An unexpected expection happened during "
                     "the parsing of \"" << file << "\"" << std::endl;
        okay = false;
    }

    return (okay ? 0 : 1);
}