//
//  rankingsectionheader.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/11/2023.
//

#ifndef rankingsectionheader_h
#define rankingsectionheader_h

#include "common.h"
#include "../lib/melice.h"

#define kRankingSectionHeaderHeight 32
#define kRankingSectionHeaderMargin 8

typedef enum {
    RankingSectionHeaderLabelLocal,
    RankingSectionHeaderLabelOnline,
} RankingSectionHeaderLabel;

LCDSprite * _Nonnull RankingSectionHeaderConstructor(MELPoint origin, RankingSectionHeaderLabel label);
const MELSpriteClass * _Nonnull RankingSectionHeaderGetClass(void);

#endif /* rankingsectionheader_h */
