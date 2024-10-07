#include "StdInc.h"

#include "Messages.h"
#include "Hud.h"
#include <ControllerConfigManager.h>

constexpr auto MSG_BUF_SZ = 400u;

void CMessages::InjectHooks() {
    RH_ScopedClass(CMessages);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetStringLength, 0x69DB50);
    RH_ScopedInstall(StringCopy, 0x69DB70, { .reversed = false });
    RH_ScopedInstall(StringCompare, 0x69DBD0);
    RH_ScopedInstall(CutString, 0x69DC50);
    RH_ScopedInstall(ClearMessages, 0x69DCD0);
    RH_ScopedInstall(ClearSmallMessagesOnly, 0x69DD30);
    RH_ScopedInstall(AddToPreviousBriefArray, 0x69DD50);
    RH_ScopedInstall(ClearPreviousBriefArray, 0x69DE70);
    RH_ScopedInstall(InsertNumberInString, 0x69DE90);
    RH_ScopedInstall(InsertStringInString, 0x69E040);
    RH_ScopedInstall(InsertPlayerControlKeysInString, 0x69E160);
    RH_ScopedInstall(AddMessageWithNumberQ, 0x69E360);
    RH_ScopedInstall(AddMessageJumpQWithNumber, 0x69E4E0);
    RH_ScopedInstall(AddBigMessageWithNumber, 0x69E5F0);
    RH_ScopedInstall(AddBigMessageWithNumberQ, 0x69E6E0);
    RH_ScopedInstall(AddMessageWithStringQ, 0x69E800);
    RH_ScopedInstall(AddMessageJumpQWithString, 0x69E950);
    RH_ScopedInstall(ClearThisPrint, 0x69EA30);
    RH_ScopedInstall(ClearThisBigPrint, 0x69EBE0);
    RH_ScopedInstall(ClearThisPrintBigNow, 0x69ED80);
    RH_ScopedInstall(Init, 0x69EE00);
    RH_ScopedInstall(ClearAllMessagesDisplayedByGame, 0x69EDC0);
    RH_ScopedInstall(Process, 0x69EE60);
    RH_ScopedInstall(Display, 0x69EFC0);
    RH_ScopedInstall(AddMessageQ, 0x69F0B0);
    RH_ScopedInstall(AddMessageJump, 0x69F1E0);
    RH_ScopedInstall(AddBigMessage, 0x69F2B0);
    RH_ScopedInstall(AddBigMessageQ, 0x69F370);
}

// Initialises messages
// 0x69EE00
void CMessages::Init() {
    ZoneScoped;

    ClearMessages(true);
    rng::fill(PreviousBriefs, tPreviousBrief{});
}

template<size_t N>
tMessage* FindUnusedMsgInArray(std::array<tMessage, N>& arr) {
    for (auto& msg : arr) {
        if (!msg.IsValid()) {
            return &msg;
        }
    }
    DEV_LOG("No free brief message found!");
    return nullptr;
}

/*********************
* BRIEF MESSAGES
**********************/

/*!
* Add a new brief message.
* @param text           The text (Possibly with format characters, eg.: ~~~~
* @param flag           Flags
* @param bPreviousBrief Whenever to call `AddToPreviousBriefArray`
* @param showInstantly  Whenever to show the message instantly
* @param str            String to insert [May be null]
* @param numbers        Numbers to insert (Use -1 as placeholder)
* @notsa
*/
void CMessages::AddMessage2(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief, bool showInstantly, GxtChar* str, std::optional<std::array<int32, 6>> numbers) {
    const auto CreateAt = [&](tMessage* p) {
        new (p) tMessage{
            text,
            str,
            flag,
            time,
            bPreviousBrief,
            numbers
        };
        if (bPreviousBrief && (showInstantly || p == &BriefMessages.front())) {
		    AddToPreviousBriefArray(
			    p->Text,
			    p->NumbersToInsert[0],
			    p->NumbersToInsert[1],
			    p->NumbersToInsert[2],
			    p->NumbersToInsert[3],
			    p->NumbersToInsert[4],
			    p->NumbersToInsert[5],
			    p->StringToInsert
            );
        }
    };
    if (showInstantly) {
        CreateAt(&BriefMessages[0]);
    } else if (const auto p = FindUnusedMsgInArray(BriefMessages)) {
        CreateAt(p);
    }
}

