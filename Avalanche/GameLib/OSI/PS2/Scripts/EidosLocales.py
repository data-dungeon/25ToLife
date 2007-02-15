# -*- coding: latin1 -*-

Locales = {

	# Platforms

	('PC','english')	: 'PC',
	('PC','french')		: 'PC',
	('PC','german')		: 'PC',
	('PC','italian')	: 'PC',
	('PC','spanish')	: 'PC',

	('PS2','english')	: 'PlayStation®2',
	('PS2','french')	: 'PlayStation®2',
	('PS2','german')	: 'PlayStation®2',
	('PS2','italian')	: 'PlayStation®2',
	('PS2','spanish')	: 'PlayStation®2',

	# Titles

	('Snowblind','english')	: 'Project: Snowblind',
	('Snowblind','french')	: 'Project: Snowblind',
	('Snowblind','german')	: 'Project: Snowblind',
	('Snowblind','italian')	: 'Project: Snowblind',
	('Snowblind','spanish')	: 'Project: Snowblind',

	('25ToLife','english')  : '25 To Life',
	('25ToLife','french')   : '25 To Life',
	('25ToLife','german')   : '25 To Life',
	('25ToLife','italian')  : '25 To Life',
	('25ToLife','spanish')  : '25 To Life',
}

def Lookup(string,locale):
	output = string

	try: latin1 = Locales[(string,locale)]
	except: print 'EidosLocale.Lookup: No entry for', (string,locale)
	else: output = unicode(latin1,'latin1').encode('utf8')

	return output

if False: # Simple unit test
	print Lookup('Snowblind','english')
