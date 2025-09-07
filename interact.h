#ifndef INTERACT_H
#define INTERACT_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#endif


#define MAX_INPUT_CHARS 2048
#define MAX_INPUT_CHARS_ARRAY_LEN 2049 //+1

// --- Ollama config (can later be loaded from a file) ---
#define OLLAMA_MAX_REQ   16384
#define OLLAMA_MAX_RESP 262144

static char g_ollamaHost[64] = "127.0.0.1";
static int  g_ollamaPort = 11434;
static char g_ollamaModel[32] = "llama3";

// Response buffer (null-terminated text to display)
static char g_ollamaResponse[OLLAMA_MAX_RESP];

// Simple state flags
#ifdef _WIN32
static volatile LONG g_ollamaBusy = 0;  // 0 = idle, 1 = in-flight
static volatile LONG g_ollamaDone = 0;  // 1 = a fresh reply is available
static HANDLE g_ollamaThread = NULL;
#endif


typedef enum {
    TALK_TYPE_TOL
} TALK_TYPE;

typedef enum {
    POI_TYPE_NONE = -1,
    POI_TYPE_TRUCK,
    POI_TYPE_TREE_OF_LIFE,
    POI_TYPE_TOTAL_COUNT
} POI_Type;

// Type Definitions
typedef struct {
    POI_Type type;
    Vector3* pos;
} POI;

POI InteractivePoints[POI_TYPE_TOTAL_COUNT];
char *TalkInput;      // NOTE: One extra space required for null terminator char '\0'
int LetterCount;

void InitTalkingInteractions()
{
    TalkInput = (char*)malloc(sizeof(char) * MAX_INPUT_CHARS_ARRAY_LEN);
    TalkInput[0] = '\0';
    LetterCount = 0;
}
// Check if any key is pressed
// NOTE: We limit keys check to keys between 32 (KEY_SPACE) and 126
bool IsAnyKeyPressed()
{
    bool keyPressed = false;
    int key = GetKeyPressed();

    if ((key >= 32) && (key <= 126)) keyPressed = true;

    return keyPressed;
}

// Draw text using font inside rectangle limits with support for text selection
static void DrawTextBoxedSelectable(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;          // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize / (float)font.baseSize;     // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;         // Index where to begin drawing (where a line begins)
    int endLine = -1;           // Index where to stop drawing (where a line ends)
    int lastk = -1;             // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in startLine and endLine, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container)
        // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container
        if (state == MEASURE_STATE)
        {
            // TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // Ref: http://jkorpela.fi/chars/spaces.html
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1) ? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n') state = !state;

            if (state == DRAW_STATE)
            {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (codepoint == '\n')
            {
                if (!wordWrap)
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize * scaleFactor) > rec.height) break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec((Rectangle) { rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize* scaleFactor }, selectBackTint);
                    isGlyphSelected = true;
                }

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, (Vector2) { rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, isGlyphSelected ? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;  // avoid leading spaces
    }
}

// Draw text using font inside rectangle limits
static void DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}

// Minimal JSON escaper for prompt -> JSON string value
static size_t JsonEscape(const char* in, char* out, size_t cap) {
    size_t i = 0;
    for (const unsigned char* p = (const unsigned char*)in; *p && i < cap - 1; ++p) {
        unsigned char c = *p;
        if (c == '\"' || c == '\\') { if (i + 2 >= cap) break; out[i++] = '\\'; out[i++] = (char)c; }
        else if (c == '\n') { if (i + 2 >= cap) break; out[i++] = '\\'; out[i++] = 'n'; }
        else if (c == '\r') { if (i + 2 >= cap) break; out[i++] = '\\'; out[i++] = 'r'; }
        else if (c == '\t') { if (i + 2 >= cap) break; out[i++] = '\\'; out[i++] = 't'; }
        else if (c < 0x20) { // control -> \u00XX
            if (i + 6 >= cap) break;
            i += (size_t)snprintf(out + i, cap - i, "\\u%04x", c);
        }
        else out[i++] = (char)c;
    }
    out[i] = 0;
    return i;
}

// Pull "response": "..." out of Ollama's JSON (stream:false)
static void ExtractResponseText(const char* json, char* out, size_t cap) {
    const char* p = strstr(json, "\"response\"");
    if (!p) { // fallback: dump raw json (truncated) if format surprises us
        strncpy(out, json, cap - 1); out[cap - 1] = 0; return;
    }
    p = strchr(p, ':'); if (!p) { out[0] = 0; return; }
    p++;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\"') p++;

    size_t i = 0;
    int esc = 0;
    for (; *p && i < cap - 1; ++p) {
        char c = *p;
        if (esc) {
            switch (c) {
            case 'n': out[i++] = '\n'; break;
            case 'r': out[i++] = '\r'; break;
            case 't': out[i++] = '\t'; break;
            case '\\': out[i++] = '\\'; break;
            case '"': out[i++] = '"'; break;
            case 'u': /* naive: skip \uXXXX */ p += 4; break;
            default: out[i++] = c; break;
            }
            esc = 0;
        }
        else if (c == '\\') esc = 1;
        else if (c == '\"') break;
        else out[i++] = c;
    }
    out[i] = 0;
}

