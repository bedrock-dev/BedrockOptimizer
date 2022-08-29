//
// Created by xhy on 2022/8/8.
//

#include <MC/Block.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Container.hpp>
#include <MC/Hopper.hpp>
#include <MC/Item.hpp>
#include <MC/ItemStack.hpp>
#include <cctype>
#include <cstdint>

#include "BetterBDSMod.h"
#include "HookAPI.h"
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
        //        if (above) {
        //            return self->_tryPullInItemsFromAboveContainer(bs, cont, v) || changed;
        //        } else {
        //            return tryPullInItemsFromWorld(self, bs, cont, v) || changed;
        //        }

        //        if (!self->_tryPullInItemsFromAboveContainer(bs, cont, v)) {
        //            if (!above) {
        //                auto res = tryPullInItemsFromWorld(self, bs, cont, v);
        //                return res | changed;
        //            } else {
        //                return changed;
        //            }
        //        }
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
    //  bool aboveIsContainer = aboveBlock.getId() == 154;
    bool aboveIsContainer = aboveBlock.isContainerBlock();



    //    if (aboveIsContainer) {
    //        trapdoor::logger().debug("name: {} id: {}", aboveBlock.getTypeName(),
    //        aboveBlock.getId());
    //    }

    //! reinterpret_cast<uint64_t>(&aboveBlock);

    //    auto dir = tr::facingToBlockPos(static_cast<tr::TFACING>(attachedFace));
    //    auto attachPos = BlockPos(pos.x + dir.x, pos.y + dir.y, pos.z + dir.z);
    //    auto attachIsContainer = bs.getBlock(attachPos).isContainerBlock();
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

// THook(bool, "?isStackable@ItemStackBase@@QEBA_NAEBV1@@Z", ItemStackBase *self,
//       const ItemStackBase *other) {
//     if (self->getId() == 0 || other->getId() == 0) {
//         return original(self, other);
//     }
//     if (self->getMaxStackSize() == 1) {
//         return false;
//     }
//
//     // tr::logger().debug("{} check stackable with {}", self->getName(), other->getName());
//     auto res = original(self, other);
//     return res;
// }
THook(bool, "?_isFullContainer@Hopper@@IEAA_NAEAVBlockSource@@AEAVContainer@@H@Z", Hopper *hopper,
      BlockSource &bs, Container &container, int face) {
    auto hma = trapdoor::mod().hopper;
    if (!hma) {
        return original(hopper, bs, container, face);
    }
    auto sz = container.getSize();
    for (int i = 0; i < sz; i++) {
        auto &item = container.getItem(i);
        if (item.isNull() || item.getId() == 0 || item.getCount() < item.getMaxStackSize()) {
            return false;
        }
    }
    return true;
}