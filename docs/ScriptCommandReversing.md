### Script command reversing reference

Script commands are implemented as plain C++ functions. When the game tries to run an implemented command, our command parser populates the arguments of the implemented function as they're intended in `Scripts/CommandParser/ReadArg.hpp`. For the return types, `StoreArg.hpp` does the job. Although their implementation can be read as documentation itself, I decided to make a kind of' cheat sheet' for easy access.

#### ReadArg Types
* Getting parameters by value copies them, use ptr/ref types if you want to modify.
* Using a pointer allows null values, references assert the game in this case.
* Do not access complex types (such as `CPed`) by value, it is either will fail the compilation or be very inefficient anyway.

| Type                | Nullable?          | Behavior                                                                                                  |
|---------------------|--------------------|-----------------------------------------------------------------------------------------------------------|
| `CRunningScript`    | No                 | Returns the current running script.                                                                       |
| Any arithmetic type | No                 | Reads vars, arrays, and statics; cast safely when read by value. Use &/* to reference.                    |
| Any enum type       | No                 | Same behavior as any arithmetic type, then cast to the enum type.                                         |
| `script::Model`*    | No                 | When the script arg is ≥ 0, returns the value, `UsedObjectArray[-value]` otherwise.                       |
| `script::Hash`      | No                 | Use specifically for model IDs, where they may not fit into ordinary signed integer types.                |
| `CPlayerPed`        | No                 | `FindPlayerPed(<arg>)`                                                                                    |
| `CPlayerInfo`       | No                 | `FindPlayerInfo(<arg>)`                                                                                   |
| Any pooled type     | Yes (empty handle) | `Pool->GetAtRef(<i32 handle>)`                                                                            |
| Script things       | Yes (invalid)      | Checks if the handle is invalid, inactive, or reused after deletion, if not: `Script*Array[<i32 handle>]` |
| `scm::StringRef`    | No                 | Reads vars, arrays, and statics; returns an **mutable** string reference regardless of the string type.   |
| `std::string_view`  | No                 | Reads vars, arrays, and statics; returns an **immutable** string reference regardless of the string type. |
| `const char*`       | No                 | Same behavior as `std::string_view`, but with null-termination guarantee.                                 |
| `CVector`           | No                 | Reads three floats.                                                                                       |
| `CVector2D`         | No                 | Reads two floats.                                                                                         |
| `CRect`             | No                 | Reads two `CVector2D` as (minX, minY), (maxX, maxY) or (top, left, bottom, right)                         |

\*: Use `script::Model` only when there is an access to `UsedObjectArray` in vanilla code, use `eModelID` otherwise.

#### StoreArg Types

| Type                | Behavior                                                                                                 |
|---------------------|----------------------------------------------------------------------------------------------------------|
| `bool`              | It isn't stored but updates the compare flag.                                                            |
| Any arithmetic type | Zeroes the target (var or array) then copies the value with `memcpy`. (which might be less than 4 bytes) |
| Any enum type       | Same behavior as any arithmetic type, cast to the underlying integer type.                               |
| Any pooled type     | Stores the handle of the pooled type, `-1` if `nullptr` is provided.                                     |
| `CVector`           | Stores three floats.                                                                                     |
| `CVector2D`         | Stores two floats.                                                                                       |
| `MultiRet<T...>`    | Stores the values separately in the order as they're defined.                                            |