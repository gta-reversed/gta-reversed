# Audio (AE engine)

How GTA SA's "AE" audio engine is laid out in the repo, how a frame of audio is serviced, and what is still left to
reverse. Everything lives in `source/game_sa/Audio/`. General workflow and hook rules:
[../ReversingGuide.md](../ReversingGuide.md), [../ReversingRules.md](../ReversingRules.md). Build options:
[../Architecture.md](../Architecture.md).

---

## 1. Overview

```
 game code (peds, vehicles, weapons, scripts, menus...)
        |  AddAudioEvent(eAudioEvents, ...) / AudioEngine.ReportXxx(...)
        v
 CAEAudioEntity subclasses  ──PlaySound / RequestNewSound──>  CAESoundManager (300 virtual sounds)
        ^   UpdateParameters(sound, playPos) callback                |  picks the loudest N for physical channels
        |                                                            v
        +──────────────────────────────────────────────  CAEAudioHardware (DirectSound 8)
                                                           |- CAEStaticChannel x N   <- CAEMP3BankLoader (SFX banks/slots)
                                                           |- CAEStreamingChannel #0 <- CAEStreamThread <- CAEMP3TrackLoader / decoders
 CAERadioTrackManager / CAECutsceneTrackManager / CAEAmbienceTrackManager ──PlayTrack/StopTrack──^
```

- **`CAudioEngine`** is the facade. Its single instance is `AudioEngine` (`StaticRef<CAudioEngine>(0xB6BC90)` in
  `AudioEngine.cpp`). Gameplay code almost always goes through it: `AudioEngine.ReportCollision(...)`,
  `ReportFrontendAudioEvent(...)`, `StartRadio(...)`, `PreloadMissionAudio(...)`...
  It also *owns* several global audio entities: `m_FrontendAE`, `m_ScriptAE`, `m_CollisionAE`, `m_GlobalWeaponAE`
  (heap pointer, created in `InitialisePostLoading`), `m_PedlessSpeechAE`, `m_DoorAE`.
- **Audio entities** (`CAEAudioEntity` subclasses) translate a game event (`eAudioEvents`) into one or more `CAESound`s.
  Most are embedded in the game objects that make noise: `CPed::m_pedAudio`, `m_pedSpeech`, `m_weaponAudio`,
  `CVehicle::m_vehicleAudio`, `FxSystem_c::m_FireAE`, `CWanted::m_PoliceScannerAudioEntity`, `CWaterCannon::m_Audio`.
  `CWeather::m_WeatherAudioEntity` is a static global (0xC81360).
- **`CAESoundManager`** (`AESoundManager`, 0xB62CB0) holds a fixed array of 300 *virtual* sounds and decides each frame
  which of them get a *physical* hardware channel.
- **`CAEAudioHardware`** (`AEAudioHardware`, 0xB5F8B8) wraps DirectSound: channels, volumes, 3D positions, reverb, SFX
  bank loading (through `CAEMP3BankLoader`) and the music/radio stream (through `CAEStreamThread`).
- **Track managers** drive the single streaming channel: radio (`AERadioTrackManager`, 0x8CB6F8), cutscene/beat tracks
  (`AECutsceneTrackManager`, 0x8AE554), ambience/mission tracks (`AEAmbienceTrackManager`, 0x8AC15C) and user tracks
  (`AEUserRadioTrackManager`, 0xB6B970).

