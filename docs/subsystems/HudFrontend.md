# HUD, text and frontend menus

The 2D side of the game: the in-game HUD (`CHud`), the radar as drawn on screen (`CRadar`), text rendering
(`CFont`), the GXT text tables (`CText`), queued on-screen messages (`CMessages`), the pause/front-end menu
(`CMenuManager`, the global `FrontEndMenuManager`) and the 2D sprite helpers (`CSprite2d`, `CSprite`). The 3D side of
the frame and where RenderWare takes over is in [Rendering.md](Rendering.md); the save/load code that the menu calls is
in [SaveLoad.md](SaveLoad.md).

All paths are relative to the repo root. Addresses are the ones written in the repo (`RH_*Install` or `// 0xADDR`) and
were checked against the IDB.

---

## 1. Overview

```
Idle (0x53E920, app_game.cpp)                         FrontendIdle (0x53E770, app_game.cpp)
  CSprite2d::InitPerFrame, CFont::InitPerFrame          CSprite2d::InitPerFrame, CFont::InitPerFrame
  CGame::Process -> CWorld -> CMessages::Process        FrontEndMenuManager.Process  (input, file actions)
  ...3D...                                              DrawFrontEnd (if m_bMenuActive)
  Render2dStuff (0x53E230)   <- HUD                     SecuromStateDisplay, DoFade
  RenderMenus   (0x53E530)   <- pause menu              CHud::DrawAfterFade, CMessages::Display(false)
  DoFade                                                CFont::DrawFonts
  CHud::DrawAfterFade        <- area name, titles
  CMessages::Display(false)  <- subtitles after fade
  CFont::DrawFonts           <- flush text buffer
  CCredits::Render, ImGui (NOTSA)
```

Text is never drawn immediately. `CFont::PrintString` parses the string and appends characters to a render buffer;
`CFont::DrawFonts` (== `RenderFontBuffer`) draws the buffer. Strings come from GXT tables (`TheText.Get("KEY")`), may
contain `~x~` tokens (colour, newline, `~1~` numbers, `~k~~ACTION~` key names) and are 8-bit `GxtChar`
(`source/game_sa/GxtChar.h`).

---

## 2. Key classes and files

| Class / file | Role |
|---|---|
| `CHud` (`source/game_sa/Hud.h/.cpp`) | All in-game HUD drawing: player info (money, clock, weapon, health/armour/breath), wanted stars, radar frame, vital stats, vehicle/zone names, help box, big messages, mission timers, subtitles. Static only. **Its `InjectHooks` is not called** (section 6) |
| `CHudColours` (`HudColours.h/.cpp`) | Palette, global `HudColour` (`0xBAB22C`). `GetRGB` `0x58FEA0`, `GetRGBA` `0x58FEE0` |
| `eHud.h` (`Enums/eHud.h`) | `eMessageStyle` (7 big-message styles), `eHudItem` (flashing items) |
| `CRadar` (`Radar.h/.cpp`) | Radar map tiles, mask, blips, legend, 3D markers, front-end map. This page covers drawing; blip creation from scripts is in [Scripts.md](Scripts.md) |
| `CFont` (`Font.h/.cpp`) | Text styles (global state at `0xC71A54..0xC71A9C`), string width/line count, token parsing, the font render buffer |
| `CText` (`Text/Text.h/.cpp`), `CKeyArray`, `CData`, `CMissionTextOffsets` (`Text/`) | GXT loading. Global `TheText` (`0xC1B340`, size `0xA90`) |
| `CMessages` (`Messages.h/.cpp`) | Brief (subtitle) queue, big-message queues, previous-brief history, number/string/key insertion |
| `CMenuManager` (`MenuManager.h/.cpp`, `Frontend/MenuManager_*.cpp`) | Pause/front-end menu. Global `FrontEndMenuManager` (`0xBA6748`, size `0x1B78`). Split by topic: `_Draw`, `_Input`, `_Process`, `_Pages` (map, stats, briefs, radio list) |
| `MenuManager_Internal.h` | `eMenuScreen` (44 pages), `eMenuAction`, `tMenuScreen` (`0xE2`), the page table `aScreens` |
| `CMenuSystem` (`MenuSystem.h/.cpp`) | Script-created menus (shops, garages). Drawn from `CHud::Draw` |
| `CSprite2d` (`Sprite2d.h/.cpp`) | A texture plus static Im2D helpers: `Draw` overloads, `DrawRect` (`0x727B60`), `DrawBarChart` (`0x728640`), vertex buffer for batched quads |
| `CSprite` (`Sprite.h/.cpp`) | Screen-projected sprites (`CalcScreenCoors` `0x70CE30`, `RenderOneXLUSprite*`, buffered XLU sprites). Used by coronas and by the font buffer (`CSprite::InitSpriteBuffer`, `FlushSpriteBuffer`) |
| `CLoadingScreen` (`LoadingScreen.h/.cpp`) | Splash screens and loading bar (not per-frame) |
| `CDraw` (`Draw.h/.cpp`) | `CalculateAspectRatio` (`0x6FF420`), `DoFade` (`0x53E600`) |

