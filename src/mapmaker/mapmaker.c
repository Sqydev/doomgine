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

#include "./mapmaker.h"

#include "../coredata.h"

#include <raylib.h>
#include <stdlib.h>

void InitMapMaker(void) {
    DATA.MapMaker.inited = true;

    DATA.MapMaker.map.sectorsCount = 0;
    DATA.MapMaker.map.sectors = NULL;

    DATA.MapMaker.selectedSector = 0;
    DATA.MapMaker.selectedCorner = 0;
}

void MapMaker(void) {
	if(!DATA.MapMaker.inited) { InitMapMaker(); }

	// Yes. I remamber that there is another loop. It's planned
	while(DATA.gamestate == GAMESTATE_MAPMAKER && !WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(WHITE);

		if(IsKeyReleased(KEY_N)) {
			DATA.MapMaker.map.sectorsCount++;
			DATA.MapMaker.map.sectors = realloc(DATA.MapMaker.map.sectors, DATA.MapMaker.map.sectorsCount * sizeof(sector_t));
			DATA.MapMaker.selectedSector = DATA.MapMaker.map.sectorsCount - 1;
			DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.count = 0;
			DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.positions = NULL;
		}
		if(IsKeyReleased(KEY_C) && DATA.MapMaker.map.sectorsCount > 0) {
			DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.count++;
			DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.positions = realloc(DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.positions, DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.count * sizeof(Vector2));
			DATA.MapMaker.selectedCorner = DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.count - 1;
			DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.positions[DATA.MapMaker.selectedCorner] = GetMousePosition();
		}

		for(size_t currSec = 0; currSec < DATA.MapMaker.map.sectorsCount; currSec++) {
			for(size_t currCorner = 0; currCorner < DATA.MapMaker.map.sectors[currSec].corners.count; currCorner++) {
				if(currCorner > 0) { DrawLine(DATA.MapMaker.map.sectors[currSec].corners.positions[currCorner - 1].x, DATA.MapMaker.map.sectors[currSec].corners.positions[currCorner - 1].y, DATA.MapMaker.map.sectors[currSec].corners.positions[currCorner].x, DATA.MapMaker.map.sectors[currSec].corners.positions[currCorner].y, BLACK); }
				DrawCircle(DATA.MapMaker.map.sectors[currSec].corners.positions[currCorner].x, DATA.MapMaker.map.sectors[currSec].corners.positions[currCorner].y, 10, BLACK);
			}
		}

		EndDrawing();
	}

	if(DATA.MapMaker.map.sectors) {
		for(size_t i = 0; i < DATA.MapMaker.map.sectorsCount; i++) {
			if(DATA.MapMaker.map.sectors[i].corners.positions) { free(DATA.MapMaker.map.sectors[i].corners.positions); }
		}
		free(DATA.MapMaker.map.sectors);
	}
}
