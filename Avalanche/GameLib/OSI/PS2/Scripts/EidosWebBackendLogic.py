# -----------------------------------------------------------------------------
# EidosWebBackendLogic.py
#
# Python implementation of backend services available to web sites via RPC.
#
# This file contains cross-game functionality.
# See also TTLWebBackendLogic.py for 25 to Life-specific functionality.
#
# Originally written for "Project: Snowblind" (by Crystal Dynamics) website.
# (Branched from 2005-02-14 (a)).
#
# This file is one of many scripts hosted in Quazal's Rendez-Vous
# environment.
#
#
# Author: Dan Kamins <dew@axonchisel.net> / <dkamins@crystald.com>
# Date: 2005-08-16
#
# (c) Copyright 2005, Eidos Inc.
# ALL RIGHTS RESERVED
# -----------------------------------------------------------------------------

# Python core modules:
import base64
import datetime
import md5
import random
import re

# Quazal modules:
import quazal
import AccountManagement

# Eidos modules:
import EidosLocales


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# CONSTANTS
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# Successful result code (shared with web client code):
RES_OKAY = 0

# Generic backend error (shared with web client code):
ERROR_BACKEND_GENERIC = 400

# Backend error for function not yet implemented (shared with web client code):
ERROR_BACKEND_NOT_IMPLEMENTED = 403

# How often users are allowed to reset their password (in hours):
# (Recommended value: 48)
ACCT_PASSWORD_RESET_PERIOD_IN_HOURS = 1

# How long a password Reset Code is valid for, from generation (in hours):
# (Recommended value: 72)
ACCT_PASSWORD_RESET_CODE_LIFETIME_IN_HOURS = 1

# Friend relation types, as stored in DB (Left *REL* Right):
FRIEND_REL_NONE		= 0		# No relation yet from Left to Right.
FRIEND_REL_ASKED	= 1		# Left asked Right.
FRIEND_REL_BLOCKED	= 2		# Left blocked Right.

# Friend states, as returned by EidosBack_Friend_GetFriends:
FRIENDS_INV_BY		= 'FIBY'	# I was invited by him, but I haven't responded.
FRIENDS_INV_TO		= 'FITO'	# I invited him, but no response (or is blocking).
FRIENDS_MUTUAL		= 'FMUT'	# We are mutual friends.
FRIENDS_BLOCKED		= 'FBLK'	# I have him on my block list.


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# API DEFINITION
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Define public methods of this API:
#  (User_*, Friend_*, Test_*)
myPublicMethods = [
	'EidosBack_DescribeInterface',

	'EidosBack_User_LoginAttempt',
	'EidosBack_User_RequestPasswordResetCode',
	'EidosBack_User_PasswordReset',

	'EidosBack_Friend_GetFriends',

	'EidosBack_Test_01',
	'EidosBack_Test_02',
	'EidosBack_Test_SetPassword',
	'EidosBack_Test_03',
	'EidosBack_Test_GetPlayerList',
	'EidosBack_Test_SQLSelect'
	]




# -----------------------------------------------------------------------------
def RegisterMethods(protocol):
	"""Register externally available RPC methods.
	
	This method is called from UserConfiguration.py.
	All RPC methods exposed by this interface are enumerated here.

	@author Dan Kamins <dew@axonchisel.net>
	"""
	for methodName in myPublicMethods:
		protocol.RegisterMethod(__name__ + "." + methodName)



# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# PUBLIC INTERFACE
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
def EidosBack_DescribeInterface():

	"""List available RPC methods.

	@author Dan Kamins <dew@axonchisel.net>
	"""
	return myPublicMethods


