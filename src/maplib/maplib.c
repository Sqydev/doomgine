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

#include "../coredata.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	for(size_t i = 0; i < DATA.maps.mapsCount; i++) {
        if(DATA.maps.maps[i].id == id) {
            return &DATA.maps.maps[i];
        }
    }
    return NULL;
}

map_t* LoadMap(const char* path) {
	FILE* file = fopen(path, "r");

	DATA.maps.maps = realloc(DATA.maps.maps, DATA.maps.mapsCount * sizeof(map_t));

	DATA.maps.maps[DATA.maps.mapsCount].id = ++DATA.maps.lastId;
	DATA.maps.maps[DATA.maps.mapsCount].sectorsCount = countLines(file);
	DATA.maps.maps[DATA.maps.mapsCount].sectors = realloc(DATA.maps.maps[DATA.maps.mapsCount].sectors, DATA.maps.maps[DATA.maps.mapsCount].sectorsCount * sizeof(sector_t));

	char* line = NULL;
	size_t lineSize = 0;
	ssize_t read = 0;

	size_t currentSector = 0;

	while((read = getline(&line, &lineSize, file)) != -1) {
		DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].floor.visible = atoi(strtok(line, ";"));
		DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].floor.height = atof(strtok(NULL, ";"));

		DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].ceiling.visible = atoi(strtok(line, ";"));
		DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].ceiling.height = atof(strtok(NULL, ";"));

		DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].light.level = atof(strtok(NULL, ";"));

		DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].corners.count = atoi(strtok(NULL, ";"));
		DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].corners.positions = malloc(DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].corners.count * sizeof(Vector2));
		for(size_t i = 0; i < DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].corners.count; i++) {
			DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].corners.positions[i].x = atof(strtok(NULL, ";"));
			DATA.maps.maps[DATA.maps.mapsCount].sectors[currentSector].corners.positions[i].y = atof(strtok(NULL, ";"));
		}

		currentSector++;
	}

	if(line) { free(line); }

	fclose(file);

	DATA.maps.mapsCount++;

	return &DATA.maps.maps[DATA.maps.mapsCount - 1];
}

void UnloadMap(map_t* map) {
	if(!map || DATA.maps.mapsCount == 0) { return; }

	ptrdiff_t indexOfMap = map - DATA.maps.maps; // MATH!
	
	if(indexOfMap < 0 || (size_t)indexOfMap >= DATA.maps.mapsCount) {
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

	if((size_t)index < DATA.maps.mapsCount - 1) {
        memmove(&DATA.maps.maps[indexOfMap], &DATA.maps.maps[indexOfMap + 1], (DATA.maps.mapsCount - indexOfMap - 1) * sizeof(map_t));
    }

    DATA.maps.mapsCount--;

    if(DATA.maps.mapsCount > 0) {
        DATA.maps.maps = realloc(DATA.maps.maps, DATA.maps.mapsCount * sizeof(map_t));
	} else {
    	free(DATA.maps.maps);
    	DATA.maps.maps = NULL;
    }
}

void SaveMap(const char* path, map_t* map) {
	FILE* file = fopen(path, "w");
	
	for(size_t i = 0; i < map->sectorsCount; i++) {
		fprintf(file, "%d;%f;%d;%f;%f;%zu",
		  map->sectors[i].floor.visible,
		  map->sectors[i].floor.height,
		  map->sectors[i].ceiling.visible,
		  map->sectors[i].ceiling.height,
		  map->sectors[i].light.level,
		  map->sectors[i].corners.count
		);

		for(size_t j = 0; j < map->sectors->corners.count; j++) {
			fprintf(file, ";%f", map->sectors[i].corners.positions->x);
			fprintf(file, ";%f", map->sectors[i].corners.positions->y);
		}
	}

	fclose(file);
}
