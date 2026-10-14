# Events/ — AI event classes, queues and the event handler

Guide: [../../../docs/subsystems/Events.md](../../../docs/subsystems/Events.md) (flow, `CEvent` vtable order,
templates, gotchas). Decision makers live in `../DecisionMakers/`; response tasks in `../Tasks/`
([Tasks.md](../../../docs/subsystems/Tasks.md)).

## Layout
- `Event.h` (`CEvent`, 0xC, pooled via `GetEventPool()`), `EventEditableResponse.h` (decision-maker-driven events,
  0x14, adds `m_TaskId`, pure `CloneEditable()`), `EventGroup.*` (per-ped queue of 16), `EventGlobalGroup.*`,
  `EventHandler.*` (~60 `Compute*Response` methods, no vtable), `EventHandlerHistory.*`, `EventScanner.*`.
- Templates: `EventOnFire.*` (plain), `EventGunShot.*` / `EventDanger.*` (editable + entity ref), `EventSeenCop.*`
  (inherited constructor via `using`).

## Conventions
- `class NOTSA_EXPORT_VTABLE CEventXxx : public CEvent | CEventEditableResponse` (required for VMT hooks; a few
  headers such as `EventHealthLow.h` still lack it).
- Declare overrides in `CEvent` vtable order. Trivial getters (`GetEventType`, `GetEventPriority`, `GetLifeTime`,
  `Clone`/`CloneEditable`) stay inline in the header and are usually not hooked.
- Plain events override `Clone()`. Editable events override only `CloneEditable()`; `CEventEditableResponse::Clone`
  copies `m_TaskId`, facial and `m_bAddToEventGroup`: don't duplicate that.
- `static constexpr auto Type = EVENT_X;` only if you need `notsa::isa/dyn_cast`.
- Size must fit the event pool slot (`sizeof(CEventDamage)` = 0x44).
- Hooks: `RH_ScopedVirtualClass(CEventXxx, 0x<vtable>, N)` (16 for plain events, 17 for editable ones, e.g.
  `CEventOnFire`, `CEventGunShot`) + category `"Events"`; constructors through `Constructor` wrappers with
  `RH_ScopedInstall`, virtuals with `RH_ScopedVMTInstall`. Older files hook virtuals with `RH_ScopedInstall`.
- Register in the `Events` lambda in `source/InjectHooksMain.cpp`; lines `// + CEventX::InjectHooks();` mark classes
  that exist but hook nothing yet.

## Pitfalls
- Every `CEventGroup::Add` clones: a `Clone` missing a field breaks the event silently everywhere.
- `Remove` leaves null holes until `Reorganise()`.
- Equal priorities: the newer event wins (`>=`), except script commands.
- `eEventType` has gaps; don't size arrays by counting enumerators.
- Many event classes are complete in C++ but have no `InjectHooks`: the game still runs the original.
- When reversing an event, also check `CEventHandler::Compute<X>Response` and `CGroupEventHandler::ComputeResponse<X>`.
- Standalone scanner headers (`NearbyFireScanner.h`, `PedAcquaintanceScanner.h`, ...) duplicate classes of
  `EventScanner.h`; including both is a redefinition error.