# -----------------------------------------------------------------------------
def EidosBack_User_LoginAttempt(account, passwordHash):

	"""Attempt user login, or just account name resolution to Uid.

	The password is not sent in cleartext, but in a hashed format,
	using the Uid as a component of the algorithm.

	Two ways to get the Uid:
	 1. Cache it (e.g. in a client-side cookie).
	 2. Call this method first with any password (eg blank), which will result
		in "Incorrect Password" error, but a valid Uid (if valid account).

	Note: On "incorrect password" errors, the assumed Uid may have been wrong
	(and thus initially hashed password came out incorrectly).
	If returned Uid is different from the one used for hashing, try again!

	@param string account		Account name to login as. Case-INsensitive.
								Real (proper-case) account name is returned.
	@param string passwordHash	Password (hashed).

	Returns (rescode, account, uid, lastLogin).
	
	@return rescode		int indicates result of call.
	@return account		string reveals real (proper case) account name (if valid acct).
	@return uid 		int Uid of account (Principal ID) (if valid acct).
	@return lastLogin	int timestamp of last login time (if valid acct).
	
		Rescode   0: Okay.
		Error 10001: Unknown account.
		Error 10002: Incorrect password.
		Error 10003: Not allowed (eg banned).

	@author Dan Kamins <dew@axonchisel.net>
	"""
	
	# Convert Unicode strings (from XML-RPC lib) to UTF8 raw (for backend):
	account = account.encode('utf8')

	#
	strMethod = "EidosBack_User_LoginAttempt"
	LogMethodCall(strMethod, "account=%s, pw=%s" % (account, (len(passwordHash)>0)) );
	c = GetDBCursor()

	# Load/validate Principal db rec:
	principalFields = ["p.id", "p.name", "p.password"]
	c.execute("SELECT "+",".join(principalFields)+" FROM Principals p WHERE (p.name=%s)", [account])
	principalRec = c.fetchone()
	if (principalRec == None):
		LogDetails(strMethod, "Cannot resolve account %s" % (account))
		return (10001, "", "", "")
	acctPid = principalRec[principalFields.index('p.id')]
	acctName = principalRec[principalFields.index('p.name')]
	acctPass = principalRec[principalFields.index('p.password')]

	# Check password (hashed version against hashed version):
	if (passwordHash != acctPass):
		LogDetails(strMethod, "Incorrect password for account %s" % (account))
		return (10002, acctName, acctPid, "")

	# TODO: Check for banned users. Return 10003.
	
	# Return OK:
	LogDetails(strMethod, "Login "+account+" ok.")
	return (RES_OKAY, acctName, acctPid, "")


