#include <iostream>
#include "cpp-can-parser/CANDatabase.h"

int main(int argc, char** argv) {
    using namespace CppCAN;
    bool okay = true;
    const std::string file("dbc-files/decode-test.dbc");

    try {
        CANDatabase db = CANDatabase::fromFile(file);
        CANFrame frame = db[1520];
        uint64_t data = 0x023B000000000000;
        // uint64_t data = 0xFFFFFFFFFFFFFFFF;
        for (const auto &signal_itr : frame)
        {
            const CANSignal &sig = signal_itr.second;
            CANSignal::raw_t raw = sig.decode(&data);
            printf(
                "%s: scale = %f, offset = %f, raw = %lu, phys = %f\n",
                sig.name().c_str(),
                sig.scale(),
                sig.offset(),
                raw,
                sig.rawToPhys(raw));
        }
    }
    catch(const CANDatabaseException& e) {
        std::cerr << "Error with file \"" << file << "\": " << e.what() << std::endl;
        okay = false;
    }
    catch(...) {
        std::cerr << "An unexpected expection happened during "
                     "the parsing of \"" << file << "\"" << std::endl;
        okay = false;
    }

    return (okay ? 0 : 1);
}