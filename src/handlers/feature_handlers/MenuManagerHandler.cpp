#include "MenuManagerHandler.h"

using namespace MM2;

bool canPlayUIDriverSound = true;

void MenuManagerHandler::Update()
{
    auto manager = reinterpret_cast<MenuManager*>(this);
    auto menuSwitchSound = manager->GetMenuSwitchSound();
    if (menuSwitchSound != nullptr)
    {
        int currentMenuID = manager->GetCurrentMenuID();
        if (currentMenuID == 1 && canPlayUIDriverSound)
        {
            menuSwitchSound->SetSoundHandleIndex(0);
            if (!menuSwitchSound->IsPlaying())
                menuSwitchSound->PlayOnce(-1.f, -1.f);
            canPlayUIDriverSound = false;
        }
        else if (currentMenuID != 1)
        {
            canPlayUIDriverSound = true;
        }
    }

    // call original
    hook::Thunk<0x4E5410>::Call<void>(this);
}

void MenuManagerHandler::Install() {
    InstallVTableHook("MenuManager::Update",
        &Update, {
            0x5B3290,
        }
    );
}