//
#include "stdafx.h"
#include "CommandPlugin.h"

CommandPlugin::~CommandPlugin()
{
}

String CommandPlugin::GetClassType() const
{
   return "Command";
}

#define MAXSIZE 150
String CommandPlugin::GetCommandInfo( const String &strParameters, BOOL bLocalizedName /*= TRUE*/, BOOL bAbbreviated/*=TRUE*/ ) const
{
   String strText;
   if (bLocalizedName)
      strText = GetTitleName() + " " + strParameters;
   else
      strText = GetCLIName() + " " + strParameters;

   if (bAbbreviated && strText.Length() > MAXSIZE)
      strText = strText.Left(MAXSIZE-4) + "..." + strText.Right(1);

   return strText;   
}

void CommandPlugin::Validate()
{
   //Validate function pointers
   if (GetCreateCommandInstanceCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Create Command Callback");

   if (GetSupportUndoCommandInstanceCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Undo Support Command Callback");

   if (GetDestroyCommandInstanceCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Destroy Command Callback");

   if (GetDoCommandInstanceCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Do Command Callback");

   if (GetRedoCommandInstanceCallback()==NULL) // Lets just set it to the do's.
      SetRedoCommandInstanceCallback((PFN_REDO_COMMANDINSTANCE)GetDoCommandInstanceCallback());

   if (GetUndoCommandInstanceCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Undo Command Callback");
}
