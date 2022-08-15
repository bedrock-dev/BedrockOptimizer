//
// Created by xhy on 2022/7/14.
//

#ifndef BETTERBDS_BETTERBDSMOD_H
#define BETTERBDS_BETTERBDSMOD_H
#include "LoggerAPI.h"

namespace trapdoor {

    void registerOptCommand(int level);

    class BetterBDSMod {
       public:
        bool componentRemove = false;
        bool actorPush = false;
        bool hopper = false;
    };

    BetterBDSMod &mod();
    Logger &logger();
    void init();
}  // namespace trapdoor
#endif  // BETTERBDS_BETTERBDSMOD_H