#ifdef _WIN32
static DWORD WINAPI OllamaThreadProc(LPVOID lp) {
    char* heapPrompt = (char*)lp;
    InterlockedExchange(&g_ollamaDone, 0);
    g_ollamaResponse[0] = '\0';

    // Wide host
    wchar_t whost[64];
    MultiByteToWideChar(CP_UTF8, 0, g_ollamaHost, -1, whost, (int)(sizeof(whost) / sizeof(wchar_t)));

    // Session / connect / request
    HINTERNET hSession = WinHttpOpen(L"Donogan/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, NULL, NULL, 0);
    if (!hSession) goto cleanup;

    HINTERNET hConnect = WinHttpConnect(hSession, whost, (INTERNET_PORT)g_ollamaPort, 0);
    if (!hConnect) goto cleanup;

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"POST", L"/api/generate", NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) goto cleanup;

    // Header
    const wchar_t* hdr = L"Content-Type: application/json\r\n";
    WinHttpAddRequestHeaders(hRequest, hdr, (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);

    // Body
    char esc[OLLAMA_MAX_REQ / 2];
    JsonEscape(heapPrompt ? heapPrompt : "", esc, sizeof(esc));

    char body[OLLAMA_MAX_REQ];
    int bodyLen = snprintf(body, sizeof(body),
        "{"
        "\"model\":\"%s\","
        "\"prompt\":\"%s\","
        "\"stream\":false"
        "}",
        g_ollamaModel, esc);

    BOOL ok = WinHttpSendRequest(
        hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)body, (DWORD)bodyLen, (DWORD)bodyLen, 0);
    if (!ok) goto cleanup;

    ok = WinHttpReceiveResponse(hRequest, NULL);
    if (!ok) goto cleanup;

    // Read response
    DWORD avail = 0, read = 0;
    size_t wr = 0;
    g_ollamaResponse[0] = '\0';
    do {
        if (!WinHttpQueryDataAvailable(hRequest, &avail)) break;
        if (avail == 0) break;
        char tmp[4096];
        DWORD toRead = (avail > sizeof(tmp)) ? (DWORD)sizeof(tmp) : avail;
        if (!WinHttpReadData(hRequest, tmp, toRead, &read)) break;
        if (read == 0) break;
        size_t left = (OLLAMA_MAX_RESP - 1) - wr;
        size_t copy = (read < left) ? read : left;
        if (copy > 0) { memcpy(g_ollamaResponse + wr, tmp, copy); wr += copy; g_ollamaResponse[wr] = 0; }
    } while (read > 0);

    // Extract just the "response" text into the same buffer
    {
        char extracted[OLLAMA_MAX_RESP];
        ExtractResponseText(g_ollamaResponse, extracted, sizeof(extracted));
        strncpy(g_ollamaResponse, extracted, sizeof(g_ollamaResponse) - 1);
        g_ollamaResponse[sizeof(g_ollamaResponse) - 1] = 0;
    }

cleanup:
    if (hRequest)  WinHttpCloseHandle(hRequest);
    if (hConnect)  WinHttpCloseHandle(hConnect);
    if (hSession)  WinHttpCloseHandle(hSession);
    if (heapPrompt) free(heapPrompt);

    InterlockedExchange(&g_ollamaBusy, 0);
    InterlockedExchange(&g_ollamaDone, 1);
    return 0;
}
#endif

// API you call from game code:
static bool StartOllamaGenerate(const char* prompt) {
#ifdef _WIN32
    if (InterlockedCompareExchange(&g_ollamaBusy, 1, 0) != 0) return false; // already running
    g_ollamaResponse[0] = '\0';
    InterlockedExchange(&g_ollamaDone, 0);
    char* copy = _strdup(prompt ? prompt : "");
    g_ollamaThread = CreateThread(NULL, 0, OllamaThreadProc, copy, 0, NULL);
    if (!g_ollamaThread) { InterlockedExchange(&g_ollamaBusy, 0); if (copy) free(copy); return false; }
    return true;
#else
    (void)prompt; return false;
#endif
}

static bool OllamaIsBusy(void) {
#ifdef _WIN32
    return InterlockedCompareExchange(&g_ollamaBusy, 0, 0) != 0;
#else
    return false;
#endif
}

static bool OllamaHasReply(void) {
#ifdef _WIN32
    return InterlockedCompareExchange(&g_ollamaDone, 0, 0) != 0;
#else
    return false;
#endif
}

void GetKeyBoardInput()
{
    // Get char pressed (unicode character) on the queue
    int key = GetCharPressed();

    // Check if more characters have been pressed on the same frame
    while (key > 0)
    {
        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && (LetterCount < MAX_INPUT_CHARS))
        {
            TalkInput[LetterCount] = (char)key;
            TalkInput[LetterCount + 1] = '\0'; // Add null terminator at the end of the string
            LetterCount++;
        }

        key = GetCharPressed();  // Check next character in the queue
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        LetterCount--;
        if (LetterCount < 0) { LetterCount = 0; }
        TalkInput[LetterCount] = '\0';
    }
    // Kick off the call when Enter is pressed and we’re not busy
    if (IsKeyPressed(KEY_ENTER) && LetterCount > 0 && !OllamaIsBusy()) {
        StartOllamaGenerate(TalkInput);
        // optional: clear the input for next message
        TalkInput[0] = '\0';
        LetterCount = 0;
    }
}

static const char* OllamaGetReply(void) { return g_ollamaResponse; }

#endif // INTERACT_H
