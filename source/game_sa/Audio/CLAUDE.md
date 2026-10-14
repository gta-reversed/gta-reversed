# Audio/ — the AE audio engine

Guide: [../../../docs/subsystems/Audio.md](../../../docs/subsystems/Audio.md) (class table, init/per-frame flow,
backends, gotchas).

## Layout
- `AudioEngine.*` — `CAudioEngine` facade, global `AudioEngine` (`0xB6BC90`). Gameplay code calls
  `AudioEngine.ReportXxx` / `AddAudioEvent`. It owns the frontend, script, collision, pedless-speech and door
  entities, and a pointer to the global weapon entity.
- `Entities/` — `CAEAudioEntity` subclasses (one per sound source: vehicle, ped, speech, weapon, collision, ...),
  embedded in game objects (`CPed::m_pedAudio`, `CVehicle::m_vehicleAudio`, ...).
- `Managers/` — `CAESoundManager` (`MAX_NUM_SOUNDS` = 300 virtual sounds -> physical channels), radio, cutscene,
  ambience and user-track managers.
- `Hardware/` — `CAEAudioHardware` + channels (DirectSound; channel 0 = streaming). `Loaders/` — bank/track loaders,
  decoders, data streams. `OpenAL/` — optional NOTSA backend (`<project>_WITH_OPENAL`, OFF by default).
- Enums: `Enums/eSoundBank.h`, `eSoundBankSlot.h`, `SoundIDs.h`; `eAudioEvents` is in `source/game_sa/Enums/`.

## Conventions
- New code plays sounds with `AESoundManager.PlaySound({ .BankSlotID = ..., .SoundID = ..., .AudioEntity = this, ... })`
  (`tSoundPlayParams`); the original `m_tempSound.Initialise(...)` + `RequestNewSound` form is also fine.
  Example: `Entities/AEExplosionAudioEntity.cpp`.
- Volumes are dB (`GetDefaultVolume(event) + delta`, -100 = silent).
- Call `AEAudioHardware.EnsureSoundBankIsLoaded(bank, slot)` before playing; `false` = "queued, skip this time".
- Randomness: `CAEAudioUtility::GetRandomNumberInRange` (integer max is inclusive, unlike `CGeneral`) and
  `ResolveProbability`, where the original uses them.
- Hooks: entities are `NOTSA_EXPORT_VTABLE` + `RH_ScopedVirtualClass`, virtuals via `RH_ScopedVMTInstall`; categories
  `"Audio"`, `"Audio/Entities"`, `"Audio/Managers"`, `"Audio/Hardware"`, `"Audio/Loaders"`.
- F7 debug modules: `source/toolsmenu/DebugModules/Audio/`.

## Pitfalls
- The stream thread and the smooth-fade thread run concurrently; respect `CAEStreamThread::m_criticalSection`.
  Toggling hooks of thread-called functions in F7 can race.
- Some reversed functions stay `{ .Reversed = false }` because our version is wrong: `CAEStreamingChannel`
  constructor ("makes game not load radio"), `PrepareStream`, `Service`, and
  `CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume` ("bad"). Read the comment before flipping one.
- Negative track play times are states (`StreamingChannelState`), not durations.
- `SOUND_IS_CANCELLABLE` (0x2) maps to `m_IsUnancellable`; trust the usage, not the name.
- `CAESound::UpdateParameters` has a NOTSA SEH `__try/__except` around `m_PhysicalEntity->GetPosition()`: the entity
  can be dangling. Keep it.
- A freeze during loading usually means a broken `CAEMP3BankLoader` hook (blocking load loops).
- `windows.h` defines `PlaySound`: `AESoundManager.h` does `#undef PlaySound`; include order matters.
