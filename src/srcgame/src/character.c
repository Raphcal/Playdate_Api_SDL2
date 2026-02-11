//
//  character.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#include "character.h"

#include "../gen/spriteportaits.h"
#include "../gen/spritelargeportaits.h"

const MELLocalizedString CharacterName[CharacterCount] = {
    (MELLocalizedString) {},
    {
        .text = {
            "Caroline",
            "Caroline",
            "キャロライン",
            "Caroline",
        }
    },
    {
        .text = {
            "Ceres",
            "Cérès",
            "セレス",
            "Ceres",
        }
    },
    {
        .text = {
            "Crocoford",
            "Crocoford",
            "クロコフォード",
            "Crocoford",
        }
    },
    {
        .text = {
            "Magical Baguette",
            "Baguette Magique",
            "不思議のパン",
            "Baguete Mágica",
        }
    },
    {
        .text = {
            "Prince",
            "Prince",
            "プリンス",
            "Prince",
        }
    },
    {
        .text = {
            "Horse",
            "Cheval",
            "ウマ",
            "Cavala",
        }
    },
    {
        .text = {
            "Clovis",
            "Clovis",
            "クロヴィス",
            "Clóvis",
        }
    },
    {
        .text = {
            "Finepen",
            "Belleplume",
            "ミツペン",
            "Plumabela",
        }
    },
    {
        .text = {
            "Firmidable",
            "Sapique",
            "マツ",
            "Pinheirinho"
        }
    },
    {
        .text = {
            "Pierrot",
            "Pierrot",
            "ピエロ",
            "Pierrot",
        }
    },
    {
        .text = {
            "Crowd",
            "Public",
            "観客",
            "Público",
        }
    },
    {
        .text = {
            "Earika",
            "Orelia",
            "ミミ",
            "Oreia",
        }
    },
    {
        .text = {
            "Shuffle",
            "Shuffle",
            "シャッフル",
            "Shuffle",
        }
    },
    {
        .text = {
            "Garouge",
            "Chaperon Garouge",
            "ガルージュ",
            "Lobelinha Vermelha",
        }
    },
    {
        .text = {
            "El Joker",
            "El Joker",
            "エル・ジョーカー",
            "El Joker",
        }
    },
    {
        .text = {
            "Paskey",
            "Passepartout",
            "パスキー",
            "Passaqui",
        }
    },
    {
        .text = {
            "Hat",
            "Chapeau",
            "帽子",
            "Chapéu",
        },
    },
    {
        .text = {
            "Shadow",
            "Silhouette",
            "かげ",
            "Sombra",
        },
    },
    {
        .text = {
            "Melusine",
            "Mélusine",
            "メリュジーヌ",
            "Melusina",
        },
    },
    {
        .text = {
            "Paguro",
            "Paguro",
            "ヤドカ",
            "Paguro",
        }
    },
};

const AnimationName CharacterAnimationName[CharacterCount] = {
    AnimationNameStand, // None
    AnimationNameStand, // Caroline
    AnimationNameWalk, // Ceres
    AnimationNameRun, // Crocoford
    AnimationNameSkid, // Baguette Magique
    AnimationNameJump, // Prince
    AnimationNameJump, // Cheval
    AnimationNameFall, // Clovis
    AnimationNameShaky, // Belleplume
    AnimationNameBounce, // Sapique
    AnimationNameRaise, // Pierrot
    AnimationNameRaise, // Public
    AnimationNameAppear, // Orelia
    AnimationNameDisappear, // Shuffle
    AnimationNameAttack, // Chaperon Garouge
    AnimationNameDuck, // El Joker
    AnimationNameHurt, // Passepartout
    AnimationNameDie, // Chapeau
    AnimationNameDie, // Silhouette
    AnimationNameDie, // Mélusine
    AnimationNameSkid, // Paguro
};

const MELAnimationDirection CharacterAnimationDirection[CharacterCount] = {
    MELAnimationDirectionRight, // None
    MELAnimationDirectionRight, // Caroline
    MELAnimationDirectionRight, // Ceres
    MELAnimationDirectionRight, // Crocoford
    MELAnimationDirectionRight, // Baguette Magique
    MELAnimationDirectionRight, // Prince
    MELAnimationDirectionLeft, // Cheval
    MELAnimationDirectionRight, // Clovis
    MELAnimationDirectionRight, // Belleplume
    MELAnimationDirectionRight, // Sapique
    MELAnimationDirectionRight, // Pierrot
    MELAnimationDirectionBottomRight, // Public
    MELAnimationDirectionRight, // Orelia
    MELAnimationDirectionRight, // Shuffle
    MELAnimationDirectionRight, // Chaperon Garouge
    MELAnimationDirectionRight, // El Joker
    MELAnimationDirectionRight, // Passepartout
    MELAnimationDirectionLeft, // Chapeau
    MELAnimationDirectionBottom, // Silhouette
    MELAnimationDirectionRight, // Mélusine
    MELAnimationDirectionLeft, // Paguro
};
