from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
from termcolor import colored
from ScoreDatabase import *
from pprint import pprint
from time import time
import colorama
import sqlite3
import json
import os

colorama.init()

###########################################################################
############# SERVER PART
###########################################################################

acklist = {}
def classifyAck(dic):
    ack_req = dic["ACK_REQ"]
    ack_code = dic["ACK_CODE"]
    ack_install = dic["INSTALL_ID"]
    
    if ack_install not in acklist: acklist[ack_install] = []
    if ack_req == "1": #ack requested. queue it.
        acklist[ack_install].append(ack_code)
    elif ack_req == "2": #ack of ack recieved. dequeue it.
        #print colored("Ack {} dequeued!".format(ack_code), "blue")
        if ack_code in acklist[ack_install]: 
            acklist[ack_install].remove(ack_code)
    #print "classifyAck req={}, code={}, install={}, stored={}".format(ack_req, ack_code, ack_install,len(acklist[ack_install]))        

def isMessageNew(dic):
    ack_install = dic["INSTALL_ID"]
    if ack_install not in acklist:
        acklist[ack_install] = []
        return True
    ack_code = dic["ACK_CODE"]
    return ack_code not in acklist[ack_install]

def respondAcks(dic, wfile):
    odic = {"RESPONDACKSLEN": len(acklist[dic["INSTALL_ID"]])}
    odic["Event"] = "respondAcks"
    count = 0
    for a in acklist[dic["INSTALL_ID"]]: 
        odic["RESPONDACK{}".format(count)] = a
        count += 1
    print "[{}]: {} acks replied to {}. Total served users: {}".format(\
        timeStr(), count, dic["INSTALL_ID"], len(acklist))
    wfile.write(dic2post([odic]))

def writeScores(dic, wfile):
    print colored("[{}] Reporting Scores of {} to {}.".format(timeStr(), dic["Game"], dic["INSTALL_ID"]), "yellow", attrs=["bold"])
    wfile.write(getHighScores(dic["Game"]))

def respond(dic, wfile):
    event = dic["Event"]
    if event == "GetScores" and "Game" in dic:
        writeScores(dic, wfile)
    elif event == "ReportScore" and "Score" in dic and "Game" in dic and "Alias" in dic:
        addScore(dic["Game"], dic["Alias"], dic["INSTALL_ID"], dic["Score"])
        print colored("[{}] Added Score for {},{},{},{}.".format(timeStr(), dic["Game"], dic["Alias"], dic["INSTALL_ID"], dic["Score"]), "yellow", attrs=["bold"])
        writeScores(dic, wfile)

class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        print "GET"
        print self.rfile.read(100)
        self.wfile.write("Bob,40\nJoe,30")

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        clientaddress = str(self.client_address)
        diclist = post2dic(post_data)
        for dic in diclist:
            #print colored(dic, "yellow")
            if "INSTALL_ID" not in dic or "ACK_CODE" not in dic: 
                print colored(dic, "red")
                continue
            if "Event" in dic: 
                if isMessageNew(dic):
                    print colored("[{}]: {} from {} ({})".format(timeStr(), dic["Event"], dic["INSTALL_ID"], clientaddress), "green", attrs=["bold"])
                    respond(dic, self.wfile)
            if "ACK_REQ" in dic: classifyAck(dic)
            else: print colored(dic, "red", attrs=["bold"])
            respondAcks(dic, self.wfile)
        addRequest(post_data, clientaddress)
        
print "High Scores Server Listening on 700"
server = HTTPServer(('', 700), RequestHandler)
server.serve_forever()