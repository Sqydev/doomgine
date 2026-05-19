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
 
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector2 SnapToExistingCornerIFItsClose(Vector2 pos) {
	for(size_t i = 0; i < DATA.MapMaker.map.sectorsCount; i++) {
		for(size_t j = 0; j < DATA.MapMaker.map.sectors[i].corners.count; j++) {
			if(DATA.MapMaker.isDragging && i == DATA.MapMaker.draggedSector && j == DATA.MapMaker.draggedCorner) { continue; }

			if(CheckCollisionPointCircle(pos, DATA.MapMaker.map.sectors[i].corners.positions[j], 0.4f)) {
				return DATA.MapMaker.map.sectors[i].corners.positions[j];
			}
		}
	}
	return pos;
}

void HandleSelection(void) {
	Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), DATA.MapMaker.camera);
	float radius = 15.0f / DATA.MapMaker.camera.zoom;
 
	for(size_t i = 0; i < DATA.MapMaker.map.sectorsCount; i++) {
		for(size_t j = 0; j < DATA.MapMaker.map.sectors[i].corners.count; j++) {
			if(CheckCollisionPointCircle(mouseWorld, DATA.MapMaker.map.sectors[i].corners.positions[j], radius)) {
				DATA.MapMaker.selectedSector = i;
				DATA.MapMaker.selectedCorner = j;

				DATA.MapMaker.isDragging = true;

				DATA.MapMaker.draggedSector = i;
				DATA.MapMaker.draggedCorner = j;

				return;
			}
		}
	}
}
 
void HandleDrag(void) {
	if(!DATA.MapMaker.isDragging) { return; }
 
	if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		Vector2 pos = GetScreenToWorld2D(GetMousePosition(), DATA.MapMaker.camera);

		if(!IsKeyDown(KEY_LEFT_CONTROL)) { pos = SnapToGrid(pos); }
		if(!IsKeyDown(KEY_LEFT_SHIFT)) { pos = SnapToExistingCornerIFItsClose(pos); }

		DATA.MapMaker.map.sectors[DATA.MapMaker.draggedSector].corners.positions[DATA.MapMaker.draggedCorner] = pos;
	}
	else {
		DATA.MapMaker.isDragging = false;
	}
}

void DrawSectors(void) {
	for(size_t currSec = 0; currSec < DATA.MapMaker.map.sectorsCount; currSec++) {
		for(size_t currCorner = 0; currCorner < DATA.MapMaker.map.sectors[currSec].corners.count; currCorner++) {
			size_t next = (currCorner + 1) % DATA.MapMaker.map.sectors[currSec].corners.count;

			Vector2 current = DATA.MapMaker.map.sectors[currSec].corners.positions[currCorner];
			Vector2 nextV = DATA.MapMaker.map.sectors[currSec].corners.positions[next];
 
			if(currSec == DATA.MapMaker.selectedSector) {
				DrawCircleLinesV(current, 0.65f, DATA.MapMaker.sectorColors[currSec]);
			}
			DrawLineV(current, nextV, DATA.MapMaker.sectorColors[currSec]);
			DrawCircleV(current, 0.5f, DATA.MapMaker.sectorColors[currSec]);
		}
	}
}

void CleanupMapMaker(void) {
	if(DATA.MapMaker.map.sectors) {
		for(size_t i = 0; i < DATA.MapMaker.map.sectorsCount; i++) {
			if(DATA.MapMaker.map.sectors[i].corners.positions) { free(DATA.MapMaker.map.sectors[i].corners.positions); }
			if(DATA.MapMaker.map.sectors[i].corners.exists) { free(DATA.MapMaker.map.sectors[i].corners.exists); }
		}
		free(DATA.MapMaker.map.sectors);
	}

	if(DATA.MapMaker.path) { free(DATA.MapMaker.path); }
}
 
void InitMapMaker(void) {
	DATA.MapMaker.inited = true;
 
	DATA.MapMaker.map.sectorsCount = 0;
	DATA.MapMaker.map.sectors = NULL;
 
	DATA.MapMaker.selectedSector = 0;
	DATA.MapMaker.selectedCorner = 0;
 
	DATA.MapMaker.camera.offset = (Vector2){ 0, 0 };
	DATA.MapMaker.camera.rotation = 0;
	DATA.MapMaker.camera.target = (Vector2){ 0, 0 };
	DATA.MapMaker.camera.zoom = 20.0f;
 
	DATA.MapMaker.sectorColors = NULL;

	DATA.MapMaker.isDragging = false;

	DATA.MapMaker.draggedSector = 0;
	DATA.MapMaker.draggedCorner = 0;
}
 
void MapMaker(void) {
	if(!DATA.MapMaker.inited) {
		InitMapMaker();
	}
 
	while(DATA.gamestate == GAMESTATE_MAPMAKER && !WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(WHITE);

		if(IsKeyReleased(KEY_N)) { AddSector(); AddCorner(); }
		if(IsKeyReleased(KEY_C) && DATA.MapMaker.map.sectorsCount > 0) { AddCorner(); }
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { HandleSelection(); }

		HandlePropertiesPanel();

		HandleCamera();
		HandleDrag();
 
		BeginMode2D(DATA.MapMaker.camera);

		DrawMYGrid();

		DrawSectors();

		EndMode2D();

		DrawPropertiesPanel();
 
		HandleSaveLoad();

		EndDrawing();
	}
 
	CleanupMapMaker();
}
