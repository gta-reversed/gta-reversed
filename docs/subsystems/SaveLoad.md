# Save and load

How a PC save file is written and read: `CGenericGameStorage` (the block stream, the work buffer, the checksum),
`C_PcSave` (slots and file names), `CSimpleVariablesSaveStructure` (the first block), and the `Load`/`Save` pair each
subsystem contributes. Also covers how to check a reversed `Load`/`Save` against the original. The menu pages that
trigger saving and loading are described in [HudFrontend.md](HudFrontend.md) ("Front-end menu").

All paths are relative to the repo root. Addresses are the ones written in the repo (`RH_*Install` or `// 0xADDR`) and
were checked against the IDB (`GenericSave` `0x5D13E0` and `GenericLoad` `0x5D17B0` decompiled side by side with
`source/game_sa/GenericGameStorage.cpp`).

---

## 1. Overview

```
Menu "Save" (SCREEN_SAVE_DONE_1)                         Menu "Load" (SCREEN_LOAD_FIRST_SAVE)
  CMenuManager::ProcessFileActions 0x578D60               CMenuManager::ProcessFileActions
  C_PcSave::SaveSlot(slot)          0x619060                CGenericGameStorage::CheckSlotDataValid 0x5D1380
    MakeValidSaveName -> ms_SaveFileName                      GenerateGameFilename, CheckDataNotCorrupt (checksum)
    DoGameSpecificStuffBeforeSave   0x618F50                  m_bLoadingData = true
    GenericSave                     0x5D13E0               ...main loop restarts the game...
      OpenFileForWriting                                   CGame::InitialiseWhenRestarting
      28 x { "BLOCK" + <subsystem>::Save() }                 GenericLoad 0x5D17B0
      pad to 202,748 bytes, append uint32 checksum             OpenFileForReading(ms_LoadFileName)
      CloseFile                                                 28 x { check "BLOCK" + <subsystem>::Load() }
  C_PcSave::PopulateSlotInfo        0x619140                   CloseFile, DoGameSpecificStuffAfterSucessLoad 0x618E90
```

The file is one stream: for each of the 28 blocks, the 5 bytes `BLOCK` (no terminator) followed by whatever that
subsystem's `Save` wrote. There are no per-block lengths. Block boundaries are found only by reading every block in
order. After the last block the stream is padded to `SIZE_OF_ONE_GAME_IN_BYTES = 202748` (`GenericGameStorage.cpp`),
and the 32-bit checksum is appended. **Every PC save is exactly 202,752 bytes.**

---

## 2. Key classes and files

| Class / file | Role |
|---|---|
| `CGenericGameStorage` (`source/game_sa/GenericGameStorage.h/.cpp`) | Static. Block order (`eBlocks`), work buffer, checksum, file open/close, slot state arrays |
| `C_PcSave` (`C_PcSave.h/.cpp`) | Slot helper, global `s_PcSaveHelper` (`0xC17034`, a single `eErrorCode`). Save directory, file names, slot list for the menu, save/delete one slot |
| `CSimpleVariablesSaveStructure` (`SimpleVariablesSaveStructure.h/.cpp`) | The first block (`0x138` = 312 bytes): save name, version id, clock, weather, camera zoom, area, riots, system time... `Construct` `0x5D1B80` fills it from the game, `Extract` `0x5D1EA0` applies it back |
| `CVehicleSaveStructure`, `CObjectSaveStructure`, `CGangWarsSaveStructure` (`*SaveStructure.h/.cpp`) | Per-entity / per-subsystem POD layouts used inside some blocks |
| `CFileMgr` (`FileMgr.h/.cpp`) | `OpenFile`/`Read`/`Write`/`CloseFile`, `InitUserDirectories` (`0x744FB0`), `SetDirMyDocuments` |
| `FastLoaderConfig` (`source/extensions/Configs/FastLoader.hpp`) | NOTSA. `[FastLoader] SaveGameToLoad` auto-loads a slot at start-up through `CMenuManager::SimulateGameLoad` |
| `MiscConfig` (`source/extensions/Configs/Miscellaneous.hpp`) | NOTSA. `[Misc] SaveDirectoryPath` (custom user folder) and `LoadSavesWithMismatchingVersion` |

Globals (`GenericGameStorage.h`, all `StaticRef`):

