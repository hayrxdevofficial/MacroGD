#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace macrogd {
struct InputEvent { std::uint64_t frame = 0; std::uint8_t button = 1; bool player2 = false; bool down = false; };
struct ReplayData {
 std::string author = "HayrX_Dev", description = "Recorded with MacroGD";
 std::uint64_t duration = 0;
 std::uint32_t gameVersion = 22074, framerate = 240, seed = 0, coins = 0;
 bool lowDetailMode = false, platformer = false;
 std::string botName = "MacroGD";
 std::uint32_t botVersion = 1, levelID = 0;
 std::string levelName;
 std::vector<std::uint64_t> deaths;
 std::vector<InputEvent> inputs;
};
class Writer {
 static void byte(std::string& out, std::uint8_t v) { out.push_back(static_cast<char>(v)); }
 static void varint(std::string& out, std::uint64_t v) {
  while (v >= 0x80) { byte(out, static_cast<std::uint8_t>((v & 0x7f) | 0x80)); v >>= 7; }
  byte(out, static_cast<std::uint8_t>(v));
 }
 static void cstring(std::string& out, std::string const& s) { out.append(s); out.push_back('\0'); }
 static void writeInputs(std::string& out, std::vector<InputEvent> const& inputs) {
  std::uint64_t previous = 0;
  for (auto const& e : inputs) {
   const auto delta = e.frame >= previous ? e.frame - previous : 0;
   const auto packed = (delta << 2) | (e.down ? 2ULL : 0ULL);
   varint(out, packed); previous = e.frame;
  }
 }
public:
 static std::string encode(ReplayData const& r) {
  std::string out; out.append("GDR", 3); varint(out, 1); cstring(out, "");
  cstring(out, r.author); cstring(out, r.description);
  varint(out, r.duration); varint(out, r.gameVersion); varint(out, r.framerate);
  varint(out, r.seed); varint(out, r.coins); byte(out, r.lowDetailMode ? 1 : 0);
  byte(out, r.platformer ? 1 : 0); cstring(out, r.botName); varint(out, r.botVersion);
  varint(out, r.levelID); cstring(out, r.levelName); varint(out, 0);
  varint(out, r.deaths.size()); std::uint64_t prevDeath = 0;
  for (auto f : r.deaths) { varint(out, f - prevDeath); prevDeath = f; }
  std::vector<InputEvent> p1, p2;
  for (auto const& e : r.inputs) (e.player2 ? p2 : p1).push_back(e);
  varint(out, p1.size() + p2.size()); varint(out, p1.size());
  writeInputs(out, p1); writeInputs(out, p2); return out;
 }
};
}