---

## 3. Per-frame flow

### `Render2dStuff` (`0x53E230`, hook `.Locked = true`)

Sets 2D render states (no Z test/write, vertex alpha, src-alpha blending, no cull), then:
`CReplay::Display`, `CPickups::RenderPickUpText`, widescreen borders (`TheCamera.m_bWideScreenOn &&
!FrontEndMenuManager.m_bWidescreenOn`), the sniper/rocket zoom box, `AudioEngine.DisplayRadioStationName`,
**`CHud::Draw`**, `CSpecialFX::Render2DFXs`, `CUserDisplay::OnscnTimer.ProcessForDisplay`, **`CMessages::Display(true)`**,
`CDarkel::DrawMessages`, `CGarages::PrintMessages`, **`CFont::DrawFonts`**. `RenderDebugShit()` at the top is NOTSA.

### `CHud::Draw` (`0x58FAE0`)

Returns early in replay playback, while taking a photo, when the menu opens next frame, or on a cinematic-camera frame.
Then (every call below is skipped while `TheCamera.m_bWideScreenOn`):

1. `DrawCrossHairs` (STUB); if `FrontEndMenuManager.m_bHudOn && CTheScripts::bDisplayHud`: `DrawPlayerInfo` (STUB,
   4,065 bytes: money, clock, weapon icon/ammo, health/armour/breath bars), `DrawWanted` (STUB).
2. `DrawVehicleName` unless the script hid it, `DrawMissionTimers` (STUB).
3. If the radar is not hidden by script: `DrawRadar` (`0x58A330`) or, when the pad asks for vital stats on foot,
   `DrawVitalStats` (STUB). Then the trip-skip icon and the `SKIP_1` help text.
4. `CRadar::Draw3dMarkers` when `m_bDraw3dMarkers`.
5. If not user-paused: script menus (`CMenuSystem::Process`) and `DrawScriptText(true)` unless a `STYLE_MIDDLE` big
   message is up, subtitles if `CTheScripts::bDrawSubtitlesBeforeFade`, `DrawHelpText` (STUB), `DrawOddJobMessage(true)`,
   `DrawSuccessFailedMessage` (STUB), `DrawBustedWastedMessage`.

`CHud::DrawRadar` -> `CRadar::DrawMap` (picks the radar range from speed or plane altitude, sets
`vec2DRadarOrigin`, then `DrawRadarMap` = `DrawRadarMask` + a 3x3 block of `DrawRadarSection`) -> plane attitude ring,
altimeter, the four `radardisc` quarters -> `CRadar::DrawBlips` (which calls `DrawCoordBlip` and the
STUB `DrawEntityBlip`).

### After the fade

`CHud::DrawAfterFade` (`0x58D490`, matches the IDB): `DrawAreaName` (not in a plane/heli, cutscene,
menu, widescreen, or when the script hid it), `DrawScriptText(false)`, subtitles if they are drawn after the fade,
`DrawMissionTitle`, `DrawOddJobMessage(false)`. Then `CMessages::Display(false)` and a second `CFont::DrawFonts`.

### Menus

