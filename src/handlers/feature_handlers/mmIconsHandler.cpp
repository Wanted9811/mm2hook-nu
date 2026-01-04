#include "mmIconsHandler.h"

using namespace MM2;

/*
    mmIconsHandler
*/
int opponentIconStyle = 0;
int opponentIconColor = 5;
int blitzIconColor = 6;
bool blitzIconMultiColor = false;
bool opponentIconTransparency = false;
bool blitzIconTransparency = false;

static ConfigValue<bool> cfgBlitzIconMultiColor("BlitzIconMultiColor", false);
static ConfigValue<bool> cfgOpponentIconTransparency("OpponentIconTransparency", false);
static ConfigValue<bool> cfgBlitzIconTransparency("BlitzIconTransparency", false);
static ConfigValue<int> cfgOpponentIconStyle("OpponentIconStyle", 0);
static ConfigValue<int> cfgOpponentIconColor("OpponentIconColor", 5);
static ConfigValue<int> cfgBlitzIconColor("BlitzIconColor", 6);

unsigned int IconColors[12] = {
    0xFF0000EF, // Blue
    0xFF00EF00, // Green
    0xFFEF0000, // Red
    0xFFFFFF00, // Yellow
    0xFFFF5A00, // Orange
    0xFFB400FF, // Purple
    0xFF00FFFF, // Aqua
    0xFFFF0390, // Pink

    /* NEW ADDITIONAL COLORS */

    0xFFFFFFFF, // White
    0xFF3F3F3F, // Grey
    0xFF000000, // Black
    0xFFC0EC42, // Light Green
};

unsigned int SemiTransIconColors[12] = {
    0x800000EF, // Blue
    0x8000EF00, // Green
    0x80EF0000, // Red
    0x80FFFF00, // Yellow
    0x80FF5A00, // Orange
    0x80B400FF, // Purple
    0x8000FFFF, // Aqua
    0x80FF0390, // Pink

    /* NEW ADDITIONAL COLORS */

    0x80FFFFFF, // White
    0x803F3F3F, // Grey
    0x80000000, // Black
    0x80C0EC42, // Light Green
};

void mmIconsHandler::Cull()
{
    reinterpret_cast<mmIcons*>(this)->mmIcons::Cull();
}

void mmIconsHandler::RegisterOpponents(OppIconInfo *icons, int count, LocFont *a3)
{
    for (int i = 0; i < count; i++)
    {
        if (opponentIconTransparency)
        {
            if (opponentIconStyle == 0) // MM2
            {
                icons[i].Color = SemiTransIconColors[5];
            }
            if (opponentIconStyle == 1) // MM1
            {
                icons[i].Color = SemiTransIconColors[i];
            }
            if (opponentIconStyle == 2) // NFSHP2
            {
                icons[i].Color = SemiTransIconColors[1];
            }
            if (opponentIconStyle == 3) // NFSMW
            {
                icons[i].Color = SemiTransIconColors[11];
            }
            if (opponentIconStyle == 4) // NFSC
            {
                icons[i].Color = SemiTransIconColors[4];
            }
            if (opponentIconStyle >= 5) // CUSTOM
            {
                icons[i].Color = SemiTransIconColors[opponentIconColor];
            }
        }
        else
        {
            if (opponentIconStyle == 0) // MM2
            {
                icons[i].Color = IconColors[5];
            }
            if (opponentIconStyle == 1) // MM1
            {
                icons[i].Color = IconColors[i];
            }
            if (opponentIconStyle == 2) // NFSHP2
            {
                icons[i].Color = IconColors[1];
            }
            if (opponentIconStyle == 3) // NFSMW
            {
                icons[i].Color = IconColors[11];
            }
            if (opponentIconStyle == 4) // NFSC
            {
                icons[i].Color = IconColors[4];
            }
            if (opponentIconStyle >= 5) // CUSTOM
            {
                icons[i].Color = IconColors[opponentIconColor];
            }
        }
    }

    //call original
    reinterpret_cast<mmIcons*>(this)->RegisterOpponents(icons, count, a3);
}

void mmIconsHandler::RegisterOpponents_Blitz(OppIconInfo *icons, int count, LocFont *a3)
{
    for (int i = 0; i < count; i++)
    {
        if (blitzIconTransparency)
        {
            if (blitzIconMultiColor)
            {
                icons[i].Color = SemiTransIconColors[i];
            }
            else
            {
                icons[i].Color = SemiTransIconColors[blitzIconColor];
            }
        }
        else
        {
            if (blitzIconMultiColor)
            {
                icons[i].Color = IconColors[i];
            }
            else
            {
                icons[i].Color = IconColors[blitzIconColor];
            }
        }
    }

    //call original
    reinterpret_cast<mmIcons*>(this)->RegisterOpponents(icons, count, nullptr);
}

void mmIconsHandler::Install() {
    opponentIconStyle = cfgOpponentIconStyle.Get();
    opponentIconColor = cfgOpponentIconColor.Get();
    opponentIconTransparency = cfgOpponentIconTransparency.Get();
    blitzIconColor = cfgBlitzIconColor.Get();
    blitzIconMultiColor = cfgBlitzIconMultiColor.Get();
    blitzIconTransparency = cfgBlitzIconTransparency.Get();
    
    // remove 10 player limit from mmIcons
    InstallPatch({ 0xEB }, {
        0x432303
        });

    // rewrite mmIcons
    InstallVTableHook("mmIconsHandler::Cull",
        &Cull, {
            0x5B0E20, // mmIcons::Cull
        }
    );

    InstallCallback("mmIcons::RegisterOpponents",
        &RegisterOpponents, {
            cb::call(0x412C40), // mmGame::Init
        }
    );

    InstallCallback("mmIcons::RegisterOpponents [mmSingleBlitz]",
        &RegisterOpponents_Blitz, {
            cb::call(0x41B065), // mmSingleBlitz::Init
        }
    );
}