/*!
* Adds message to [Q]ueue
* @addr 0x69F0B0
*/
void CMessages::AddMessageQ(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief) { // Renamed from AddMessage
    /* Some string copy code here */
    AddMessage2(text, time, flag, bPreviousBrief, false);
}

/*!
* Adds message to [Q]ueue [With a string in `text` that will be replaced by `string`]
* @addr 0x69E800
*/
void CMessages::AddMessageWithStringQ(const GxtChar* text, uint32 time, uint16 flag, GxtChar* string, bool bPreviousBrief) {
    /* Some string copy code here */
    AddMessage2(text, time, flag, bPreviousBrief, false, string);
}

/*!
* Show a message instantly
* @addr 0x69F1E0
*/
void CMessages::AddMessageJump(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief) { // Renamed from AddMessageJumpQ
    /* unused string copy here */
    AddMessage2(text, time, flag, bPreviousBrief, true);
}

/*!
* Adds message to [Q]ueue [With numbers in `text`]
* @addr 0x69E360
*/
void CMessages::AddMessageWithNumberQ(const GxtChar* text, uint32 time, uint16 flag, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, bool bPreviousBrief) {
    /* Some string copy code here */
    AddMessage2(text, time, flag, bPreviousBrief, false, nullptr, { { n1, n2, n3, n4, n5, n6 } });
}

/*!
* Show a message instantly [With numbers in `text`]
* @addr 0x69E4E0
*/
void CMessages::AddMessageJumpQWithNumber(const GxtChar* text, uint32 time, uint16 flag, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, bool bPreviousBrief) {
    AddMessage2(text, time, flag, bPreviousBrief, true, nullptr, { { n1, n2, n3, n4, n5, n6 } });
}

/*!
* Show a message instantly [With a string in `text`]
* @addr 0x69E950
*/
void CMessages::AddMessageJumpQWithString(const GxtChar* text, uint32 time, uint16 flag, GxtChar* string, bool bPreviousBrief) {
    AddMessage2(text, time, flag, bPreviousBrief, true, string);
}

/*********************
* BIG MESSAGES
**********************/

/*!
* Add a new big message.
* @param text           The text (Possibly with format characters, eg.: ~~~~)
* @param bPreviousBrief Whenever to call `AddToPreviousBriefArray`
* @param showInstantly  Whenever to show the message instantly
* @param str            String to insert [May be null]
* @param numbers        Numbers to insert (Use -1 as placeholder)
* @notsa
*/
void CMessages::AddBigMessage2(const GxtChar* text, uint32 time, eMessageStyle style, bool showInstantly, GxtChar* str, std::optional<std::array<int32, 6>> numbers) {
    const auto CreateAt = [&](auto* ptr) {
        new (ptr) tMessage{
            text,
            str,
            0,
            time,
            false,
            numbers
        };
    };
    if (showInstantly) {
        CreateAt(&BIGMessages[style].Stack.front());
    } else if (const auto p = FindUnusedMsgInArray(BIGMessages[style].Stack)) {
        CreateAt(p);
    }
}

// Adds big message and shows it instantly
// 0x69F2B0
void CMessages::AddBigMessage(const GxtChar* text, uint32 time, eMessageStyle style) {
    AddBigMessage2(text, time, style, true);
}

// Adds big message to [Q]ueue
// 0x69F370
void CMessages::AddBigMessageQ(const GxtChar* text, uint32 time, eMessageStyle style) {
    AddBigMessage2(text, time, style, false);
}

// Adds big message with numbers and shows it instantly
// 0x69E5F0
void CMessages::AddBigMessageWithNumber(const GxtChar* text, uint32 time, eMessageStyle style, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6) {
    AddBigMessage2(text, time, style, true, nullptr, {{n1, n2, n3, n4, n5, n6}});
}

// Adds big message with numbers to [Q]ueue
// 0x69E6E0
void CMessages::AddBigMessageWithNumberQ(const GxtChar* text, uint32 time, eMessageStyle style, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6) {
    AddBigMessage2(text, time, style, false, nullptr, { {n1, n2, n3, n4, n5, n6} });
}

