#pragma once
#include "hidpp/HidppTypes.h"
#include <cstdint>
#include <vector>

namespace logitune::hidpp::features {

struct ControlInfo {
    uint16_t controlId;    // button ID (e.g., 0x0050 = left click)
    uint16_t taskId;
    bool divertable;       // can be diverted to host
    bool persist;          // diversion persists across power cycle
};

class ReprogControls {
public:
    static int parseControlCount(const Report &r);
    static ControlInfo parseControlInfo(const Report &r);
    static std::vector<uint8_t> buildSetDivert(uint16_t controlId, bool divert);
    static uint16_t parseDivertedButtonEvent(const Report &r);

    static constexpr uint8_t kFnGetControlCount     = 0x00;
    static constexpr uint8_t kFnGetControlInfo       = 0x01;
    static constexpr uint8_t kFnSetControlReporting  = 0x03;
};

} // namespace logitune::hidpp::features
