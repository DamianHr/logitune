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
    info.rawXY      = (r.params[4] & 0x20) != 0; // additionalFlags bit 5 = rawXY capable
    return info;
}

// From logid: setControlReporting flags byte layout:
// byte[2] bits: 0=divert, 1=persist
// byte[3] bits: 0=rawXYDiverted, 1=persistRawXY (need ChangeRawXYDivert flag)
// byte[4] bits: for forceRawXY
std::vector<uint8_t> ReprogControls::buildSetDivert(uint16_t controlId, bool divert, bool rawXY)
{
    uint8_t flags = divert ? 0x03 : 0x00; // divert + persist
    uint8_t rawFlags = 0;
    if (rawXY) {
        rawFlags = 0x03; // rawXYDiverted + persistRawXY
    }
    return {
        static_cast<uint8_t>(controlId >> 8),
        static_cast<uint8_t>(controlId & 0xFF),
        flags,
        rawFlags
    };
}

uint16_t ReprogControls::parseDivertedButtonEvent(const Report &r)
{
    return static_cast<uint16_t>((r.params[0] << 8) | r.params[1]);
}

// DivertedRawXYEvent: params[0-1]=dx (int16 BE), params[2-3]=dy (int16 BE)
RawXYEvent ReprogControls::parseDivertedRawXYEvent(const Report &r)
{
    RawXYEvent evt;
    evt.dx = static_cast<int16_t>((r.params[0] << 8) | r.params[1]);
    evt.dy = static_cast<int16_t>((r.params[2] << 8) | r.params[3]);
    return evt;
}

} // namespace logitune::hidpp::features