// Adds message to previous brief
// 0x69DD50
void CMessages::AddToPreviousBriefArray(const GxtChar* text, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, const GxtChar* string) {
    const auto numbers = std::array{ n1, n2, n3, n4, n5, n6 };

    // Find unused entry + duplicate check
    size_t i = 0;
    for (; i < PreviousBriefs.size(); i++) {
        const auto& prev = PreviousBriefs[i];
        if (!prev.Text) {
            break; // Found unused
        }
        // Check if it's a duplicate of the one we're about to insert
        if (prev.Text != text || prev.StringToInsert != string) { // I mean that's a pretty naive way, but okay
            continue;
        }
        if (numbers != prev.NumbersToInsert) {
            continue; 
        }
        return; // Duplicate messages after each other, so skip
    }

    // In case no free entries were found we overwrite the last one
    if (i == PreviousBriefs.size()) {
        i = PreviousBriefs.size() - 1;
    }

    // Move entries so that we overwrite the unused entry at `i` and thus free up the 0th entry
    // if `i == 0` this won't do anything.
    std::shift_right(
        PreviousBriefs.begin(),
        PreviousBriefs.begin() + i,
        1
    );

    // Now construct at the 0th entry (as it's now freed up)
    new (&PreviousBriefs[0]) tPreviousBrief {
        .Text            = text,
        .NumbersToInsert = numbers,
        .StringToInsert  = string,
    };
}

// Removes registered messages
// 0x69DCD0
void CMessages::ClearMessages(bool bIgnoreMissionTitle) {
    for (auto i = BIGMessages.size(); i-- > 0;) {
        if (bIgnoreMissionTitle || (i != STYLE_BOTTOM_RIGHT && i != STYLE_MIDDLE_SMALLER_HIGHER)) {
            BIGMessages[i] = {};
        }
    }
    ClearSmallMessagesOnly();
}

// Removes messages in brief
// 0x69DD30
void CMessages::ClearSmallMessagesOnly() {
    rng::fill(BriefMessages, tMessage{});
}

// Remove messages from previous brief
// 0x69DE70
void CMessages::ClearPreviousBriefArray() {
    rng::fill(PreviousBriefs, tPreviousBrief{});
}

template<size_t N>
void ClearThisPrint_Impl(std::array<tMessage, N>& messages, const GxtChar* text, auto&& OnFirstMessageCleared) {
    assert(text);
    for (;;) {
        size_t i = 0;
        for (;;) {
            const auto& msg = messages[i];
            if (!msg.IsValid()) {
                return; // Reached end of active texts
            }
            if (strcmp(AsciiFromGxtChar(msg.Text), AsciiFromGxtChar(text)) == 0) {
                break; // Texts match, so clear this
            }
            if (++i >= messages.size()) {
                return; // Reached end
            }
        }
        
        // Overwrite this element
        std::shift_left(
            messages.begin() + i,
            messages.end(),
            1
        );

        // Clear last (As it was left in an unspecified state)
        messages.back() = {};

        if (i == 0) {
            auto& msg = messages[i];
            msg.CreatedAtMS = CTimer::GetTimeInMS();
            if (OnFirstMessageCleared) {
                OnFirstMessageCleared(msg);
            }
        }
    }
}

// Removes small message with this text
// 0x69EA30
void CMessages::ClearThisPrint(const GxtChar* text) {
    ClearThisPrint_Impl(BriefMessages, text, [](tMessage& msg) {
        if (!msg.Text) {
            return;
        }
        AddToPreviousBriefArray(
            msg.Text,
            msg.NumbersToInsert[0],
            msg.NumbersToInsert[1],
            msg.NumbersToInsert[2],
            msg.NumbersToInsert[3],
            msg.NumbersToInsert[4],
            msg.NumbersToInsert[5],
            msg.StringToInsert
        );
    });
}

// Removes big message with this text
// 0x69EBE0
void CMessages::ClearThisBigPrint(const GxtChar* text) {
    assert(text);
    for (size_t b = 0; b < NUM_MESSAGE_STYLES; b++) {
        ClearThisPrint_Impl(BIGMessages[b].Stack, text, [](tMessage&) { /*nop*/ });
    }
}

// Removes first big message in messages stack
// 0x69ED80
void CMessages::ClearThisPrintBigNow(eMessageStyle style) {
    if (auto& msg = BIGMessages[style].Stack[0]; msg.Text) {
        ClearThisBigPrint(msg.Text);
    }
    CHud::BigMessageX[style] = 0.f;
    CHud::m_BigMessage[style][0] = 0;
}