| Name | Address | Notes |
|---|---|---|
| `ms_WorkBuffer` | `0xC16EE8` | `new uint8[51201]` (`BUFFER_SIZE = 50 * 1024` + 1), freed by `CloseFile` |
| `ms_WorkBufferPos` / `ms_WorkBufferSize` | `0xC16EEC` / `0x8D2BE0` | read/write position; valid bytes in the buffer when loading |
| `ms_FileHandle`, `ms_FilePos`, `ms_FileSize` | `0xC16EF0`, `0xC16EF4`, `0xC16EF8` | |
| `ms_CheckSum` | `0xC16134` | running sum of every byte written |
| `ms_bFailed` / `ms_bLoading` | `0xC16EFC` / `0xC16EFD` | `ms_bFailed` makes every later buffer call fail |
| `ms_SaveFileName` | `0xC16DB8` | IDB name `ms_ValidSaveName`; target path for `GenericSave` |
| `ms_LoadFileName` / `ms_LoadFileNameWithPath` | `0xC15FC8` / `0xC15F60` | set by `OpenFileForReading(name, slot)` |
| `ms_SaveFileNameJustSaved` | `0xC16030` | |
| `ms_Slots`, `ms_SlotFileName`, `ms_SlotSaveDate` | `0xC16EBC`, `0xC16368`, `0xC16138` | 8 slots, filled by `PopulateSlotInfo` |
| `C_PcSave::DefaultPCSaveFileName` | `0xC16F18` | `<user dir>\GTASAsf` |

---

## 3. Files and directory

- The user directory comes from `InitUserDirectories` (`FileMgr.cpp`): `<Documents>\GTA San Andreas User Files`
  (or `[Misc] SaveDirectoryPath` from `gta-reversed.ini`, NOTSA; falls back to `data` if the shell folder can't be
  found). At start-up `WinPs.cpp` passes it to `C_PcSave::SetSaveDirectory` (`0x619040`), which stores
  `<dir>\GTASAsf` in `DefaultPCSaveFileName`.
- Slot `i` (0-7) is `<dir>\GTASAsf<i+1>.b` (`GenerateGameFilename` `0x6190A0`: `"%s%i%s"`, `slot + 1`, `".b"`).
  `MakeValidSaveName` (`0x5D0E90`) builds the same name with `"%s%i"` + `".b"` into `ms_SaveFileName` and replaces `?`
  with spaces.
