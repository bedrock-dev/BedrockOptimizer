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

struct THopper {
    int mCooldownTime = -1;
    bool mTransferedFromChestMinecart = false;
    bool mIsEntity = false;
    uint8_t valid{};
    uint8_t info{};
    int mMoveItemSpeed = 4;
};

namespace {
    static_assert(sizeof(THopper) == 12);

    std::pair<int8_t, int8_t> getHopperFullAndEmpty(Container &cont) {
        int8_t emptySlot = 0;
        int8_t fullSlot = 0;
        for (int i = 0; i < 5; i++) {
            auto &item = cont.getItem(i);
            if (item.getId() == 0) {
                emptySlot++;
            } else if (item.getCount() >= item.getMaxStackSize()) {
                fullSlot++;
            }
        }
        return {emptySlot, fullSlot};
    }

    inline void getFullAndEmptyFromCache(THopper *p, uint8_t *full, uint8_t *empty) {
        // 高四位empty 低四位full
        uint8_t info = p->info;
        *full = info >> 4u;
        *empty = info & 0x0F;
    }

    bool tryPullInItemsFromWorld(Hopper *hopper, BlockSource &bs, Container &cont,
                                 const Vec3 &vec) {
        auto *itemActor = hopper->_getItemAt(bs, vec + Vec3(0.0f, 0.625f, 0.0f));
        return itemActor && hopper->_addItem(cont, *itemActor);
    }

    bool HopperAction(Hopper *self, const std::pair<int8_t, int8_t> &slotInfo, bool canPushItems,
                      BlockSource &bs, Container &cont, const Vec3 &v, bool above, int face) {
        auto changed = false;
        auto th = reinterpret_cast<THopper *>(self);
        // 有物品且对着容器就往下送物品
        if (canPushItems && slotInfo.first < 5) {
            changed = self->_pushOutItems(bs, cont, v, face);
        }

        th->mTransferedFromChestMinecart = false;
        // 漏斗是满的的就直接返回
        if (slotInfo.second == 5) {
            return changed;
        }

        if (above) {
            return changed | self->_tryPullInItemsFromAboveContainer(bs, cont, v);
        } else {
            return changed | tryPullInItemsFromWorld(self, bs, cont, v);
        }

        //        if (!self->_tryPullInItemsFromAboveContainer(bs, cont, v)) {
        //            if (!above) {
        //                auto res = tryPullInItemsFromWorld(self, bs, cont, v);
        //                return res | changed;
        //            } else {
        //                return changed;
        //            }
        //        }

        return true;
    }
}  // namespace

THook(void, "?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z", void *self, unsigned int index,
      ItemStackBase *itemStack) {
    auto &ba = dAccess<BlockActor, -200>(self);
    auto &hp = dAccess<Hopper, 424>(&ba);
    reinterpret_cast<THopper *>(&hp)->valid = 0;
    original(self, index, itemStack);
}

THook(bool, "?_tryMoveItems@Hopper@@IEAA_NAEAVBlockSource@@AEAVContainer@@AEBVVec3@@H_N@Z",
      Hopper *self, BlockSource &bs, Container &fromContainer, Vec3 const &v, int attachedFace,
      bool canPushItems) {
    auto *tHopper = reinterpret_cast<THopper *>(self);
    if (tHopper->mIsEntity) return original(self, bs, fromContainer, v, attachedFace, canPushItems);

    auto hma = trapdoor::mod().hopper;
    if (!hma) {
        return original(self, bs, fromContainer, v, attachedFace, canPushItems);
    }

    // 漏斗矿车直接返回

    auto pos = v.toBlockPos();
    auto aboveBlockPos = pos + BlockPos(0, 1, 0);
    auto &aboveBlock = bs.getBlock(aboveBlockPos);
    bool aboveIsContainer = aboveBlock.isContainerBlock();

    //    auto dir = tr::facingToBlockPos(static_cast<tr::TFACING>(attachedFace));
    //    auto attachPos = BlockPos(pos.x + dir.x, pos.y + dir.y, pos.z + dir.z);
    //    auto attachIsContainer = bs.getBlock(attachPos).isContainerBlock();
    bool change = false;
    if (tHopper->valid) {
        uint8_t fullCnt = 0, emptyCnt = 0;
        getFullAndEmptyFromCache(reinterpret_cast<THopper *>(self), &fullCnt, &emptyCnt);

        change = HopperAction(self, {fullCnt, emptyCnt}, canPushItems, bs, fromContainer, v,
                              aboveIsContainer, attachedFace);
    } else {
        auto slotInfo = getHopperFullAndEmpty(fromContainer);
        tHopper->valid = true;
        tHopper->info = (slotInfo.first << 4) | slotInfo.second;
        change = HopperAction(self, slotInfo, canPushItems, bs, fromContainer, v, aboveIsContainer,
                              attachedFace);
    }
    if (change) {
        tHopper->mCooldownTime = tHopper->mMoveItemSpeed;
    }
    return change;
}

THook(bool, "?isStackable@ItemStackBase@@QEBA_NAEBV1@@Z", ItemStackBase *self,
      const ItemStackBase *other) {
    if (self->getId() == 0 || other->getId() == 0) {
        return original(self, other);
    }
    if (self->getMaxStackSize() == 1) {
        return false;
    }

    // tr::logger().debug("{} check stackable with {}", self->getName(), other->getName());
    auto res = original(self, other);
    return res;
}
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