# -----------------------------------------------------------------------------
def EidosBack_User_RequestPasswordResetCode(emailAddr, account, fromClientIp):
	
	"""Request a password reset code for a particular user (step 1).
	
	THIS METHOD AUTHORIZES a user to reset their forgotten password.
	The "password reset code" generated and returned can then be passed
	to the EidosBack_User_PasswordReset() function to reset the password.
	
	There are TWO STEPS IN ORDER TO PREVENT ABUSE and ensure that 
	the user is fully aware of and involved in the password reset.

	The RESET CODE GENERATED here is typically sent by email to the
	user, who then enters it into the web site, at which point the
	actual password is reset.
	
	To SPECIFY WHICH ACCOUNT TO RESET, either or both of the params
	"emailAddr" and "account" must be specified.
	Because email addresses may not be unique, the account name may be 
	necessary to disambiguate the account in question.
	This method will fail with "account ambiguous" (10003) if it is
	unable to determine exactly which account is requested.
	
	There is also a LIMIT ON HOW FREQUENTLY PASSWORD CAN BE RESET,
	which is enforced by this function.
	This is based on the constant "ACCT_PASSWORD_RESET_PERIOD_IN_HOURS".
	
	Reset Codes are only VALID FOR A SMALL WINDOW OF TIME.
	This is based on the constant "ACCT_PASSWORD_RESET_CODE_LIFETIME_IN_HOURS".
		
	@param string emailAddr	Optional email address of account to reset password for.
							Empty string to rely on account param only.
	@param string account	Optional account name to reset password for.
							Empty string to rely on emailAddr param only.
	@param string fromClientIp	Client IP address, to log along with request
								(to prevent abuse).
	
	Returns (rescode, emailAddr, account, uid, resetCode).
	
	@return rescode		int indicates result of call.
	@return emailAddr	string actual email addr on file.
	@return account		string actual account name.
	@return uid			int actual uid of account.
	@return resetCode	string (22 alphanumeric chars) code generated for 
						EidosBack_User_PasswordReset to reset password.

	rescodes:
		Rescode   0: Okay.
		Error 10001: invalid email addr.
		Error 10002: invalid account name.
		Error 10003: account ambiguous. (must specify email AND account).
		Error 10004: no matching accounts.
		Error 10011: too recent.

	@author Dan Kamins <dew@axonchisel.net>
	"""
	
	# Convert Unicode strings (from XML-RPC lib) to UTF8 raw (for backend):
	emailAddr = emailAddr.encode('utf8')
	account = account.encode('utf8')

	#
	strMethod = "EidosBack_User_RequestPasswordResetCode"
	LogMethodCall(strMethod, "emailAddr='%s', account='%s', fromClientIp='%s'" % (emailAddr, account, fromClientIp));
	c = GetDBCursor()

	# Find account(s) in question:
	acctsResult = FindAccountByEmailAndOrName(emailAddr, account)
	(foundCount, acctPid, acctName, acctEmail, acctPassword) = acctsResult

	# Ambiguous (more than 1 result)?
	if (foundCount > 1):
		LogDetails(strMethod, "Ambiguous (emailAddr='%s', account='%s')." % (emailAddr, account))
		return (10003, '', '', 0, '')

	# None found?
	if (foundCount == 0):
		LogDetails(strMethod, "No matching accts (emailAddr='%s', account='%s')." % (emailAddr, account))
		return (10004, '', '', 0, '')

	LogDetails(strMethod, "Found account '%s' (pid=%s)." % (acctName, acctPid))

	# Load time of last forgotten password email:
	c.execute("SELECT lastforgotemailsent FROM EidosAccounts WHERE (pid=%s)", [acctPid])
	acctLastForgot = c.fetchone()[0]
	
	# Make sure enough time has passed since last reset:
	timeNow = datetime.datetime.now()
	if (acctLastForgot is not None):
		timeDelta = datetime.timedelta(0, ACCT_PASSWORD_RESET_PERIOD_IN_HOURS * 3600)
		timeAllowedReset = acctLastForgot + timeDelta
		LogDetails(strMethod, "Checking dates: last=%s, next=%s, now=%s" % (
			acctLastForgot.isoformat(), timeAllowedReset.isoformat(), timeNow.isoformat()))
		if (timeNow < timeAllowedReset):
			LogDetails(strMethod, "Too recent!")
			return (10011, acctEmail, acctName, acctPid, '')
	else:
		LogDetails(strMethod, "First time forgot password: now=%s" % (timeNow.isoformat()))

	# Update last forgot password time:
	c.execute("UPDATE EidosAccounts SET lastforgotemailsent=%s WHERE pid=%s",
		[timeNow, acctPid])
	if (c.rowcount < 1):
		LogDetails(strMethod, "Unable to update lastforgotemailsent!")
		return (ERROR_BACKEND_GENERIC, acctEmail, acctName, acctPid, '')

	# Load time of last forgotten password email 
	# (to make sure it's EXACTLY how it will be when we compare it later):
	c.execute("SELECT lastforgotemailsent FROM EidosAccounts WHERE (pid=%s)", [acctPid])
	acctLastForgot = c.fetchone()[0]

	# Create reset code (based on new time we just set):
	resetCode = CreatePasswordResetCode(acctPassword, acctLastForgot)
	LogDetails(strMethod, "Generated ResetCode: '%s'" % (resetCode))
	
	# Return:
	return (RES_OKAY, acctEmail, acctName, acctPid, resetCode)

	

