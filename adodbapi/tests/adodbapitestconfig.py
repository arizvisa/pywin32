# Configure this in order to run the testcases.
"adodbapitestconfig.py v 2.2.3"
from __future__ import print_function
from __future__ import unicode_literals

import os
import sys
print(sys.version)
#attempt to find adodbapi in this directory's parent
cwd = os.getcwd()
adoPath = os.path.normpath(cwd + '/../adodbapi.py')
if os.path.exists(adoPath):
    if adoPath not in sys.path:
        sys.path.insert(1,os.path.dirname(adoPath))
import adodbapi
# adodbapitest.py will import this same version when we return

try:
    print(adodbapi.version) # show version
except:
    print('"adodbapi.version" not present or not working.')
print(__doc__)

doAccessTest = True
doSqlServerTest = False #True
doMySqlTest = False #True

try: #If mx extensions are installed, use mxDateTime
    import mx.DateTime
    doMxDateTimeTest=True
except: 
    doMxDateTimeTest=False #Requires eGenixMXExtensions
doMySqlTest = False

doDateTimeTest=True

iterateOverTimeTests=False 

if doAccessTest:
    _accessdatasource = None  #set to None for automatic creation
    #r"C:\Program Files\Microsoft Office\Office\Samples\northwind.mdb;"
                #Typically C:\Program Files
    if _accessdatasource == None:
        # following setup code borrowed from pywin32 odbc test suite
        # kindly contributed by Frank Millman.
        import tempfile
        import os
        try:
            from win32com.client.gencache import EnsureDispatch
            from win32com.client import constants
            win32 = True
        except ImportError: #perhaps we are running IronPython
            win32 = False
        if not win32:
            from System import Activator, Type
        _accessdatasource = os.path.join(tempfile.gettempdir(), "test_odbc.mdb")
        if os.path.isfile(_accessdatasource):
            os.unlink(_accessdatasource)
        # Create a brand-new database - what is the story with these?
        for suffix in (".36", ".35", ".30"):
            try:
                if win32:
                    dbe = EnsureDispatch("DAO.DBEngine" + suffix)
                else:
                    type= Type.GetTypeFromProgID("DAO.DBEngine" + suffix)
                    dbe =  Activator.CreateInstance(type)
                break
            except:
                pass
        else:
            raise RuntimeError("Can't find a DB engine")
        print('    ...Creating ACCESS db at',_accessdatasource)    
        if win32:
            workspace = dbe.Workspaces(0)
            newdb = workspace.CreateDatabase(_accessdatasource, 
                                            constants.dbLangGeneral,
                                            constants.dbEncrypt)
        else:
            newdb = dbe.CreateDatabase(_accessdatasource,';LANGID=0x0409;CP=1252;COUNTRY=0')
        newdb.Close()
    connStrAccess = r"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + _accessdatasource
    # for ODBC connection try...
    # connStrAccess = "Driver={Microsoft Access Driver (*.mdb)};db=%s;Uid=;Pwd=;" + _accessdatasource

if True: #needed for dbapi20 test
    _computername="franklin" #or name of computer with SQL Server
    _databasename="Northwind" #or something else
    _username="guest"
    _password="12345678"
    #connStrSQLServer = r"Provider=SQLOLEDB.1; Integrated Security=SSPI; Initial Catalog=%s;Data Source=%s" %(_databasename, _computername)
    connStrSQLServer = r"Provider=SQLOLEDB.1; User ID=%s; Password=%s; Initial Catalog=%s;Data Source=%s" %(_username,_password,_databasename, _computername)
if doSqlServerTest:
    print('    ...Testing MS-SQL login...')

    try:
        s = adodbapi.connect(connStrSQLServer) #connect to server
        s.close()
    except adodbapi.DatabaseError as inst:
        print(inst.args[0][2])    # should be the error message
        doSqlServerTest = False

if doMySqlTest:
    _computername='10.100.5.249'
    _databasename='test'
   #_driver="MySQL ODBC 3.51 Driver"
    _driver="MySQL ODBC 5.1 Driver"
    connStrMySql = 'Driver={%s};Server=%s;Port=3306;Database=%s;Option=3;' % \
                   (_driver,_computername,_databasename)
    print('    ...Testing MySql login...')

    try:
        s = adodbapi.connect(connStrMySql) #connect to server
        s.close()
    except adodbapi.DatabaseError as inst:
        print(inst.args[0][2])    # should be the error message
        doMySqlTest = False
