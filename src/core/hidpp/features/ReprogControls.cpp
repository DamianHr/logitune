#include "hidpp/features/ReprogControls.h"

namespace logitune::hidpp::features {

int ReprogControls::parseControlCount(const Report &r)
{
    return static_cast<int>(r.params[0]);
}

ControlInfo ReprogControls::parseControlInfo(const Report &r)
{
    ControlInfo info;
    info.controlId  = static_cast<uint16_t>((r.params[0] << 8) | r.params[1]);
    info.taskId     = static_cast<uint16_t>((r.params[2] << 8) | r.params[3]);
    info.divertable = (r.params[4] & 0x01) != 0;
    info.persist    = (r.params[4] & 0x02) != 0;
    return info;
}

std::vector<uint8_t> ReprogControls::buildSetDivert(uint16_t controlId, bool divert)
{
    uint8_t flags = divert ? 0x03 : 0x00;
    return {
        static_cast<uint8_t>(controlId >> 8),
        static_cast<uint8_t>(controlId & 0xFF),
        flags
    };
}

uint16_t ReprogControls::parseDivertedButtonEvent(const Report &r)
{
    return static_cast<uint16_t>((r.params[0] << 8) | r.params[1]);
}

} // namespace logitune::hidpp::features
