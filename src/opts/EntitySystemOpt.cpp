//
// Created by xhy on 2022/7/25.
//
#include <llapi/mc/Actor.hpp>
#include <llapi/mc/BlockPos.hpp>
#include <llapi/mc/ChunkBlockPos.hpp>
#include <llapi/mc/Vec3.hpp>
#include <chrono>

#include "llapi/HookAPI.h"
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

// FILE *profile = nullptr;
// #include <BetterBDSMod.h>
//
// THook(void,
//       "?_tickLevelChunksAroundActor@LevelChunkTickingSystem@@CAXAEAVActor@@AEAVBlockSource@@"
//       "AEAVLoadedChunksComponent@@@Z",
//       Actor *actor, void *region, void *loadedChunksComponent) {
//     //    trapdoor::logger().debug("{} {} {}", actor == nullptr, region == nullptr,
//     //                             loadedChunksComponent == nullptr);
//     //    if (actor) {
//     //        trapdoor::logger().debug("actor type is {}", actor->getTypeName());
//     //    }
//     //
//
//     // trapdoor::logger().debug("Around actor {}", region);
//     original(actor, region, loadedChunksComponent);
// }
//
//// MCAPI class BlockPos const getTopRainBlockPos(class ChunkBlockPos const &);
//
// THook(BlockPos, "?getTopRainBlockPos@LevelChunk@@QEAA?BVBlockPos@@AEBVChunkBlockPos@@@Z",
//      void *chunk, const ChunkBlockPos &cp) {
//    auto r = original(chunk, cp);
//    printf("%d %d %d\n", r.x, r.y, r.z);
//    return r;
//}