- `RenderMenus` (`0x53E530`): `DrawFrontEnd` if `m_bMenuActive`.
- `CMenuManager::Process` (`Frontend/MenuManager_Process.cpp`, `0x57B440`): when active, `ProcessStreaming` (map
  tiles), `UserInput`, `ProcessFileActions` (load/delete/save, section 5), D3D resource tidy-up; always
  `CheckForMenuClosing` (`0x576B70`, `.Locked = true`: it also drives the ImGui/SDL input path).
- `DrawFrontEnd` (`Frontend/MenuManager_Draw.cpp`, `0x57C290`) -> `DrawBackground` (`0x57B750`):
  background sprite for the page, `PrintMap` on `SCREEN_MAP`, then `DrawControllerSetupScreen` /
  `DrawQuitGameScreen` / **`DrawStandardMenus`** (`0x5794A0`), controller error boxes, the user-track scan
  screen, and the mouse cursor.

---

## 4. Text pipeline

### `CText` / GXT (`Text/Text.cpp`)

- `CText::Load(bool keepMissionPack)` (`0x6A01A0`): opens `TEXT\<LANG>.GXT` (`GetGxtName`, NOTSA helper), checks the 8-bit encoding (`CheckFileEncoding`, NOTSA), reads chunks: `TABL` -> mission table offsets,
  `TKEY` -> `m_MainKeyArray`, `TDAT` -> `m_MainText`, then rebases key offsets (`CKeyArray::Update`). Caches the
  `CDERROR` text. Called from `CGame::InitialiseEssentialsAfterRW`, `WinPs.cpp`, `CMenuManager::LoadSettings`,
  `CMenuManager::InitialiseChangedLanguageSettings` (language change), `CTheScripts::Init` and after a save is loaded.
- `LoadMissionText(name)` (`0x69FBF0`): finds the mission's table in `m_MissionTextOffsets` and loads its own
  TKEY/TDAT into `m_MissionKeyArray`/`m_MissionText`. `LoadMissionPackText` (`0x69F9A0`) does the same from
  `MPACK//MPACK%d//TEXT.GXT` in the user folder.
- `CText::Get(key)` (`0x6A0050`): binary search (`CKeyArray::BinarySearch` `0x69F570`) on the uppercase key
  hash in the main table, then in the mission table when one is loaded. Not found: returns `GxtErrorString`
  (`0xC1AEB8`), which the original fills with an **empty** string (`sprintf(buf, "")`), not the key name.

### `CMessages` (`Messages.cpp`)

| Store | Address | Shape |
|---|---|---|
| `BriefMessages` | `0xC1A7F0` | 8 `tMessage` (subtitles); `[0]` is the one on screen |
| `BIGMessages` | `0xC1A970` | 7 styles x 4 `tMessage` |
| `PreviousBriefs` | `0xC1A570` | 20 entries, shown on the menu's Brief page (`PrintBriefs` `0x576320`) |

- `AddMessage*` / `AddBigMessage*`: the plain versions show the text at once (slot 0), the `Q` versions queue it in the
  first free slot. The repo funnels them through NOTSA helpers `AddMessage2` / `AddBigMessage2`.
- `Process` (`0x69EE60`, called from `CWorld::Process`): when slot 0 expires, the queue shifts down and the
  new head's start time is reset. A new brief head with `m_bPreviousBrief` is added to `PreviousBriefs`.
- `Display(bool notFading)` (`0x69EFC0`): for each big-message style and for brief `[0]`, builds the final
  string (`InsertNumberInString` -> `InsertStringInString` -> `InsertPlayerControlKeysInString`) and hands it to
  `CHud::SetBigMessage` / `CHud::SetMessage`. `CHud` then draws those buffers.
- Number insertion replaces each `~1~` with the next of up to 6 numbers (`-1` = unused). Key insertion replaces
  `~k~~ACTION_NAME~` with the bound key (`ControlsManager.GetGxtStringOfCommandKeys`). Buffers are 400 `GxtChar`
  (`MSG_BUF_SZ`).

### `CFont` (`Font.cpp`)

