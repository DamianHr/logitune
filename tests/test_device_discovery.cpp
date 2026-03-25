#include <gtest/gtest.h>
#include "DeviceManager.h"

using namespace logitune;

TEST(DeviceDiscovery, IdentifyBoltReceiver) {
    EXPECT_TRUE(DeviceManager::isReceiver(0xc548));
    EXPECT_TRUE(DeviceManager::isReceiver(0xc52b));
    EXPECT_FALSE(DeviceManager::isReceiver(0xb034));
    EXPECT_FALSE(DeviceManager::isReceiver(0x0000));
}

TEST(DeviceDiscovery, IdentifyDirectDevice) {
    EXPECT_TRUE(DeviceManager::isDirectDevice(0xb034));
    EXPECT_FALSE(DeviceManager::isDirectDevice(0xc548));
    EXPECT_FALSE(DeviceManager::isDirectDevice(0x0000));
}

TEST(DeviceDiscovery, DeviceIndexForTransport) {
    EXPECT_EQ(DeviceManager::deviceIndexForDirect(), 0xFF);
    EXPECT_EQ(DeviceManager::deviceIndexForReceiver(1), 1);
    EXPECT_EQ(DeviceManager::deviceIndexForReceiver(6), 6);
}
