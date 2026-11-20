#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "gdr2.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
using namespace geode::prelude;

namespace macrogd {
class Recorder {
public:
 static Recorder& get() { static Recorder value; return value; }
 void begin() { m_data = ReplayData{}; m_frame = 0; m_recording = true; }
 void tick() { if (m_recording) ++m_frame; }
 void input(bool down, int button, bool player2) {
  // Classic GDR2 inputs represent the jump button; platformer buttons need a separate encoding.
  if (!m_recording || button != 1) return;
  m_data.inputs.push_back(InputEvent{m_frame, 1, player2, down});
 }
 bool isRecording() const { return m_recording; }
 void finish(PlayLayer* layer) {
  if (!m_recording) return;
  m_recording = false;
  m_data.duration = m_frame;
  if (layer && layer->m_level) {
   m_data.levelID = static_cast<std::uint32_t>(std::max(0, layer->m_level->m_levelID));
   m_data.levelName = std::string(layer->m_level->m_levelName);
  }
  save();
 }
private:
 ReplayData m_data;
 std::uint64_t m_frame = 0;
 bool m_recording = false;
 void save() {
  const auto bytes = Writer::encode(m_data);
  const auto folder = Mod::get()->getSaveDir();
  std::error_code ec;
  std::filesystem::create_directories(folder, ec);
  if (ec) { log::error("MacroGD: cannot create save folder: {}", ec.message()); return; }
  auto name = m_data.levelName.empty() ? std::string("macro") : m_data.levelName;
  for (auto& ch : name) {
   if (ch == '/' || ch == 92 || ch == ':' || ch == '*' || ch == '?' || ch == '"' || ch == '<' || ch == '>' || ch == '|') ch = '_';
  }
  const auto path = folder / (name + ".gdr2");
  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  if (!file) { log::error("MacroGD: cannot open {}", path.string()); return; }
  file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
  if (file.good()) log::info("MacroGD: replay saved to {}", path.string());
  else log::error("MacroGD: writing replay failed");
 }
};
} // namespace macrogd

class $modify(MacroGDPlayLayer, PlayLayer) {
 void levelComplete() { macrogd::Recorder::get().finish(this); PlayLayer::levelComplete(); }
};

class $modify(MacroGDInputLayer, GJBaseGameLayer) {
 void processCommands(float dt, bool isHalfTick, bool isLastTick) {
  GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
  if (PlayLayer::get() == this && isLastTick) macrogd::Recorder::get().tick();
 }
 void handleButton(bool down, int button, bool isPlayer1) {
  if (PlayLayer::get() == this) macrogd::Recorder::get().input(down, button, !isPlayer1);
  GJBaseGameLayer::handleButton(down, button, isPlayer1);
 }
};

class $modify(MacroGDPauseLayer, PauseLayer) {
 void customSetup() {
  PauseLayer::customSetup();
  auto label = CCLabelBMFont::create(macrogd::Recorder::get().isRecording() ? "MacroGD: ON" : "MacroGD", "bigFont.fnt");
  if (!label) return;
  auto button = CCMenuItemSpriteExtra::create(label, this, menu_selector(MacroGDPauseLayer::onMacroButton));
  if (!button) return;
  auto menu = CCMenu::create();
  if (!menu) return;
  menu->addChild(button);
  auto size = CCDirector::get()->getWinSize();
  menu->setPosition({size.width / 2.f, 42.f});
  menu->setID("macrogd-pause-menu"_spr);
  this->addChild(menu, 100);
 }
 void onMacroButton(CCObject*) {
  auto& recorder = macrogd::Recorder::get();
  if (recorder.isRecording()) {
   FLAlertLayer::create("MacroGD", "Recording is already active. It will be exported after the level is completed.", "OK")->show();
   return;
  }
  recorder.begin();
  FLAlertLayer::create("MacroGD", "Recording enabled. Press Resume and complete the level. The .gdr2 file will be saved in the MacroGD save folder.", "OK")->show();
 }
};
