/*
**
**  File:   ActorCreator.h
**  Date:   May 28, 2004
**  By:     Bryant Collard
**  Desc:   A mechanism for creating actors with a given name.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/ActorCreator.h $
**  $Revision: #2 $
**  $DateTime: 2004/12/16 19:58:42 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef ACTOR_CREATOR_H
#define ACTOR_CREATOR_H

#include "GameHelper/Creator.h"

// Forward declarations.
class CActor;

// Base class of objects that create actors.
class CActorCreator : public CCreator
{
  public:
	static CActor* Create(const char* i_name);
	static bool Query(const char* i_name);

  protected:
	virtual CActor* Create(void) = 0;
	static CActorCreator* s_head;
};

// A macro for defining a "standard" actor creator. The arguments are:
//   actorClass    - the class to be created.
//   actorName     - the name (not quoted) used to find the creator.
//
// This macro implements the Create() function which simply uses new to create
// the object.
//
// NOTE: The constuctor is implemented here because Add() calls Name() to
// check for multiple creators for a single name and Name() is not a valid
// function until its constructor is called.
//
// NOTE: See comment below to explain exporting.
#define STANDARD_ACTOR_CREATOR(actorClass, actorName)                         \
int g_export##actorClass;                                                     \
static class actorClass##Creator : public CActorCreator                       \
{                                                                             \
  public:                                                                     \
	actorClass##Creator() {Add((CCreator**)&s_head);}                          \
                                                                              \
  protected:                                                                  \
	virtual const char* Name(void) {return(#actorName);}                       \
	virtual CActor* Create(void) {return(new actorClass);}                     \
} l_##actorClass##Creator

// A macro for defining a "custom" actor creator. The arguments are:
//   actorClass    - the class to be created.
//   actorName     - the name (not quoted) used to find the creator.
//
// This macro expects the Create() function to be implemented. Use of this
// macro would look something like:
//
// CUSTOM_ACTOR_CREATOR(CFoo, Foo)::Create(void)
// {
// 	...
// }
//
// NOTE: The constuctor is implemented here because Add() calls Name() to
// check for multiple creators for a single name and Name() is not a valid
// function until its constructor is called.
//
// NOTE: See comment below to explain exporting.
#define CUSTOM_ACTOR_CREATOR(actorClass, actorName)                           \
int g_export##actorClass;                                                     \
static class actorClass##Creator : public CActorCreator                       \
{                                                                             \
  public:                                                                     \
	actorClass##Creator() {Add((CCreator**)&s_head);}                          \
                                                                              \
  protected:                                                                  \
	virtual const char* Name(void) {return(#actorName);}                       \
	virtual CActor* Create(void);                                              \
} l_##actorClass##Creator;                                                    \
CActor* actorClass##Creator

// NOTE: The following comment is derived from a comment in ComponentCreator.h
// and refers to a non-existent Actors project; however, such a project is
// anticipated so I left it in. -bec
//
// This is kind of ugly. Since the whole purpose of this class is to make it
// possible for something like a script to create an actor, it is very
// easy to write code that never explicitly references any member of the
// actor class. If the actor is in a static library, the linker will
// decide to ignore such an actor's object file. This characteristic results
// in the following rules:
//
// 1 - Actors must be defined in either game specific code or in the
//     Actors project.
//
// 2 - Use the following macro in ActorCreator.cpp to force a reference to
//     actors defined in the Actors project. Note that this applies
//     ONLY to general actors. The linker behavior does not affect game
//     specific actors (as long as all game specific code is in one project
//     that produces the game executable.
//
// Eventually, the Actors project should probably build a dynamically linked
// library. Rule #2 would go away (but #1 would become more important). Putting
// actors in a dynamic library would be a good thing anyway for a couple of
// reasons. First, there is a good conceptual argument for it. Actors can
// be thought of as a collection of things that can be used (or not used) as
// needed. That is sort of the idea behind dynamic libraries. Second, a game
// could be expanded by issuing a new collection of actors with scripts
// that use them.
#define EXPORT_ACTOR_CREATOR(actorClass)                                      \
extern int g_export##actorClass;                                                  \
static int l_export##actorClass = g_export##actorClass

#endif // ACTOR_CREATOR_H
