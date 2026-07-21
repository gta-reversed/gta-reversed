### General rules
* Do not change code already written unless it's related to your contribution or absolutely needed.
* Please check and try to eliminate warnings from your code.

### Code style
* 4 space indentation, LF line endings
* No hungarian notation [It's useless]:
```cpp
class Foo {
    int32 m_iPlayerHealth; // Bad
    int32 m_PlayerHealth; // Good
}
```
* Variables are named in `camelCase` and types in `PascalCase`.
* Class/struct members and functions are named in `CamelCase`
* In classes (**not** structs), use `m_` prefix to member variables:
```cpp
class Foo {
public:
    static int ms_TotalFoos; // Static member variable

private:
    int m_FooCount; // Non-static member variable
}
```
* For shared variables outside a class, use `g_` prefix, and define them in the header:
```cpp
// Header-defined shared global
inline int g_InlineGlobalCounter = 0; // Prefer `inline` over `extern`
```
* For static variables in a source file (eg.: `.cpp`), use `s_` prefix:
```cpp
// Source-defined static variable
static int s_StaticGlobalCounter = 0;
```
* For shared variables inside a class, use `ms_` prefix, and define them in the header:
```cpp
class Foo {
public:
    static inline int ms_StaticGlobalCounter = 0; // Static member variable
}
```
* Static and global variables should reference back to the original game address using `StaticRef` (In cases the original data is const, eg. its some configuration the value can be copied directly instead of referencing it):
```cpp
class Foo {
public:
    static inline auto& ms_StaticGlobalCounter = StaticRef<int>(0xDEADBEEF); // Static member variable
}
``` 
* If some rule about something is not specified here, refer to how it's done in the code
* Some classes may have *helper* functions to make code more readable. (Denoted by *NOTSA*) - Try adding new ones, or looking for and using them.
* If you made *helper* functions in a source file, mark them as `static`.
* If a helper doesn't fit into any of the existing files, see the `extensions` folder
* Prefer `get`-ters/`set`-ters over raw member access
* Use range-based for loops as much as possible:
```cpp
for (auto& element : array) { // Good
    // ...
} 

for (int i = 0; i < std::size(array); i++) { // Bad
    // ...
}
```
* Use `ranges` versions of `std` functions as much as possible.
* Use `rng::` instead of `std::ranges` and `rngv::` instead of `std::views`.
* We encourage you to write modern C++, but if that's not your style please adhere to the existing code style of the original game or in the file you're working on.
* Use `rngv::enumerate` if you need both the index and object.
```cpp
for (auto&& [i, e] : rngv::enumerate(array));
```
* If there's a dynamic `count` variable associated with a fixed size array, use `std::span` or `rng::views::take`. E.g.:
```cpp
// Bad
for (auto i = 0u; i < m_numThings; i++);

// Good
for (auto& thing : std::span{ m_things, m_numThings });
// Also good
for (auto& thing : m_things | rng::views::take(m_numThings));

// ^ If these funcs are called more than once, make a helper function in the header. Like below:
auto GetActiveThings() {
    return std::span{ m_things, m_numThings }
}
```
* Use `f` in float literals [As omitting it would make them a `double`] (e.g. `1.0f`)
* Use `std` library for generic functions like `min`, `max`, `lerp`, etc...
* `CVector` is interchangible with 3 floats [As is `CVector2D` with 2 floats] for function args
* Use lambdas for repetitive procedures in functions
* Use `constexpr` variables instead of macros
* Use `static inline` instead of `extern` and `static` in headers:
```cpp
class Foo {
    static uint32& m_FooCount; // Bad

    static inline auto& m_FooCount = StaticRef<uint32, 0xDEADBEEF>(); // Good
}
```

#### Types
* Use `auto` in function bodies if the variables' type is guessable.
* Guess for enum values [Or at least leave a `TODO` comment]
* Take care of const correctness [Especially of class methods] (e.g. `const char*` over `char*` or `const CVector&` over `CVector&`)
* Try to use SA types over RW as much as possible, **except** `RwMatrix`. (e.g. `CVector` for `RwV3d`, `CRGBA` for `RwRGBA`)
* Use fixed width integer types (e.g. `uint8`, `int32` over `unsigned char`, `int` etc).
* Do not use Win32 integer types. [Except for Win32 exclusive code] (e.g. `DWORD` -> `uint32`)
* For array sizes, etc... prefer using `unsigned` (u) types over `signed` ones (eg.: `uint32` over `int32`). If possible use `size_t`.
* Whenever possible use `std::array` over `C-Style` array [as the former has bounds checking in debug mode, and can help us discover bugs]

#### Fixing bugs
Whenever you find a bug, we encourage you to fix it [and/or at least] leave a comment explaining what the bug is.
If you do a bugfix you're required to use `notsa::bugfixes` - this helps documenting and testing these fixes.
See `reversiblebugfixes/Bugs.hpp` for more info.

#### Using `assert`
We encourage the usage of `assert` - if you think something may be out-of-bounds, or otherwise bug-prone, make sure to add an `assert`, it can help debugging the code a lot!
Do **not** add early returns for possible error conditions, use `assert` instead!
If the original game did early outs for possible unexpected error conditions then please also prefer using `assert` instead of just quietly erroring. Do not use it if the game can handle/recover from that error condition, an error or a warning log is enough.

### Handling translated (GXT) text
* GXT code page is a partial superset of ASCII, it's one-to-one except for `^`, `[` and `]`. (translated to [`¡`](https://en.wikipedia.org/wiki/Inverted_question_and_exclamation_marks), `<` and `>` respectively)
* GXT encoded characters are 1-byte long and strings are null-terminated. They can be used in `char` typed C copy/compare functions.
* Do not assume anything other than above for GXT strings.
* Use `AsciiToGxtChar` and `GxtCharToUTF8` for safely converting. UTF-8 strings should be safe to print and manipulate in general.
* Use `AsciiFromGxtChar` (or `""_gxt` for literals) and `GxtCharFromAscii` for implicitly converting. You **must** be sure that all characters are ASCII.

### Contributing
Please make sure to test your code before opening a PR. Guess what places/missions are affected by your code and test them. Use debug menu (F7) for quick access to stuff.

If you don't know how to test the code or think you have not tested enough specify it in the PR message.