- Other files in the same folder: `gta_sa.set` (menu settings, `CMenuManager::LoadSettings`/`SaveSettings`),
  `stats.html`, `Gallery\`, `User Tracks\`. Mission packs read `MPACK\MPACK<n>\` from there too.
- `C_PcSave::DeleteSlot` (`0x6190D0`) uses `std::filesystem::remove` (NOTSA implementation of a plain file delete).

---

## 4. Saving

`C_PcSave::SaveSlot` (`0x619060`): `MakeValidSaveName(slot)`, clear the error,
`CFileMgr::SetDirMyDocuments()` (the original only calls `InitUserDirectories`), `DoGameSpecificStuffBeforeSave`, then
`GenericSave`. It returns `0` for success and `2` for failure, and `ProcessFileActions` treats any non-zero value as
"save failed" (`FES_CMP`).

`DoGameSpecificStuffBeforeSave` (`0x618F50`) **changes the game state before writing**:
shuts down ropes, removes pickup objects, caps the player's health, ends a gang war, increments
`STAT_SAFEHOUSE_VISITS`, passes 6 game hours, resets hunger, sprint energy and wanted level, and tidies memory.

`GenericSave` (`0x5D13E0`):

1. `OpenFileForWriting` (`0x5D0DD0`): `fopen(ms_SaveFileName, "wb")`, reset positions, allocate the buffer.
2. `ms_CheckSum = 0`, then for each block in `eBlocks` order: write `"BLOCK"` with `SaveDataToWorkBuffer`, call the
   block's `Save` (table in section 6). If `ms_bFailed` is set, close the file and fail.
3. Pad: while less than 202,748 bytes are in the file and at least a full buffer is left, flush a full 51,200-byte
   buffer (old buffer contents, i.e. garbage). Then set the position so the total is exactly 202,748.
4. `SaveWorkBuffer(true)` (`0x5D0F80`) adds every buffered byte to `ms_CheckSum`, appends the checksum (flushing first
   if fewer than 4 bytes are free), writes, and closes. On success: `CPad::UpdatePads` (the original calls the
   `UpdatePadsThunk`).

`SaveDataToWorkBuffer(data, size)` (`0x5D1270`) copies into the buffer and flushes it with `SaveWorkBuffer(false)`
whenever it fills up, so a single call can span buffers. Subsystems call it once per field or array. The repo adds
templates `SaveDataToWorkBuffer(const T&)` and `LoadDataFromWorkBuffer<T>()` (`GenericGameStorage.h`).

---

## 5. Loading

`CGenericGameStorage::CheckSlotDataValid(slot)` (`0x5D1380`) runs from the menu: `GenerateGameFilename`, then
`CheckDataNotCorrupt` (`0x5D1170`), which opens the file, sums every byte except the last 4 and compares the sum with
the last 4 bytes. On success it calls `CStreaming::DeleteAllRwObjects` and the menu sets `m_bLoadingData`. The game
then restarts (main loop in `WinMain.cpp`: `CGame::ShutDownForRestart` + `InitialiseWhenRestarting`), and
`CGame::InitialiseWhenRestarting` calls `GenericLoad`. The file name is the `ms_LoadFileName` that
the checksum pass left behind.

`GenericLoad` (`0x5D17B0`):

1. `CCheat::ResetCheats`, `OpenFileForReading(nullptr, 0)` (`0x5D0D20`). The buffer starts "empty" (`pos == size ==
   51200`), so the first read triggers `LoadWorkBuffer`.
2. For each block: read 5 bytes and compare with `BLOCK`. On a mismatch: report the previous block, restore the
   simple variables backup if the mismatch is right after block 0, close, fail. Then call the block's `Load`.
   `SIMPLE_VARIABLES` loads the 312-byte structure and `Extract`s it; `POOLS` calls
   `CTheScripts::DoScriptSetupAfterPoolsHaveLoaded` (`0x5D3390`) when `CPools::Load` succeeds.
3. If `ms_bFailed` is set after a block: `ReportError(block, LOADING)`, close, fail.
4. Close, then `DoGameSpecificStuffAfterSucessLoad` (`0x618E90`): reload GXT, collision and scene, fade in, run
   scripts once, rebuild the player's clothes, update the population cycle and gang zones.

`LoadWorkBuffer` (`0x5D10B0`) reads up to 51,200 bytes. The last chunk must be a multiple of 4 bytes, otherwise it
fails. `LoadDataFromWorkBuffer` (`0x5D1300`) mirrors the save side. There are no per-block size checks: a `Load`
that reads a different number of bytes than its `Save` wrote shifts the stream, and the error only shows up as a
`BLOCK` mismatch at the start of the **next** block. `ReportError` (`0x5D08C0`) then names the block before it.

### Version check

`GetCurrentVersionNumber` (`0x5D0F50`) is `CKeyGen::GetKey("Apr 28 200510:28:55")`, the build date of v1.0 US. The
value is stored in `m_nVersionId`. **The original computes it on load and ignores it.** The repo adds a NOTSA check in
`GenericLoad`: a mismatching save is refused (and a new game starts) unless
`[Misc] LoadSavesWithMismatchingVersion = true`. The NOTSA helper `SaveVersionName` maps known ids to names (PS2, v1, v1.1,
v2, Austrian, German).

### Slot list

`C_PcSave::PopulateSlotInfo` (`0x619140`), called after saving/deleting and when the menu opens:
for each slot, read `CSimpleVariablesSaveStructure` at offset 5 (after the first `BLOCK`). If the save name isn't
`"THIS FILE IS NOT VALID YET"`, mark the slot `SLOT_FILLED` and copy the name (cut at 24 characters). Then
`CheckDataNotCorrupt`: if it passes, format the date from `m_systemTime` with the `MONTH<n>` GXT keys. If it fails, put
`FEC_SLC` with the slot number into the name. The repo also sets `SLOT_CORRUPTED` (NOTSA).

---

## 6. Block order and the subsystem `Load`/`Save` pairs

`eBlocks` (`GenericGameStorage.h`). All pairs below are hooked and reversed unless noted; addresses are from the
`GenericSave`/`GenericLoad` calls in the IDB.

| # | Block | Load | Save | Repo file |
|---|---|---|---|---|
| 0 | `SIMPLE_VARIABLES` | `CSimpleVariablesSaveStructure` 312 bytes, `Extract` `0x5D1EA0` | `Construct` `0x5D1B80` | `SimpleVariablesSaveStructure.cpp` |
| 1 | `SCRIPTS` | `CTheScripts::Load` `0x5D4FD0` | `CTheScripts::Save` `0x5D4C40` | `Scripts/TheScripts.cpp` |
| 2 | `POOLS` | `CPools::Load` `0x5D0890` | `CPools::Save` `0x5D0880` | `Pools.cpp` |
| 3 | `GARAGES` | `CGarages::Load` `0x5D3270` | `CGarages::Save` `0x5D3160` | `Garages.cpp` |
| 4 | `GAMELOGIC` | `CGameLogic::Load` `0x5D3440` | `CGameLogic::Save` `0x5D33C0` | `GameLogic.cpp` |
| 5 | `PATHS` | `CPathFind::Load` `0x5D3500` | `CPathFind::Save` `0x5D34C0` | `PathFind.cpp` |
| 6 | `PICKUPS` | `CPickups::Load` `0x5D35A0` | `CPickups::Save` `0x5D3540` | `Pickups.cpp` |
| 7 | `PHONEINFO` | (empty block: tag only) | | |
| 8 | `RESTART` | `CRestart::Load` `0x5D3770` | `CRestart::Save` `0x5D3620` | `Restart.cpp` |
| 9 | `RADAR` | `CRadar::Load` `0x5D53C0` | `CRadar::Save` `0x5D5860` | `Radar.cpp` |
| 10 | `ZONES` | `CTheZones::Load` `0x5D2F40` | `CTheZones::Save` `0x5D2E60` | `TheZones.cpp` |
| 11 | `GANGS` | `CGangs::Load` `0x5D3A90` | `CGangs::Save` `0x5D3A60` | `Gangs.cpp` |
| 12 | `CAR_GENERATORS` | `CTheCarGenerators::Load` `0x5D39B0` | `CTheCarGenerators::Save` `0x5D38C0` | `TheCarGenerators.cpp` |
| 13 | `PED_GENERATORS` | (empty) | | |
| 14 | `AUDIO_SCRIPT_OBJECT` | (empty) | | |
| 15 | `PLAYERINFO` | `CPlayerInfo::Load` `0x5D3B00` | `CPlayerInfo::Save` `0x5D3AC0` | `PlayerInfo.cpp` |
| 16 | `STATS` | `CStats::Load` `0x5D3BF0` | `CStats::Save` `0x5D3B40` | `Stats.cpp` |
| 17 | `SET_PIECES` | `CSetPieces::Load` `0x5D3CA0` | `CSetPieces::Save` `0x5D3C70` | `SetPieces.cpp` |
| 18 | `STREAMING` | `CStreaming::Load` `0x5D29E0` | `CStreaming::Save` `0x5D29A0` | `Streaming.cpp` |
| 19 | `PED_TYPES` | `CPedType::Load` `0x5D3D10` | `CPedType::Save` `0x5D3CD0` | `PedType.cpp` |
| 20 | `TAGS` | `CTagManager::Load` `0x5D3DA0` | `CTagManager::Save` `0x5D3D60` | `TagManager.cpp` |
| 21 | `IPLS` | `CIplStore::Load` `0x5D54A0` | `CIplStore::Save` `0x5D5420` | `IplStore.cpp` |
| 22 | `SHOPPING` | `CShopping::Load` `0x5D3E40` | `CShopping::Save` `0x5D3DE0` | `Shopping.cpp` |
| 23 | `GANGWARS` | `CGangWars::Load` `0x5D3EB0` | `CGangWars::Save` `0x5D5530` | `GangWars.cpp` |
| 24 | `STUNTJUMPS` | `CStuntJumpManager::Load` `0x5D5920` | `CStuntJumpManager::Save` `0x5D5570` | `StuntJumpManager.cpp` |
| 25 | `ENTRY_EXITS` | `CEntryExitManager::Load` `0x5D55C0` | `CEntryExitManager::Save` `0x5D5970` | `EntryExitManager.cpp` |
| 26 | `RADIOTRACKS` | `CAERadioTrackManager::Load` `0x5D40E0` | `CAERadioTrackManager::Save` `0x5D3EE0` | `Audio/Managers/AERadioTrackManager.cpp` |
| 27 | `USER3DMARKERS` | `C3dMarkers::LoadUser3dMarkers` `0x5D42E0` | `C3dMarkers::SaveUser3dMarkers` `0x5D4300` | `3dMarkers.cpp` |

Notes on some blocks:

- **SCRIPTS**: the size of the global-variable area (from the SCM's first chunk header), then that much of
  `ScriptSpace` in chunks of `MAX_SAVED_GVAR_PART_SIZE`, the script brains, `OnAMissionFlag`,
  `LastMissionPassedTime`, building swaps (pool index + 1, 0 = none), invisibility settings, blocked vehicle models, LOD connections, attached anim groups, the script sizes, then every non-external running script (struct + IP and IP stack as offsets into `ScriptSpace`).
  `CTheScripts::Load` begins with `Init()`. This is by far the largest block and the one most sensitive to the SCM
  (a save made with another `main.scm` loads garbage).
- **POOLS**: on PC only the **player ped** and the **objects** are saved (`CPools::Save` = `SavePedPool` +
  `SaveObjectPool`). `SavePedPool` (`0x5D4B40`, stub) writes `ref, model, pedType` and the ped's own `Save` for peds
  with `m_nPedType == PED_TYPE_PLAYER1` and `bInVehicle` clear (bit 8 of `m_nPedFlags`, `+0x46C`). `LoadPedPool` creates a
  `CPlayerPed` at that pool ref. `LoadVehiclePool`/`SaveVehiclePool` exist (Android) but aren't called on PC. The
  three `Save*Pool` are still `.Reversed = false` stubs (`CPools::InjectHooks`).
- **PATHS**: `m_nNumNodeSwitches` and that many 28-byte node-switch areas (the IDB calls them forbidden areas).
- **STATS**: does a side effect before writing: adds `STAT_KILLS_SINCE_LAST_CHECKPOINT` to
  `STAT_TOTAL_LEGITIMATE_KILLS` and zeroes it.
- **RADAR**: the whole `ms_RadarTrace` array. Entry/exit pointers are written as pool index + 1, and tracking flags are
  cleared just for the write and restored afterwards (`CRadar::Save`). `Load` starts with `Initialise()`.
- **PED_TYPES**: `PED_TYPE_COUNT x ACQUAINTANCE_NUM` `uint32` acquaintance masks.

---

## 7. Hook status of the storage core

`CGenericGameStorage::InjectHooks` registers 19 hooks. Only `ReportError`, `GenericLoad`, `GenericSave`,
`LoadDataFromWorkBuffer` and `SaveDataToWorkBuffer` are live. The other 14 are `.Reversed = false` even though most
have a complete C++ body: `OpenFileForReading/Writing`, `CloseFile`, `LoadWorkBuffer`, `SaveWorkBuffer`,
`CheckDataNotCorrupt`, `CheckSlotDataValid`, `MakeValidSaveName`, `GetCurrentVersionNumber`,
`DoGameSpecificStuffBeforeSave/AfterSucessLoad`, `InitRadioStationPositionList`, `RestoreForStartLoad` and the free
function `GetSavedGameDateAndTime`. The comment there says they couldn't be tested because common mods interfere, and that the
originals passed the file pointer in `ebp`. In `RedirectToGTA` state our body is patched to jump to the original, so
our direct calls from `GenericSave`/`GenericLoad` also run the original code. `C_PcSave::GenerateGameFilename` is
also off (`// bad`). `idb_coverage.py` reports all of these as STUB.

