//
// Created by xhy on 2022/7/14.
//

#include "BetterBDSMod.h"
namespace trapdoor {
    Logger& logger() {
        static Logger logger("BedrockOptimizer");
        return logger;
    }
    void init() {
        logger().consoleLevel = 8;
        registerOptCommand(1);
        trapdoor::mod().readConfig();
    }

    BetterBDSMod& mod() {
        static BetterBDSMod mod;
        return mod;
    }

    void BetterBDSMod::readConfig() {
        // TODO
    }
}  // namespace trapdoor
