import ControlScript
import AccountManagement

from Validation import ProtocolParameterValidationHook
from ValidationException import ValidationError

import re
import os

profanities = []
initialized = False
if not initialized:
	print 'EidosFilter: Loading profanity patterns'

	file = open('Scripts/User/EidosFilterExact.txt')
	for line in file:
		# Remove training newline
		line = line.strip()
		# Convert from latin1 to unicode
		u = unicode(line,'latin1')
		# Escape special chars in input
		u = re.escape(u)
		# Add exact match semantics
		u = u + '$'
		# Compile into regular expression
		r = re.compile(u,re.IGNORECASE)
		# Add regexp and printable to db
		profanities.append((r,u))

	file = open('Scripts/User/EidosFilterSubst.txt')
	for line in file:
		# Remove training newline
		line = line.strip()
		# Convert from latin1 to unicode
		u = unicode(line,'latin1')
		# Escape special chars in input
		u = re.escape(u)
		# Add substring match semantics
		u = '.*' + u
		# Compile into regular expression
		r = re.compile(u,re.IGNORECASE)
		# Add regexp and printable to db
		profanities.append((r,u))

	file.close()
	initialized = True
	print 'EidosFilter: %s profanity patterns' % len(profanities)

def Check(utf8):
	# Convert from utf8 to unicode
	u = utf8.decode('utf-8')

	# This is *necessary* for case insensitive match
	l = u.lower()

	# NOTE: This throws for non latin1 chars!
	#print 'EidosFilter.Check <= ',utf8,u,l

	for (pr,pu) in profanities:
		if pr.match(l):
			#print 'EidosFilter.Check => Matched ',pu
			return False
	return True

if False: # Simple unit test
	print 'EidosFilter: Testing'

	# Exact match
	assert Check('aanal') == True
	assert Check('anal') == False
	assert Check('anall') == True

	# Substring match
	assert Check('aacacio') == False
	assert Check('acacio') == False
	assert Check('acacioo') == False

	# Case insensitivity
	assert Check(unicode('\xe4p\xe4r\xe4','latin1').encode('utf-8')) == False
	assert Check(unicode('\xc4P\xc4R\xc4','latin1').encode('utf-8')) == False

	# Non-latin1 character
	utf8 = '\xc3\xbf'
	# Convert from utf8 to unicode
	u = unicode('.*'+utf8,'utf-8')
	# Compile into regular expression
	r = re.compile(u,re.IGNORECASE)
	profanities.append((r,u))
	assert Check('\xc3\xbf') == False
	assert Check('\xc5\xb8') == False

class StringCheck(ProtocolParameterValidationHook):
	# NOTE: This can get an extra list or tulpe argument telling what
	# checks to perform (dictionary, no space, min/max, character set.)
	# This argument would be taken into account in the Check function.
	def __init__(self,protocol,iParamPos):
		ProtocolParameterValidationHook.__init__(self,protocol,iParamPos)

	def __call__(self,methodname,argl,argd):
		print 'EidosFilter.StringCheck',(methodname,argl,argd)
		if not Check(argl[self.iParamPos]):
			raise ValidationError

class GatheringCheck(StringCheck):
	def __call__(self,methodname,argl,argd):
		print 'EidosFilter.GatheringCheck',(methodname,argl,argd)
		# Sample argl is [('TTLSession', (0, 0, 0, 1,
		#     16, 7, 0, 16, 0, 'stas2', 0, 0, 0, 255, 1, 3043))]
		if argl[self.iParamPos][0] == 'TTLSession':
			if not Check(argl[self.iParamPos][1][9]):
				raise ValidationError