The bodies matter as soon as someone turns them on, so check them against the IDB first. `MakeValidSaveName` already
has a bug (section 9).

---

## 8. Verifying a reversed `Load`/`Save` pair

Every pair is a registered hook (category `Global/<Class>` for most, see the F7 Hooks window), so the original and our
version can be swapped at runtime. Three levels, cheapest first.

**Static.** `python .claude/skills/validate-against-binary/scripts/asm_validate.py CStats::Save` (and `Load`). Check
that the sequence of `SaveDataToWorkBuffer`/`LoadDataFromWorkBuffer` calls and their **sizes** match the asm exactly.
The order and sizes are the file format. `sizeof` of the repo struct must equal the size in the original call (keep
the `VALIDATE_SIZE`).

**In-process differential test** (preferred for blocks under 51,200 bytes). In a `NOTSA_DIFFTEST_INGAME` (see
`source/difftests/CLAUDE.md`):

1. Point `CGenericGameStorage::ms_WorkBuffer` at your own buffer (at least 51,201 bytes), set
   `ms_WorkBufferPos = 0` and `ms_bFailed = false`. The original `SaveDataToWorkBuffer` flushes at a hard-coded
   51,200 bytes, which would write to `ms_FileHandle`, so stay below that.
2. Snapshot the state the `Save` mutates (for `CStats::Save` the two stat arrays; for `CRadar::Save`, nothing, because
   it restores what it changes) with `MemorySnapshot`.