# -----------------------------------------------------------------------------
def EidosBack_User_PasswordReset(emailAddr, account, resetCode, fromClientIp):
	"""Reset a password for a particular user (step 2).
	
	THIS METHOD RESETS THE PASSWORD for a user who forgot his password.
	A "password reset code" (generated by EidosBack_User_RequestPasswordResetCode)
	is required to properly complete this step.
	
	There are TWO STEPS IN ORDER TO PREVENT ABUSE and ensure that 
	the user is fully aware of and involved in the password reset.

	To SPECIFY WHICH ACCOUNT TO RESET, either or both of the params
	"emailAddr" and "account" must be specified.
	Because email addresses may not be unique, the account name may be 
	necessary to disambiguate the account in question.
	This method will fail with "account ambiguous" (10003) if it is
	unable to determine exactly which account is requested.
	
	Once the password is reset, the reset code will no longer be valid.

	@param string emailAddr	Optional email address of account to reset password for.
							Empty string to rely on account param only.
	@param string account	Optional account name to reset password for.
							Empty string to rely on emailAddr param only.
	@param string resetCode	Reset Code, to authorize the password reset.
							From EidosBack_User_RequestPasswordResetCode.
	@param string fromClientIp	Client IP address, to log along with request
								(to prevent abuse).
	
	Returns (rescode, emailAddr, account, uid, newPassword).
	
	@return rescode		int indicates result of call.
	@return emailAddr	string actual email addr on file.
	@return account		string actual account name.
	@return uid			int actual uid of account.
	@return newPassword	string new random password generated for the user.

	rescodes:
		Rescode   0	Okay.
		Error 10001: invalid email addr.
		Error 10002: invalid account name.
		Error 10003: account ambiguous. (must specify email AND account).
		Error 10004: no matching accounts.
		Error 10021: invalid reset code.

	@author Dan Kamins <dew@axonchisel.net>
	"""

	# Convert Unicode strings (from XML-RPC lib) to UTF8 raw (for backend):
	emailAddr = emailAddr.encode('utf8')
	account = account.encode('utf8')
	resetCode = resetCode.encode('utf8')

	#
	strMethod = "EidosBack_User_PasswordReset"
	LogMethodCall(strMethod, "emailAddr='%s', account='%s', code='%s', fromClientIp='%s'" % (emailAddr, account, resetCode, fromClientIp));
	c = GetDBCursor()

	# Find account(s) in question:
	acctsResult = FindAccountByEmailAndOrName(emailAddr, account)
	(foundCount, acctPid, acctName, acctEmail, acctPassword) = acctsResult

	# Ambiguous (more than 1 result)?
	if (foundCount > 1):
		LogDetails(strMethod, "Ambiguous (emailAddr='%s', account='%s')." % (emailAddr, account))
		return (10003, '', '', 0, '')

	# None found?
	if (foundCount == 0):
		LogDetails(strMethod, "No matching accts (emailAddr='%s', account='%s')." % (emailAddr, account))
		return (10004, '', '', 0, '')

	LogDetails(strMethod, "Found account '%s' (pid=%s)." % (acctName, acctPid))

	# Load time of last forgotten password email:
	c.execute("SELECT lastforgotemailsent FROM EidosAccounts WHERE (pid=%s)", [acctPid])
	acctLastForgot = c.fetchone()[0]

	# Make sure reset code hasn't expired:
	timeNow = datetime.datetime.now()
	if (acctLastForgot is not None):
		timeDelta = datetime.timedelta(0, ACCT_PASSWORD_RESET_CODE_LIFETIME_IN_HOURS * 3600)
		timeLatestAllowedReset = acctLastForgot + timeDelta
		LogDetails(strMethod, "Checking dates: last=%s, latest=%s, now=%s" % (
			acctLastForgot.isoformat(), timeLatestAllowedReset.isoformat(), timeNow.isoformat()))
		if (timeNow > timeLatestAllowedReset):
			LogDetails(strMethod, "Too late!")
			return (10021, acctEmail, acctName, acctPid, '')

	# Create reset code:
	realResetCode = CreatePasswordResetCode(acctPassword, acctLastForgot)
	LogDetails(strMethod, "Generated ResetCode: '%s' vs requested '%s'" % (realResetCode, resetCode))
	
	# Compare reset code requested to real one:
	if (resetCode != realResetCode):
		LogDetails(strMethod, "Reset codes mismatch.")
		return (10021, '', '', 0, '')

	# Pick a random password (str -> md5 -> base64 -> substr) (result like '3g7rXSt9'):
	passwordLength = 6
	randStr = datetime.datetime.now().isoformat() + str(random.randint(1000000, 9999999))
	randHash = md5.new(randStr).digest()
	randBigPass = base64.b64encode(randHash, "DK")
	newPassword = randBigPass[:passwordLength]
	# Replace visually ambiguous characters with sweet-ass replacements:
	newPassword = newPassword.replace('1', 'D').replace('I', 'd').replace('l', 'K')
	newPassword = newPassword.replace('0', 'k').replace('O', 'D')
	
	# Update password:
	quazal.SecurityContextManager.GetInstance().Push(acctPid, 1)
	try:
		AM = AccountManagement.AccountManagementProtocol()
		initialKey = AM.scriptengine.InitializeKey(str(newPassword))
		res = AM.ChangePassword(initialKey)
		if (not res):
			LogError(strMethod, "Error changing password,")
			return (ERROR_BACKEND_GENERIC, '', '', 0, '')
	finally:
		quazal.SecurityContextManager.GetInstance().Pop()
	
	LogDetails(strMethod, "Reset codes approved. New password for pid(%s)='%s'." % (acctPid, newPassword))

	# Return:
	return (RES_OKAY, acctEmail, acctName, acctPid, newPassword)





