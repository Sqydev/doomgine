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

void HandleSaveLoad(void) {
	if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) { DATA.MapMaker.saveMode = true; }
	if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L)) { DATA.MapMaker.loadMode = true; }

	if(DATA.MapMaker.saveMode) {
		if(IsKeyPressed(KEY_ESCAPE)) { DATA.MapMaker.saveMode = false; }

		DoPathInput(&DATA.MapMaker.path, &DATA.MapMaker.lenghtOfPath, &DATA.MapMaker.sizeOfPath);

		if(IsKeyPressed(KEY_ENTER) && DATA.MapMaker.path && DATA.MapMaker.lenghtOfPath > 0) {
			DATA.MapMaker.saveMode = false;

			char* extendedPath = ExpandPath(DATA.MapMaker.path);

			SaveMap(extendedPath, &DATA.MapMaker.map);

			if(extendedPath) { free(extendedPath); }
		}
	}
	if(DATA.MapMaker.loadMode) {
		if(IsKeyPressed(KEY_ESCAPE)) { DATA.MapMaker.loadMode = false; }

		DoPathInput(&DATA.MapMaker.path, &DATA.MapMaker.lenghtOfPath, &DATA.MapMaker.sizeOfPath);

		if(IsKeyPressed(KEY_ENTER) && DATA.MapMaker.path && DATA.MapMaker.lenghtOfPath > 0) {
			DATA.MapMaker.loadMode = false;

			char* extendedPath = ExpandPath(DATA.MapMaker.path);

			// Yes, I've done more efficient version of this. But it added completly unneccecery complecicy to map system for a thing that will be used only once. So yes, as much as it hurts me, I'm doing unnececery memory allocations :0
			map_t* tempMap = LoadMap(extendedPath);

			if(extendedPath) { free(extendedPath); }
			
			CopyMap(&DATA.MapMaker.map, tempMap);

			UnloadMap(tempMap);

			if(DATA.MapMaker.sectorColors) { free(DATA.MapMaker.sectorColors); }
			DATA.MapMaker.sectorColors = malloc(DATA.MapMaker.map.sectorsCount * sizeof(Color));
			for(size_t i = 0; i < DATA.MapMaker.map.sectorsCount; i++) {
        		DATA.MapMaker.sectorColors[i] = GetRandomDarkColor();
    		}
		}
	}
}