3. Run the original (`GTAFn<bool(__cdecl*)()>(0x5D3B40)()`), copy `ms_WorkBufferPos` bytes, restore the snapshot,
   reset the position, run ours, then `CompareBytes` the two outputs. A different **length** is the most serious
   failure, because it desyncs every later block.
4. For `Load`, feed the same bytes to both (reset `ms_WorkBufferPos`, `ms_WorkBufferSize = 51200`) and compare the
   resulting globals with `CompareSideEffects`. Loads that create entities (`CPools`, `CPickups`, `CGarages`) can't be
   run twice in place. Test them in game instead.

**File round trip** (any block, in game):

1. Pick a save `S`. Set the pair's hooks to GTA. Load `S`, walk into the save point, save to slot 7.
2. Load `S` again, set the hooks to ours, do the same, save to slot 8. Make the second run as close to the first as
   possible: same route, same game time. Both saves go through `DoGameSpecificStuffBeforeSave`, so its changes
   (+6 hours, +1 safehouse visit, ...) are the same in both.
3. Byte-compare `GTASAsf7.b` and `GTASAsf8.b`. Split both files on the 28 `BLOCK` tags (in order, starting at offset 0)
   and compare **only the block under test**. Ignore the last 4 bytes (checksum), the padding after the last block
   (stale buffer memory), and in `SIMPLE_VARIABLES` the fields that always differ (`m_systemTime`,
   `m_nTimeInMilliseconds`, `m_nFrameCounter`, time step).
