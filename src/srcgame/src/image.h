//
//  image.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/07/2023.
//

#ifndef image_h
#define image_h

#include "common.h"
#include "../lib/melice.h"

const MELSpriteClass * _Nonnull ImageGetClass(void);

/**
 * Créé une nouvelle image et la positionne au point central donné.
 *
 * @discussion Ajoute le sprite à la liste des sprites de la scène courante.
 * @discussion L'emplacement est dans le monde, donc relatif à la position de la caméra.
 * @param origin Centre de l'image.
 * @param image Image à afficher dans un sprite.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull ImageConstructor(MELPoint origin, LCDBitmap * _Nonnull image);

/**
 * Charge et affiche une nouvelle image. Elle sera positionnée au point central donné.
 *
 * @discussion Ajoute le sprite à la liste des sprites de la scène courante.
 * @discussion L'emplacement est dans le monde, donc relatif à la position de la caméra.
 * @param origin Centre de l'image.
 * @param path Chemin de l'image à charger et à afficher.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull ImageConstructorWithPath(MELPoint origin, const char * _Nonnull path);

LCDSprite * _Nonnull ImageConstructorWithSelf(MELSprite * _Nonnull self, MELPoint origin, LCDBitmap * _Nonnull image);
LCDSprite * _Nonnull ImageConstructorWithSelfDontPush(MELSprite * _Nonnull self, MELPoint origin, LCDBitmap * _Nonnull image);

void ImageDealloc(LCDSprite * _Nonnull sprite);

#endif /* image_h */
