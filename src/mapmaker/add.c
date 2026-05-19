/*
* Copyright (c) 2025-present W.K. ( _Sqyd_ / Sqydev )
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
#include "../utils/utils.h"

#include <stdlib.h>
#include <raylib.h>

void AddSector(void) {
	DATA.MapMaker.map.sectorsCount++;
	DATA.MapMaker.map.sectors = realloc(DATA.MapMaker.map.sectors, DATA.MapMaker.map.sectorsCount * sizeof(sector_t));

	DATA.MapMaker.selectedSector = DATA.MapMaker.map.sectorsCount - 1;

	DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.count = 0;
	DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.positions = NULL;
	DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.exists = NULL;

	DATA.MapMaker.sectorColors = realloc(DATA.MapMaker.sectorColors, DATA.MapMaker.map.sectorsCount * sizeof(Color));
	DATA.MapMaker.sectorColors[DATA.MapMaker.map.sectorsCount - 1] = GetColorBasedOnIndex(DATA.MapMaker.map.sectorsCount);
}

void AddCorner(void) {
	size_t sel = DATA.MapMaker.selectedSector;
 
	DATA.MapMaker.map.sectors[sel].corners.count++;
	DATA.MapMaker.map.sectors[sel].corners.positions = realloc(DATA.MapMaker.map.sectors[sel].corners.positions, DATA.MapMaker.map.sectors[sel].corners.count * sizeof(Vector2));
	DATA.MapMaker.map.sectors[sel].corners.exists = realloc(DATA.MapMaker.map.sectors[sel].corners.exists, DATA.MapMaker.map.sectors[sel].corners.count * sizeof(bool));
	DATA.MapMaker.selectedCorner = DATA.MapMaker.map.sectors[sel].corners.count - 1;

	Vector2 pos = GetScreenToWorld2D(GetMousePosition(), DATA.MapMaker.camera);
 
	if(!IsKeyDown(KEY_LEFT_CONTROL)) { pos = SnapToGrid(pos); }
	if(!IsKeyDown(KEY_LEFT_SHIFT)) { pos = SnapToExistingCornerIFItsClose(pos); }
 
	DATA.MapMaker.map.sectors[sel].corners.exists[DATA.MapMaker.selectedCorner] = true;
	DATA.MapMaker.map.sectors[sel].corners.positions[DATA.MapMaker.selectedCorner] = pos;
}
