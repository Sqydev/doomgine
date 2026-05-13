/*
* Copyright (c) 2025-present Wojciech Kaptur ( _Sqyd_ / Sqydev )
* Github: https://github.com/Sqydev
* GPG Fingerprint: 6DC2516B0DFDA9C59661650722F7B8A777F33B56
* 
* This software is provided "as-is", without any express or implied warranty. In no event
* will the authors be held liable for any damages arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose, including commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
* 
* 1. Non-Misrepresentation: The origin of this software must not be misrepresented; 
*    you must not claim that you wrote the original software. An acknowledgment in 
*    product documentation is appreciated but not required.
* 
* 2. Source-Level Copyleft: Any altered versions (forks) of this software's source code, 
*    or files containing significant portions of this code, must be distributed under 
*    these same license terms. Such modified source code must be made publicly available 
*    to any recipient, even if used over a network (SaaS).
* 
* 3. Proprietary Integration: This software may be integrated into, linked with, or 
*    used as a component of proprietary and closed-source products. In such cases, 
*    the surrounding proprietary application code does not need to be disclosed, 
*    provided that the original or modified source code of THIS software remains 
*    available under the terms of Section 2.
* 
* 4. Persistent Metadata: All original credits, including those in the source code headers 
*    and binary metadata (e.g., ELF .comment section, PE StringFileInfo, or equivalent), 
*    must not be removed. You may add your own credits to forks, provided the original 
*    authorship remains clearly identified.
* 
* 5. Notice Retention: This license notice may not be removed or altered from any 
*    source or binary distribution.
*/

#include "./maplib.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

map_t* maps;
size_t mapsCount = 0;
size_t lastId = 0;

int countLines(FILE *file) {
    int count = 0;
    char ch;
    while((ch = fgetc(file)) != EOF) {
        if(ch == '\n') {
            count++;
        }
    }

	rewind(file);
    return count;
}

map_t* GetMapById(size_t id) {
	for(size_t i = 0; i < mapsCount; i++) {
        if(maps[i].id == id) {
            return &maps[i];
        }
    }
    return NULL;
}

map_t* LoadMap(const char* path) {
	FILE* file = fopen(path, "r");

	maps = realloc(maps, mapsCount * sizeof(map_t));

	maps[mapsCount].id = ++lastId;
	maps[mapsCount].sectorsCount = countLines(file);
	maps[mapsCount].sectors = realloc(maps[mapsCount].sectors, maps[mapsCount].sectorsCount * sizeof(sector_t));

	char* line = NULL;
	size_t lineSize = 0;
	ssize_t read = 0;

	size_t currentSector = 0;

	while((read = getline(&line, &lineSize, file)) != -1) {
		maps[mapsCount].sectors[currentSector].floor.visible = atoi(strtok(line, ";"));
		maps[mapsCount].sectors[currentSector].floor.height = atof(strtok(NULL, ";"));

		maps[mapsCount].sectors[currentSector].ceiling.visible = atoi(strtok(line, ";"));
		maps[mapsCount].sectors[currentSector].ceiling.height = atof(strtok(NULL, ";"));

		maps[mapsCount].sectors[currentSector].light.level = atof(strtok(NULL, ";"));

		maps[mapsCount].sectors[currentSector].corners.count = atoi(strtok(NULL, ";"));
		maps[mapsCount].sectors[currentSector].corners.positions = malloc(maps[mapsCount].sectors[currentSector].corners.count * sizeof(Vector2));
		for(size_t i = 0; i < maps[mapsCount].sectors[currentSector].corners.count; i++) {
			maps[mapsCount].sectors[currentSector].corners.positions[i].x = atof(strtok(NULL, ";"));
			maps[mapsCount].sectors[currentSector].corners.positions[i].y = atof(strtok(NULL, ";"));
		}

		currentSector++;
	}

	if(line) { free(line); }

	fclose(file);

	mapsCount++;

	return &maps[mapsCount - 1];
}

void UnloadMap(map_t* map) {
	if(!map || mapsCount == 0) { return; }

	ptrdiff_t indexOfMap = map - maps; // MATH!
	
	if(index < 0 || (size_t)index >= mapsCount) {
        return; 
    }

	for(size_t i = 0; i < map->sectorsCount; i++) {
		if(map->sectors[i].corners.positions != NULL) {
			free(map->sectors[i].corners.positions);
			map->sectors[i].corners.positions = NULL;
		}
	}
	if(map->sectors != NULL) {
        free(map->sectors);
        map->sectors = NULL;
    }

	if ((size_t)index < mapsCount - 1) {
        memmove(&maps[indexOfMap], &maps[indexOfMap + 1], (mapsCount - indexOfMap - 1) * sizeof(map_t));
    }

    mapsCount--;

    if(mapsCount > 0) {
        maps = realloc(maps, mapsCount * sizeof(map_t));
	} else {
    	free(maps);
    	maps = NULL;
    }
}

void SaveMap(const char* path, map_t* map);
