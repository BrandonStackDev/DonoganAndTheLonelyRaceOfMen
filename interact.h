#ifndef INTERACT_H
#define INTERACT_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>

typedef enum {
    TALK_TYPE_TOL,
    TALK_TYPE_ATREYU,
    TALK_TYPE_ATREYU_BOW,
    TALK_TYPE_DARREL,
    TALK_TYPE_LUCY_ONE,
    TALK_TYPE_LUCY_TWO,
    TALK_TYPE_NICK,
    TALK_TYPE_WIZARD,
    TALK_TYPE_ABBY
} TALK_TYPE;
static TALK_TYPE g_currentTalkWho = TALK_TYPE_TOL; // sane default

//missions
typedef enum {
    MISSION_NONE = -1,
    MISSION_CLARENCE_CHICKEN,
    MISSION_GET_TRUCK,
    MISSION_FIND_ATREYU,
    MISSION_FIND_TOL,
    MISSION_KILL_GHOST,
    MISSION_KILL_YETI,
    MISSION_KILL_ROBO,
    MISSION_START_ALL_MILLS,
    MISSION_RESCUE_NICK,
    MISSION_FART_WHALE,
    MISSION_TOTAL_COUNT
} MissionType;

typedef struct {
    MissionType type;
    bool complete;
    char* name;
    char* desc;
} Mission;

//points of interest, for tri interact
typedef enum {
    POI_TYPE_NONE = -1,
    POI_TYPE_TRUCK,
    POI_TYPE_TREE_OF_LIFE,
    POI_TYPE_ATREYU,
    POI_TYPE_DARREL,
    POI_TYPE_CHICKEN,
    POI_TYPE_LUCY,
    POI_TYPE_NICK,
    POI_TYPE_WIZARD,
    POI_TYPE_ABBY,
    POI_TYPE_TOTAL_COUNT
} POI_Type;

typedef struct {
    POI_Type type;
    Vector3* pos;
} POI;

//fireplaces
typedef enum {
    FIREPIT_HOME = 0,
    FIREPIT_CASTLE,
    FIREPIT_VILLAGE,
    FIREPIT_COTTAGE,
    FIREPIT_BARN,
    FIREPIT_YETI_MT,
    FIREPIT_RAP,
    FIREPIT_TOTAL_COUNT
} FirepitLocation;

typedef struct {
    FirepitLocation location;
    Vector3 pos;
    bool lit;
    char* name;
} Firepit;

Mission missions[MISSION_TOTAL_COUNT];
Firepit fires[FIREPIT_TOTAL_COUNT];
POI InteractivePoints[POI_TYPE_TOTAL_COUNT];

typedef enum {
    TALK_OPTION_OK = 0,
    TALK_OPTION_YES_NO,
} TalkOptionType;

typedef enum {
    TALK_RESULT_NONE = 0,
    TALK_RESULT_ADVANCED,
    TALK_RESULT_FINISHED,
    TALK_RESULT_YES,
    TALK_RESULT_NO,
} TalkResult;

#define TALK_LINE_MAX 8

typedef struct TalkData {
    TALK_TYPE type;
    const char* speaker;
    const char* lines[TALK_LINE_MAX];
    int lineCount;
    TalkOptionType optionType;
} TalkData;

