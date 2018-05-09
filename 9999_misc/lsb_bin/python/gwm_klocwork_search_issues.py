import urllib, urllib2, json, sys, os.path, getpass, time

def getToken(host, port, user) :
   ltoken = os.path.normpath(os.path.expanduser("~/.klocwork/ltoken"))
   ltokenFile = open(ltoken, 'r')
   for r in ltokenFile :
      rd = r.strip().split(';')
      if rd[0] == host and rd[1] == str(port) and rd[2] == user :
        ltokenFile.close()
        return rd[3]
   ltokenFile.close()

# {"id":552,"status":"Analyze","severity":"Unexpected","severityCode":4,"state":"Existing","code":"MISRA.DEFINE.NOTGLOBAL","title":"Define not at the global level","message":"Define not at the global level","file":"tmp/staging/armv7a-mv-linux/usr/include/glib-2.0/glib/gthread.h","method":"gthread.h","owner":"unowned","taxonomyName":"MISRA C++ 2008","reference":"16-0-2 (C++ 2008 req.)","dateOriginated":1484055504368,"line":243,"url":"http://hias008x.hi.cn.conti.de:8098/review/insight-review.html#issuedetails_goto:problemid\u003d552,project\u003dGWM_MY18_SOC,searchquery\u003dseverity:1-4","issueIds":[],"trace":[]}        

class Issue(object) :
   def __init__(self, attrs) :
      self.id = attrs["id"]
      self.message = attrs["message"]
      self.file = attrs["file"]
      self.method = attrs["method"]
      self.code = attrs["code"]
      self.severity = attrs["severity"]
      self.severityCode = attrs["severityCode"]
      self.state = attrs["state"]
      self.status = attrs["status"]
      self.taxonomyName = attrs["taxonomyName"]
      self.url = attrs["url"]
      self.created=time.ctime(attrs["dateOriginated"]/1000)
      self.line = attrs["line"]
      self.title = attrs["title"]
      self.owner = attrs["owner"]

   def __str__(self) :
      sResult = ""
      
      sResult += "[%d] Message: %s | Title: %s\n" % (self.id, self.message, self.title)
      sResult += "\tFile: %s | line: %d | method: %s\n" % (self.file, self.line, self.method)
      sResult += "\tOwner: %s | Code: %s | Severity: %s[%d] | State: %s | Status: %s | Taxonomy: %s | Created: %s\n" % (self.owner, self.code, self.severity, self.severityCode, self.state, self.status, self.taxonomyName, self.created)
      sResult += "\t%s" % (self.url)
      
      return sResult;

def from_json(json_object) :
   if 'id' in json_object :
      return Issue(json_object)
   return json_object

host = "hias008x.hi.cn.conti.de"
port = 8098
#user = getpass.getuser()
user = "Gao Lingling (uidp0151)"
project = "GWM_MY18_SOC"
url = "http://%s:%d/review/api" % (host, port)
values = {"project": project, "user": user, "action": "search"}

loginToken = getToken(host, port, user)
#loginToken = "e5393afb3cacba49a0e350559640af6449b5bdf59777667a7330d9aed3e894fc"
if loginToken is not None :
   values["ltoken"] = loginToken

values["query"] = "severity:1-4"
data = urllib.urlencode(values)
req = urllib2.Request(url, data)
response = urllib2.urlopen(req)
for record in response :
    #print record
    try:
        print json.loads(record, encoding="utf-8", object_hook=from_json)
    except UnicodeEncodeError:
        print "*** UnicodeEncodeError: " + record