Data files (all opened by our code): `AUDIO\CONFIG\EVENTVOL.DAT` (per-event default volumes,
`CAEAudioEntity::StaticInitialise`), `BANKLKUP.DAT` / `BANKSLOT.DAT` / `PAKFILES.DAT` (`CAEBankLoader`), SFX paks under
`AUDIO\SFX\` (opened with `CdStreamOpen` by `CAEBankLoader`), `STRMPAKS.DAT` / `TRAKLKUP.DAT` and streams under
`AUDIO\STREAMS\` (`CAEMP3TrackLoader`).

---

## 2. Key classes and files

| Class / global | File | Role |
|---|---|---|
| `CAudioEngine` / `AudioEngine` (0xB6BC90) | `AudioEngine.h/.cpp` | Facade; owns global entities; `Initialise` (0x5B9C60), `Service`, `Report*` API, radio/mission-audio/cutscene-track API |
| `CAEAudioHardware` / `AEAudioHardware` (0xB5F8B8) | `Hardware/AEAudioHardware.*` | DirectSound device, up to 64 channels (`MAX_NUM_AUDIO_CHANNELS`), bank loading, stream thread, reverb, fader scaling |
| `CAEAudioChannel` -> `CAEStaticChannel`, `CAEStreamingChannel` | `Hardware/` | One DirectSound buffer each. Channel 0 is the streaming channel, the rest are static (SFX) channels (`CAEAudioHardware::Initialise`) |
| `CAESoundManager` / `AESoundManager` (0xB62CB0) | `Managers/AESoundManager.*` | 300 virtual sounds (`MAX_NUM_SOUNDS`), priority sorting, physical channel assignment |
| `CAESound` | `AESound.h/.cpp` | One playing sound: bank slot + sound id, owner entity, position, volume (dB), speed, flags (`eSoundEnvironment`) |
| `CAEAudioEntity` | `Entities/AEAudioEntity.*` | Base: `m_Entity`, `m_tempSound`, virtual `UpdateParameters(CAESound*, int16)`, `GetDefaultVolume(event)` |
| `CAEVehicleAudioEntity` | `Entities/AEVehicleAudioEntity.*` | Engines, horns, sirens, gears, per-vehicle-type `Process*` (107 functions) |
| `CAEPedAudioEntity` | `Entities/AEPedAudioEntity.*` | Footsteps, swimming, punches/hits, jacking |
| `CAEPedSpeechAudioEntity` / `CAEPedlessSpeechAudioEntity` | `Entities/AEPedSpeechAudioEntity.*`, `PedSpeechAudioEntityLUTs.inc.h` | Ped dialogue (contexts in `Enums/PedSpeechContexts.h`, voices in `Enums/PedSpeechVoices.h`); pedless = speech without a ped |
| `CAEWeaponAudioEntity` -> `CAEPedWeaponAudioEntity`, `CAEGlobalWeaponAudioEntity` | `Entities/` | Gun shots, reloads; the global one also does ambient gunfire, waterfalls, fog horn |
| `CAEFrontendAudioEntity` | `Entities/AEFrontendAudioEntity.*` | Menu/HUD sounds, loading tune, radio retune noise |
| `CAEScriptAudioEntity` | `Entities/AEScriptAudioEntity.*` | Mission audio slots (`PreloadMissionAudio`/`PlayLoadedMissionAudio`), script one-shots |
| `CAECollisionAudioEntity` | `Entities/AECollisionAudioEntity.*`, `.CollisionLookup.h` | Surface-vs-surface impacts, bullet hits, scrapes |
| `CAEExplosionAudioEntity`, `CAEFireAudioEntity`, `CAEDoorAudioEntity`, `CAEWaterCannonAudioEntity`, `CAEWeatherAudioEntity`, `CAETwinLoopSoundEntity` | `Entities/` | Smaller entities; twin-loop alternates two looping sounds |
| `CAEPoliceScannerAudioEntity` | `Entities/AEPoliceScannerAudioEntity.*` | Police radio dialogue assembled from slots |
| `CAEBankLoader` -> `CAEMP3BankLoader` | `Loaders/AEBankLoader.*`, `AEMP3BankLoader.*` | Loads SFX banks into bank slots; 50 queued `CAESoundRequest`s; `Service()` pumps CdStream reads |
| `CAETrackLoader` -> `CAEMP3TrackLoader` | `Loaders/` | Finds tracks in stream paks |
| `CAEStreamingDecoder` -> `CAEVorbisDecoder`, `CAEWaveDecoder`, `CAEWMADecoder`, `CAEMFDecoder` | `Loaders/` | Decoders; WMA/MF/WAV are used for user tracks (`CAEUserRadioTrackManager`) |
| `CAEDataStream`, `CAEStreamTransformer` | `Loaders/` | File stream wrapper, optionally encrypted (`m_bIsEncrypted`) |
| `CAEStreamThread` | `AEStreamThread.*` | Win32 thread feeding the streaming channel (`CreateThread` in `CAEStreamThread::Initialise`, loop in `MainLoop` 0x4F15C0) |
| `CAESmoothFadeThread` / `AESmoothFadeThread` (0xB608D0) | `AESmoothFadeThread.*` | Second thread that fades DirectSound buffer volumes |
| `CAERadioTrackManager` | `Managers/AERadioTrackManager.*` | Station choice, DJ banter/adverts/idents history, retune, station name display |
| `CAECutsceneTrackManager`, `CAEAmbienceTrackManager`, `CAEUserRadioTrackManager` | `Managers/` | Cutscene/beat tracks, zone/mission ambience, user MP3 scanning |
| `CAEAudioEnvironment` | `AEAudioEnvironment.*` | Distance attenuation, Doppler, directional mic, reverb environment from audio zones |
| `CAEAudioUtility` | `AEAudioUtility.*` | Random numbers, `ResolveProbability`, `AudioLog10`, piecewise-linear curves, bytes<->ms, timing |
| `CAudioZones` | `AudioZones.*` | Up to 158 boxes / 3 spheres (from IPL `auzo`), active lists feed reverb and ambience |

Enums: `Enums/eSoundBank.h` (banks), `Enums/eSoundBankSlot.h` (slots, with an explanatory comment at the top),
`Enums/SoundIDs.h` / `BankSlotSoundIDs.h`, `Enums/eAERadioType.h`, `eAEVehicle*.h`, `ePedAudioType.h`, `eBassSetting.h`.
`eAudioEvents` itself is in `source/game_sa/Enums/eAudioEvents.h` (7,000+ lines).

---

## 3. Lifetime and per-frame flow

### Initialisation

`CGame::InitialiseCoreDataAfterRW()` calls `AudioEngine.Initialise()`. `CAudioEngine::Initialise`:

1. `CAEAudioUtility::StaticInitialise()` (moved earlier than in SA to avoid a division by zero, NOTSA comment).
2. `AEAudioHardware.Initialise()`: creates the bank/track loaders, `DirectSoundCreate8`, the listener, the streaming
   channel (channel 0) and `CAEStaticChannel`s for the remaining channels. The channel count is 48 (software mixing)
   when fewer than 24 free hardware 3D buffers exist, otherwise `min(free, 64) - 7`. Then `AEUserRadioTrackManager`,
   `AESmoothFadeThread` and the stream thread.
3. Allocates one background channel, initialises the radio/cutscene/ambience track managers and `AESoundManager`.
4. Loads `EVENTVOL.DAT` (`CAEAudioEntity::StaticInitialise`) and calls every entity's `StaticInitialise` (these mostly
   queue `LoadSoundBank(bank, slot)` for their permanent banks).

`InitialisePostLoading` (0x5078F0) creates `m_GlobalWeaponAE` and **spins** on `AEAudioHardware.Service()` until a
fixed list of banks is resident.

### Per frame

`Idle()` calls `CGame::Process()` then `AudioEngine.Service()`; `FrontendIdle()` calls it too (both in
`app/app_game.cpp`). Gameplay-side, entities do their own work during `CGame::Process`: vehicles call
`m_vehicleAudio.Service()` from their control code (`CAutomobile`, `CBoat`, `CTrain`), `CWeather::Update` services
weather audio, `CWaterCannons` theirs, and `CGame::Process` runs `CAudioZones::Update(false, camPos)`.

`CAudioEngine::Service()`:

```
m_FrontendAE.AddAudioEvent(AE_FRONTEND_WAKEUP_AMPLIFIER) (+ RETUNE_STOP_PAUSED when not paused)
trackPlayTime = AEAudioHardware.GetTrackPlayTime()          // negative = StreamingChannelState (-4 finished, -6 stopped...)
mission-complete-track bookkeeping
AECutsceneTrackManager.Service(t); AEAmbienceTrackManager.Service(t); AERadioTrackManager.Service(t)
m_GlobalWeaponAE->ServiceAmbientGunFire()                   // only if a player ped exists
m_CollisionAE.Service()
set stream fader (1.0, or 0.0 during cutscenes)
m_ScriptAE.Service(); CAEPedSpeechAudioEntity::Service(); AESoundManager.Service(); CAEPoliceScannerAudioEntity::Service()
```

`CAESoundManager::Service()` is the heart of the mixer. Its comments carry the original address of each block:

1. Compute elapsed time (pause-aware: `CTimer::GetTimeInMSPauseMode()` when paused).
2. Read channel play positions and virtual-channel lengths/loop starts back from the hardware.
3. Resolve percentage start positions (0x4F011C), stop sounds that requested stop (0x4F016D), advance play time
   (0x4F01E4), clear ended channels (0x4F02FB), call `SoundHasFinished()` for ended sounds (0x4F0329).
4. For every active sound: `UpdateParameters(playTime)` then `CalculateVolume()` (0x4F03E5). `CAESound::UpdateParameters`
   moves the sound with its `m_PhysicalEntity` and, if `SOUND_REQUEST_UPDATES`, calls back
   `m_AudioEntity->UpdateParameters(sound, playPos)`. Entities fade, re-pitch or stop sounds there.
5. Build the prioritised list: uncancellable sounds first, then by `m_ListenerVolume` (0x4F042C-0x4F0529).
6. Stop physical channels whose sound fell out of the list (0x4F0585), start new ones with `AEAudioHardware.PlaySound`
   + volume/position/frequency (0x4F0628), refresh the rest (0x4F0894).
7. `AEAudioHardware.Service()` (0x4F0AB8).

`CAEAudioHardware::Service()` (0x4D9870): commits deferred 3D listener settings, rescales channel volumes, updates
reverb (if the EAX/3D query succeeded), `SynchPlayback()` + `Service()` on each channel except 0, updates virtual
channels and pumps `m_pMP3BankLoader->Service()` (bank streaming).

**Channel 0 (streaming) is not serviced on the main thread.** `CAEStreamThread::MainLoop` loops every ~5 ms:
`Service()` + `m_pStreamingChannel->Service()`, pausing the buffer when the window loses focus. Track managers talk to
it via `AEAudioHardware.PlayTrack/StopTrack/GetTrackPlayTime`.

Blocking helpers such as `CAudioEngine::Reset`, `Shutdown`, `PreloadCutsceneTrack(..., wait=true)` and
`InitialisePostLoading` loop on `AEAudioHardware.Service()` / `AERadioTrackManager.Service()` until a state is reached.
Keep those loops when reversing; they are what the original does.

### Pause

`PauseAllSounds`/`ResumeAllSounds` (0x507430/0x507440). While paused, `CAESoundManager::IsSoundPaused` silences
pausable sounds (volume -100 dB, frequency 0) instead of stopping them.

---

## 4. Conventions and helpers

- **Playing a sound** (modern form): `AESoundManager.PlaySound({ .BankSlotID = ..., .SoundID = ..., .AudioEntity = this,
  .Pos = ..., .Volume = ..., .Flags = ... })`. `tSoundPlayParams` (`AESoundManager.h`) uses designated initialisers
  and has NOTSA extras (`RegisterWithEntity`, `EventID`, `ClientVariable`). The original form is
  `m_tempSound.Initialise(...)` + `AESoundManager.RequestNewSound(&m_tempSound)`. Both behave the same; prefer
  `PlaySound` for new code. Example: `CAEExplosionAudioEntity::AddAudioEvent`.
- **Volumes are in dB**, typically `GetDefaultVolume(event) + delta` (from `EVENTVOL.DAT`). -100 dB is silence. Convert
  with `CAEAudioUtility::AudioLog10` and `CAEAudioEnvironment::GetDistanceAttenuation`.
- **Banks and slots**: a slot is a buffer, a bank is the content loaded into it. Before playing, call
  `AEAudioHardware.EnsureSoundBankIsLoaded(bank, slot)`: it returns `true` only if the bank is already resident,
  otherwise it queues the load and returns `false` (so the sound is skipped this time, like SA).
  `IsSoundBankLoaded` / `LoadSoundBank` are the raw versions.
- **Flags**: `eSoundEnvironment` in `AESound.h` (`SOUND_FRONT_END`, `SOUND_REQUEST_UPDATES`,
  `SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY`, `SOUND_FORCED_FRONT`, ...). `CAESound` exposes them as named bitfields.
- **Events**: `eAudioEvents` ranges: game events `AE_GAME_FIRST(0)..AE_GAME_LAST`, script events from 1000
  (`AE_SCRIPT_FIRST`), script bank events from 1800 (`AE_SCRIPT_BANK_FIRST`), script slot events from 2000
  (`AE_SCRIPT_SLOT_FIRST`) up to `AE_SCRIPT_SLOT_LAST`. Entities usually `switch` on the event in `AddAudioEvent`.
  `CAESound::m_Event` is an `int32` and some entities reuse it for non-event ids (comment in `AESound.h`).
- **Randomness**: use `CAEAudioUtility::GetRandomNumberInRange` / `ResolveProbability` (not `CGeneral`) where the
  original does, to keep the random sequence identical (R2 in [../ReversingRules.md](../ReversingRules.md)).
- **Hook categories**: entities with a vtable use `RH_ScopedVirtualClass(Cls, vtblAddr, nVirtuals)` +
  `RH_ScopedVMTInstall(UpdateParameters, addr)`; categories are `"Audio/Entities"`, `"Audio/Managers"`, etc. Entity
  classes are tagged `NOTSA_EXPORT_VTABLE`.
- **Size checks**: most classes have `VALIDATE_SIZE`/`VALIDATE_OFFSET` (e.g. `CAudioEngine` 0x1FD8, `CAEAudioEntity`
  0x7C, `CAEMP3BankLoader` 0x6E4).
- **Debug tools**: F7 ImGui modules in `source/toolsmenu/DebugModules/Audio/` (sound manager, ambience/cutscene track
  managers, user radio, entities) and `AudioZonesDebugModule.cpp`.

---

## 5. Backends: DirectSound vs OpenAL

`source/CMakeLists.txt` ("Configure the Audio API" section): `GTASA_WITH_OPENAL=ON` defines `USE_OPENAL` and adds
`game_sa/Audio/OpenAL` to the include path; otherwise `USE_DSOUND` is defined, `dsound` is linked and the `OpenAL/`
sources are removed from the build. The option defaults to `OFF` on Windows and is forced `ON` elsewhere.

In practice **only DirectSound is implemented**. `USE_DSOUND` guards appear only in `Hardware/AEAudioChannel.h/.cpp`
and `app/platform/win/winincl.h`; `USE_OPENAL` is not referenced by any C++ file. `CAEAudioHardware` calls DirectSound
unconditionally (`DirectSoundCreate8`, `IDirectSound3DListener::CommitDeferredSettings`), and `AEAudioChannel.h` always
includes `dsound.h`. The `OpenAL/` folder (`OALBase`, `OALBuffer`, `OALSource`) is NOTSA scaffolding that nothing uses
yet. Do not assume an OpenAL build works.

EAX is compiled out (`#if 0 // No EAX` around `EAXDirectSoundCreate` in `CAEAudioHardware::Initialise`).

