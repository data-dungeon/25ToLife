/*
**
**  File:   ComponentCreator.h
**  Date:   May 28, 2004
**  By:     Bryant Collard
**  Desc:   A mechanism for creating components with a given name.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Components/ComponentCreator.h $
**  $Revision: #4 $
**  $DateTime: 2005/05/04 17:12:13 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef COMPONENT_CREATOR_H
#define COMPONENT_CREATOR_H

#include "GameHelper/Creator.h"

// Define some generally available priorities with plenty of room for others.
// Bigger prioritiy numbers are run first.
#define COMPONENT_STATE_OVERRIDE_PRIORITY  -100
#define COMPONENT_NEUTRAL_PRIORITY        0
#define COMPONENT_CONTROLLER_PRIORITY     100
#define COMPONENT_DRIVER_PRIORITY         200

// Forward declarations.
class CCompActor;
class CActorComponent;

// Base class of objects that create components.
class CComponentCreator : public CCreator
{
  public:
	static CActorComponent* Create(const char* i_name, CCompActor &i_actor);

  protected:
	virtual CActorComponent* Create(CCompActor &i_actor) = 0;
	virtual int Priority(void) {return(COMPONENT_NEUTRAL_PRIORITY);}
	static CComponentCreator* s_head;
};

// A macro for defining a "standard" component creator. The arguments are:
//   compClass    - the class to be created.
//   compName     - the name (not quoted) used to find the creator.
//   compPriority - the priority of the created object.
//
// This macro implements the Create() function which simply uses new to create
// the object with just the actor argument.
//
// NOTE: The constuctor is implemented here because Add() calls Name() to
// check for multiple creators for a single name and Name() is not a valid
// function until its constructor is called.
//
// NOTE: See comment below to explain exporting.
#define STANDARD_COMPONENT_CREATOR(compClass, compName, compPriority)          \
int g_export##compClass;                                                       \
static class compClass##Creator : public CComponentCreator                     \
{                                                                              \
  public:                                                                      \
	compClass##Creator() {Add((CCreator**)&s_head);}                            \
                                                                               \
  protected:                                                                   \
	virtual const char* Name(void) {return(#compName);}                         \
	virtual CActorComponent* Create(CCompActor &i_actor)                        \
		{return(new compClass(i_actor));}                                        \
	virtual int Priority(void) {return(compPriority);}                          \
} l_##compClass##Creator

// A macro for defining a "custom" component creator. The arguments are:
//   compClass    - the class to be created.
//   compName     - the name (not quoted) used to find the creator.
//   compPriority - the priority of the created object.
//
// This macro expects the Create() function to be implemented. Use of this macro
// would look something like:
//
// CUSTOM_COMPONENT_CREATOR(CFoo, Foo, COMPONENT_NEUTRAL_PRIORITY)::Create(
// 		CCompActor &i_actor, ParamaterSet* i_params)
// {
// 	...
// }
//
// NOTE: The constuctor is implemented here because Add() calls Name() to
// check for multiple creators for a single name and Name() is not a valid
// function until its constructor is called.
//
// NOTE: See comment below to explain exporting.
#define CUSTOM_COMPONENT_CREATOR(compClass, compName, compPriority)            \
int g_export##compClass;                                                       \
static class compClass##Creator : public CComponentCreator                     \
{                                                                              \
  public:                                                                      \
	compClass##Creator() {Add((CCreator**)&s_head);}                            \
                                                                               \
  protected:                                                                   \
	virtual const char* Name(void) {return(#compName);}                         \
	virtual CActorComponent* Create(CCompActor &i_actor);                       \
	virtual int Priority(void) {return(compPriority);}                          \
} l_##compClass##Creator;                                                      \
CActorComponent* compClass##Creator

// This is kind of ugly. Since the whole purpose of this class is to make it
// possible for something like a script to create a component, it is very
// easy to write code that never explicitly references any member of the
// component class. If the component is in a static library, the linker will
// decide to ignore such a component's object file. This characteristic results
// in the following rules:
//
// 1 - Components must be defined in either game specific code or in the
//     Components project.
//
// 2 - Use the following macro in ComponentCreator.cpp to force a reference to
//     components defined in the Components project. Note that this applies
//     ONLY to general components. The linker behavior does not affect game
//     specific components (as long as all game specific code is in one project
//     that produces the game executable.
//
// Eventually, the components project should probably build a dynamically linked
// library. Rule #2 would go away (but #1 would become more important). Putting
// components in a dynamic library would be a good thing anyway for a couple of
// reasons. First, there is a good conceptual argument for it. Components can
// be thought of as a collection of things that can be used (or not used) as
// needed. That is sort of the idea behind dynamic libraries. Second, a game
// could be expanded by issuing a new collection of components with scripts
// that use them.
#define EXPORT_COMPONENT_CREATOR(compClass)                                    \
extern int g_export##compClass;                                                \
static int l_export##compClass = g_export##compClass

#endif // COMPONENT_CREATOR_H
