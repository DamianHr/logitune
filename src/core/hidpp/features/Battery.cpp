#include "hidpp/features/Battery.h"

namespace logitune::hidpp::features {

BatteryStatus Battery::parseStatus(const Report &r)
{
    // UnifiedBattery (0x1004) response/notification format (from Solaar):
    //   params[0] = percentage (0-100), 0 means use level bitmask
    //   params[1] = level bitmask (bit3=full, bit2=good, bit1=low, bit0=critical)
    //   params[2] = BatteryState enum (0=discharging, 1=recharging, etc.)
    //   params[3] = reserved
    BatteryStatus status;
    status.level = static_cast<int>(r.params[0]);
    status.levelBitmask = r.params[1];
    status.state = static_cast<BatteryState>(r.params[2]);

    // If percentage is 0, approximate from bitmask
    if (status.level == 0) {
        if (status.levelBitmask & 0x08)      status.level = 90;
        else if (status.levelBitmask & 0x04) status.level = 50;
        else if (status.levelBitmask & 0x02) status.level = 20;
        else if (status.levelBitmask & 0x01) status.level = 5;
    }

    // Charging = any state that implies power is connected
    status.charging = (status.state == BatteryState::Recharging ||
                       status.state == BatteryState::AlmostFull ||
                       status.state == BatteryState::Full ||
                       status.state == BatteryState::SlowRecharge);
    return status;
}

} // namespace logitune::hidpp::features
