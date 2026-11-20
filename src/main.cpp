#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/utils/file.hpp>
#include "gdr2.hpp"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <string>

using namespace geode::prelude;

namespace macrogd {
class Recorder {
public:
    static Recorder& get() {
        static Recorder instance;
        return instance;
    }

    void begin(std::uint64_t currentCommand) {
        m_replay = MacroReplay{};
        m_startCommand = currentCommand;
        m_lastFrame = 0;
        m_recording = true;
    }

    void input(std::uint64_t currentCommand, bool down, int button, bool player2) {
        if (!m_recording || button != 1 || currentCommand < m_startCommand) return;

        const auto frame = currentCommand - m_startCommand;
        m_replay.inputs.emplace_back(frame, 1, player2, down);
        m_lastFrame = std::max(m_lastFrame, frame);
    }

    void tick(std::uint64_t currentCommand) {
        if (!m_recording || currentCommand < m_startCommand) return;
        m_lastFrame = std::max(m_lastFrame, currentCommand - m_startCommand);
    }

    bool isRecording() const { return m_recording; }

    void finish(PlayLayer* layer) {
        if (!m_recording) return;
        m_recording = false;

        m_replay.duration = static_cast<float>(m_lastFrame / m_replay.framerate);
        if (layer && layer->m_level) {
            m_replay.levelInfo.id = 0;
            m_replay.levelInfo.name = std::string(layer->m_level->m_levelName.c_str());
        }

        m_replay.sortInputs();
        save();
    }

private:
    MacroReplay m_replay;
    std::uint64_t m_startCommand = 0;
    std::uint64_t m_lastFrame = 0;
    bool m_recording = false;

    void save() {
        auto folder = Mod::get()->getSaveDir();
        auto directoryResult = utils::file::createDirectoryAll(folder);
        if (directoryResult.isErr()) {
            log::error("MacroGD: cannot create save directory: {}", directoryResult.unwrapErr());
            return;
        }

        auto name = m_replay.levelInfo.name.empty() ? std::string("macro") : m_replay.levelInfo.name;
        for (auto& ch : name) {
            const auto c = static_cast<unsigned char>(ch);
            if (c < 32 || ch == '/' || ch == 92 || ch == ':' || ch == '*' ||
                ch == '?' || ch == '"' || ch == '<' || ch == '>' || ch == '|') {
                ch = '_';
            }
        }

        auto path = folder / (name + ".gdr2");
        for (unsigned int suffix = 2; asp::fs::exists(path); ++suffix) {
            path = folder / (name + "_" + std::to_string(suffix) + ".gdr2");
        }

        auto result = m_replay.exportData();
        if (result.isErr()) {
            log::error("MacroGD: GDR2 serialization failed: {}", result.unwrapErr());
            return;
        }

        auto const& bytes = result.unwrap();
        std::ofstream file(path.string(), std::ios::binary | std::ios::trunc);
        if (!file) {
            log::error("MacroGD: cannot open replay file {}", path.string());
            return;
        }
        file.write(
            reinterpret_cast<char const*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size())
        );
        if (!file) {
            log::error("MacroGD: failed writing replay file {}", path.string());
            return;
        }
        log::info("MacroGD: saved replay to {}", path.string());
    }
};
} // namespace macrogd

class $modify(MacroGDPlayLayer, PlayLayer) {
    void levelComplete() {
        macrogd::Recorder::get().finish(this);
        PlayLayer::levelComplete();
    }
};

class $modify(MacroGDInputLayer, GJBaseGameLayer) {
    void postUpdate(float dt) {
        GJBaseGameLayer::postUpdate(dt);
        if (PlayLayer::get() == this) {
            macrogd::Recorder::get().tick(m_gameState.m_commandIndex);
        }
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        if (PlayLayer::get() == this) {
            macrogd::Recorder::get().input(
                m_gameState.m_commandIndex, down, button, !isPlayer1
            );
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};

class $modify(MacroGDPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto label = CCLabelBMFont::create(
            macrogd::Recorder::get().isRecording() ? "MacroGD: ON" : "MacroGD",
            "bigFont.fnt"
        );
        if (!label) return;

        auto button = CCMenuItemSpriteExtra::create(
            label, this, menu_selector(MacroGDPauseLayer::onMacroButton)
        );
        if (!button) return;

        auto menu = CCMenu::create();
        if (!menu) return;
        menu->addChild(button);
        const auto size = CCDirector::get()->getWinSize();
        menu->setPosition({size.width / 2.f, 42.f});
        menu->setID("macrogd-pause-menu"_spr);
        this->addChild(menu, 100);
    }

    void onMacroButton(CCObject*) {
        auto* playLayer = PlayLayer::get();
        if (!playLayer) {
            FLAlertLayer::create("MacroGD", "No active level found.", "OK")->show();
            return;
        }

        auto& recorder = macrogd::Recorder::get();
        if (recorder.isRecording()) {
            FLAlertLayer::create(
                "MacroGD",
                "Recording is already active. Complete the level to export the replay.",
                "OK"
            )->show();
            return;
        }

        recorder.begin(playLayer->m_gameState.m_commandIndex);
        FLAlertLayer::create(
            "MacroGD",
            "Recording enabled. Press Resume and complete the level. The .gdr2 file will be saved in the MacroGD save folder.",
            "OK"
        )->show();
    }
};
