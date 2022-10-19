//
// Created by xhy on 2022/8/16.
//
#include <llapi/mc/Block.hpp>

#include "BetterBDSMod.h"
#include "llapi/HookAPI.h"

// THook(uint64_t, "?getHighestNonAirSubChunkIndex@LevelChunk@@QEBAFXZ", void *chunk) {
//     //    auto r = original(chunk);
//     //    trapdoor::logger().debug("res is {}", r);
//     //    return r;
//     if (trapdoor::mod().actorPush) {
//         return 3;
//     } else {
//         return original(chunk);
//     }
// }

THook(bool, "?shouldRandomTick@Block@@QEBA_NXZ", Block *block) {
    if (!trapdoor::mod().randomTick) {
        return original(block);
    }

    auto id = block->getId();
    /*
     * -378 => minecraft:deepslate
     *    0 => minecraft:air
     *    1 => minecraft:stone
     *    3 => minecraft:dirt
     *    9 => minecraft:water
     *    87 => minecraft:netherrack
     *
     */

    if (id == -378 || id == 0 || id == 1 || id == 3 || id == 9 || id == 87) return false;
    return original(block);
}
