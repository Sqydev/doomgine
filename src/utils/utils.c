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

#include "./utils.h"

#include "../coredata.h"

#include <stdlib.h>
#include <string.h>

Color GetRandomDarkColor(void) {
    Color c;

    do {
        c.r = rand() % 120;
        c.g = rand() % 120;
        c.b = rand() % 120;
        c.a = 255;

        int rg = abs(c.r - c.g);
        int rb = abs(c.r - c.b);
        int gb = abs(c.g - c.b);

        int brightness = c.r + c.g + c.b;

        if((rg < 15 && rb < 15 && gb < 15) || brightness < 60 || brightness > 250) {
            continue;
        }

        break;

    } while (1);

    return c;
}


Color GetLighterColor(Color color, float factor) {
	return (Color){
		color.r * factor,
		color.g * factor,
		color.b * factor,
		color.a
	};
}

void DoPathInput(char** path, size_t* lengthOfPath, size_t* sizeOfPath) {
    if(*path == NULL) {
        *sizeOfPath = 16;
        *lengthOfPath = 0;

        *path = malloc(*sizeOfPath);

        if(*path) { (*path)[0] = '\0'; }
    }

    Rectangle box = { 10, 10, 500, 40 };

    int key = GetCharPressed();

    while(key > 0) {
        if(key >= 32 && key <= 126) {
            if(*lengthOfPath + 1 >= *sizeOfPath) {
                *sizeOfPath *= 2;

                char* temp = realloc(*path, *sizeOfPath);

                if(temp) { *path = temp; }
            }

            (*path)[(*lengthOfPath)++] = (char)key;
            (*path)[*lengthOfPath] = '\0';
        }

        key = GetCharPressed();
    }

    if(IsKeyPressed(KEY_BACKSPACE) && *lengthOfPath > 0) {
        (*lengthOfPath)--;

        (*path)[*lengthOfPath] = '\0';
    }

    DrawRectangleRec(box, WHITE);
    DrawRectangleLinesEx(box, 2, BLACK);

    DrawText(*path, 15, 20, 20, BLACK);
}
