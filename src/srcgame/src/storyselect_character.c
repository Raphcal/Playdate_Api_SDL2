//
//  storyselect_character.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 29/05/2024.
//

#include "storyselect_character.h"

#include "story.h"
#include "../gen/spritestoryselectcharacter.h"

static void update(LCDSprite * _Nonnull sprite);

LCDSprite * _Nonnull StorySelectCharacterConstructor(unsigned int index, MELGridView * _Nonnull gridView) {
    MELSprite *self = new(MELSprite);
    loadSpriteStorySelectCharacterPalette();
    LCDSprite *sprite = MELSpriteInitWithCenter(self, &spriteStorySelectCharacter, (MELPoint) {
        .x = gridView->inset.left + (gridView->cellSize.width + gridView->cellSpacing.width) * index + gridView->cellSize.width / 2,
        .y = 148
    });

    self->userdata = gridView;

    if (StoryIsUnlocked(index)) {
        const Character character = kStories[index]->character;
        const AnimationName animationName = CharacterAnimationName[character];
        MELSpriteSetAnimation(self, animationName);
    } else {
        self->animationName = AnimationNameRun;
        MELSpriteSetAnimationAndDirection(self, AnimationNameStand, MELAnimationDirectionLeft);
    }

    playdate->sprite->setUpdateFunction(sprite, update);
    return sprite;
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELGridView *gridView = self->userdata;
    const Story *selectedStory = kStories[gridView->selection.x];
    const Character selectedCharacter = selectedStory->character;
    const AnimationName selectedAnimation = CharacterAnimationName[selectedCharacter];
    const MELBoolean isSelected = self->animationName == selectedAnimation;

    MELAnimation *animation = self->animation;
    if (animation->class == &MELLoopingAnimationClass) {
        MELLoopingAnimation *loopingAnimation = (MELLoopingAnimation *)animation;
        if (isSelected) {
            animation->speed = 1.0f;
        } else {
            animation->speed = 0.0f;
            loopingAnimation->time = 0.0f;
            MELAnimationSetFrameIndex(animation, 0);
        }
    }
    animation->class->update(animation, DELTA);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);

    const MELPoint center = self->frame.origin;
    playdate->sprite->moveTo(sprite, center.x - gridView->camera.x, center.y);
}
