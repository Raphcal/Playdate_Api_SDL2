//
//  rally.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 20/07/2025.
//

#include "rally.h"

#include "cpu.h"
#include "instantcpu.h"
#include "randomcpu.h"
#include "patterncpu.h"

// TODO: Augmenter la vitesse de jeu de x0.5 tous les 3 adversaires ?

const EncounterList kRallyEncounters = (EncounterList) {
    .count = 20,
    .memory = (Encounter[20]) {
        {
            .opponent = CharacterCeres,
            .actor = (Actor *) &((Cpu) {
                UseRandomCpu(1.0f)
            }),
        },
        {
            .opponent = CharacterSapique,
            .actor = (Actor *) &((Cpu) {
                UseRandomCpu(1.0f),
                .isPressingDown = true,
            }),
        },
        {
            .opponent = CharacterChataigne,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(1.0f),
                .sameSuitScore = 1,
                .rowScore = 1,
            }),
        },
        {
            .opponent = CharacterBaguetteMagique,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(1.0f),
                .isFastWhenGoodHandIsFound = true,
                .randomFast = 50,
                .sameSuitScore = 1,
                .miss = 40,
            }),
        },
        {
            .opponent = CharacterClown,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(1.0f),
                .isFastWhenGoodHandIsFound = true,
                .randomFast = 50,
                .sameSuitScore = 1,
                .miss = 20,
            }),
        },
        {
            .opponent = CharacterOrelia,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(1.0f),
                .isAlwaysFast = true,
                .sameSuitScore = 1,
                .miss = 40,
            }),
        },
        {
            .opponent = CharacterPrince,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(1.0f),
                .isAlwaysFast = true,
                .sameSuitScore = 1,
                .straightScore = 1,
                .miss = 20,
            }),
        },
        {
            .opponent = CharacterClovis,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.69f),
                .isFastWhenGoodHandIsFound = true,
                .straightScore = 1,
                .miss = 10,
            }),
        },
        {
            .opponent = CharacterHat,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.69f),
                .isFastWhenGoodHandIsFound = true,
                .sameKindScore = 1,
            }),
        },
        {
            .opponent = CharacterGuide,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.5f),
                .isFastWhenGoodHandIsFound = true,
                .sameKindScore = 1,
                .sameSuitScore = 2,
                .straightScore = 3,
            }),
        },
        {
            .opponent = CharacterPetitChaperonRouge,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.5f),
                .isAlwaysFast = true,
                .preferSides = true,
                .sameSuitScore = 1,
            }),
        },
        {
            .opponent = CharacterShadow,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.2f),
                .isFastWhenGoodHandIsFound = true,
                .sameKindScore = 1,
                .avoidCenter = true,
            }),
        },
        {
            .opponent = CharacterCaroline,
            .actor = (Actor *) &((PatternCpu) {
                .super = {
                    UsePatternCpu(0.1f),
                    .isFastWhenGoodHandIsFound = true,
                    .preferSides = true,
                    .sameSuitScore = 1,
                },
                    .matchLimit = 2,
                    .switchToInstantCpuLimit = 18,
                    .backToPatternCpuLimit = 6,
                    .patterns = {
                        .count = 2,
                        .memory = (Pattern[2]) {
                            {
                                .name = "Pattern 1",
                                .size = {
                                    .width = 2,
                                    .height = 3
                                },
                                    .variableCount = 2,
                                    .pattern = "B*AABB",
                            },
                            {
                                .name = "Pattern 2",
                                .size = {
                                    .width = 4,
                                    .height = 3
                                },
                                    .variableCount = 2,
                                    .pattern = "  *B  AA  BB",
                            },
                        }
                    }
            }),
        },
        {
            .opponent = CharacterElJoker,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.1f),
                .isFastWhenAboveTarget = true,
                .straightScore = 1,
                .avoidCenter = true,
            }),
        },
        {
            .opponent = CharacterCheval,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.1f),
                .isFastWhenGoodHandIsFound = true,
                .preferSides = true,
                .straightScore = 1,
            }),
        },
        {
            .opponent = CharacterPaguro,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.1f),
                .isFastWhenGoodHandIsFound = true,
                .sameSuitScore = 1,
            }),
        },
        {
            .opponent = CharacterMelusine,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.1f),
                .isFastWhenAboveTarget = true,
                .sameKindScore = 1,
                .avoidCenter = true,
            }),
        },
        {
            .opponent = CharacterBelleplume,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0.1f),
                .isFastWhenAboveTarget = true,
                .preferSides = true,
                .straightScore = 8,
                .rowScore = 1,
            }),
        },
        {
            .opponent = CharacterCrocoford,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0),
                .isFastWhenAboveTarget = true,
                .sameKindScore = 1,
                .avoidCenter = true,
            }),
        },
        {
            .opponent = CharacterCrowd,
            .actor = (Actor *) &((InstantCpu) {
                UseInstantCpu(0),
                .isFastWhenGoodHandIsFound = true,
                .straightScore = 1,
                .sameKindScore = 1,
                .sameSuitScore = 1,
                .avoidCenter = true,
            }),
        },
    }
};

MELBoolean ArcadeGameIsEnding(ArcadeGame game) {
    return game.stage == kRallyEncounters.count || (game.stage == kRallyEncounters.count - 1 && game.deathCount > 0);
}