// Removes all displayed messages
// 0x69EDC0
void CMessages::ClearAllMessagesDisplayedByGame(bool unk) {
    ZoneScoped;

    ClearMessages(unk);
    ClearPreviousBriefArray();
    CHud::GetRidOfAllHudMessages(unk);
}

// Returns length of a string
// 0x69DB50
uint32 CMessages::GetStringLength(const GxtChar* string) {
    return strlen(AsciiFromGxtChar(string));
}

// Copies string src to dest
// 0x69DB70
void CMessages::StringCopy(GxtChar* dest, const GxtChar* src, uint16 len) {
    //strncpy(dest, src, len); - TODO: Can't use this cause it's unsafe
    plugin::Call<0x69DB70, const GxtChar*, const GxtChar*, uint16>(dest, src, len);
}

/*!
* Check if 2 string are equal
* @addr 0x69DBD0
*/
bool CMessages::StringCompare(const GxtChar* str1, const GxtChar* str2, uint16 len) {
    return strncmp(AsciiFromGxtChar(str1), AsciiFromGxtChar(str1), len) == 0;
}

// 0x69DC50
void CMessages::CutString(int32 count, const GxtChar* str, GxtChar** dest) {
    NOTSA_UNREACHABLE(); // unused
}

template<size_t N>
void StringReplace(const char* haystack, const char (&needle)[N], char* dst, auto&& Replace) {
    // Based on https://stackoverflow.com/a/32413923
    auto        pDst = dst;
    const char* pHS  = haystack;
    
    for(;;) {
        const char* pNeedle = strstr(pHS, needle);

        // walked past last occurrence of needle; copy remaining part
        if (!pNeedle) {
            pDst = std::copy(pHS, pHS + strlen(pHS), pDst);
            break;
        }

        // copy part before needle
        pDst = std::copy(pHS, pNeedle, pDst);

        // replace portion of string
        pDst = Replace(pDst);

        // adjust pointers, move on
        pHS = pNeedle + (N - 1);
    }
    *pDst = 0; // Null terminate
}

// Insert numbers into string
// 0x69DE90
void CMessages::InsertNumberInString(const GxtChar* str, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, GxtChar* dst) {
    if (!str) { // Appereantly that's valid?
        *dst = 0; // Null terminate result anyways
        return;
    }
    StringReplace(AsciiFromGxtChar(str), "~1~", (char*)dst, [n = 0, numbers = std::array{ n1, n2, n3, n4, n5, n6 }](char* where) mutable {
        // Originally they used sprintf + AsciiToGxtChar, but this should work. We don't care about any substring except '~1~'.
        return std::format_to(where, "{}", numbers[n++]);
    });
}

// Inserts string into src
// 0x69E040
void CMessages::InsertStringInString(GxtChar* target, const GxtChar* replacement) {
    if (!target || !replacement) { // Appereantly that's valid?
        return;
    }

    GxtChar buf[MSG_BUF_SZ];
    notsa::string_copy((char*)buf, AsciiFromGxtChar(target), sizeof(buf));
    StringReplace(AsciiFromGxtChar(buf), "~1~", (char*)target, [&, szrepl = strlen(AsciiFromGxtChar(replacement))](char* where) {
        return std::copy(replacement, replacement + szrepl, where);
    });
}