---

## 6. Gotchas

- **Threads.** The stream thread and the smooth-fade thread run concurrently with the game. `CAEStreamThread` has a
  `CRITICAL_SECTION` (`m_criticalSection`); anything touching the streaming channel or decoders from the main thread
  must respect it. Reversible hooks on functions called from these threads are swapped while they run, so toggling
  them in F7 can race.
- **Stub hooks that are disabled on purpose.** Some functions are reversed in C++ but registered with
  `{ .Reversed = false }` because our version is wrong, e.g. the `CAEStreamingChannel` constructor ("makes game not
  load radio") and `CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume` ("bad"). Check the body before assuming it
  is a `plugin::Call` stub.
- **Negative play times are states.** `GetTrackPlayTime()` returns `StreamingChannelState` values (-7 paused,
  -6 stopped, -5 stopping, -4 finished, -1 started; `Hardware/AEStreamingChannel.h`); a sound play time of `-1` means
  finished.
- **Flag naming mismatch.** Bit `0x2` is `SOUND_IS_CANCELLABLE` in `eSoundEnvironment`, but the bitfield is called
  `m_IsUnancellable` and the sound manager uses it as "uncancellable". Trust the usage, not the enum name.
- **Dangling physical entities.** `CAESound::UpdateParameters` wraps `m_PhysicalEntity->GetPosition()` in SEH
  (`__try/__except`) because the entity can be deleted before the sound. Do not remove it.
- **`m_AudioEntity` can be null** (references are clearable, see the comment in `CAESound::UpdateParameters`); always
  check before calling back.
- **Blocking loops** in `CAudioEngine` (see section 3) hang forever if a bank never loads, e.g. when a hook in
  `CAEMP3BankLoader` is broken. Suspect the loader first when the game freezes on loading.
- **`#undef PlaySound`** is needed because `windows.h` defines it (top of `AESoundManager.h`). Include order matters.
- **Unity builds**: entity `.cpp` files have file-level globals such as `gfExplosionFrequencyVariations`; keep new
  file-local helpers in `namespace {}` with unique names.
- **Pause timing**: use `CTimer::GetTimeInMSPauseMode()` where SA does; `CAESoundManager` keeps separate paused and
  unpaused timestamps.

---

## 7. What is left

From `idb_coverage.py --class <C>` (IDB export, method scope). Columns: reversed / stub (hooked, `.Reversed = false`) /
known (address in repo, no hook) / missing. Refresh with `idb_coverage.py --classes 60 --filter AE` before relying on
the numbers.

| Class | Total | Rev | Stub | Known | Missing | Bytes left |
|---|---:|---:|---:|---:|---:|---:|
| `CAERadioTrackManager` | 43 | 29 | 14 | 0 | 0 | 9,634 |
| `CAEScriptAudioEntity` | 20 | 8 | 11 | 0 | 1 | 9,467 |
| `CAEWeaponAudioEntity` | 20 | 0 (+20 dormant) | 0 | 0 | 0 | 7,604 |
| `CAEFrontendAudioEntity` | 8 | 4 | 2 | 0 | 2 | 6,666 |
| `CAEPedAudioEntity` | 22 | 16 | 4 | 1 | 1 | 4,016 |
| `CAEVehicleAudioEntity` | 107 | 102 | 4 | 0 | 1 | 3,214 |
| `CAEAmbienceTrackManager` | 12 | 6 | 1 | 4 | 1 | 2,197 |
| `CAEPoliceScannerAudioEntity` | 17 | 13 | 4 | 0 | 0 | 2,029 |
| `CAEStreamingChannel` | 25 | 21 | 3 | 0 | 1 | 1,184 |
| `CAEStaticChannel` | 11 | 7 | 1 | 0 | 3 | 984 |
| `CAESound` | 34 | 17 | 0 | 0 | 17 | 923 |
| `CAEMFDecoder` | 14 | 9 | 0 | 0 | 5 | 727 |
| `CAETwinLoopSoundEntity` | 14 | 7 | 0 | 6 | 1 | 492 |
| `CAEAudioHardware` | 55 | 54 | 1 | 0 | 0 | 344 |
| `CAECollisionAudioEntity` | 20 | 17 | 0 | 0 | 3 | 317 |
| `CAEWaterCannonAudioEntity` | 9 | 7 | 0 | 2 | 0 | 309 |
| `CAEAudioChannel` | 17 | 12 | 0 | 2 | 3 | 274 |
| `CAEPedWeaponAudioEntity` | 4 | 0 | 0 | 4 | 0 | 266 |
| `CAECutsceneTrackManager` | 12 | 8 | 0 | 3 | 1 | 238 |

Fully reversed (0 bytes left): `CAEAudioEnvironment`, `CAEAudioUtility`, `CAEBankLoader`, `CAEPedlessSpeechAudioEntity`,
`CAEStreamTransformer`, `CAEStreamingDecoder`, `CAEUserRadioTrackManager`, `CAEVorbisDecoder`, `CAudioZones`.
`CAEPedSpeechAudioEntity` (58/59) and `CAEDataStream` (19/20) are practically done. The other classes have under 150
bytes left, mostly tiny inlined or compiler-generated helpers.

Remaining work, biggest first:

1. **Radio** (`CAERadioTrackManager`): `Service` (0x4EB9A0), `CheckForStationRetune`, `TrackRadioStation`,
   `QueueUpTracksForStation`, `CheckForTrackConcatenation`, `UpdateRadioVolumes`, `StopRadio` and the `Choose*Index`
   family are still `.Reversed = false` (see `CAERadioTrackManager::InjectHooks`).
2. **Mission audio** (`CAEScriptAudioEntity`): `PreloadMissionAudio`, `PlayLoadedMissionAudio`,
   `ProcessMissionAudioEvent`, `PlayMissionBankSound`, `UpdateParameters` and the mission-audio slot getters/setters.
3. **Weapon audio**: `CAEWeaponAudioEntity` is reversed, but `CAEWeaponAudioEntity::InjectHooks()` is commented out in
   `InjectHooksMain` (`Audio()` block), so none of its 20 hooks is installed. Validate it and register it.
4. **Frontend**: `CAEFrontendAudioEntity::AddAudioEvent` (0x4DD4A0) and `UpdateParameters` (0x4DEDA0) are large stubs.
5. **Ped audio**: `HandlePedSwing`, `HandlePedHit`, `HandlePedJacked`, `Service`.
6. **Vehicle audio**: `ProcessGenericJet`, `ProcessDummyBicycle`, `ProcessPlayerBicycle`, `ProcessDummyHovercraft`.
7. **Police scanner**: `AddAudioEvent`, `LoadSlots`, `PlayLoadedDialogue`, `Service`.
8. **Streaming / hardware**: `CAEStreamingChannel::PrepareStream`, `Service` (0x4F2550) and the disabled constructor;
   `CAEStaticChannel::SetAudioBuffer`; `CAEAudioHardware::PlaySound` (0x4D86B0).
9. Small leftovers: `CAESound` has 17 missing functions (mostly small accessors/inlined pieces);
   `CAEPedWeaponAudioEntity` has 4 known addresses and no hooks.

Test audio work with the radio on/off, during a cutscene, while paused and with user tracks, since all of those go
through the streaming channel. The hooks are under the `Audio/...` categories of the F7 hooks menu.