- Style setters write the global state (`m_Scale`, `m_Color`, `m_bFontCentreAlign`, `m_fWrapx`, ...,
  `StaticRef`s in `Font.h`): `SetScale` `0x719380`, `SetColor` `0x719430`, `SetFontStyle` `0x719490`, `SetOrientation`
  `0x719610`, `SetEdge`/`SetDropShadowPosition`, `SetBackground`, `SetProportional`, `SetWrapx`, `SetCentreSize`,
  `SetRightJustifyWrap`. Callers set everything they need before each print; state leaks between callers.
- Four styles (`eFontStyle`): `FONT_GOTHIC`, `FONT_SUBTITLES`, `FONT_MENU`, `FONT_PRICEDOWN`. Two font
  textures plus 15 PS2 button sprites (`Sprite` `0xC71AD0`, `ButtonSprite` `0xC71AD8`). Character widths come from
  `DATA\FONTS.DAT` (`LoadFontValues` `0x7187C0`) into `gFontData` (`0xC718B0`).
- `PrintString(x, y, text)` (`0x71A700`): optional background box (`GetTextRect` + `DrawRect` or
  `CMenuManager::DrawWindow`), then `ProcessStringToDisplay` (`0x71A600`) -> `ProcessCurrentString(true, ...)`
  (`0x71A220`, **STUB**; IDB name `GetNumberLines(bool,...)`). That stub word-wraps and calls the inner
  `CFont::PrintString(float,float,uint,ushort*,ushort*,float)` at `0x719B40` (**MISSING**), which copies characters and
  a `CFontChar` state record into the buffer at `0xC716B0` and flushes it with `RenderFontBuffer` when full.
- `RenderFontBuffer` (`0x719840`, **STUB**) walks the buffer, applies tokens (`ParseToken` `0x718F00`), maps characters
  (`FindSubFontCharacter` `0x7192C0`, IDB `GetIDforPropVal`) and draws each with `PrintChar` (`0x718A10`, **STUB**),
  then `CSprite::FlushSpriteBuffer` and `CSprite2d::RenderVertexBuffer`. `DrawFonts` (`0x71A210`) just calls it.
- `GetStringWidth` (`0x71A0E0`), `GetNumberLines` (`0x71A5E0` = `ProcessCurrentString(false, ...)`),
  `PrintStringFromBottom` (`0x71A820`), `GetCharacterSize` (`0x719750`) are reversed.

So everything above the buffer is ours, while the code that actually lays out and draws glyphs is still the original.

---

## 5. Front-end menu

- **Pages.** `eMenuScreen` (`MenuManager_Internal.h`): 44 pages (`SCREEN_STATS` .. `SCREEN_EMPTY`). Each
  `tMenuScreen` has a GXT title, a parent page, a start entry and 12 `tMenuScreenItem`s (action, GXT key, entry type,
  target page, x/y, alignment). The table the code uses is the repo's own copy `aScreens` (`MenuManager_Internal.h`,
  "From 0x8CE008"), not a `StaticRef` to the original. `Frontend/FrontendScreensPC.h` holds a second copy,
  `aScreensX`, included by `MenuManager_Internal.cpp` and not used anywhere.
- **Navigation.** `SwitchToNewScreen` (`0x573680`), `JumpToGenericMessageScreen` (`0x576AE0`, title/text GXT keys),
  `ProcessMenuOptions` (`0x576FE0`) and `ProcessPCMenuOptions` (`0x57CD50`) handle actions; `UserInput` (`0x57FD70`)
  and `AdditionalOptionInput` (`0x5773D0`) read the pad/mouse; `CheckSliderMovement` (`0x573440`) moves sliders.
- **Settings.** `LoadSettings` (`0x57C8F0`) / `SaveSettings` (`0x57C660`) read and
  write `gta_sa.set` in the user folder; version constant `SETTINGS_FILE_VERSION = 6` (`MenuManager.h`); controls
  are stored by `ControlsManager.LoadSettings`. `SetDefaultPreferences(screen)` (`0x573AE0`) resets one page.
  `SaveStatsToFile` (`0x57DDE0`) writes `stats.html`.