// Inserts key events into string
// 0x69E160
void CMessages::InsertPlayerControlKeysInString(GxtChar* string) {
    GxtChar haystack[MSG_BUF_SZ];
    GxtCharStrcpy(haystack, string);

    // Based on https://stackoverflow.com/a/32413923
    auto        pDst = string;
    const auto* pHS  = haystack;

    // Skip over a portion in the haystack and copy it into `dst`
    const auto SkipTo = [&](const GxtChar* to) {
        pDst = std::copy(pHS, to, pDst);
        pHS  = to;
    };

    // An action is in the format of ~k~~ACTION_NAME_HERE~
    // We tolerate errors because this function is called for all strings printed on the screen
    // So in case it's a debug string that happens to contain ~k~ we don't want to `assert` and crash.
    for(;;) {
        const auto pNeedle = strstr(AsciiFromGxtChar(pHS), "~k~~");

        // walked past last occurrence of needle; copy remaining part (up to the end)
        if (!pNeedle) {
            SkipTo(pHS + strlen(AsciiFromGxtChar(pHS)));
            break;
        }

        // Now pneedle points at the beginning of ~k~~
        // there should be another `~` after it, and in
        // [pControlName, pend) should be the action name
        const auto pControlName = pNeedle + 4; // + 4 to skip `~k~~`

        const auto pNameEnd = strchr(pControlName, '~');  

        // If not found, skip `~k`, as that for sure won't be a valid format string the next time. (This way ~k~k would still work)
        if (!pNameEnd) {
            DEV_LOG("Closing tag of ~k~ not found [String: {}]", AsciiFromGxtChar(haystack));
            SkipTo(pHS + 2);
            continue;
        }
        
        // Okay, find this action's ID
        const auto actionName = std::string_view{ pControlName, pNameEnd };
        const auto actionId   = ControlsManager.GetActionIDByName(actionName);

        // If not found we know the location of the next `~`, so skip to there
        if (actionId == (uint16)-1) {
            SkipTo((const GxtChar*)pNameEnd);
            DEV_LOG("Invalid action name({}) [String: {}]", std::string_view{ pNeedle, pNameEnd }, AsciiFromGxtChar(haystack));
            continue;
        }

        // Copy stuff from before the control name (Using SkipTo for ease of life)
        SkipTo((const GxtChar*)pNeedle);

        // Get action name....
        char aname[256];
        ControlsManager.GetDefinedKeyByGxtName(actionId, aname, (uint16)std::size(aname));

        // ...and insert it into the string.
        // This is kinda shit for now, as we  don't know the 
        // size of the output (dst) buffer so we might as
        // well just be writing over the stack :)
        pDst = std::copy(aname, aname + strlen(aname), pDst);

        // We don't use SkipTo here, as we don't want to copy anything
        pHS  = GxtCharFromAscii(pNameEnd) + 1; // Go past the `~` that's after the control name
    }

    // null terminate
    *pDst = 0;
}

// Processing messages. This is called from CWorld::Process
// 0x69EE60
void CMessages::Process() {
    const auto ProcessMessagesArray = [](auto&& msgs) {
        auto& f = msgs.front();
        if (!f.IsValid()) { // Not even valid (Neither are the ones after it)
            return;
        }
        if (CTimer::GetTimeInMS() <= f.GetTimeToDisappearAtMS()) { // Still visible
            return;
        }
        std::destroy_at(&f); // First one disappeared, so we can delete it
        std::shift_right(msgs.begin(), msgs.end(), 1);
        std::destroy_at(&msgs.back()); // Last is def. unused now
        if (f.IsValid()) { // front is now another object (because of the shift)
            f.CreatedAtMS = CTimer::GetTimeInMS();
        }
    };

    // Process big messages
    for (auto& omgVeryBig : BIGMessages) {
        ProcessMessagesArray(omgVeryBig.Stack);
	}

    // Process briefs
    ProcessMessagesArray(BriefMessages);
    if (const auto f = BriefMessages.front(); f.IsValid()) {
		AddToPreviousBriefArray(
            f.Text,
            f.NumbersToInsert[0],
            f.NumbersToInsert[1],
            f.NumbersToInsert[2],
            f.NumbersToInsert[3],
            f.NumbersToInsert[4],
            f.NumbersToInsert[5],
            f.StringToInsert
        );
    }
}

// Displays messages
// 0x69EFC0
void CMessages::Display(bool bNotFading) {
    ZoneScoped;

    GxtChar msgText[MSG_BUF_SZ];
    const auto PreProcessMsgText = [&](tMessage msg) {
        InsertNumberInString(
            msg.Text,
            msg.NumbersToInsert[0],
            msg.NumbersToInsert[1],
            msg.NumbersToInsert[2],
            msg.NumbersToInsert[3],
            msg.NumbersToInsert[4],
            msg.NumbersToInsert[5],
            msgText
        );
        InsertStringInString(msgText, msg.StringToInsert);
        InsertPlayerControlKeysInString(msgText);
    };

    if (bNotFading) {
        for (auto&& [style, omgVeryBig] : notsa::enumerate(BIGMessages)) {
            const auto& msg = omgVeryBig.Stack.front();
            if (!msg.IsValid()) {
                continue;
            }
            PreProcessMsgText(msg);
            CHud::SetBigMessage(msgText, (eMessageStyle)style);
        }
    }

    if (bNotFading == CTheScripts::bDrawSubtitlesBeforeFade) {
        PreProcessMsgText(BriefMessages[0]);
        CHud::SetMessage(msgText);
    }
}
