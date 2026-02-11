//
//  rankingitem.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 18/06/2023.
//

#ifndef rankingitem_h
#define rankingitem_h

#include "common.h"
#include "../lib/melice.h"

#include "rankingboard.h"

#define kRankingItemWidth 324
#define kRankingItemHeight 42
#define kRankingItemMargin 4
#define kRankingItemRankWidth 24

#define kPlayerNameSuffixKatsuo " (katsuo)"
#define kPlayerNameSuffixSaki " (saki)"

typedef struct {
    MELSprite super;
    Score score;
    int lastRank;
    MELBoolean isDirty;
    MELBoolean isEditable;
    MELBoolean isCursorVisible;
    LCDBitmapTable * _Nullable faces;
} RankingItem;

/**
 * Créé une entrée du tableau des scores.
 *
 * @discussion Fait un push vers la liste des sprites de la scène courante.
 * @param y Ordonnée du sprite.
 * @param score Score à afficher.
 * @param faces Visages à afficher.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull RankingItemConstructor(const int y, const Score score, const int lastRank, LCDBitmapTable * _Nullable faces);
const MELSpriteClass * _Nonnull RankingItemGetClass(void);
/**
 * Indique si le sprite donné est une instance de <code>RankingItem</code>.
 *
 * @param self Sprite à vérifier.
 * @returns <code>true</code> si <code>self</code> est un <code>RankingItem</code>, <code>false</code> sinon.
*/
MELBoolean isRankingItem(MELSprite * _Nonnull self);

#endif /* rankingitem_h */
