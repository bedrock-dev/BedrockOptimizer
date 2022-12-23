//
// Created by xhy on 2022/8/8.
//

#include <llapi/mc/Block.hpp>
#include <llapi/mc/BlockSource.hpp>
#include <llapi/mc/Container.hpp>
#include <llapi/mc/Hopper.hpp>
#include <llapi/mc/Item.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Vec3.hpp>
#include <cctype>
#include <cstdint>

#include "BetterBDSMod.h"
#include "llapi/HookAPI.h"

struct HopperCache {
    uint8_t valid : 8;
    uint8_t empty : 4;
    uint8_t full : 4;
};

struct THopper {
    int mCooldownTime = -1;
    bool mTransferedFromChestMinecart = false;
    bool mIsEntity = false;
    HopperCache cache{};
    int mMoveItemSpeed = 4;
};

static_assert(sizeof(HopperCache) == 2);
static_assert(sizeof(THopper) == 12);

namespace {

    void refreshHopperCache(Container &cont, THopper *hopper) {
        uint8_t emptySlot = 0;
        uint8_t fullSlot = 0;
        for (int i = 0; i < 5; i++) {
            auto &item = cont.getItem(i);
            if (item.getId() == 0) {
                emptySlot++;
            } else if (item.getCount() >= item.getMaxStackSize()) {
                fullSlot++;
            }
        }
        hopper->cache.empty = emptySlot;
        hopper->cache.full = fullSlot;
    }

    bool tryPullInItemsFromWorld(Hopper *hopper, BlockSource &bs, Container &cont,
                                 const Vec3 &vec) {
        auto *itemActor = hopper->_getItemAt(bs, vec + Vec3(0.0f, 0.625f, 0.0f));
        return itemActor && hopper->_addItem(cont, *itemActor);
    }

    bool HopperAction(Hopper *self, const HopperCache &cache, bool canPushItems, BlockSource &bs,
                      Container &cont, const Vec3 &v, bool above, int face) {
        auto changed = false;
        auto th = reinterpret_cast<THopper *>(self);
        // 有物品且对着容器就往下送物品
        if (canPushItems && cache.empty < 5) {
            changed = self->_pushOutItems(bs, cont, v, face);
        }

        th->mTransferedFromChestMinecart = false;
        // 漏斗是满的的就直接返回
        if (cache.full == 5) {
            return changed;
        }

        auto c1 = self->_tryPullInItemsFromAboveContainer(bs, cont, v);
        auto c2 = !above && tryPullInItemsFromWorld(self, bs, cont, v);
        return c1 || c2 || changed;
    }
}  // namespace

THook(void, "?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z", void *self, unsigned int index,
      ItemStackBase *itemStack) {
    auto &ba = dAccess<BlockActor, -200>(self);
    auto &hp = dAccess<Hopper, 424>(&ba);
    reinterpret_cast<THopper *>(&hp)->cache.valid = 0;
    original(self, index, itemStack);
}

THook(bool, "?_tryMoveItems@Hopper@@IEAA_NAEAVBlockSource@@AEAVContainer@@AEBVVec3@@H_N@Z",
      Hopper *self, BlockSource &bs, Container &fromContainer, Vec3 const &v, int attachedFace,
      bool canPushItems) {
    auto *tHopper = reinterpret_cast<THopper *>(self);
    // 漏斗矿车直接返回
    if (tHopper->mIsEntity) return original(self, bs, fromContainer, v, attachedFace, canPushItems);

    auto hma = trapdoor::mod().hopper;
    if (!hma) {
        return original(self, bs, fromContainer, v, attachedFace, canPushItems);
    }

    auto pos = v.toBlockPos();
    auto aboveBlockPos = pos + BlockPos(0, 1, 0);
    auto &aboveBlock = bs.getBlock(aboveBlockPos);
    bool aboveIsContainer = aboveBlock.isContainerBlock();
    bool change = false;
    if (!tHopper->cache.valid) {
        refreshHopperCache(fromContainer, tHopper);
        tHopper->cache.valid = true;
    }
    change = HopperAction(self, tHopper->cache, canPushItems, bs, fromContainer, v,
                          aboveIsContainer, attachedFace);
    if (change) {
        tHopper->mCooldownTime = tHopper->mMoveItemSpeed;
    }
    return change;
}

// Container

THook(bool, "?_isFullContainer@Hopper@@IEAA_NAEAVContainer@@H@Z", Hopper *hopper,
      Container &self, int face) {
    auto hma = trapdoor::mod().hopper;
    if (!hma) {
        return original(hopper, self, face);
    }

    auto &cache = dAccess<std::string, 184>(&self);
    if (cache.size() == 4 && cache[2] == 1) {  // full cache有效
        return cache[3] != 0;
    } else {  // cache失效， 刷新cache
        auto full = original(hopper, self, face);
        if (cache.size() != 4) cache = std::string(4, 0);  // 创建cache
        cache[2] = 1;
        cache[3] = full ? 1 : 0;
        return full;
    }
}

THook(bool, "?_isEmptyContainer@Hopper@@IEAA_NAEAVContainer@@H@Z", Hopper *self, Container &c) {
    auto hma = trapdoor::mod().hopper;
    if (!hma) {
        return original(self, c);
    }
    return c.isEmpty();
}

THook(bool, "?isEmpty@Container@@UEBA_NXZ", Container *self) {
    auto hma = trapdoor::mod().hopper;
    if (!hma) {
        return original(self);
    }

    auto &cache = dAccess<std::string, 184>(self);
    if (cache.size() == 4 && cache[0] == 1) {  // cache有效
        return cache[1] != 0;
    } else {  // cache失效， 刷新cache
        auto isEmpty = original(self);
        if (cache.size() != 4) cache = std::string(4, 0);  // 刷新cache
        cache[0] = 1;
        cache[1] = isEmpty ? 1 : 0;
        return isEmpty;
    }
}

THook(void, "?setContainerChanged@Container@@UEAAXH@Z", Container *self, unsigned int a2) {
    // 让cache失效
    auto hma = trapdoor::mod().hopper;
    if (!hma) {
        original(self, a2);
        return;
    }
    // 0 empty valid
    // 1 empty
    // 2 full valid
    // 3 full

    auto &cache = dAccess<std::string, 184>(self);
    if (cache.size() != 4) cache = std::string(4, 0);  // 创建cache
    cache[0] = cache[2] = 0;                           // 设置失效
    original(self, a2);
}