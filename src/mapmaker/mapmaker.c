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
 
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
 
Vector2 SnapToGrid(Vector2 pos) { return (Vector2){ roundf(pos.x), roundf(pos.y) }; }
 
void AddSector(void) {
	DATA.MapMaker.map.sectorsCount++;
	DATA.MapMaker.map.sectors = realloc(DATA.MapMaker.map.sectors, DATA.MapMaker.map.sectorsCount * sizeof(sector_t));

	DATA.MapMaker.selectedSector = DATA.MapMaker.map.sectorsCount - 1;

	DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.count = 0;
	DATA.MapMaker.map.sectors[DATA.MapMaker.selectedSector].corners.positions = NULL;

	DATA.MapMaker.sectorColors = realloc(DATA.MapMaker.sectorColors, DATA.MapMaker.map.sectorsCount * sizeof(Color));
	DATA.MapMaker.sectorColors[DATA.MapMaker.map.sectorsCount - 1] = GetRandomDarkColor();
}
 
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
 
void AddCorner(void) {
	size_t sel = DATA.MapMaker.selectedSector;
 
	DATA.MapMaker.map.sectors[sel].corners.count++;
	DATA.MapMaker.map.sectors[sel].corners.positions = realloc(DATA.MapMaker.map.sectors[sel].corners.positions, DATA.MapMaker.map.sectors[sel].corners.count * sizeof(Vector2));
	DATA.MapMaker.selectedCorner = DATA.MapMaker.map.sectors[sel].corners.count - 1;
 
	Vector2 pos = GetScreenToWorld2D(GetMousePosition(), DATA.MapMaker.camera);
 
	if(!IsKeyDown(KEY_LEFT_CONTROL)) { pos = SnapToGrid(pos); }
	if(!IsKeyDown(KEY_LEFT_SHIFT)) { pos = SnapToExistingCornerIFItsClose(pos); }
 
	DATA.MapMaker.map.sectors[sel].corners.positions[DATA.MapMaker.selectedCorner] = pos;
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
 
void HandleCamera(void) {
	if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
		Vector2 delta = GetMouseDelta();

		DATA.MapMaker.camera.target.x -= delta.x / DATA.MapMaker.camera.zoom;
		DATA.MapMaker.camera.target.y -= delta.y / DATA.MapMaker.camera.zoom;
	}
 
	float wheel = GetMouseWheelMove();
	if(wheel != 0) {
		Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), DATA.MapMaker.camera);

		DATA.MapMaker.camera.zoom *= (1.0f + wheel * 0.1f);

		if(DATA.MapMaker.camera.zoom < 4.0f) { DATA.MapMaker.camera.zoom = 4.0f; }
		if(DATA.MapMaker.camera.zoom > 200.0f) { DATA.MapMaker.camera.zoom = 200.0f; }

		Vector2 newMouseWorld = GetScreenToWorld2D(GetMousePosition(), DATA.MapMaker.camera);

		DATA.MapMaker.camera.target.x += mouseWorld.x - newMouseWorld.x;
		DATA.MapMaker.camera.target.y += mouseWorld.y - newMouseWorld.y;
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
 
void HandleInput(void) {
	if(IsKeyReleased(KEY_N)) { AddSector(); AddCorner(); }
	if(IsKeyReleased(KEY_C) && DATA.MapMaker.map.sectorsCount > 0) { AddCorner(); }
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { HandleSelection(); }
	HandleCamera();
	HandleDrag();
}
 
void DrawMYGrid(void) {
	Vector2 topLeft = GetScreenToWorld2D((Vector2){ 0, 0 }, DATA.MapMaker.camera);
	Vector2 bottomRight = GetScreenToWorld2D((Vector2){ (float)DATA.Info.screenWidth, (float)DATA.Info.screenHeight }, DATA.MapMaker.camera);
 
	for(int x = (int)floorf(topLeft.x); x <= (int)ceilf(bottomRight.x); x++) {
		DrawLine(x, (int)floorf(topLeft.y), x, (int)ceilf(bottomRight.y), LIGHTGRAY);
	}
	for(int y = (int)floorf(topLeft.y); y <= (int)ceilf(bottomRight.y); y++) {
		DrawLine((int)floorf(topLeft.x), y, (int)ceilf(bottomRight.x), y, LIGHTGRAY);
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
		}
		free(DATA.MapMaker.map.sectors);
	}
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
 
		HandleInput();
 
		BeginMode2D(DATA.MapMaker.camera);

		DrawMYGrid();
		DrawSectors();

		EndMode2D();
 
		EndDrawing();
	}
 
	CleanupMapMaker();
}

