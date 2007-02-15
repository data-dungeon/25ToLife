//
//
//  File:   MessageDefines.h
//  Date:   March 10, 2004
//  By:     Bryant Collard
//  Desc:   Some constants used by the message system AND used within scripts
//          using the message system.
//
//  Copyright (c) 2004, Avalanche Software Inc.
//  All Rights Reserved.
//
//      $File: //depot/Avalanche/GameLib/GameHelper/MessageDefines.h $
//  $Revision: #2 $
//  $DateTime: 2005/02/10 17:31:24 $
//    $Author: Bryant_Collard $
//
//

#ifndef MESSAGE_DEFINES_H
#define MESSAGE_DEFINES_H

// Handling/routing rules. 
#define MESSAGE_DISABLE              0x00   // Totally ignore
#define MESSAGE_HANDLE_IF_MASTER     0x01   // Handle if master
#define MESSAGE_HANDLE_IF_DUPLICA    0x02   // Handle if duplica
#define MESSAGE_ROUTE_TO_MASTER      0x04   // If duplica, route to master
#define MESSAGE_ROUTE_TO_DUPLICAS    0x08   // If master, route to duplicas
#define MESSAGE_ROUTE_TO_NEIGHBORS   0x10   // If master, route to duplicas.
                                            // If duplica, route to master
                                            // and duplicas.

// Basic types of data that may be sent in messages. Data in the form of a
// void* is sent with a message. The handler then casts it to the type of
// data sent. Also, knowledge about the data is needed to save the data when
// messages are posted or to pack and unpack the data for transmission over the
// network. While custom types can always be used, support for some basic types
// is supplied for ease of use.
//
// Many of the types are "owned". Owned data is transformed into the
// coordinate system of the handler's owner before transmission over the
// network and then transformed back on receipt. Say, for example, that the
// location where a bullet hit an actor expressed in world coordinates is the
// data. Transforming the location into and out of the actor's coordinates
// results in the hit appearing on the same place on the actor even if the
// actor's master and duplica are not in exactly the same location or
// orientation.
enum EMessageBasicDataType
{
	MESSAGE_VALUE,                  // The data argument is not a pointer but
	                                // actually contains a value such as an
	                                // int or a float.
	MESSAGE_ACTOR_PTR,              // A CActor.
	MESSAGE_ACTOR_HANDLE,           // A ActorHandle.
	MESSAGE_STRING,                 // A constant char array.
	MESSAGE_VECTOR3_PTR,            // A Vector3.
	MESSAGE_OWNED_VECTOR3_PTR,      // An owned Vector3 in world coordinates.
	MESSAGE_OWNED_POSITION_PTR,     // An owned position in the world.
	MESSAGE_MATRIX3X3_PTR,          // Any of the 3x3 matrices.
	MESSAGE_OWNED_ORIENTATION_PTR,  // An owned DirCos3x3 expressing an
	                                // orientation with respect to the world.
	MESSAGE_MATRIX4X4_PTR,          // Any of the 4x4 matrices.
	MESSAGE_OWNED_TRANSFORM_PTR,    // An owned Cartesian4x4 expressing a
	                                // transformation from a coordinate system
	                                // to the world.
	MESSAGE_CUSTOM                  // The data argument points to a custom
	                                // type.
};

#endif // MESSAGE_DEFINES_H
