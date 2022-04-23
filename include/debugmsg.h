/* Copyright (C) Benjamin James Read, 2022 - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Benjamin Read <benjamin-read@hotmail.co.uk>, Jnauary 2022
 */

#ifndef _DEBUGMSG_H
#define _DEBUGMSG_H

#define DEBUG_MSG

#if defined(DEBUG_MSG)

#define DEB(...) printf(__VA_ARGS__)

#else

#define DEB(message) 

#endif // DEBUG_MSG

#endif // _DEBUGMSG_H