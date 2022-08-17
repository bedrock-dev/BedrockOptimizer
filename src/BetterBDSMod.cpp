//
// Created by xhy on 2022/7/14.
//

#include "BetterBDSMod.h"

#include <filesystem>

#include "Nlohmann/json.hpp"
namespace trapdoor {

    namespace {
        void tryCreateConfig() {
            namespace fs = std::filesystem;
            if (!fs::exists("./plugins/beopt")) {
                trapdoor::logger().warn(
                    "Can not find config file, Automatically generating configuration files");
                fs::create_directory("./plugins/beopt");
                const std::string cfg = R"(
{
  "random-tick": true,
  "actor-push": true,
  "component-remove": true,
  "hopper": false
}
)";
                std::ofstream f("./plugins/beopt/config.json");
                f << cfg;
                f.close();
            }
        }
    }  // namespace

    Logger& logger() {
        static Logger logger("BedrockOptimizer");
        return logger;
    }

    void init() {
        logger().consoleLevel = 8;
        registerOptCommand(1);
        tryCreateConfig();
        trapdoor::mod().readConfig();
    }

    BetterBDSMod& mod() {
        static BetterBDSMod mod;
        return mod;
    }

    void BetterBDSMod::readConfig() {
        nlohmann::json config;
        const std::string path = "./plugins/beopt/config.json";
        try {
            std::ifstream i(path);
            i >> config;
            this->randomTick = config["random-tick"].get<bool>();
            this->actorPush = config["actor-push"].get<bool>();
            this->hopper = config["hopper"].get<bool>();
            this->componentRemove = config["component-remove"].get<bool>();

            trapdoor::logger().info("Read getConfig file {} successfully", path);
        } catch (std::exception&) {
            trapdoor::logger().error(
                "Can't read configuration file: {},Opts will be disable, use [opt] command to "
                "enable them",
                path);
        }
        this->dumpConfig();
    }

    void BetterBDSMod::dumpConfig() const {
        trapdoor::logger().info("[Random tick]: {}", this->randomTick);
        trapdoor::logger().info("[Actor push]: {}", this->actorPush);
        trapdoor::logger().info("[Component Remove]: {}", this->componentRemove);
        trapdoor::logger().info("[Hopper]: {}", this->hopper);
    }
}  // namespace trapdoor