# -----------------------------------------------------------------------------
def EidosBack_Friend_GetFriends(ofUid, locale):
	"""For indicated user, get list of friends/potential friends and their statuses.
	
	This method returns information about friends of the indicated user,
	including mutual friends, potential friends we invited, 
	pending friend invitations to us, and people we have blocked as friends.
	
	The order of the returned friends list is consistent but not specified,
	and subject to change.
	
	@param ofUid	int		Principal Id of user to retrieve friend list for.
	
	@param locale	string	Locale to translate status text components into.
							Eg "english", "french", or "" (empty) for raw.
							See EidosLocales module for more info.

	
	@return rescode followed by a list of friend records (one per friend):
	
		( rescode, (friendState, uid, name, Status)* )
		
		Status:
				(statusRaw, statusLocalTitle, statusLocalPlatform, statusLocalRegion)


		rescode		int		indicates result of call.
		friendState	string	Friend state token. One of:
								'FIBY'=invited by,     'FITO'=sent invite to,
								'FMUT'=mutual friends, 'FBLK'=on my block list.
		uid 		int		Principal Id of friend entry.
		name 		string	Name (account) of friend.
		statusRaw	string	Eidos status string, (eg "Snowblind|PS2|US|...."),
							indicates which game user is logged into (if any).
							"Offline" (a global string constant) if offline.
		statusLocalTitle string Localized game name, eg "Project: Snowblind".
		statusLocalPlatform string Localized platform, eg "Playstation 2".
		statusLocalRegion string Localized region, eg "United States"
							(Region localization not currently supported).

	rescodes:
		Rescode   0	Okay.
		Error 10001	invalid uid requested.

	"""
	strMethod = "EidosBack_Friend_GetFriends(ofUid=%d)" % (ofUid)
	LogMethodCall(strMethod, "...");
	c = GetDBCursor()

	# Clean params:
	ofUid = int(ofUid)

	# Build lstFriendRels containing (otherPid, relationTo, relationFrom)
	# (Relationship types: 0=Nothing, 1=Asked, 2=Blocked. See FRIEND_REL_* constants)
	c.execute("SELECT pid2, rel1, rel2 FROM EidosFriends ef WHERE (pid1=%s)", [ofUid])
	lstFriendRels = c.fetchall()
	c.execute("SELECT pid1, rel2, rel1 FROM EidosFriends ef WHERE (pid2=%s)", [ofUid])
	lstFriendRels += c.fetchall()

	# Log result and return now if no friends:
	LogDetails(strMethod, " Found %d 'friend' relationships." % len(lstFriendRels))
	if (len(lstFriendRels) == 0):
		return (RES_OKAY, [])

	# Make sCslFriendPids string (comma separated list of friend pids):
	lstFriendStrPids = map(lambda friendRel: str(int(friendRel[0])), lstFriendRels)
	sCslFriendPids = ",".join(lstFriendStrPids) 

	# Select detailed info about friends and build mapping of pid to friend details:
	c.execute("SELECT p.id, p.name, s.status "
		" FROM Principals p "
		" LEFT JOIN PrincipalStatus s on (s.pid=p.id) "
		" WHERE (p.id IN (%s)) " % sCslFriendPids, [])
	mapFriendDetailsByPid = dict( [ (row[0], (row[1:])) for row in c.fetchall() ] )

	# Build return array:
	lstFriendReturn = []
	for friendRec in lstFriendRels:
		# Extract friendRel parts and find this guy's friendDetail rec:
		(friendUid, relTo, relFrom) = friendRec
		friendDetail = mapFriendDetailsByPid[friendUid]
		friendName = str(friendDetail[0])
		friendStatusRaw = str(friendDetail[1] or 'Offline')
		
		# Determine friendState:
		friendState = None
		if (relFrom == FRIEND_REL_ASKED):
			if   (relTo == FRIEND_REL_BLOCKED):	friendState = FRIENDS_BLOCKED
			elif (relTo == FRIEND_REL_ASKED):	friendState = FRIENDS_MUTUAL
			else:								friendState = FRIENDS_INV_BY
		elif (relFrom == FRIEND_REL_NONE):
			if   (relTo == FRIEND_REL_BLOCKED):	friendState = FRIENDS_BLOCKED
			elif (relTo == FRIEND_REL_ASKED):	friendState = FRIENDS_INV_TO
		elif (relFrom == FRIEND_REL_BLOCKED):
			if   (relTo == FRIEND_REL_ASKED):	friendState = FRIENDS_INV_TO
			elif (relTo == FRIEND_REL_NONE):	continue
		if (friendState is None):
			LogError(strMethod, "Unknown relationship: (pid:%d, rel:%d, pid:%d, rel:%d)" % (ofUid, relTo, friendUid, relFrom))
			continue
		
		# Assemble/append a friendReturn record:
		friendStatusStruct = LocalizeStatusFromRaw(friendStatusRaw, locale)
		friendReturn = (friendState, int(friendUid), friendName, friendStatusStruct)
		lstFriendReturn.append(friendReturn)

	# Return:
	return (RES_OKAY, lstFriendReturn)
	
	


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# PRIVATE HELPERS
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# -------------------------------------------------------------------------
def Log(strMessage):
	"""Helper function to log a general message, top level. 

	@author Dan Kamins <dew@axonchisel.net>
	"""
	print "EidosBack:: %s." % (strMessage)

