//
//  bonus.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 30/07/2023.
//

#ifndef bonus_h
#define bonus_h

#include "common.h"
#include "../lib/melice.h"

typedef enum {
    BonusTypeSmall,
    BonusTypeMedium,
    BonusTypeBig
} BonusType;

extern const int kBonusTypeScore[3];

LCDSprite * _Nonnull BonusConstructorWithCenter(BonusType type, MELPoint center, float angle, float speed);

LCDSprite * _Nonnull BonusConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable BonusLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

void BonusCreate(MELSprite * _Nonnull melSprite, float angle, float speed);

const MELSpriteClass * _Nonnull BonusGetClass(void);

#endif /* bonus_h */
