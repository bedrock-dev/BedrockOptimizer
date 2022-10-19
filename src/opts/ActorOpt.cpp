//
// Created by xhy on 2022/7/17.
//
#include <llapi/mc/Actor.hpp>

#include "BetterBDSMod.h"
#include "llapi/HookAPI.h"
#include "Remotery.h"

THook(void, "?push@PushableComponent@@QEAAXAEAVActor@@AEBVVec3@@@Z", void *comp, void *a1,
      const Vec3 &v) {
    if (!trapdoor::mod().actorPush) {
        original(comp, a1, v);
        return;
    }
    if (v != Vec3::ZERO) {
        original(comp, a1, v);
    }
}

// THook(__int64, "_ZN5Actor4tickER11BlockSource", Actor *self, BlockSource *a2) {
//     return original(self, a2);
// }

// THook(void, "?tick@Actor@@QEAA_NAEAVBlockSource@@@Z", Actor *actor, void *bs) {
//     rmt_ScopedCPUSample(ActorTick, 0);
//     original(actor, bs);
// }
//
// THook(void, "?tick@ActorLegacyTickSystem@@UEAAXAEAVEntityRegistry@@@Z", void *self, void *a) {
//     rmt_ScopedCPUSample(ActorLegacySystem, 0);
//     original(self, a);
// }
//
// THook(void,
//       "?tick@PredictedMovementComponent@@QEAAXAEBV?$time_point@Usteady_clock@chrono@std@@V?$"
//       "duration@_JU?$ratio@$00$0DLJKMKAA@@std@@@23@@chrono@std@@@Z",
//       void *s, void *a) {
//     rmt_ScopedCPUSample(PredictedMovementComponentTick, 0);
//     original(s, a);
// }
// THook(void, "?refreshComponents@Actor@@QEAAXXZ", void *s) {
//     rmt_ScopedCPUSample(ActorRefreshComponents, 0);
//     original(s);
// }

// THook(void, "?updateTickingData@Actor@@QEAAXXZ", void *s) {
//     rmt_ScopedCPUSample(ActorupdateTickingData, 0);
//     original(s);
// }
// THook(void, "?baseTick@Mob@@UEAAXXZ", void *s) {
//    rmt_ScopedCPUSample(MobBaseTick, 0);
//    original(s);
//}
// THook(void, "?aiStep@Mob@@UEAAXXZ", void *s) {
//    rmt_ScopedCPUSample(AI, 0);
//    original(s);
//}
// THook(void, "?_aiStep@Mob@@SAXAEAUIMobMovementProxy@@@Z", void *s, void *p) {
//    rmt_ScopedCPUSample(InnerAI, 0);
//    original(s, p);
//}
// THook(void, "?normalTick@Mob@@UEAAXXZ", void *s) {
//    rmt_ScopedCPUSample(MobNormalTick, 0);
//    original(s);
//}

// THook(void, "?_updateMobTravel@Mob@@SAXAEAUIMobMovementProxy@@@Z", void *self, void *p) {
//     rmt_ScopedCPUSample(UpdateMobTravel, 0);
//     original(self, p);
// }

// THook(void, "?normalTick@Actor@@UEAAXXZ", void *s) {
//     rmt_ScopedCPUSample(ActorNormalTick, 0);
//     original(s);
// }
//
// THook(void, "?baseTick@Actor@@UEAAXXZ", void *s) {
//     rmt_ScopedCPUSample(ActorBaseTick, 0);
//     original(s);
// }
// THook(void, "?pushActors@Mob@@UEAAXXZ", void *s) {
//     rmt_ScopedCPUSample(MobPushActors, 0);
//     original(s);
// }

// THook(bool, "?canExistWhenDisallowMob@Mob@@UEBA_NXZ", void *s) {
//     rmt_ScopedCPUSample(MobPushActors, 0);
//     return original(s);
// }

// THook(int, "?update@CompassSpriteCalculator@@QEAAHAEAVActor@@_N@Z", void *s, void *a1, void *a2)
// {
//     rmt_ScopedCPUSample(CompassSpriteUpdate, 0);
//     auto v = original(s, a1, a2);
//     return v;
// }
//  THook(int, "?update@ClockSpriteCalculator@@QEAAHAEAVActor@@_N@Z", void *s, void *a1, void *a2) {
//      rmt_ScopedCPUSample(CompassSpriteUpdate, 0);
//      auto v = original(s, a1, a2);
//      return v;
//  }
// THook(int, "?canBeginOrContinueClimbingLadder@Actor@@SA_NAEBUIActorMovementProxy@@@Z", void *s,
//       void *a1) {
//     rmt_ScopedCPUSample(ClimbingLadder, 0);
//     auto v = original(s, a1);
//     return v;
// }
// THook(void *,
//       "?fetchEntities@BlockSource@@UEAA?AV?$span@V?$not_null@PEAVActor@@@gsl@@$0?0@gsl@@"
//       "W4ActorType@@AEBVAABB@@PEBVActor@@@Z",
//       void *bs, int64_t type, void *a, void *b) {
//     rmt_ScopedCPUSample(FetchEntities, 0);
//     return original(bs, type, a, b);
// }

// THook(void *, "?getGameRules@Level@@UEAAAEAVGameRules@@XZ", void *l) {
//     rmt_ScopedCPUSample(LevelGetGameRule, 0);
//     return original(l);
// }
//  THook(void, "?_playMovementSound@Actor@@IEAAX_N@", void *actor, bool c) {
//      if (!trapdoor::mod().componentRemove) {
//          original(actor, c);
//      }

// }