//
//  main.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/01/2023.
//

#include "common.h"

#include "openingscene.h"
#include "savegame.h"
#include "scores.h"
#include "achievements.h"

#include "../lib/melice.h"

PlaydateAPI * _Nullable playdate;
LCDFont * _Nullable fontMagic;
LCDFont * _Nullable fontMagicSmall;
LCDFont * _Nullable fontRoobert10Bold;
LCDFont * _Nullable fontRoobert11Bold;
LCDFont * _Nullable fontRoobert11MonoMediumNumerals;
LCDFont * _Nullable fontJfDotK14Bold;
LCDFont * _Nullable fontJfDotIzumi16Bold;
LCDFont * _Nullable fontNontendoBold;

int REFRESH_RATE = DEFAULT_REFRESH_RATE;
float DELTA;

LCDFont * _Nullable currentFont;
LCDFont * _Nullable smallFont;

PDMenuItem * _Nullable languageMenuItem = NULL;
static const char * _Nonnull kLanguageOptions[] = {"EN", "FR", "JP", "PT BR"};
static const int kLanguageCount = 4;


void setRefreshRate(const int refreshRate) {
    DELTA = 1.0f / refreshRate;
    playdate->display->setRefreshRate(refreshRate);
}

static void languageChanged(void * _Nullable userdata) {
    const int selection = playdate->system->getMenuItemValue(languageMenuItem);
    setLanguage(selection);
    if (currentSaveGame.index != kNoSaveSelected) {
        SaveGameToDisk(&currentSaveGame);
    }
}

void setLanguage(MELLanguage language) {
    if (language == currentLanguage && currentFont != NULL) {
        // Pas de changement
        return;
    }
    switch (language) {
        case MELLanguageFrench:
            currentLanguage = MELLanguageFrench;
            currentFont = fontRoobert11Bold;
            smallFont = fontRoobert10Bold;
            break;
        case MELLanguageJapanese:
            currentLanguage = MELLanguageJapanese;
            currentFont = fontJfDotIzumi16Bold;
            smallFont = fontJfDotK14Bold;
            break;
        case MELLanguageBrazilianPortuguese:
            currentLanguage = MELLanguageBrazilianPortuguese;
            currentFont = fontRoobert11Bold;
            smallFont = fontRoobert10Bold;
            break;
        default:
            currentLanguage = MELLanguageEnglish;
            currentFont = fontRoobert11Bold;
            smallFont = fontRoobert10Bold;
            break;
    }
    if (languageMenuItem) {
        playdate->system->setMenuItemValue(languageMenuItem, language);
    }
    MELEventBusFireEvent(EventLanguageChanged, currentLanguage);
}

static void init(void) {
    playdate->graphics->setBackgroundColor(kColorWhite);
    languageMenuItem = playdate->system->addOptionsMenuItem("language", kLanguageOptions, kLanguageCount, languageChanged, NULL);

    SaveGame firstSaveGame = SavedGameLoadFirst();
    currentLanguage = firstSaveGame.language;

    LocalScoresLoad();

    playdate->system->setMenuItemValue(languageMenuItem, currentLanguage);
    setLanguage(currentLanguage);
    playdate->system->resetElapsedTime();
    OpeningScene *openingScene = OpeningSceneAlloc();
    MELSceneMakeCurrent(&openingScene->super);
}

static void loadFonts(void) {
    fontMagic = LCDFontLoadOrError("fonts/font-magic");
    fontMagicSmall = LCDFontLoadOrError("fonts/font-magicsmall");
    fontRoobert10Bold = LCDFontLoadOrError("fonts/Roobert-10-Bold");
    fontRoobert11Bold = LCDFontLoadOrError("fonts/Roobert-11-Bold");
    fontRoobert11MonoMediumNumerals = LCDFontLoadOrError("fonts/Roobert-11-Mono-Medium-Numerals");
    fontJfDotK14Bold = LCDFontLoadOrError("fonts/JF-Dot-K14B-2004");
    fontJfDotIzumi16Bold = LCDFontLoadOrError("fonts/JF-Dot-Izumi16B");
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
            init();
            break;
        case kEventTerminate:
        case kEventLowPower:
            api->system->logToConsole("Terminate (%d) or low power event (%d) received: %d", kEventTerminate, kEventLowPower, event);
            if (currentScene && currentScene->beforeQuit) {
                currentScene->beforeQuit(currentScene);
            }
            LocalScoresSave();
            MELAchievementSaveStatus();
            break;
        case kEventPause:
            // TODO: Afficher la carte via setMenuImage
            break;
#if DEBUG
        case kEventKeyPressed:
            if (arg == 'w') {
                MELEventBusFireEvent(EventRightSideLost, 0);
            }
            break;
#endif
        default:
            break;
    }
    return 0;
}

