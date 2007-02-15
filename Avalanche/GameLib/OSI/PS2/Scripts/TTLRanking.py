# 
# (c) Copyright 1998-2003, Quazal Technologies Inc.
# ALL RIGHTS RESERVED
# 
# Net-Z, Eterna, Q-Platform, Duplicated Object and Duplication Space 
# are trademarks of Quazal.
# 
# Use of this software indicates acceptance of the End User licence
# Agreement for this product. A copy of the EULA is available from
# www.quazal.com.
#

import Competition
import StatisticFlags


class NonladderRanking(Competition.Logic):
	# The recorded stats for each player. These need to be always entered in the order given.
	userRating			= 0
	copRating			= 1
	gangRating			= 2
	copWins				= 3
	copGamesPlayed		= 4
	copPlayersInRound	= 5
	gangWins			= 6
	gangGamesPlayed		= 7
	gangPlayersInRound	= 8
	killsAsCop			= 9
	deathsAsCop			= 10
	killsAsGangsta		= 11
	deathsAsGangsta		= 12
	copMVPs				= 13
	gangMVPs			= 14

	def ChangeStat(self, competitionstats, player, entry, diff):
		# If you add stats above, be sure to add more entires to the last parameter of the next line.
		Competition.Logic.ChangeStat(self, competitionstats, player, entry, diff, [1600,1600,1600,0,0,0,0,0,0,0,0,0,0,0,0])

	def SetStat(self, competitionstats, player, entry, newstat):
		for playerstat in competitionstats:
			if (playerstat[0]==player):
				playerstat[2][entry]=newstat
				return

	# Retrieve the match results then recompute and update (return) each player's new stats.
	def ProcessEventResults(self, competitionstats, eventstats):
		print 'In RunCompetition. entry competitionstats = ', competitionstats
		print 'In RunCompetition. entry eventstats = ', eventstats        

		for playerStats in eventstats:
			playerID = playerStats[0]
			print 'Updating stats for pid ', playerID
			if playerStats[1] != 1:
				continue
			
			self.ChangeStat(competitionstats, playerID, NonladderRanking.userRating, playerStats[2][0])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.copRating, playerStats[2][1])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.gangRating, playerStats[2][2])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.copWins, playerStats[2][3])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.copGamesPlayed, playerStats[2][4])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.copPlayersInRound, playerStats[2][5])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.gangWins, playerStats[2][6])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.gangGamesPlayed, playerStats[2][7])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.gangPlayersInRound, playerStats[2][8])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.killsAsCop, playerStats[2][9])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.deathsAsCop, playerStats[2][10])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.killsAsGangsta, playerStats[2][11])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.deathsAsGangsta, playerStats[2][12])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.copMVPs, playerStats[2][13])
			self.ChangeStat(competitionstats, playerID, NonladderRanking.gangMVPs, playerStats[2][14])

			c = self.DB().cursor()
			c.execute('SELECT clan_id FROM  EidosClanPlayer WHERE pid=%s' % playerID)
			clanid = c.fetchone()[0]

			# If we're not in a clan, just continue, otherwise fill in the new rating.			
			if (clanid != None):
				print 'Reporting Stats: Got ClanID ', clanid
				query = 'SELECT c.clan_rating FROM EidosClan c INNER JOIN EidosClanPlayer cp ON c.clan_id = cp.clan_id WHERE cp.pid=%s' % playerID
				c.execute(query)
				rating = c.fetchone()[0]
				print 'Reporting Stats: Got Rating ', rating

				# Increment or Decrement based on the user rating delta
				print 'User Rating Delta is ', playerStats[2][0]
				if (playerStats[2][0] > 0):
					rating = rating + 1
					print 'Increasing Clan Rating by 1'
				elif (playerStats[2][0] < 0):
					rating = rating - 1
					if( rating < 0 ): rating = 0
					print 'Decreasing Clan Rating by 1'

				# Do the update
				print 'Reporting Stats: Updating Clan Rating to ', rating
				c.execute('UPDATE EidosClan SET clan_rating=%s WHERE clan_id=%s' % (rating, clanid))

				kills = playerStats[2][9] + playerStats[2][11]			
				c.execute('UPDATE EidosClan SET clan_kills=clan_kills+%s WHERE clan_id=%s' % (kills, clanid))
			
						
		print 'In RunCompetition. exit competitionstats = ', competitionstats
		return competitionstats
	