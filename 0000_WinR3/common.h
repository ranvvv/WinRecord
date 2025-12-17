#pragma once

#include<windows.h>
#include<locale.h>
#include<stdio.h>
#include<tchar.h>
#include<Psapi.h>
#include<tlhelp32.h>
#include<process.h>
#include"resource.h"


void p000_000();
int mCreateConsole(const TCHAR title[]);
void mCloseConsole();


void p000_001();
void mDbgPrint(const TCHAR* const format, ...);
void mPError();

void p000_002();
void p000_003();
void p000_004();
void p000_005();
