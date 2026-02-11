//
//  main.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/01/2023.
//

#include "common.h"

#include "titlescene.h"
#include "gamescene.h"
#include "rankingboard.h"
#include "savegame.h"

#include "../lib/melice.h"

PlaydateAPI * _Nullable playdate;
LCDFont * _Nullable numbersFont;
LCDFont * _Nullable numbers32Font;
LCDFont * _Nullable romajiFont;
LCDFont * _Nullable rains2xFont;
LCDFont * _Nullable nontendoLightFont;

int REFRESH_RATE = 50;
float DELTA;

void setRefreshRate(const int refreshRate) {
    DELTA = 1.0f / refreshRate;
    playdate->display->setRefreshRate(refreshRate);
}

static void init(void) {
    playdate->graphics->setBackgroundColor(kColorWhite);
    if (!LoadState()) {
        TitleScene *titleScene = TitleSceneAlloc();
        MELSceneMakeCurrent(&titleScene->super);
    }
}

static void loadFonts(void) {
    numbersFont = LCDFontLoadOrError("fonts/font-numbers");
    numbers32Font = LCDFontLoadOrError("fonts/font-numbers32");
    romajiFont = LCDFontLoadOrError("fonts/font-romaji");
    rains2xFont = LCDFontLoadOrError("fonts/font-rains-2x");
    nontendoLightFont = LCDFontLoadOrError("fonts/font-nontendo-light");
}

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI * _Nonnull api, PDSystemEvent event, uint32_t arg) {
    switch (event) {
        case kEventInit:
            playdate = api;
            setRefreshRate(DEFAULT_REFRESH_RATE);
            MELRandomInit();
            loadFonts();
            LocalScoresLoad();
            init();
            break;
        case kEventTerminate:
        case kEventLowPower:
            api->system->logToConsole("Terminate (%d) or low power event (%d) received: %d", kEventTerminate, kEventLowPower, event);
            LocalScoresSave();
            SaveState();
            break;
        case kEventPause:
            // TODO: Afficher la carte via setMenuImage
            break;
        default:
            break;
    }
    return 0;
}