- **Save slots.** Entry types `FIRST_SAVE_SLOT..MAX_SAVE_SLOT` (`IsSaveSlot`, NOTSA). `DrawStandardMenus` shows
  `GetNameOfSavedGame` / `GetSavedGameDateAndTime` per slot state, filled by
  `C_PcSave::PopulateSlotInfo`. `ProcessFileActions` (`0x578D60`) runs the action on the
  frame after the confirmation page is shown: `SCREEN_LOAD_FIRST_SAVE` -> `CGenericGameStorage::CheckSlotDataValid`
  and `m_bLoadingData = true`; `SCREEN_DELETE_FINISHED` -> `C_PcSave::DeleteSlot`; `SCREEN_SAVE_DONE_1` ->
  `C_PcSave::SaveSlot` (non-zero = failed). Details in [SaveLoad.md](SaveLoad.md).
- **NOTSA.** `CMenuManager::SimulateGameLoad(newGame, slot)` is used by the fast loader
  (`extensions/Configs/FastLoader.hpp`) to auto-load a save. `DrawBuildInfo` is drawn when `USE_BUILD_INFORMATION` is
  defined.

---

## 6. Why `CHud` is special (hooks not registered)

`CHud::InjectHooks()` exists but `InjectHooksMain()` deliberately doesn't call it, because it hasn't been tested in
game yet. `CHud::InjectHooks` registers 38 functions:
30 reversed and 8 `.Reversed = false` stubs. Because it is never called:

- **No hook exists for any `CHud` function.** Nothing is patched at the original addresses and nothing appears under
  `CHud` in the F7 Hooks window, so you cannot switch any of it between ours and GTA.
- **Calls coming from original code run the original.** For example, the original script-command dispatchers
  (`ProcessCommands*`) call the original `SetHelpMessage`, and the original `DrawPlayerInfo` calls the original
  `RenderHealthBar`, `DrawAmmo`, etc.
- **Calls coming from our C++ run our C++.** This matters more than the comment suggests. `Render2dStuff` (ours,
  locked) calls `CHud::Draw`, and `Idle`/`FrontendIdle` call `CHud::DrawAfterFade`. So our `Draw`, `DrawAfterFade`,
  `DrawRadar`, `DrawVehicleName`, `DrawAreaName`, `DrawScriptText`, `DrawMissionTitle`, `DrawOddJobMessage`,
  `DrawBustedWastedMessage` and `DrawTripSkip` run **every frame** today. The same goes for `Initialise` /
  `ReInitialise` / `Shutdown` (from `Game.cpp`), `SetMessage` / `SetBigMessage` (from `CMessages::Display`) and the
  roughly 40 `SetHelpMessage*` / `SetZoneName` / `SetVehicleName` calls in reversed code.
- **Code that really never runs:** functions reached only from original callers. These are `RenderHealthBar`,
  `RenderArmorBar`, `RenderBreathBar`, `DrawAmmo` and `DrawWeaponIcon` (only the original `DrawPlayerInfo` calls them),
  `DrawFadeState`, and the inline `DrawClock` / `DrawMoney` / `DrawWeapon` helpers written for the future
  `DrawPlayerInfo`.
- The 8 stubs still work, because nothing is patched: `plugin::Call<0x58EAF0>()` just calls the original.
- `idb_coverage.py` reports the 30 as DORMANT, not REVERSED.

**Testing `CHud` before registering it:**

