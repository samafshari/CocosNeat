from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
import json
from pprint import pprint
from time import time
import os
import sqlite3
import datetime

def timeStr():
    return datetime.datetime.now().strftime("%m-%d %H:%M:%f")[:-3]

# (Score, Game, Alias, INSTALL_ID, SERVER_TIME)
database = []
dataFile = "scores.db"
conn = sqlite3.connect(dataFile)
cur = conn.cursor()

def post2dic(post_data): #out is [{...}]
    o = []
    for msg in post_data.split("\r\n"):
        try:
            dic = {}
            for k in msg.split("&"):
                kv = k.split("=")
                if len(kv) == 2:
                    dic[kv[0]] = kv[1]
            dic["SERVER_TIME"] = time()
            o.append(dic)
        except:
            print "Incorrect dictionary format received: " + msg
    return o

def dic2post(diclist): #dic is [{...}]
    o = ""
    for dic in diclist:
        msg = ""
        for key in dic.keys():
            msg += str(key).replace("&","").replace("=","").replace("\r\n","\n") + "="
            msg += str(dic[key]).replace("&","").replace("=","").replace("\r\n","\n") + "&"
        o += msg + "\r\n"
    return o

def executeQuery(queryIn, values):
    query = queryIn
    query = query.replace("1=1", "").replace(";","")
    if query == queryIn:
        o = cur.executemany(query, values)
        conn.commit()
        return o
    else:
        print("Query was manipulated. Not executing.")
        return None

def addScore(game, alias, installid, score):
    #try:
    game = game[:255]
    alias = alias[:255]
    installid = installid[:100]
    score = int(score)
    executeQuery("""
    INSERT INTO scores (game, alias, installid, score, servertime) VALUES
    (?,?,?,?,?)""", [(game, alias, installid, score, time())])
    executeQuery("""
    UPDATE scores
    SET alias=?
    WHERE installid=?""",[(alias,installid)])
    #except:
    #    print("There was an error in addScore({},{},{},{})".
    #          format(game,alias,installid,score))

def addRequest(rawrequest, clientaddress):
    executeQuery("""
    INSERT INTO requests (rawrequest, clientaddress, timestamp) VALUES
    (?,?,?)""", [(rawrequest, clientaddress, time())])

def getScores(game):
    o = []
    q = """
    SELECT 
    max(game),
    max(alias),
    installid,
    max(score) AS highscore,
    sum(score) AS totalscore 
    FROM scores 
    WHERE game=='{}'
    GROUP BY installid 
    ORDER BY totalscore DESC
    """

    #try:
    for row in cur.execute(q.format(game)):
        o.append( {
            "game": row[0],
            "alias": row[1],
            "installid": row[2],
            "highscore": row[3],
            "totalscore": row[4]
            } )
    #except:
    #    print("There was an error in getScores({})".format(game))
    return o

def getAllScores():
    o = []
    q = """
    SELECT 
    game,
    max(alias),
    installid,
    max(score) AS highscore,
    sum(score) AS totalscore 
    FROM scores 
    GROUP BY game, installid 
    ORDER BY game, totalscore DESC
    """

    #try:
    for row in cur.execute(q):
        o.append( {
            "game": row[0],
            "alias": row[1],
            "installid": row[2],
            "highscore": row[3],
            "totalscore": row[4]
            } )
    #except:
    #    print("There was an error in getScores({})".format(game))
    return o

def getHighScores(gameName, max=None):
    #format: HIGHSCORELEN=3&&HIGHSCORE0NAME=name&HIGHSCORE0HIGHSCORE=highscore&HIGHSCORE0TOTALSCORE=totalscore
    count = 0
    scores = getScores(gameName)
    dic = {"HIGHSCORELEN": len(scores)}
    dic["Event"] = "getHighScores"
    dic["Game"] = gameName
    if max is not None: dic["HIGHSCORELEN"] = min(max, len(scores))

    for item in scores:
        if max is not None and count >= max: break
        if item["game"] == gameName:
            dic["HIGHSCORE{}alias".format(count)] = item["alias"]
            dic["HIGHSCORE{}highscore".format(count)] = item["highscore"]
            dic["HIGHSCORE{}totalscore".format(count)] = item["totalscore"]
            dic["HIGHSCORE{}installid".format(count)] = item["installid"]
            count += 1

    outstr = dic2post([dic])
    return outstr
