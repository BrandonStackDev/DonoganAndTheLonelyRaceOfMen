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
    TALK_TYPE_TOL_2,
    TALK_TYPE_ATREYU,
    TALK_TYPE_ATREYU_BOW,
    TALK_TYPE_DARREL,
    TALK_TYPE_LUCY_ONE,
    TALK_TYPE_LUCY_TWO,
    TALK_TYPE_NICK,
    TALK_TYPE_WIZARD,
    TALK_TYPE_ABBY, //no electricity
    TALK_TYPE_ABBY_2, //electricity but needs medicine
    TALK_TYPE_ABBY_3, //has the medicine, not yet given, (yes,no)
    TALK_TYPE_ABBY_4, //has given the medicine and the electricity
    TALK_TYPE_GAL_1, //if the user has no good books
    TALK_TYPE_GAL_2, //if the user has any good books, and has not given any to galadriel
    TALK_TYPE_GAL_3, //the user has given all 10 good books to galadriel
    TALK_TYPE_ROGER,
    TALK_TYPE_GEOFF,
    TALK_TYPE_MARY,
    TALK_TYPE_JARED,
    TALK_TYPE_STORE
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
    MISSION_KILL_SKEL,
    MISSION_FART_WHALE,
    MISSION_RESCUE_NICK,
    MISSION_ABBY_LIGHT,
    MISSION_ABBY_RX,
    MISSION_START_ALL_MILLS,
    MISSION_LIGHT_ALL_FIRES,
    MISSION_ALL_MAPS,
    MISSION_GALADRIEL,
    MISSION_ALISTER,
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
    POI_TYPE_STORE_1,
    POI_TYPE_GALADRIEL,
    POI_TYPE_ROGER,
    POI_TYPE_GEOFF,
    POI_TYPE_MARY,
    POI_TYPE_JARED,
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
    FIREPIT_WRENVILLE,
    FIREPIT_CINDERSPIRE,
    FIREPIT_ELYNDOR_ISLAND,
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
    TALK_OPTION_STORE
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

