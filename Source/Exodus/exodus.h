#pragma once
#ifndef PERIMETER_EXODUS_H
#define PERIMETER_EXODUS_H

#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include "types.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Window/UI related

//These seem to be used as mask for storing state in game code
#define MK_LBUTTON  0b1
#define MK_RBUTTON 0b10

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Sleep(uint32_t millis);

int __iscsym(int c);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Event/Thread stuff

#include <pevents.h>

#define INFINITE neosmart::WAIT_INFINITE

#define WAIT_OBJECT_0 0

HANDLE CreateEvent(int, bool manualReset, bool initialState, int);

void DestroyEvent(HANDLE event);

void SetEvent(HANDLE event);

void ResetEvent(HANDLE event);

uint32_t WaitForSingleObject(HANDLE event, uint64_t milliseconds);

uint32_t WaitForMultipleObjects(int count, HANDLE* events, bool waitAll, uint64_t milliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //PERIMETER_EXODUS_H