4. For `Load`: load the same file once with the `Load` hook set to GTA and once set to ours, then save immediately
   with both `Save` hooks set to GTA and compare as above. A shifted stream shows up as a `ReportError` for that block
   in `logs/log.log` ("Loading error: ...") and a new game instead of the save.
5. Keep at least one save from an unmodified game as the reference. `LoadSavesWithMismatchingVersion` must stay off,
   or a wrong version id goes unnoticed.

Useful while debugging: `#define ENABLE_SAVE_DATA_LOG` at the top of `GenericGameStorage.cpp` logs every block name
and a hex dump of every buffer read/write (`LOG_SAVE`, `LOG_HEX_SPAN`).

---

## 9. Gotchas

- **No lengths, no versions per block.** One wrong field size anywhere breaks every block after it. Loads never check
  sizes or counts against limits: `CPathFind::Load` trusts `m_nNumNodeSwitches` from the file, like the original.
- **Saving changes the game.** `DoGameSpecificStuffBeforeSave` and some `Save`s (`CStats`) mutate state, so two saves
  in a row aren't identical. `CRopes::Shutdown` and `CPickups::RemovePickupObjects` run before anything is written.
- **The checksum covers the padding.** The padding bytes are whatever was left in `ms_WorkBuffer`, so two saves of the
  same state differ there and in the checksum. Never compare whole files.
- **`ms_WorkBufferSize` vs `0xC800`.** The original `SaveDataToWorkBuffer` (`0x5D1270`) flushes at a hard-coded 51,200
  bytes. The repo uses `ms_WorkBufferSize`, which the last read leaves at the size of
  the final chunk (49,152 after a checksum pass over a full save, e.g. from `PopulateSlotInfo`). The byte stream is
  the same, only the flush points move. Keep that in mind when you compare `ms_FilePos` values between versions.
- **`ReportError` on a tag mismatch** says `LOADING` in the repo (`GenericLoad`); the original passes
  `SAVING` there (`0x5D17B0`). The message is only logged, but asm_validate will flag it.
- **The original ignores the version id.** The refusal to load mismatching saves is NOTSA (section 5). It is on by
  default and turns old saves into "new game".
- **`CloseFile` returns `true` on success** in the repo (`== 0` on `fclose`); the original callers test
  `CFileMgr::CloseFile(...) == 0` inline. Don't invert it when you inline it.
