//------------------------------------------------------------------------------
//
// This file is part of AnandamideAPI Script
//
// copyright:	(c) 2010 - 2016
// author:		Alexey Egorov (FadeToBlack aka EvilSpirit)
// mailto:		anandamide@mail.ru
// 				anandamide.script@gmail.com
//
// AnandamideAPI is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// AnandamideAPI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with AnandamideAPI. If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------

#ifndef ANANDAMIDEAPI_GLOBAL_H
#define ANANDAMIDEAPI_GLOBAL_H
#if _WIN32
#ifdef ANANDAMIDEAPI_LIBRARY
	#define ANANDAMIDE_API __declspec(dllexport)
#else
	#define ANANDAMIDE_API __declspec(dllimport)
#endif
#else
#ifdef ANANDAMIDEAPI_LIBRARY
	#define ANANDAMIDE_API __attribute__((visibility("default")))
#else
	#define ANANDAMIDE_API __attribute__((visibility("default")))
#endif
#endif

#define ANANDAMIDE_UNUSED(x) (void)x

#pragma warning(disable:4996 4100 4804)

#endif