//must define in order of enum
static TalkData talkData[] = {
    {
        TALK_TYPE_TOL,
        "Tree of Life",
        {
            "I am the Tree of Life, old and wise.",
            "But this land tires me. I am so old. I cant keep the trees as pretty as I used to.",
            "Could you try to hold your square spell for a while? Maybe you can help the dead trees grow again?"
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_TOL_2,
        "Tree of Life",
        {
            "Thank you Donogan. I have bloomed, and now ... it is my time to die, to become mulch, in the soil...",
            "[Donogan] No! Wait! I...",
            "Im just kidding. I totally had you going. Im fine. Everything is wonderful.",
            "If you need to find him, Atreyu used to meditate on a mountain top kind of near his old village."
        },
        4,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_ATREYU,
        "Atreyu",
        {
            "(uhg!) Hello, I am Atreyu.",
            "Please... keep your voice low. I am trying to listen.",
            "Not to you. To everything else.",
            "The wind, the grass... even the silence between them.",
            "People think the world is quiet when they stop talking.",
            "But it is only then that it finally begins to speak.",
            "Out here, nothing is alone. Not the trees, not the sky... not even us.",
            "If we learn to listen, we might remember how to belong again."
        },
        8,
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
            "Oh wow! I cant beleive this tiny tree is the Tree of Life!? The world is so strange.",
            "[Donogan] You said it pal!"
        },
        4,
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
            "I wanted to tell you a hint or two.",
            "The Tree of Life is near the center of the mainland, in a large gorge.",
            "And Atreyu is meditating on a high mountain top.",
            "Atreyu's canoe can be a clue for you."
        },
        5,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_NICK,
        "Nick",
        {
            "Thank you for rescuing me from those robot orbs!",
            "I thought I was finished. Who built those stupid things anyway?",
            "I owe you one, Donogan."
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_WIZARD,
        "Blue Wizard",
        {
            "Ah, Donogan. The world is older than your map.",
            "Find the good books. Beware the shadow books.",
            "[Donogan] Why, what are they for?",
            "Because Donogan, you are the Queso Cataracts! The ultimate form of power this world has ever known!",
            "[Donogan] You want me to shove cheese into my eyeballs?!",
            "No!, I mean ... No, just find the maps and the books. You'll want to use X to jump, and O to roll. Triangle to interact with your environment.",
            "L3 lets you run and jump higher. Left joystick is to move and Right joystick is for the camera.",
            "Use square for your freeze attack spell and sometimes to cast other spells, and R2 to shoot radiant spell-spheres."
        },
        8,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_ABBY,
        "Abby",
        {
            "Donogan, our electricity is out.",
            "Can you start that nearby windmill so we have light?"
        },
        2,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_ABBY_2,
        "Abby",
        {
            "Donogan, Thank you so much. Now we dont have to worry.",
            "Can I ask you another favor?",
            "My father is anemic, he needs medicine.",
            "Can you find medicine for my father?"
        },
        4,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_ABBY_3,
        "Abby",
        {
            "Can you spare medicine for my father?"
        },
        1,
        TALK_OPTION_YES_NO
    },
    {
        TALK_TYPE_ABBY_4,
        "Abby",
        {
            "Donogan, Thank you so much. Now we dont have to worry.",
            "I wanted to tell you, you can use R2 to shoot spell balls.",
            "Also, R1 and L1 buttons are for melee attacks.",
            "If you have the wrench or guitar, they will be used in place of a melee attack."
        },
        4,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_STORE,
        "Store Clerk",
        {
            "Welcome to my store.",
            "Use Up and Down to pick an item.",
            "Press Jump to buy, or Interact to leave."
        },
        3,
        TALK_OPTION_STORE
    },
    {
        TALK_TYPE_GAL_1,
        "Galadriel",
        {
            "Donogan, I seek the good books.",
            "Bring them to me, and I will pay you $80 for each one.",
            "Beware the shadow books. I have no use for those."
        },
        3,
        TALK_OPTION_OK
    },
    {
        TALK_TYPE_GAL_2,
        "Galadriel",
        {
            "Ah, you have found good books. Will you sell them to me for $80 each? (X to accept Triangle to decline)",
        },
        1,
        TALK_OPTION_YES_NO
    },
    {
        TALK_TYPE_GAL_3,
        "Galadriel",
        {
            "You have brought me all ten good books.",
            "Take this guitar, Donogan.",
            "Press L1 to use it as an attack.",
            "May its song bonk evil directly in the forehead."
        },
        4,
        TALK_OPTION_OK
    },
    {
    TALK_TYPE_ROGER,
    "Roger",
    {
        "Did you know, there are maps hidden across the islands?",
        "Not all of them are easy to find... some are tucked away in strange places.",
        "Old ruins, quiet cliffs, even places people stopped visiting long ago.",
        "If you collect them, they will reveal more of the world to you.",
        "[Donogan] Great, do you know where any are?",
        "No, sorry. There is no map for maps."
    },
    6,
    TALK_OPTION_OK
    },
{
    TALK_TYPE_GEOFF,
    "Geoff",
    {
        "Hey Donogan, I think I saw something you might want.",
        "On top of the water wheel just outside of Wrenville.",
        "It looked like a map, just sitting up high... kind of strange, honestly.",
        "I didnt grab it because the windmill needs to be started to get up on the platforms.",
        "You should check out there if you havent already."
    },
    5,
    TALK_OPTION_OK
},
{
    TALK_TYPE_MARY,
    "Mary",
    {
        "Donogan, your square spell can freeze enemies.",
        "If you spin, then you can throw them.",
        "But it can do more than that. It can even grow things.",
        "You can grow pumpkins and flowers from small garden patches.",
        "Also certain trees let you grow berries and apples. You press triangle (interact) to collect them.",
        "But the apples youll have to find a way to get them down from the tree...Sorry I dont have a ladder."
    },
    6,
    TALK_OPTION_OK
},
{
    TALK_TYPE_JARED,
    "Mary",
    {
        "Hey Donogan! (Cough) Im smoking two ... (Cough) ... because Clarence ditched me again. That chicken!",
        "[Donogan] Do you know where he is?",
        "No, sometimes he likes to look for whales tho, off the coast. Maybe he is down the road near the open sea?"
    },
    3,
    TALK_OPTION_OK
},
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
    missions[MISSION_START_ALL_MILLS].name = "Start all windmills";
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
    //kill skeletons
    missions[MISSION_KILL_SKEL].complete = false;
    missions[MISSION_KILL_SKEL].type = MISSION_KILL_SKEL;
    missions[MISSION_KILL_SKEL].name = "Kill 25 Skeletons";
    missions[MISSION_KILL_SKEL].desc = "Kill 25 Skeletons in a single session.";
    //rescue nick
    missions[MISSION_RESCUE_NICK].complete = false;
    missions[MISSION_RESCUE_NICK].type = MISSION_RESCUE_NICK;
    missions[MISSION_RESCUE_NICK].name = "Rescue Nick from the Robot Orbs";
    missions[MISSION_RESCUE_NICK].desc = "Press triangle near nick to rescue him.";
    //fart on whale
    missions[MISSION_FART_WHALE].complete = false;
    missions[MISSION_FART_WHALE].type = MISSION_FART_WHALE;
    missions[MISSION_FART_WHALE].name = "Fart on a Whale";
    missions[MISSION_FART_WHALE].desc = "Blow bubbles under water at a whale";
    //abby 1
    missions[MISSION_ABBY_LIGHT].complete = false;
    missions[MISSION_ABBY_LIGHT].type = MISSION_ABBY_LIGHT;
    missions[MISSION_ABBY_LIGHT].name = "Get Electricity to Abby's Home";
    missions[MISSION_ABBY_LIGHT].desc = "Get electricity to Abby's home by starting the windmill nearby.";
    //abby 2
    missions[MISSION_ABBY_RX].complete = false;
    missions[MISSION_ABBY_RX].type = MISSION_ABBY_RX;
    missions[MISSION_ABBY_RX].name = "Get medicine for Abby's father";
    missions[MISSION_ABBY_RX].desc = "Get medicine for Abby's father at a nearby store.";
    //MISSION_LIGHT_ALL_FIRES
    missions[MISSION_LIGHT_ALL_FIRES].complete = false;
    missions[MISSION_LIGHT_ALL_FIRES].type = MISSION_LIGHT_ALL_FIRES;
    missions[MISSION_LIGHT_ALL_FIRES].name = "Light all fires";
    missions[MISSION_LIGHT_ALL_FIRES].desc = "Light all fire-pits.";
    //MISSION_ALL_MAPS
    missions[MISSION_ALL_MAPS].complete = false;
    missions[MISSION_ALL_MAPS].type = MISSION_ALL_MAPS;
    missions[MISSION_ALL_MAPS].name = "Find all of the maps";
    missions[MISSION_ALL_MAPS].desc = "Find all of the maps.";
    //MISSION_GALADRIEL
    missions[MISSION_GALADRIEL].complete = false;
    missions[MISSION_GALADRIEL].type = MISSION_GALADRIEL;
    missions[MISSION_GALADRIEL].name = "Give 10 good books to Galadriel";
    missions[MISSION_GALADRIEL].desc = "Give 10 good books to Galadriel.";
    //MISSION_ALISTER
    missions[MISSION_ALISTER].complete = false;
    missions[MISSION_ALISTER].type = MISSION_ALISTER;
    missions[MISSION_ALISTER].name = "Give 10 shadow books to Alister";
    missions[MISSION_ALISTER].desc = "Give 10 shadow books to Alister.";
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
    TalkData* t = GetTalkData(g_currentTalkWho);
    if (!t) return TALK_RESULT_NONE;

    switch (t->optionType)
    {
    case TALK_OPTION_OK:
        if (triPressed) return TALK_RESULT_FINISHED;
        if (xPressed) return Talk_Advance();
        break;

    case TALK_OPTION_YES_NO:
        if (xPressed) return TALK_RESULT_YES;
        if (triPressed) return TALK_RESULT_NO;
        break;

    case TALK_OPTION_STORE:
        return TALK_RESULT_NONE;
    }

    return TALK_RESULT_NONE;
}


#endif // INTERACT_H
