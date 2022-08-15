//
// Created by xhy on 2022/7/14.
//

#include "BetterBDSMod.h"
namespace trapdoor {
    Logger& logger() {
        static Logger logger("BetterBDS");
        return logger;
    }
    void init() {
        logger().consoleLevel = 8;
        registerOptCommand(1);
    }

    BetterBDSMod& mod() {
        static BetterBDSMod mod;
        return mod;
    }

}  // namespace trapdoor
