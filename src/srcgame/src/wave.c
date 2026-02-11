//
//  wave.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 03/02/2023.
//

#include "wave.h"

MELListImplement(Wave);
MELListImplement(WaveRef);

/**
 * Longueur du préfixe "maps/map-".
 */
static const int kMapsMapPrefixLength = 9;

/**
 * Longueur du préfixe "maps/map-wave".
 */
static const int kMapWavePrefixLength = 13;

static MELSpriteInstanceList loadMapInstances(const MapName mapName);

void WaveInit(Wave * _Nonnull self, const MapName mapName) {
    const char *path = kMapNameFileNames[mapName];
    const MELBoolean isWave = MELStringStartsWith(path + kMapsMapPrefixLength, "wave");
    if (!isWave) {
        self->mapName = mapName;
        self->type = MELStringStartsWith(path + kMapsMapPrefixLength, "bg") ? WaveTypeBackground : WaveTypeOther;
        return;
    }

    const char * _Nullable startTimeString = MELStringIndexOfString(path + kMapWavePrefixLength, "time");
    const char * _Nullable tutorialString = MELStringIndexOfString(path + kMapWavePrefixLength, "tutorial");
    const char * _Nullable bossString = MELStringIndexOfString(path + kMapWavePrefixLength, "boss");
    WaveType type = WaveTypeNormal;
    if (tutorialString) {
        type = WaveTypeTutorial;
    } else if (bossString) {
        type = WaveTypeBoss;
    }
    MELMap *map = MELMapOpen(path);
    *self = (Wave) {
        .mapName = mapName,
        .type = type,
        .startTime = startTimeString != NULL ? MELStringParseInt(startTimeString + 4) : -1,
        .instances = loadMapInstances(mapName),
    };
    // Appel MELInstanceDeinit donc les variables des instances sont inutilisables.
    MELMapDealloc(map);
}

/**
 * Charge les instances sans charger les variables.
 */
static MELSpriteInstance loadInstance(MELInputStream * _Nonnull inputStream) {
    MELSpriteInstance instance = {};
    instance.name = (SpriteName) MELInputStreamReadUInt16(inputStream);
    instance.direction = (MELDirection) MELInputStreamReadByte(inputStream);
    const int x = MELInputStreamReadInt(inputStream);
    const int y = MELInputStreamReadInt(inputStream);
    instance.center = MELPointMake(x, y);
    instance.zIndex = MELInputStreamReadByte(inputStream) & 0xFF;
    instance.isUnique = MELInputStreamReadBoolean(inputStream);
    const int variableCount = MELInputStreamReadInt(inputStream);
    MELInputStreamSkipBytes(inputStream, sizeof(int32_t) * variableCount);
    return instance;
}

/**
 * Charge seulement les instances de la carte donnée et ajoute le nom de la carte aux instances.
 */
static MELSpriteInstanceList loadMapInstances(const MapName mapName) {
    const char *path = kMapNameFileNames[mapName];
    MELInputStream inputStream = MELInputStreamOpen(path, kFileRead);

    if (!inputStream.file) {
        MELInputStreamClose(&inputStream);
        playdate->system->error("Map not found: %s", path);
        return MELSpriteInstanceListEmpty;
    }

    MELInputStreamSkipBytes(&inputStream, sizeof(int32_t) // width
                            + sizeof(int32_t) // height
                            + sizeof(uint16_t) // paletteName
                            + sizeof(int32_t) // water frame.origin.x
                            + sizeof(int32_t) // water frame.origin.y
                            + sizeof(int32_t) // water frame.size.width
                            + sizeof(int32_t) // water frame.size.height
    );
    const int32_t layerCount = MELInputStreamReadInt(&inputStream);
    for (int layerIndex = 0; layerIndex < layerCount; layerIndex++) {
        MELInputStreamSkipBytes(&inputStream, sizeof(int32_t) // layer frame.origin.x
                                + sizeof(int32_t) // layer frame.origin.y
        );
        const int32_t width = MELInputStreamReadInt(&inputStream);
        const int32_t height = MELInputStreamReadInt(&inputStream);
        MELInputStreamSkipBytes(&inputStream, sizeof(float) // scrollRate.x
                                + sizeof(float) // scrollRate.y
                                + sizeof(uint8_t) // isGround
        );
        const int32_t tileCount = width * height;
        MELInputStreamSkipBytes(&inputStream, sizeof(uint16_t) * tileCount);
    }
    const int instanceCount = MELInputStreamReadInt(&inputStream);
    MELSpriteInstanceList instances = MELSpriteInstanceListMakeWithInitialCapacity(instanceCount);
    for (int instanceIndex = 0; instanceIndex < instanceCount; instanceIndex++) {
        MELSpriteInstance instance = loadInstance(&inputStream);
        instance.mapName = mapName;
        MELSpriteInstanceListPush(&instances, instance);
    }
    MELInputStreamClose(&inputStream);
    return instances;
}

void WaveDeinit(Wave * _Nonnull self) {
    MELSpriteInstanceListDeinit(&self->instances);
}

void WaveInstantiate(Wave * _Nonnull self) {
    LCDSpriteRefList *sprites = &currentScene->sprites;
    MELSpriteInstanceList instances = self->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (!definition->constructor) {
            playdate->system->logToConsole("Sprite definition %d has no constructor", instance.name);
            continue;
        }
        if (!definition->palette) {
            definition->palette = SpriteNameLoadBitmapTable(instance.name);
        }
        LCDSprite *sprite = definition->constructor(definition, instances.memory + index);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        playdate->system->logToConsole("Push Wave(%x)", sprite);
#endif
        LCDSpriteRefListPush(sprites, sprite);
    }
}