static TalkData talkData[] = {
    {
        TALK_TYPE_TOL,
        "Tree of Life",
        {
            "I am the Tree of Life, old and wise.",
            "The books you seek are scattered across this lonely world.",
            "Some books heal the soul. Some books darken it."
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_ATREYU,
        "Atreyu",
        {
            "(uhg!) Hello, I am Atreyu.",
            "Please be quiet. I am trying to meditate.",
            "The wind carries old names through these mountains."
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_ATREYU_BOW,
        "Atreyu",
        {
            "Here is my bow. You may have it.",
            "Hold L2 to aim.",
            "Press R2 to fire."
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_DARREL,
        "Darrel",
        {
            "This is the Tree of Life.",
            "It is kind of small...?",
            "Sometimes I hear a voice saying, 'I am over here.'"
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_LUCY_ONE,
        "Lucy",
        {
            "Clarence my chicken is missing!",
            "Will you help me look for him?",
            "He is brave, but not especially smart.",
            "Im worried he might be taking pot!?"
        },
        4,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_LUCY_TWO,
        "Lucy",
        {
            "Thank you for finding Clarence!",
            "I wanted to tell you a hint.",
            "The Tree of Life is near the center of the mainland, in a large gorge."
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_NICK,
        "Nick",
        {
            "Thank you for rescuing me from those robot orbs!",
            "I thought I was finished.",
            "I owe you one, Donogan."
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_WIZARD,
        "Blue Wizard",
        {
            "Ah, Donogan.",
            "The world is older than your map.",
            "Find the good books. Beware the shadow books.",
            "[Donogan] Why, what are they for?",
            "Donogan, you are the Queso Cataracts!",
            "[Donogan] You want me to shove cheese into my eyeballs?!",
            "No!, I mean ... No, just find the maps and the books. And Hover mode for the Truck is near the castle."
        },
        7,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_ABBY,
        "Abby",
        {
            "Donogan, our electricity is out.",
            "Can you start that windmill so we have light?"
        },
        2,
        TALK_OPTION_OK
    }
};

static int talkLineIndex = 0;
void InitTalkingInteractions()
{
    g_currentTalkWho = TALK_TYPE_TOL;
    talkLineIndex = 0;
}

static inline int TalkDataCount(void)
{
    return (int)(sizeof(talkData) / sizeof(talkData[0]));
}

static inline TalkData* GetTalkData(TALK_TYPE type)
{
    for (int i = 0; i < TalkDataCount(); i++)
    {
        if (talkData[i].type == type) return &talkData[i];
    }

    return &talkData[0];
}

static inline void Talk_Reset(TALK_TYPE type)
{
    g_currentTalkWho = type;
    talkLineIndex = 0;
}

static inline TalkResult Talk_Advance(void)
{
    TalkData* t = GetTalkData(g_currentTalkWho);
    if (!t) return TALK_RESULT_NONE;

    if (talkLineIndex < t->lineCount - 1)
    {
        talkLineIndex++;
        return TALK_RESULT_ADVANCED;
    }

    return TALK_RESULT_FINISHED;
}

static inline const char* Talk_GetSpeaker(void)
{
    return GetTalkData(g_currentTalkWho)->speaker;
}

static inline const char* Talk_GetLine(void)
{
    TalkData* t = GetTalkData(g_currentTalkWho);
    if (!t || t->lineCount <= 0) return "";

    if (talkLineIndex < 0) talkLineIndex = 0;
    if (talkLineIndex >= t->lineCount) talkLineIndex = t->lineCount - 1;

    return t->lines[talkLineIndex];
}

void InitMissions()
{
    //MISSION_CLARENCE_CHICKEN
    missions[MISSION_CLARENCE_CHICKEN].complete = false;
    missions[MISSION_CLARENCE_CHICKEN].type = MISSION_CLARENCE_CHICKEN;
    missions[MISSION_CLARENCE_CHICKEN].name = "Find Clarence";
    missions[MISSION_CLARENCE_CHICKEN].desc = "Lucy's chicken Clarence is missing, find him and bring him to Lucy.";
    //find and talk to atreyu
    missions[MISSION_FIND_ATREYU].complete = false;
    missions[MISSION_FIND_ATREYU].type = MISSION_FIND_ATREYU;
    missions[MISSION_FIND_ATREYU].name = "Find and Talk to Atreyu";
    missions[MISSION_FIND_ATREYU].desc = "Find and Talk to Atreyu. He has a gift for you.";
    //MISSION_GET_TRUCK
    missions[MISSION_GET_TRUCK].complete = false;
    missions[MISSION_GET_TRUCK].type = MISSION_GET_TRUCK;
    missions[MISSION_GET_TRUCK].name = "Get the Truck";
    missions[MISSION_GET_TRUCK].desc = "Get the wrench so you can lower the truck from the lift.";
    //MISSION_FIND_TOL,
    missions[MISSION_FIND_TOL].complete = false;
    missions[MISSION_FIND_TOL].type = MISSION_FIND_TOL;
    missions[MISSION_FIND_TOL].name = "Find and Talk to the Tree of Life";
    missions[MISSION_FIND_TOL].desc = "Find and Talk to the Tree of Life. It has much wisdom and many poems.";
    //MISSION_START_ALL_MILLS
    missions[MISSION_START_ALL_MILLS].complete = false;
    missions[MISSION_START_ALL_MILLS].type = MISSION_START_ALL_MILLS;
    missions[MISSION_START_ALL_MILLS].name = "Start all windmills.";
    missions[MISSION_START_ALL_MILLS].desc = "Start all of the windmills by pressing interact near the machine inside. (requires the wrench)";
    //MISSION_KILL_GHOST,
    missions[MISSION_KILL_GHOST].complete = false;
    missions[MISSION_KILL_GHOST].type = MISSION_KILL_GHOST;
    missions[MISSION_KILL_GHOST].name = "Kill 10 Ghosts";
    missions[MISSION_KILL_GHOST].desc = "Kill 10 Ghosts in a single session.";
    //MISSION_KILL_YETI,
    missions[MISSION_KILL_YETI].complete = false;
    missions[MISSION_KILL_YETI].type = MISSION_KILL_YETI;
    missions[MISSION_KILL_YETI].name = "Kill 10 Yetis";
    missions[MISSION_KILL_YETI].desc = "Kill 10 Yetis in a single session.";
    //kill robo
    missions[MISSION_KILL_ROBO].complete = false;
    missions[MISSION_KILL_ROBO].type = MISSION_KILL_ROBO;
    missions[MISSION_KILL_ROBO].name = "Kill 10 Robot Orbs";
    missions[MISSION_KILL_ROBO].desc = "Kill 10 Robot Orbs in a single session.";
    //rescue nick
    missions[MISSION_RESCUE_NICK].complete = false;
    missions[MISSION_RESCUE_NICK].type = MISSION_RESCUE_NICK;
    missions[MISSION_RESCUE_NICK].name = "Rescue Nick from the Robot Orbs";
    missions[MISSION_RESCUE_NICK].desc = "Press triangle near nick to rescue him.";
    //far on whale
    missions[MISSION_FART_WHALE].complete = false;
    missions[MISSION_FART_WHALE].type = MISSION_FART_WHALE;
    missions[MISSION_FART_WHALE].name = "Fart on a Whale";
    missions[MISSION_FART_WHALE].desc = "Blow bubbles under water at a whale.";
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

static inline TalkResult Talk_UpdateController(bool xPressed, bool triPressed)
{
    if (triPressed)
    {
        return TALK_RESULT_FINISHED;
    }

    TalkData* t = GetTalkData(g_currentTalkWho);
    if (!t) return TALK_RESULT_NONE;

    switch (t->optionType)
    {
    case TALK_OPTION_OK:
        if (xPressed)
        {
            return Talk_Advance();
        }
        break;

    case TALK_OPTION_YES_NO:
        // later:
        // dpad left/right picks yes/no
        // X confirms
        if (xPressed)
        {
            return TALK_RESULT_YES;
        }
        break;
    }

    return TALK_RESULT_NONE;
}


#endif // INTERACT_H
