//
//  animationlink.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 02/09/2023.
//

#ifndef animationlink_h
#define animationlink_h

#include "common.h"

#define ANIMATION_LINK_COUNT(chain) sizeof(chain) / sizeof(AnimationLink)

#define TEXT_X from
#define TEXT_Y to

#define TIME from
#define DURATION to

typedef struct animation_link AnimationLink;

struct animation_link {
    /**
     * @param self Instance de la chaîne.
     * @param crankChange Distance de scrolling.
     * @returns La distance de scrolling inutilisée (sera passée au lien de chaîne suivant).
     */
    float (* _Nonnull update)(AnimationLink * _Nonnull self, float crankChange);
    /**
     * Déinitialiser les contenus alloués par ce lien.
     */
    void (* _Nullable deinit)(AnimationLink * _Nonnull self);
    LCDSprite * _Nullable sprite;
    const char * _Nullable text;
    float from;
    float to;
    float progress;
    int8_t increment;
    uint8_t textLineCount;
};

float CameraAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);
float BToSkipSpriteAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);
float ScrollTextBubbleAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);
float WaitSpriteAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);

#endif /* animationlink_h */
