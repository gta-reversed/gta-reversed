### General rules

* Do not change code already written unless it's related to your contribution or absolutely needed.
* Please check and try to eliminate warnings from your code.

### Code style
* 4 space indentation, LF line endings
* No hungarian notation [It's useless]
* If some rule about something is not specified here, refer to how it's done in the code
* Some classes may have *helper* functions to make code more readable. (Denoted by *NOTSA*) - Try adding new ones, or looking for and using them.
* Prefer `get`-ters/`set`-ters over raw member access
* Use range-based for loops as much as possible.
* We encourage you to write modern C++, but if that's not your style, please keep the following in mind:
```cpp
for (auto& element : array); // <-- GOOD

for (int i = 0; i < std::size(array); i++); // <-- BAD
```
* We have our own `enumerate` implementation. Use them if you need it. [C++ in the future will have it anyways](https://github.com/cplusplus/papers/issues/875)
```cpp
for (auto&& [i, e] : notsa::enumerate(array));
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
* Take care of const correctness [Especially of class methods] (e.g. `const char*` over `char*`)
* Try to use SA types over RW as much as possible, **except** `RwMatrix`. (e.g. `CVector` for `RwV3d`, `CRGBA` for `RwRGBA`)
* Use fixed width integer types (e.g. `uint8`, `int32`).
* Do not use Win32 integer types. [Except for Win32 exclusive code] (e.g. `DWORD` -> `uint32`)
* For array sizes, etc.. prefer using `unsigned` types over `signed` ones
* Whenever possible use `std::array` over `C-Style` array [as the former has bounds checking in debug mode, and can help us discover many bugs]

#### Fixing bugs
Whenever you find a bug, we encourage you to fix it [and/or at least] leave a comment explaining what the bug is.
Bug fixes should only be active if `notsa::IsFixBugs()` returns `true`.
If that's not possible [due to code complexity], then wrap into an `#ifdef`:
```c
#ifdef FIX_BUGS
// Bug fixing code here
#endif
```

### Handling translated (GXT) text
* GXT code page is a partial superset of ASCII, it's one-to-one except for `^`, `[` and `]`. (translated to [`¡`](https://en.wikipedia.org/wiki/Inverted_question_and_exclamation_marks), `<` and `>` respectively)
* GXT encoded strings are 1-byte long and null-terminated. They can be used in C copy-compare functions.
* Do not assume anything other than above for GXT strings.
* Use `AsciiToGxtChar` and `GxtCharToUTF8` for safely converting. UTF-8 strings should be safe to print and manipulate in general.
* Use `AsciiFromGxtChar` (or `""_gxt` for literals) and `GxtCharFromAscii` for implicitly converting. You **must** be sure that all characters are ASCII.

### Contributing
Please make sure to test your code before opening a PR. Guess what places/missions are affected by your code and test them. Use debug menu (F7) for quick access to stuff.

If you don't know how to test the code or think you have not tested enough specify it in the PR message.
