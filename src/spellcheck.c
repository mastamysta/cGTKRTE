/* Copyright (C) Benjamin James Read, 2022 - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Benjamin Read <benjamin-read@hotmail.co.uk>, January 2022
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "spellcheck.h"
#include "debugmsg.h"

#include <hunspell/hunspell.h>

static Hunhandle *spellchecker;

// initialised the static spellchecker handle. At the moment this takes no args and sets up a en_US dictionary
// in future the function can take an arg to decide upon languages.

void spellcheck_init(void)
{
    DEB("%s", "Initialising spellchecker...\n");

    if (!spellchecker)
        spellchecker = Hunspell_create("../res/hunspell-en_US/en_US.aff", "../res/hunspell-en_US/en_US.dic");
}

// deinits the static spelchecker handle.

void spellcheck_deinit(void)
{
    DEB("%s", "Deinitialising spellchecker...\n");

    if(spellchecker)
        Hunspell_destroy(spellchecker);
}

// returns true for a valid word and false for an invalid word

bool spellcheck_isvalidword(const char *word)
{
    bool ret = false;

    if (!spellchecker)
        DEB("%s", "Spellchecker was not inited");
    else
        ret = (Hunspell_spell(spellchecker, word) == 1);

    return ret;
}

// tokenizes all words in a sentence

bool spellcheck_checkstring(const char *string)
{
    return true;
}