#pragma once

#include<iostream>
#include<ctime>
#include<string>

#include"dataLoader.h"
#include"sudoku.h"
#include"solve.h"
#include"ui.h"
#include"game.h"

#define GAME 0
#define SAT_PROB 1
#define SAT_CASE 0
#define UNSAT_CASE 0
#define GEN_CASE 1
#define TEST_CASE 0
#define USE_CDCL 1
#define USE_DPLL 0