#!/usr/bin/env python
# _*_ coding:utf-8 _*_
import getopt
import sys
import os
import re
import mysql_execute
import mysql.connector
import log_output
from log_output import print_log

g_test_tool_version = 1.0
g_analyze_files     = []
g_test_dir          = "./t"
g_result_dir        = "./r"
g_vardir            = "./prepare_r"
g_need_record       = False

g_prepared_mode     = True
g_working_mode      = 3

g_str_help_info     = '''
NAME
    mysql-test-py    - a test tool for dbproxy using mysql connector/python
                     - version info(python 2.7.x connector:8.0.20)

SYNOPSIS
    mysql-test-py    [-h|--help] [-v|--version]
                     [-f|--file file name]
                     [-s|--suite suite name]
                     [-o|--output result file name]
                     [-t|--vardir]

DESCRIPTION
    mysql-test-py is a test tool wrote by python, using mysql connector/Python
    APIs, testing whether dbproxy can support connector/Python well.
    
    statements listed blow using the binary protocol between client and server.
    1 update
    2 insert
    3 delete
    4 select

    mysql-test-py execute *.test files and record actual result, comparing with
    *.result files, determing if there problem existed.
    
COMMAND LINE OPTIONS
    -h|--help    
        show help information.
    -v|--version 
        show version information.
    -f|--file file name
        execute single sql script, specify file name.
    -s|--suite suite path
        execute a suite of sqls, specify suite name.
    -o|--output result file name.
    -t|--vardir
        The directory where files generated from the tool is stored (default: ./var).
'''

def print_help_info():
    '''
    显示mysql-test-py的帮助信息
    :return: None
    '''
    print g_str_help_info

def print_version_info():
    '''
    显示mysql-test-py的版本信息
    :return: None
    '''
    py_version = sys.version
    py_connector_version = mysql.connector.__version__

    version_info = '''
python:%s
connector/python:%s
tool:%s
    ''' % (py_version, py_connector_version, str(g_test_tool_version))

    print version_info

def working_mode2_proc(test_path, prepare_path, result_path):
    '''
    执行单个sql脚本文件
    '''

    query = mysql_execute.SqlExecOpr(g_prepared_mode)
    query.connect()
    query.execute_test_file(test_path)
    if not g_need_record:
        query.compare_test_file(prepare_path, result_path)
    query.disconnect()

def working_mode3_proc(test_dir, prepare_dir, result_dir):
    '''
    执行一个目录下的test文件
    '''
    files = os.listdir(test_dir)
    files.sort()

    query = mysql_execute.SqlExecOpr(g_prepared_mode)
    query.connect()

    for file in files:
        # 文件的路径
        test_path    = os.path.join(test_dir, file)
        prepare_path = os.path.join(prepare_dir, file).replace(".test", ".result")
        result_path  = os.path.join(result_dir, file).replace(".test", ".result")
        if os.path.isfile(test_path) and test_path.endswith(".test"):
            query.execute_test_file(test_path)
            if not g_need_record:
                query.compare_test_file(prepare_path, result_path)

    query.disconnect()


if __name__ == "__main__":
    try:
        options, extras = getopt.getopt(sys.argv[1:], "hvf:s:o:r",
                                       ["help", "version", "file=", "suite=", "output=", "record"])
    except getopt.GetoptError:
        print "input parameter illegal, you can use -h|--help option to get" \
              " the usage of this tool."
        sys.exit()

    bHelpExists = False
    # 优先处理[-h|--help]参数
    for name, value in options:
        if (name == '-h') or (name == '--help'):
            bHelpExists = True
            break

    if bHelpExists == False:
        for extra in extras:
            if (extra == '-h') or (extra == '--help'):
                bHelpExists = True
                break

    if bHelpExists == True:
        print_help_info()
        sys.exit()

    if extras:
        print "input parameter illegal, you can use -h|--help option to get the usage of this tool."
        sys.exit()

    for name, value in options:

        if (name == '-v') or (name == '--version'):
            print_version_info()
            sys.exit()
        # 指定整个文件夹
        if (name == '-s') or (name == '--suite'):
            g_working_mode = 3
            g_test_dir     = os.path.join('.', 'suite', value, 't')
            g_result_dir   = os.path.join('.', 'suite', value, 'r')
            g_vardir       = os.path.join('.', 'suite', value, 'prepare_r')
        # 指定单个test文件
        if (name == '-f') or (name == '--file'):
            g_working_mode = 2
            g_test_dir   = os.path.join(g_test_dir, value) + ".test"
            g_result_dir = os.path.join(g_result_dir, value) + ".result"
            g_vardir     = os.path.join(g_vardir, value) + ".result"
        if (name == '-o') or (name == '--output'):
            g_log_file = value
            log_output.g_log_file = value
            log_output.print_log.reset_log_path(value)
        if (name == '-r') or (name == '--record'):
            g_need_record = True
            mysql_execute.g_need_record = True

    print_log.open()

    print "working mode:%d prepared mode:%d, path:%s." % \
          (g_working_mode, g_prepared_mode, g_test_dir)

    try:
        # 根据working mode进行分别处理
        if g_working_mode == 2:
            working_mode2_proc(g_test_dir, g_vardir, g_result_dir)
        elif g_working_mode == 3:
            '''
            执行一个目录下的sql文件
            '''
            working_mode3_proc(g_test_dir, g_vardir, g_result_dir)
        sys.exit()
    except KeyboardInterrupt:
        pass

    print_log.close()
    sys.exit()