- **Two different "slot" numberings.** Menu entries and `ms_Slots` are 0-7, file names use 1-8, `FastLoader`'s
  `SaveGameToLoad` uses 1-8 (`-1` = first existing, `-2` = off).
- **`GenerateGameFilename` is `__stdcall` in the IDB** (no `this`), while the repo declares it as a member. It works
  because the body doesn't use `this`. The hook is off anyway (`// bad`).
- **`s_PcSaveHelper` is a single error code** (`C_PcSave::eErrorCode`: 4 open, 5 read, 6 close, 7 slot
  invalid, 11 write). The header itself has a TODO saying it may be a bit field.

### Bugs found while writing this page (not fixed)

| Where | Original | Problem |
|---|---|---|
| `CGenericGameStorage::MakeValidSaveName` | `0x5D0E90` | Builds the path with `GenerateGameFilename` (which already ends in `.b`), then appends `.b` again, giving `GTASAsf1.b.b`. The original uses `sprintf("%s%i")`. **Latent**: the hook is `.Reversed = false`, so `C_PcSave::SaveSlot`'s direct call runs the original today. It becomes live the moment the hook is enabled |
| `C_PcSave::PopulateSlotInfo` (live) | `0x619140` | Inserts the 0-based `i` into `FEC_SLC`; the original passes the slot number `i + 1`. A corrupted slot 1 is shown as slot 0 |
| `C_PcSave::SaveSlot` (live) | `0x619060` | Calls `CFileMgr::SetDirMyDocuments()`, which also changes the current directory. The original only calls `InitUserDirectories()`. Harmless for the save itself (absolute path), but later relative `CFileMgr::OpenFile` calls start from the user folder until something calls `SetDir("")` |
| `CGenericGameStorage::GenericLoad` (live) | `0x5D17B0` | `ReportError(..., LOADING)` where the original uses `SAVING` (log text only) |

---

## 10. Reversing status and what is left

From `python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --class <Class>` (checked 2026-10-14):

| Class | Funcs | Reversed | Stub | Notes |
|---|---|---|---|---|
| `CGenericGameStorage` | 16 | 4 | 12 | The IDB class count; see section 7 for the 19 registered hooks. All "stubs" except `InitRadioStationPositionList` (empty) and `RestoreForStartLoad` (returns false) have a full C++ body behind `.Reversed = false` |
| `C_PcSave` | 5 | 4 | 1 | `GenerateGameFilename` off |
| `CSimpleVariablesSaveStructure` | 2 | 2 | 0 | |
| `CPools` | 19 | 16 | 3 | `SavePedPool` `0x5D4B40` (248 B), `SaveObjectPool` `0x5D4940` (245 B), `SaveVehiclePool` `0x5D4800` (319 B, unused on PC) |

All 27 subsystem `Load`/`Save` pairs in section 6 are registered and reversed.

Prioritised work:

1. **Turn on the storage core one function at a time** (13 methods, about 1.4 KB, already written). Validate each
   with `asm_validate.py`, enable it, and do a file round trip (section 8). Fix `MakeValidSaveName` first. Good order:
   `GetCurrentVersionNumber`, `CloseFile`, `OpenFileForWriting`/`Reading`, `SaveWorkBuffer`/`LoadWorkBuffer`,
   `CheckDataNotCorrupt`, `CheckSlotDataValid`, `MakeValidSaveName`, then the two `DoGameSpecificStuff*`.
   `C_PcSave::GenerateGameFilename` can go with them.
2. **`CPools::SavePedPool` and `SaveObjectPool`** (about 500 bytes). Mirror `LoadPedPool`/`LoadObjectPool` and check
   with the round trip. The ped filter is "slot used, `!bInVehicle`, `m_nPedType == PED_TYPE_PLAYER1`" (the decompiled checks
   on `+0x46C & 0x100` and `+0x598`).
3. **Fix the live `PopulateSlotInfo` slot number** and the `SaveSlot` directory change, both small.
4. Add `NOTSA_DIFFTEST_INGAME` tests for the small, pure pairs (`CPedType`, `CStats`, `CGangs`, `CSetPieces`,
   `CTagManager`, `CPathFind`, `CRestart`) using the recipe in section 8. That gives a regression net before anyone
   touches the storage core.

For a single class, `class_dossier.py <Class>` gives a fresher view. The general workflow is in
[../ReversingGuide.md](../ReversingGuide.md).
