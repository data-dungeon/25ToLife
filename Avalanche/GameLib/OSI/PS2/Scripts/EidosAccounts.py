#
# Server Account Management Type Extension script (must be in-sync with DDL).
#

import re

import Hook
import TypeExtension

class EidosAccounts(TypeExtension.BaseClass):

	lsFieldsPublic = ['country']
	lsFieldsPrivate = ['flags','postalcode','dob']

	def GetTables(self):
		return ['EidosAccounts']

	def Save(self,sType,nPID,tData):
		print 'EidosAccounts.Save',(sType,nPID,tData)

		def SaveFields(lsFields):
			c = self.DB().cursor()
			c.execute('SELECT * FROM EidosAccounts WHERE pid=%s',nPID)
			if c.rowcount == 0: # No existing entry found - create a new one!
				sFields = ','.join(lsFields)
				sValues = ','.join(map(lambda s: '%s', lsFields))
				c.execute('INSERT INTO EidosAccounts (pid,'+sFields+') VALUES (%s,'+sValues+')',(nPID,)+tData[1])
			else:
				sFields = ','.join(map(lambda s: s+'=%s', lsFields))
				c.execute('UPDATE EidosAccounts SET '+sFields+' WHERE pid=%s',tData[1]+(nPID,))
				
			print 'Eidos Accounts Updated with type ', sType				

		if sType == 'EidosAccountPublic':
			SaveFields(self.lsFieldsPublic)
		elif sType == 'EidosAccountPrivate':
			SaveFields(self.lsFieldsPrivate)

	def Load(self,sType,nPID):
		print 'EidosAccounts.Load',(sType,nPID)

		def LoadFields(lsFields):
			c = self.DB().cursor()
			c.execute('SELECT '+','.join(lsFields)+' FROM EidosAccounts WHERE pid=%s',(nPID,))
			return (sType,tuple(c.fetchone()))

		lData = None
		if sType == 'EidosAccountPublic':
			lData = LoadFields(self.lsFieldsPublic)
		elif sType == 'EidosAccountPrivate':
			lData = LoadFields(self.lsFieldsPrivate)

		print 'EidosAccounts.Load=>',lData
		return lData

	def Delete(self,sType,nPID):
		c = self.DB().cursor()
		if sType == 'EidosAccountPublic':
			c.execute('DELETE FROM EidosAccounts WHERE pid=%s',(nPID,))
			return c.rowcount==1
		if sType == 'EidosAccountPrivate':
			c.execute('DELETE FROM EidosAccounts WHERE pid=%s',(nPID,))
			return c.rowcount==1
		return False

class EidosAccountsPublic(EidosAccounts): pass
class EidosAccountsPrivate(EidosAccounts): pass

# Prehook for RendezVous UpdateStatus call.  We want to update
# the EidosGamesPlayed table everytime a player logs onto our game.
class UpdateStatusPreHook(Hook.PreHook):
	def __init__(self,protocol):
		Hook.PreHook.__init__(self,protocol)
		self.proto = protocol
		# Setup the format for the status string (<title>|<platform>|<region>[|<other>])
		self.StatusFormat = re.compile(r'(.*?\|.*?\|.*?)($|\|.*)')

	def SQL(self,fmt,args=[]):
		c = self.proto.DB().cursor()
		c.execute(fmt, args)
		return c

	def __call__(self,methodname,argl,argd):
		print 'EidosAccounts.UpdateStatusPreHook',(methodname,argl,argd)

		# See if our status string matches the format
		m = self.StatusFormat.match(argl[0])
		if m:
			game = m.group(1)
			pid = self.proto.GetCurrentPID()
			self.SQL('REPLACE INTO EidosGamesPlayed (pid,game) VALUES (%s,%s)',[pid,game])