1. Locally add `CHud::InjectHooks();` to `InjectHooksMain` (don't commit it yet). `InjectHooksMain` runs under the loader
   lock; an exception thrown during registration freezes the game at the console (R11 in
   [../ReversingRules.md](../ReversingRules.md)), so make sure the game starts and check `logs/log.log`.
2. The direct calls from `Render2dStuff`/`Idle` bypass the hook. Only the patch at the GTA address follows the toggle.
   To A/B `CHud::Draw` itself in F7, switch it to GTA: in that state our function body is patched to jump to the
   original, so the direct call follows too (`RedirectToGTA`, see "How the project works" in [../ReversingGuide.md](../ReversingGuide.md)).
   Do the same for each `Draw*` function, one at a time, and compare screenshots at the same spot. Useful cases: on
   foot, in a car (vehicle name), in a plane and in the Vortex (radar ring, altimeter), with a wanted level, a mission
   with timers and subtitles, the trip-skip icon, busted/wasted, a zone change, and the widescreen menu option on and off.
3. Run `python .claude/skills/validate-against-binary/scripts/asm_validate.py CHud::<Fn>` for each of the 30, and
   `check_hooks.py source/game_sa/Hud.cpp`.
4. Small pure helpers (`GetYPosBasedOnHealth`) can get a `NOTSA_DIFFTEST_INGAME` (it reads `CPlayerInfo`). Draw
   functions can't be diff-tested meaningfully; compare them visually.
5. Only then move the call into `InjectHooksMain` and remove `CHud` from the comment.

---

## 7. Widescreen and NOTSA display options

- `FrontEndMenuManager.m_bWidescreenOn` is the original "Widescreen" display option
  (`MENU_ACTION_WIDESCREEN`, GXT `FED_WIS`, toggled in `CMenuManager::ProcessMenuOptions`). `CDraw::CalculateAspectRatio`
  sets 16:9 for it, 5:4 for the cutscene letterbox (`TheCamera.m_bWideScreenOn`), otherwise 4:3.
  `Render2dStuff` skips the letterbox borders when the option is on.
- `common.h`: `SCREEN_STRETCH_X/Y` is the original scaling (`RsGlobal.maximumWidth * 0.0015625 * x`, 640x448 base).
  `SCREEN_SCALE_X` additionally applies `SCREEN_SCALE_AR` (`* (4/3) / CDraw::ms_fAspectRatio`) because
  `ASPECT_RATIO_SCALE` is defined. That is a NOTSA aspect fix: any code that uses `SCREEN_SCALE_X` /
  `SCREEN_SCALE_FROM_RIGHT` differs from the original whenever the ratio isn't 4:3. Today that's `CHud::DrawScriptText`,
  `CHud::DrawMissionTitle`, `CMenuManager::DrawStandardMenus`, `DrawWindow`, `SmallMessageScreen`, `CDarkel::DrawMessages`,
  `CReplay::Display` and `CAERadioTrackManager::DisplayRadioStationName`. `SCREEN_SCALE_Y` equals `SCREEN_STRETCH_Y`.
- `CMenuManager::StretchX/StretchY` (`0x5733E0`/`0x573410`) are the menu's own stretch helpers. Use them in menu code.
- Always-on fixes marked `FIX_BUGS` (`#ifdef FIX_BUGS` or a `// FIX_BUGS` comment; the macro is defined
  unconditionally in `source/CMakeLists.txt`, so these are **not** switchable). In this area: `CRadar::SetupAirstripBlips`
  (frame-rate independent blip flash), `CRadar::DrawBlips` (second player blip), `CFont::Shutdown` (safe TXD slot
  removal), `CSprite2d::DrawBarChart` (bar chart overflow), several in the menu files. Switchable fixes go through
  `notsa::bugfixes` (`CMenuManager::AdditionalOptionInput` uses `GenericFrameRate`); see R6 in
  [../ReversingRules.md](../ReversingRules.md).

---

## 8. Gotchas

- **`CHud` runs partly ours, partly original, and you can't toggle it** (section 6). A bug in a DORMANT `Draw*`
  function is live, even though the coverage tool says the class is 0% reversed.
- **Font state is global and sticky.** Set every attribute the original sets, in the same order, even redundant ones.
  The next caller inherits whatever you leave (scale, alignment, background, wrap, drop colour).
- **`CFont::PrintString` doesn't draw.** The text appears at the next `CFont::DrawFonts` (two per frame in `Idle`), or
  when the buffer fills up. Code that changes render states or draws sprites after printing must flush first. Several
  menu paths call `CFont::RenderFontBuffer()` directly for this reason (`CMenuManager::DrawBackground`,
  `MessageScreen`).
- **`CRect` field names vs order.** In memory it's `left, top, right, bottom`, but the 4-float constructor is
  `CRect(left, bottom, right, top)` (`Core/Rect.h`), and SA's "top" is the smaller y. HUD code often fills a quad
  "upside down" to flip a sprite (the four radar-disc quarters in `CHud::DrawRadar`). Compare fields with the asm, not
  with the names.
- **Use `SCREEN_STRETCH_*` to match the original.** `SCREEN_SCALE_X` is aspect-corrected (section 7).
- **Original quirks to keep:** `CText::Get` returns an empty string for missing keys; `CHud::SetHelpMessage` clears
  `m_pHelpMessageToPrint` *before* comparing against it (same in `0x588BE0`); `SetBigMessage`
  only accepts a message when `BigMessageX[style] == 0`.
- **Two copies of the menu table.** `aScreens` is a repo-owned array. Original code that reads `0x8CE008` sees the
  original table. Fine while every reader is reversed, but edits to `aScreens` won't reach original code.
- **`CHud` statics without `StaticRef`.** `DrawBustedWastedMessage` uses a static Y position at `0xBAB220`
  (`docs/ai/Globals.md`, "Not yet in the repository"). The repo computes it inline. `DrawSuccessFailedMessage` has
  one at `0xBAB218`. Keep this in mind when you reverse those stubs.
- **`m_nFontOutline` and `m_nFontOutlineOrShadow` are both `0xC71A9C`** (`Font.h`): two names for one byte.
- **`.Reversed = false` with a full body.** `CFont::FindSubFontCharacter` (`0x7192C0`) is implemented
  and matches the IDB, but the hook is off. In `RedirectToGTA` state our body jumps to the original, so even
  `GetCharacterSize`'s direct call runs GTA's code. Flip it once validated.
- **`CHud::DrawCrossHairs`** contains two `plugin::Call<0x58E020>()` lines. The second ("for test
  purposes") is dead code after the `return`.
- **IDA names differ from repo names** for the font internals: `0x71A220` is `GetNumberLines(bool,...)` in the IDB
  and `ProcessCurrentString` here; `0x71A5E0` is `GetNumberLinesNoPrint` in the IDB and `GetNumberLines` here;
  `0x7192C0` is `GetIDforPropVal` in the IDB.

### Bugs found while writing this page (not fixed)

| Where | Original | Problem |
|---|---|---|
| `CMessages::Process` (expired-head shift) | `0x69EE60` | Uses `std::shift_right` to drop the expired head. The original shifts **left** (`[i] = [i+1]`), as `ClearThisPrint_Impl` does. With `shift_right` the destroyed head stays in slot 0, so the queue stalls, and the last 1-2 entries are dropped. Affects queued briefs (`AddMessageQ`, `...JumpQ...`) and queued big messages |
| `CMessages::Process` (previous briefs) | `0x69EE60` | Calls `AddToPreviousBriefArray` for the brief head **every frame** and ignores `PreviousBrief`. The original calls it once, only after a shift, and only if `m_bPreviousBrief`. Briefs that should stay out of the Brief page get added |
| `CText::LoadMissionPackText` (chunk loop) | `0x69F9A0` | `while (!bKey && !bText)` stops after the first chunk; the original loops until both TKEY and TDAT are read. It also doesn't skip zero-size chunks |
| `CText::LoadMissionPackText` (end) | `0x69F9A0` | `m_MainKeyArray.Update(m_MissionText.m_data)` rebases the **main** key array onto the mission text. The original rebases `m_MissionKeyArray`. Breaks all text in mission-pack games |
| `CHud::DrawRadar` altimeter (runs every frame) | `0x58A330` | Altimeter condition `IsSubPlane() \|\| IsSubHeli() && model != VORTEX` binds the Vortex check to helis only. The original excludes the Vortex for planes and helis, so ours draws an altimeter in the Vortex |
| `CHud::DrawRadar` plane ring | `0x58A330` | The plane ring goes through `CRadar::DrawRotatingRadarSprite` with half-sizes 78x59 (and that function's `Limit(x, y)`). The original inlines the rotation with 76x58 (`94-18`, `76-18`) and no clamping. The vertex order works out the same (the extra `PI` swaps the corners back), but the ring is 2 px larger |

---

## 9. Reversing status and what is left

From `python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --class <Class>` (cache
`.claude/cache/idb_export.json`, checked 2026-10-14):

| Class | Funcs | Reversed | Dormant | Stub | Known | Missing |
|---|---|---|---|---|---|---|
| `CHud` | 38 | 0 | 30 | 8 | 0 | 0 |
| `CRadar` | 64 | 59 | 0 | 2 | 1 | 2 |
| `CFont` | 37 | 30 | 0 | 3 | 0 | 4 |
| `CText` / `CKeyArray` / `CData` / `CMissionTextOffsets` | 10 / 5 / 2 / 1 | 9 / 4 / 2 / 1 | 0 | 0 | 1 / 1 / 0 / 0 | 0 |
| `CMessages` | 28 | 28 | 0 | 0 | 0 | 0 |
| `CMenuManager` | 61 | 61 | 0 | 0 | 0 | 0 |
| `CMenuSystem` | 28 | 20 | 0 | 1 | 6 | 1 |
| `CSprite2d` | 37 | 33 | 0 | 0 | 3 | 1 |
| `CSprite` | 23 | 15 | 0 | 0 | 5 | 3 |
| `CHudColours` | 6 | 5 | 0 | 0 | 0 | 1 |
| `CLoadingScreen` | 19 | 18 | 0 | 0 | 0 | 1 |

`docs/ai/FrameCallGraph.md` shows `CMenuManager::Process` with 145 unreversed functions / 42,751 bytes below it and
`CFont::PrintString` with 12 / 5,013. Almost all of that is outside these classes (the font internals below, the
frontend audio, D3D tidy-up, save storage).

Prioritised work:

1. **Validate and register `CHud`** (section 6). 30 functions / about 12.4 KB of DORMANT code, most of it already live
   through direct calls. Fix the `DrawRadar` items in section 8 first.
2. **Fix the `CMessages::Process` and `CText::LoadMissionPackText` bugs** above. Both are small, live, and easy to
   check against the IDB.
3. **The font back end** (about 4.4 KB, on every text draw): `ProcessCurrentString` `0x71A220` (953 B, stub),
   inner `PrintString` `0x719B40` (1,440 B, missing), `RenderFontBuffer` `0x719840` (767 B, stub), `PrintChar`
   `0x718A10` (1,078 B, stub; the body after the `return` in `CFont::PrintChar` is a draft with wrong logic, e.g.
   `if (!character || character == '?')`), plus `GetNextSpace` `0x718ED0` (35), `FilterOutTokensFromString`
   `0x719240` (115), `character_code` `0x719360` (19, uses `l10nTransTable` `0x8D5AD8`). Do them together: they share
   the buffer format at `0xC716B0` (512 bytes up to `gFontData`), which the repo types as `std::array<CFontChar, 9>`.
   The real buffer interleaves `CFontChar` records with text.
4. **`CHud` stubs**, largest first (17.1 KB total): `DrawPlayerInfo` `0x58EAF0` (4,065; the repo already has
   `DrawClock` / `DrawMoney` / `DrawWeapon` inline helpers in `Hud.cpp`), `DrawVitalStats` `0x589650` (2,825),
   `DrawCrossHairs` `0x58E020` (2,765), `DrawHelpText` `0x58B6E0` (2,415), `DrawWanted` `0x58D9A0` (1,649),
   `DrawMissionTimers` `0x58B180` (1,363), `DrawSubtitles` `0x58C250` (1,104), `DrawSuccessFailedMessage` `0x58C6A0`
   (935).
5. **`CRadar` drawing:** `DrawEntityBlip` `0x587000` (3,049, stub) and `ClipRadarPoly` `0x585040` (906, stub, used by
   `DrawAreaOnRadar` and `DrawRadarSection`); missing `TransformRealWorldToTexCoordSpace` `0x583600` (100) and
   `HasThisBlipBeenRevealed` `0x583AF0` (67).
6. Leftovers: `CSprite::Render_4_Rot` `0x70D830` (814) / `Render_5_Rot` `0x70DB60` (1,617), `CSprite2d::Draw`
   `0x728420` (133), `CMenuSystem::FillGridWithCarColours` `0x5820E0` (457, stub), `CMenuSystem::GetNumMenusInUse`
   `0x558DC0` (6).

For one class, `class_dossier.py <Class>` gives a fresher list. Then follow [../ReversingGuide.md](../ReversingGuide.md).
