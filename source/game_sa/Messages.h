/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

struct tMessage {
    tMessage() = default;

    tMessage(
        const GxtChar* text,
        const GxtChar* strToInsert = {},
        uint16 flags = {},
        uint32 time = {},
        bool previousBrief = {},
        std::optional<std::array<int32, 6>> numbers = {}
    ) :
        Text{ text },
        StringToInsert{ strToInsert },
        Flags{ flags },
        VisibleTime{ time },
        CreatedAtMS{ CTimer::GetTimeInMS() },
        PreviousBrief{ previousBrief }
    {
        if (numbers) {
            rng::copy(*numbers, rng::begin(NumbersToInsert));
        } else {
            rng::fill(NumbersToInsert, -1);
        }
    }

    ~tMessage() {
        Text = nullptr; // This marks that this object is destroyed
    }

    //! If this object is in use
    bool IsValid() const { return Text != nullptr; }

    //! Ticks to disappear at
    auto GetTimeToDisappearAtMS() const { return CreatedAtMS + VisibleTime; }


    const GxtChar*       Text{};
    uint16               Flags{};
    uint32               VisibleTime{};
    uint32               CreatedAtMS{};
    std::array<int32, 6> NumbersToInsert{};
    const GxtChar*       StringToInsert{};
    uint8                PreviousBrief{};
};

struct tBigMessage {
    std::array<tMessage, 4> Stack{};
};

struct tPreviousBrief {
    const GxtChar*       Text{};
    std::array<int32, 6> NumbersToInsert{};
    const GxtChar*       StringToInsert{};
};

class CMessages {
public:
    static inline std::array<tPreviousBrief, 20>& PreviousBriefs = *(std::array<tPreviousBrief, 20>*)0xC1A570;
    static inline std::array<tMessage, 8>& BriefMessages = *(std::array<tMessage, 8>*)0xC1A7F0;
    static inline std::array<tBigMessage, 7>& BIGMessages = *(std::array<tBigMessage, 7>*)0xC1A970; // (for each text style)

public:
    static void InjectHooks();

    static void Init();
    static void AddMessage2(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief, bool showInstantly = false, GxtChar* str = nullptr, std::optional<std::array<int32, 6>> numbers = {});
    static void AddMessageQ(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief); // Renamed from AddMessage
    static void AddMessageWithStringQ(const GxtChar* text, uint32 time, uint16 flag, GxtChar* string, bool bPreviousBrief);
    static void AddMessageWithNumberQ(const GxtChar* text, uint32 time, uint16 flag, int32 n1 = -1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1, bool bPreviousBrief = false);
    static void AddMessageJump(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief); // Renamed from AddMessageJumpQ
    static void AddMessageJumpQWithNumber(const GxtChar* text, uint32 time, uint16 flag, int32 n1 = -1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1, bool bPreviousBrief = false);
    static void AddMessageJumpQWithString(const GxtChar* text, uint32 time, uint16 flag, GxtChar* string, bool bPreviousBrief);

    static void AddBigMessage2(const GxtChar* text, uint32 time, eMessageStyle style, bool showInstantly, GxtChar* str = nullptr, std::optional<std::array<int32, 6>> numbers = {});
    static void AddBigMessageWithNumberQ(const GxtChar* text, uint32 time, eMessageStyle style, int32 n1 = -1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1);
    static void AddBigMessageQ(const GxtChar* text, uint32 time, eMessageStyle style);
    static void AddBigMessageWithNumber(const GxtChar* text, uint32 time, eMessageStyle style, int32 n1 = -1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1);
    static void AddBigMessage(const GxtChar* text, uint32 time, eMessageStyle style);

    static void AddToPreviousBriefArray(const GxtChar* text, int32 n1 = -1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1, const GxtChar* string = nullptr);

    static void ClearMessages(bool flag);
    static void ClearSmallMessagesOnly();
    static void ClearPreviousBriefArray();
    static void ClearThisPrint(const GxtChar* text);
    static void ClearThisBigPrint(const GxtChar* text);
    static void ClearThisPrintBigNow(eMessageStyle style);
    static void ClearAllMessagesDisplayedByGame(bool unk);

    static uint32 GetStringLength(const GxtChar* string);

    static void StringCopy(GxtChar* dest, const GxtChar* src, uint16 len);
    static bool StringCompare(const GxtChar* str1, const GxtChar* str2, uint16 len);
    static void CutString(int32 count, const GxtChar* str, GxtChar** dest);

    static void InsertNumberInString(const GxtChar* src, int32 n1 = -1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1, GxtChar* dst = nullptr);
    static void InsertStringInString(GxtChar* into, const GxtChar* str);
    static void InsertPlayerControlKeysInString(GxtChar* str);

    static void Process();
    static void Display(bool flag);

    // NOTSA helpers
    static void InsertNumberInString2(const GxtChar* text, auto&& n, GxtChar* dst) {
        InsertNumberInString(text, n[0], n[1], n[2], n[3], n[4], n[5], dst);
    }
};
