
#include "BetterBDSMod.h"
#include "Remotery.h"
#include "Version.h"
#include "pch.h"
inline void CheckProtocolVersion() {
#ifdef TARGET_BDS_PROTOCOL_VERSION
    auto currentProtocol = LL::getServerProtocolVersion();
    if (TARGET_BDS_PROTOCOL_VERSION != currentProtocol) {
        logger.warn("Protocol version not match, target version: {}, current version: {}.",
                    TARGET_BDS_PROTOCOL_VERSION, currentProtocol);
        logger.warn(
            "This will most likely crash the server, please use the Plugin that matches the BDS "
            "version!");
    }
#endif  // TARGET_BDS_PROTOCOL_VERSION
}

void initRMT() {
    Remotery* rmt;
    rmt_CreateGlobalInstance(&rmt);
}

void PluginInit() {
    initRMT();
    CheckProtocolVersion();
    trapdoor::init();
}