# -------------------------------------------------------------------------
def LogMethodCall(strMethod, strArgs=""):
	"""Helper function to log a major method invocation + args, with timestamp, top level.
	
	@author Dan Kamins <dew@axonchisel.net>
	"""
	print "EidosBack:: >>%s() @ %s - BEGIN - %s." % (
		strMethod, datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S%z"), strArgs )

# -------------------------------------------------------------------------
def LogError(strMethod, strErrorMessage):
	"""Helper function to log serious (e.g. DB) errors in method, about to return.
	
	@author Dan Kamins <dew@axonchisel.net>
	"""
	print "EidosBack:: ! ! %s - ERROR! - %s." % (strMethod, strErrorMessage)

# -------------------------------------------------------------------------
def LogDetails(strMethod, strMessage):
	"""Helper function to log a message, slightly indented, less important than self.Log().

	@author Dan Kamins <dew@axonchisel.net>
	"""
	print "EidosBack::     %s - %s." % (strMethod, strMessage)


# -------------------------------------------------------------------------
def GetDBCursor():
	"""Helper function to get a DB Cursor.
	
	@author Dan Kamins <dew@axonchisel.net>
	"""
	# For now, just hijack one from a neighborhood protocol:
	return AccountManagement.AccountManagementProtocol().DB().cursor()


# -----------------------------------------------------------------------------
def CreatePasswordResetCode(oldPasswordHash, lastForgotEmailSentDatetime):
	"""Creates a "Password Reset Code" for the user to reset his password.
	
	This is created initially in EidosBack_User_RequestPasswordResetCode.
	It is validated in EidosBack_User_PasswordReset before the actual reset.
	
	Since generation and interpretation of the reset code is all done within 
	this code module, it is not necessary to have a detailed spec here.
	To see how the reset code is generated, just look at the three lines of code 
	below along with the manual for the standard Python modules: base64 and md5.
	
	@param oldPasswordHash string : the current password hash, from DB.
	@param lastForgotEmailSentDatetime datetime : datetime obj from
								lastforgotemailsent from EidosAccounts table,
								or None if never reset before.
	@return string password reset code, 22 alphanumeric characters long.

	@author Dan Kamins <dew@axonchisel.net>
	"""
	resetCodeSrc = oldPasswordHash
	if (lastForgotEmailSentDatetime is not None):
		resetCodeSrc += "@" + lastForgotEmailSentDatetime.isoformat()
	resetCode = base64.b64encode(md5.new(resetCodeSrc).digest(), "DK")[:22]
	return resetCode
	

# -----------------------------------------------------------------------------
def FindAccountByEmailAndOrName(emailAddr, account):
	"""Utility function to query Principals for matching accounts.
	
	Either or both of emailAddr and account are used as lookup keys.
	
	@param string emailAddr	Optional email address of account to find.
							Empty string to rely on account param only.
	@param string account	Optional account name to find.
							Empty string to rely on emailAddr param only.

	Returns: (foundCount, acctPid, acctName, acctEmail, acctPassword)
	
	@return foundCount	0, 1, or 2. 0=none, 1=one found, 2=multiple found.
						Only with value 1 are other return params meaningful.
	@return acctPid		int account Principal Id.
	@return acctName	string account name.
	@return acctEmail	string email address of account.
	@return acctPassword	string stored password hash for account.
	"""
		
	c = GetDBCursor()

	# Prep query:
	sqlWhereClause = "(1=1) "
	sqlVarBinding = []
	if (len(emailAddr) > 0): 
		sqlWhereClause += " AND (p.email=%s)"
		sqlVarBinding.append(emailAddr)
	if (len(account) > 0): 
		sqlWhereClause += " AND (p.name=%s)"
		sqlVarBinding.append(account)
	fields = ["p.id", "p.name", "p.email", "p.password", "ea.lastforgotemailsent"]
	c.execute("SELECT "+",".join(fields)+" FROM Principals p "
		" LEFT JOIN EidosAccounts ea ON (ea.pid = p.id) "
		" WHERE " + sqlWhereClause + " LIMIT 2", sqlVarBinding)
	accts = c.fetchall()
	
	# Ambiguous (more than 1 result)?
	if (len(accts) > 1):
		return (2, '', '', 0, '')

	# None found?
	if (len(accts) == 0):
		return (0, '', '', 0, '')

	# Found 1. Return:
	acct = accts[0]
	return (1, int(acct[fields.index('p.id')]), acct[fields.index('p.name')], 
		acct[fields.index('p.email')], acct[fields.index('p.password')]) 

# -----------------------------------------------------------------------------
def NoneToStr(val, strForNone = 'NULL'):
	"""Return val, or strForNone if val is None."""
	if val is None: return strForNone
	else: return val


StatusFormat = re.compile(r'(.*?)\|(.*?)\|(.*?)($|\|.*)')

# -----------------------------------------------------------------------------
def LocalizeStatusFromRaw(statusRaw, locale):
	"""Return structure with localized status info parsed from raw status str.
	
	Note: Region localization not currently supported.
	
	@param statusRaw	string of raw status, like: Snowblind|PS2|US|xxxxx
	@param locale		string indicating locale, eg 'english', 'french', etc.
						Leave blank ('') for raw status tokens.
	
	@return (statusRaw, statusLocalTitle, statusLocalPlatform, statusLocalRegion)

	"""
	strMethod = "EidosWebBackendLogic.LocalizeStatusFromRaw(%s)" % (statusRaw)

	# Special case for 'Offline':
	if (statusRaw == 'Offline'):
		return ('Offline', '', '', '')
		
	# Parse:
	m = StatusFormat.match(statusRaw)
	if not m:
		LogError(strMethod, "Malformed status.")
		return (statusRaw, '', '', '')
	(title, platform, region, other) = m.groups()

	# Localize:
	if (locale != ''):
		statusLocalTitle = EidosLocales.Lookup(title,locale)
		statusLocalPlatform = EidosLocales.Lookup(platform,locale)
		#statusLocalRegion = EidosLocales.Lookup(region,locale)
		statusLocalRegion = region   # Region localization not yet supported.
	# (else raw status requested):
	else: 
		statusLocalTitle = title
		statusLocalPlatform = platform
		statusLocalRegion = region

	# Return:
	return (statusRaw, statusLocalTitle, statusLocalPlatform, statusLocalRegion)



# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================


# -----------------------------------------------------------------------------
def EidosBack_Test_01(inStr):
	return "It has come to our attention that %s immediately." % (inStr)

# -----------------------------------------------------------------------------
def EidosBack_Test_02(inNum1, inNum2):
	val = 0
	for i in range (inNum1, inNum2):
		val = val + i
	return (inNum1, val, inNum2)
	
# -----------------------------------------------------------------------------
def EidosBack_Test_03(inNum1, inNum2):
	lst = []
	for i in range (inNum1, inNum2):
		lst = lst + [i]
	return (inNum1, lst, inNum2)
	
# -----------------------------------------------------------------------------
def EidosBack_Test_SetPassword(pid, newPassword):
	strMethod = 'EidosBack_TestSetPassword(%s, %s)' % (pid, newPassword)
	c = GetDBCursor()

	c.execute("SELECT password FROM Principals WHERE id=%s", [pid])
	oldPassHash = c.fetchone()[0]

	# Update password:
	quazal.SecurityContextManager.GetInstance().Push(pid, 1)
	try:
		AM = AccountManagement.AccountManagementProtocol()
		initialKey = AM.scriptengine.InitializeKey(str(newPassword))
		res = AM.ChangePassword(initialKey)
		LogDetails(strMethod, "Change password result: %s" % (res))
		# TODO: Error handling
	finally:
		quazal.SecurityContextManager.GetInstance().Pop()
		
	c.execute("SELECT password FROM Principals WHERE id=%s", [pid])
	newPassHash = c.fetchone()[0]
	
	return (0, pid, newPassword, oldPassHash, newPassHash)


# -----------------------------------------------------------------------------
def EidosBack_Test_SQLSelect(sqlSelect):
	"""Test method to do a SQL SELECT query. 
	
	Returns row of headers, then data rows.
	
	"""
	sqlUpper = sqlSelect.upper()
	if (not sqlUpper.startswith("SELECT ") and
		not sqlUpper.startswith("EXPLAIN ") and
		not sqlUpper.startswith("DESCRIBE ") and
		not sqlUpper.startswith("SHOW ")):
		return (ERROR_BACKEND_GENERIC, 
			"Only SELECT, EXPLAIN, DESCRIBE, SHOW queries allowed.")

	strMethod = 'EidosBack_Test_SQLSelect()' 
	LogMethodCall(strMethod, " SQL='%s'" % (sqlSelect,));

	# Query:	
	c = GetDBCursor()
	c.execute(sqlSelect)

	LogDetails(strMethod, "result desc: %s " % (c.description,))

	# Start with header row (column names):
	ret = list( ([(col[0]) for col in c.description], ) )

	# Clean and return data:
	for row in c.fetchall():
		LogDetails(strMethod, "resultRow: %s" % (row,))
		rowret = []
		for col in row:
			rowret.append(str(NoneToStr(col, 'NULL')))
		ret.append(rowret)
		LogDetails(strMethod, "returnRow: %s" % (rowret,))
	return ret
    
	
# -----------------------------------------------------------------------------
def EidosBack_Test_GetPlayerList(callingPlayerPid, offset, size):
    quazal.SecurityContextManager.GetInstance().Push(callingPlayerPid,1)
    try:
	if (size > 100):
	    size = 100
	AM = AccountManagement.AccountManagementProtocol()
	lstPlayers = AM.FindByNameRegex(BuiltInGroups.Players, '.*', (offset,size))
	return lstPlayers
    finally:
        quazal.SecurityContextManager.GetInstance().Pop()
    
