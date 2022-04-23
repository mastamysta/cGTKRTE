/* Copyright (C) Benjamin James Read, 2022 - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Benjamin Read <benjamin-read@hotmail.co.uk>, January 2022
 */

#include <stdio.h>
#include <stdlib.h>

#include "maingraphics.h"
#include "spellcheck.h"

int main(int argc, char **argv)
{

    spellcheck_init();

    // call into the graphics engine

    launchGraphics(argc, argv);

    spellcheck_deinit();

    return 0;
}