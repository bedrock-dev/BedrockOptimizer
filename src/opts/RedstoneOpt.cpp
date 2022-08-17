//
// Created by xhy on 2022/7/14.
//

#include <HookAPI.h>

#include <MC/BaseCircuitComponent.hpp>
#include <MC/CircuitComponentList.hpp>
#include <MC/CircuitSceneGraph.hpp>
#include <MC/Level.hpp>

#include "BetterBDSMod.h"
#include "Global.h"
#include "Remotery.h"

struct ComponentItem {
    BaseCircuitComponent *mComponent = nullptr;  // 0 * 4 - 1 * 4
    int mDampening = 0;                          // 2 * 4
    BlockPos mPos;                               // 3 * 4 - 5 * 4
    unsigned char facing{};                      // 6 * 4
    bool mDirectlyPowered = false;               // 6* 4
    int mData = 0;                               // 7*4
};

std::vector<BlockPos> getBlockPosNeighbors(const BlockPos &p) {
    return {{p.x + 1, p.y, p.z}, {p.x - 1, p.y, p.z}, {p.x, p.y + 1, p.z},
            {p.x, p.y - 1, p.z}, {p.x, p.y, p.z + 1}, {p.x, p.y, p.z - 1}};
}

BlockPos toChunkPos(const BlockPos &p) {
    auto cx = p.x < 0 ? p.x - 15 : p.x;
    auto cz = p.z < 0 ? p.z - 15 : p.z;
    return {cx / 16 * 16, 0, cz / 16 * 16};
}

/*Don't torch it*/

struct PendingEntry {
    BaseCircuitComponent *mRawComponentPtr;
    std::unique_ptr<BaseCircuitComponent> mComponent;
    BlockPos mPos;
};

namespace std {
    template <>
    struct hash<PendingEntry> {
        std::size_t operator()(PendingEntry const &ent) const noexcept {
            return hash<BlockPos>()(ent.mPos);
        }
    };
}  // namespace std
/*
 *Core data structure
 */
struct TCircuitSceneGraph {
    std::unordered_map<BlockPos, std::unique_ptr<BaseCircuitComponent>> mAllComponents;
    std::vector<ComponentItem> mActiveComponents;
    std::unordered_map<BlockPos, std::vector<ComponentItem>> mActiveComponentsPerChunk;
    std::unordered_map<BlockPos, std::vector<ComponentItem>> mPowerAssociationMap;
    std::unordered_map<BlockPos, PendingEntry> mPendingAdds;
    std::unordered_map<BlockPos, PendingEntry> mPendingUpdates;
    std::unordered_map<BlockPos, std::vector<BlockPos>> mComponentsToReEvaluate;
    std::vector<PendingEntry> mPendingRemoves;
};

static_assert(sizeof(ComponentItem) == 32);

THook(void, "?removeComponent@CircuitSceneGraph@@AEAAXAEBVBlockPos@@@Z", void *g,
      const BlockPos &p) {}

void optPendingRemoves(CircuitSceneGraph *graph) {
    // rmt_ScopedCPUSample(PendingRemoveAll, 0);
    auto *g = reinterpret_cast<TCircuitSceneGraph *>(graph);
    std::unordered_set<BlockPos> activeToRemove;
    for (auto &e : g->mPendingRemoves) {
        // rmt_ScopedCPUSample(RemoveOne, 0);
        auto pos = e.mPos;
        // 从全局元件表表内删除
        auto componentGroupIter = g->mAllComponents.find(pos);
        if (componentGroupIter == g->mAllComponents.end()) continue;

        auto toRemove = std::move(componentGroupIter->second);
        auto targetComp = toRemove.get();
        g->mAllComponents.erase(pos);

        if ((int)targetComp->getCircuitComponentType() != 0x40000) {
            //            reinterpret_cast<CircuitComponentList *>(&g->mActiveComponents)
            //                ->removeSource(pos, targetComp);
            activeToRemove.insert(pos);
            g->mPowerAssociationMap.erase(pos);
        }

        {
            // rmt_ScopedCPUSample(RemoveChunkMap, 0);
            auto chunkPos = toChunkPos(pos);
            auto iter2 = g->mActiveComponentsPerChunk.find(chunkPos);

            // 从活跃区块表中中移除
            if (iter2 != g->mActiveComponentsPerChunk.end()) {
                auto &list = iter2->second;
                reinterpret_cast<CircuitComponentList *>(&list)->removeSource(pos, targetComp);
            }
        }

        auto &mSourceList = dAccess<std::vector<ComponentItem>, 8>(targetComp);
        for (auto &src : mSourceList) {
            graph->scheduleRelationshipUpdate(src.mPos, src.mComponent);
        }

        {
            //  rmt_ScopedCPUSample(TraverseAllComp, 0);
            auto typeGroup = (int)targetComp->getCircuitComponentGroupType();
            for (auto &allComp : g->mAllComponents) {
                if (typeGroup == 0x20000) {
                    continue;
                }

                auto allPos = allComp.first;
                auto dis = abs(allPos.x - pos.x) + abs(allPos.y - pos.y) + abs(allPos.z - pos.z);
                if (dis > 16) continue;
                auto updateComponent = allComp.second.get();

                // 不是消费者才尝试移除
                //  updateComponent->removeSource(pos, targetComp);
                auto &ss = dAccess<std::vector<ComponentItem>, 8>(updateComponent);
                auto ssit = ss.begin();
                size_t res = 0;
                while (ssit != ss.end()) {
                    if (ssit->mPos == pos || ssit->mComponent == targetComp) {
                        graph->scheduleRelationshipUpdate(allComp.first, updateComponent);
                        ss.erase(ssit);
                    } else {
                        ++ssit;
                        ++res;
                    }
                }
                if (res != 0) {
                    updateComponent->removeSource(pos, targetComp);
                }
            }
        }

        auto neighbors = getBlockPosNeighbors(pos);
        for (auto &p : neighbors) {
            auto updateComponent = graph->getBaseComponent(p);
            if (updateComponent) {
                graph->scheduleRelationshipUpdate(p, updateComponent);
                auto &sl = dAccess<std::vector<ComponentItem>, 8>(updateComponent);
                for (auto &item : sl) {
                    auto sourcePower = graph->getBaseComponent(item.mPos);
                    graph->scheduleRelationshipUpdate(item.mPos, sourcePower);
                }
            }
        }

        g->mPendingUpdates.erase(pos);
        toRemove.reset(nullptr);
    }

    if (g->mPendingRemoves.empty()) {
        return;
    }
    std::vector<ComponentItem> newActiveList;
    for (auto &item : g->mActiveComponents) {
        if (!activeToRemove.count(item.mPos)) {
            newActiveList.push_back(item);
        }
    }

    g->mActiveComponents = newActiveList;
    g->mPendingRemoves.clear();
}

THook(void, "?update@CircuitSceneGraph@@QEAAXPEAVBlockSource@@@Z", CircuitSceneGraph *self,
      void *bs) {
    optPendingRemoves(self);
    original(self, bs);
}
