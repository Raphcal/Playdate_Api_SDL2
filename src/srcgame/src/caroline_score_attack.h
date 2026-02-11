//
//  caroline_score_attack.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 14/11/2024.
//

#ifndef caroline_score_attack_h
#define caroline_score_attack_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull CarolineScoreAttackConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);
MELSprite * _Nullable CarolineScoreAttackLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

LCDSprite * _Nonnull CarolineScoreAttackConstructorDefault(void);

MELBoolean CarolineScoreAttackIsCheering(LCDSprite * _Nonnull sprite);

#endif /* caroline_score_attack_h */
