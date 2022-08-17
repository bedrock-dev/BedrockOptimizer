//
// Created by xhy on 2022/7/14.
//

#include "BetterBDSMod.h"
#include "DynamicCommandAPI.h"

namespace trapdoor {

    void setValue(const std::string &name, bool &item, bool val, CommandOutput &output) {
        // trapdoor::logger().debug("QAQ");
        item = val;
        auto msg = "Set " + name + " to " + std::to_string(item);
        output.success(msg);
    }

    void queryValue(const std::string &name, bool item, CommandOutput &output) {
        auto msg = name + " is set  to " + std::to_string(item);
        output.success(msg);
    }

    void registerOptCommand(int level) {
        //        using ParamType = DynamicCommand::ParameterType;
        //        auto command = DynamicCommand::createCommand("opt", "opt options",
        //                                                     static_cast<CommandPermissionLevel>(level));
        //
        //        auto &redstoneEnum = command->setEnum("redstone", {"compremove"});
        //        command->mandatory("opt", ParamType::Enum, redstoneEnum,
        //                           CommandParameterOption::EnumAutocompleteExpansion);
        //
        //        auto &actorEnum = command->setEnum("actor", {"actorpush"});
        //        command->mandatory("opt", ParamType::Enum, actorEnum,
        //                           CommandParameterOption::EnumAutocompleteExpansion);
        //
        //        auto &hopperEnum = command->setEnum("hopper", {"hopper"});
        //        command->mandatory("opt", ParamType::Enum, hopperEnum,
        //                           CommandParameterOption::EnumAutocompleteExpansion);
        //
        //        auto &randomTickEnum = command->setEnum("randomtick", {"randomtick"});
        //        command->mandatory("opt", ParamType::Enum, randomTickEnum,
        //                           CommandParameterOption::EnumAutocompleteExpansion);
        //
        //        command->optional("onoroff", ParamType::Bool);
        //        command->addOverload({redstoneEnum, "onoroff"});
        //        command->addOverload({actorEnum, "onoroff"});
        //        command->addOverload({hopperEnum, "onoroff"});
        //        command->addOverload({randomTickEnum, "onoroff"});
        //
        //        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
        //                     CommandOutput &output,
        //                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
        //            bool query = !results["onoroff"].isSet;
        //            auto opt = !query && results["onoroff"].get<bool>();
        //            switch (do_hash(results["opt"].getRaw<std::string>().c_str())) {
        //                case do_hash("compremove"):
        //                    if (query) {
        //                        queryValue("componentRemove", trapdoor::mod().componentRemove,
        //                        output);
        //                    } else {
        //                        setValue("componentRemove", trapdoor::mod().componentRemove, opt,
        //                        output);
        //                    }
        //                    break;
        //                case do_hash("actorpush"):
        //                    if (query) {
        //                        queryValue("actorpush", trapdoor::mod().actorPush, output);
        //                    } else {
        //                        setValue("actorpush", trapdoor::mod().actorPush, opt, output);
        //                    }
        //                    break;
        //                case do_hash("hopper"):
        //                    if (query) {
        //                        queryValue("hopper", trapdoor::mod().hopper, output);
        //                    } else {
        //                        setValue("hopper", trapdoor::mod().hopper, opt, output);
        //                    }
        //                    break;
        //                case do_hash("randomtick"):
        //                    if (query) {
        //                        queryValue("randomtick", trapdoor::mod().randomTick, output);
        //                    } else {
        //                        setValue("randomtick", trapdoor::mod().randomTick, opt, output);
        //                    }
        //                    break;
        //            }
        //        };
        //
        //        command->setCallback(cb);
        //        DynamicCommand::setup(std::move(command));
    }
    // namespace tr
}  // namespace trapdoor