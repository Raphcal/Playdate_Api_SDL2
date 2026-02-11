//
//  music.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 21/03/2024.
//

#include "music.h"

static MusicManager instance = (MusicManager) {};

static void fadeIn(FilePlayer * _Nonnull filePlayer, float duration);
static void fadeOut(FilePlayer * _Nonnull filePlayer, float duration);

void MusicManagerPlay(const char * _Nonnull music, int repeat, float crossFadeDuration) {
    const struct playdate_sound_fileplayer *filePlayerAPI = playdate->sound->fileplayer;
#if LOG_MUSIC
        playdate->system->logToConsole("Will play music with instance %d", instance.current);
#endif
    if (instance.filePlayers[instance.current] == NULL) {
#if LOG_MUSIC
        playdate->system->logToConsole("Current filePlayer (instance %d) is NULL: new player", instance.current);
#endif
        instance.filePlayers[instance.current] = filePlayerAPI->newPlayer();
    }
    FilePlayer *currentPlayer = instance.filePlayers[instance.current];
    if (!filePlayerAPI->isPlaying(currentPlayer)) {
        // Aucune musique en cours, lecture.
#if LOG_MUSIC
        playdate->system->logToConsole("FilePlayer %d is idle: play!", instance.current);
#endif
        filePlayerAPI->loadIntoPlayer(currentPlayer, music);
        filePlayerAPI->play(currentPlayer, repeat);
        filePlayerAPI->setVolume(currentPlayer, 1.0f, 1.0f);
        filePlayerAPI->setRate(currentPlayer, 1.0f);
        return;
    }
    const int next = (instance.current + 1) % 2;
    instance.current = next;
    if (instance.filePlayers[next] == NULL) {
#if LOG_MUSIC
        playdate->system->logToConsole("Next filePlayer (instance %d) is NULL: new player", next);
#endif
        instance.filePlayers[next] = filePlayerAPI->newPlayer();
    }
    FilePlayer *nextPlayer = instance.filePlayers[next];
    filePlayerAPI->loadIntoPlayer(nextPlayer, music);
    if (crossFadeDuration != 0.0f) {
#if LOG_MUSIC
        playdate->system->logToConsole("Cross fade!");
#endif

        fadeOut(currentPlayer, crossFadeDuration);
        filePlayerAPI->play(nextPlayer, repeat);
        filePlayerAPI->setRate(nextPlayer, 1.0f);
        fadeIn(nextPlayer, crossFadeDuration);
    } else {
#if LOG_MUSIC
        playdate->system->logToConsole("Stop and play!");
#endif
        filePlayerAPI->stop(currentPlayer);
        filePlayerAPI->play(nextPlayer, repeat);
        filePlayerAPI->setVolume(nextPlayer, 1.0f, 1.0f);
        filePlayerAPI->setRate(nextPlayer, 1.0f);
    }
}

void MusicManagerReset(void) {
    instance = (MusicManager) {};
}

void MusicManagerSetRate(float rate) {
    FilePlayer *currentPlayer = instance.filePlayers[instance.current];
    playdate->sound->fileplayer->setRate(currentPlayer, rate);
}

void MusicManagerFadeOut(float crossFadeDuration) {
    if (instance.filePlayers[instance.current] == NULL) {
        return;
    }
#if LOG_MUSIC
        playdate->system->logToConsole("Fade out!");
#endif
    FilePlayer *currentPlayer = instance.filePlayers[instance.current];

    const int next = (instance.current + 1) % 2;
    instance.current = next;

    fadeOut(currentPlayer, crossFadeDuration);
}

typedef struct {
    MELSprite super;
    FilePlayer * _Nullable filePlayer;
    float duration;
    float time;
    float fromLeft;
    float fadeLeft;
    float fromRight;
    float fadeRight;
    MELBoolean stopOnFadeEnd;
} MusicManagerFade;

static void update(LCDSprite * _Nonnull sprite) {
    MusicManagerFade *self = playdate->sprite->getUserdata(sprite);
    const float time = self->time;
#if LOG_MUSIC
    playdate->system->logToConsole("Fade %x: %fs...", self->filePlayer, (double)time);
#endif
    if (time < self->duration) {
        self->time = time + DELTA;
        const float progress = MELEaseInOut(0.0f, self->duration, time);
        playdate->sound->fileplayer->setVolume(self->filePlayer, self->fromLeft + self->fadeLeft * progress, self->fromRight + self->fadeRight * progress);
        return;
    }
    playdate->sound->fileplayer->setVolume(self->filePlayer, self->fromLeft + self->fadeLeft, self->fromRight + self->fadeRight);
    if (self->stopOnFadeEnd) {
        playdate->sound->fileplayer->stop(self->filePlayer);
    }
    MELSpriteDealloc(sprite);
}

static void fadeToVolume(FilePlayer * _Nonnull filePlayer, float duration, float left, float right, MELBoolean stopOnFadeEnd) {
    float fromLeft, fromRight;
    playdate->sound->fileplayer->getVolume(filePlayer, &fromLeft, &fromRight);

    MusicManagerFade *self = new(MusicManagerFade);
    *self = (MusicManagerFade) {};
    LCDSprite *sprite = MELSpriteInitHiddenWithUpdate(&self->super, update);
    self->filePlayer = filePlayer;
    self->duration = duration;
    self->fromLeft = fromLeft;
    self->fadeLeft = left - fromLeft;
    self->fromRight = fromRight;
    self->fadeRight = right - fromRight;
    self->stopOnFadeEnd = stopOnFadeEnd;

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
}

static void fadeOut(FilePlayer * _Nonnull filePlayer, float duration) {
    fadeToVolume(filePlayer, duration, 0.0f, 0.0f, true);
}

static void fadeIn(FilePlayer * _Nonnull filePlayer, float duration) {
    playdate->sound->fileplayer->setVolume(filePlayer, 0.0f, 0.0f);
    fadeToVolume(filePlayer, duration, 1.0f, 1.0f, false);
}
