/* Copyright (C) Benjamin James Read, 2022 - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Benjamin Read <benjamin-read@hotmail.co.uk>, January 2022
 */

#ifndef _SPELLCHECK_H
#define _SPELLCHECK_H

#include <stdbool.h>

void spellcheck_init(void);
void spellcheck_deinit(void);
bool spellcheck_isvalidword(const char *word);
bool spellcheck_checkstring(const char *string);

#endif // _SPELLCHECK_H