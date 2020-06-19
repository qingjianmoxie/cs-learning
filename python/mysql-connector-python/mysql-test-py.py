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
g_vardir            = "./var"

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

def working_mode2_proc(file_path):
    '''
    执行单个sql脚本文件
    '''

    query = mysql_execute.SqlExecOpr(g_prepared_mode)
    query.connect()
    query.execute_test_file(file_path)
    query.disconnect()

def working_mode3_proc(dir_path):
    '''
    执行一个目录下的test文件
    '''
    files = os.listdir(dir_path)
    files.sort()

    query = mysql_execute.SqlExecOpr(g_prepared_mode)
    query.connect()

    for file in files:
        # 文件的路径
        file_path = os.path.join(dir_path, file)
        if os.path.isfile(file_path) and file_path.endswith(".test"):
            query.execute_test_file(file_path)

    query.disconnect()

"""
def working_mode4_proc(suite_path):
    '''
    执行suite套件
    '''
    dirs = find_valid_script_dic(suite_path)

    for dir in dirs:
        exec_suite_dic_proc(dir)

def working_mode5_proc(suite_module_path):
    '''
    执行singlesuite
    '''
    exec_suite_dic_proc(suite_module_path)
"""

"""
def exec_suite_dic_proc(suite_dic_path):
    [dirname, filename] = os.path.split(suite_dic_path)
    msg = filename
    print (msg)
    print_log.write(msg)
    files = os.listdir(suite_dic_path)
    files.sort()

    f = lambda name : valid_script_pattern.match(name)

    for file in files:
        if os.path.isfile(os.path.join(suite_dic_path, file)) and f(file):
            exec_suite_sql_file_proc(os.path.join(suite_dic_path, file))


def exec_suite_sql_file_proc(suite_file_path):
    [dirname, filename] = os.path.split(suite_file_path)
    name = filename.split('.')[0]
    left_len = (100 - len(name)) // 2
    right_len = 100 - left_len - len(name)
    name = "-" * left_len + name + "-" * right_len
    print (name)
    print_log.write(name)

    with open(suite_file_path) as f:
        #跳过开头4行Settings内容
        idx = 0
        while idx < 4:
            f.readline()
            idx += 1

        lines = f.readlines()

    test_item_name = ""
    test_result    = True

    query = mysql_execute.SqlExecOpr(g_prepared_mode)
    for line in lines:
        #寻找测试case名称
        r1 = test_item_pattern.match(line)
        if r1:
            if test_item_name:

                if test_result is True:
                    msg = "||PASS||.\n"
                    print (msg)
                    print_log.write(msg)
                else:
                    msg = "||FAIL||.\n"
                    print (msg)
                    print_log.write(msg)

            test_result = True
            test_item_name = r1.group(1).strip()
            msg = "%-99s." % test_item_name
            print (msg)
            print_log.write(msg)
            continue

        # 对几种需要进行匹配的行就行处理
        r1 = connect_pattern_with_db.match(line)
        if r1:
            if query.connect(r1.group(2).strip()) is False:
                print "connect to server with db failed"
                print_log.write("connect to server with db failed")
                test_result = False
            continue

        if connect_pattern.match(line):
            if query.connect() is False:
                print "connect to server failed"
                print_log.write("connect to server failed")
                test_result = False
            continue

        if disconnect_pattern.match(line):
            query.disconnect()
            continue

        r1 = exec_sql_string_pattern.match(line)
        if r1:
            sql = r1.group(1).strip()
            if query.execute(sql) is False:
                print "exec sql:%s failed." % sql
                print_log.write("exec sql:%s failed." % sql)
                test_result = False
            continue

        r2 = exec_sql_script_pattern.match(line)
        if r2:
            script_link_name = r2.group(1).strip()
            script_link_name = "env_config." + script_link_name
            sql_file_path = eval(script_link_name)

            if query.execute_test_file(sql_file_path) is False:
                print "exec sql script file:%s failed." % sql_file_path
                print_log.write("exec sql script file:%s failed." % sql_file_path)
                test_result = False
            continue

        r3 = check_row_count_pattern.match(line)
        if r3:
            sql = r3.group(1).strip()
            rowcnt = int(r3.group(2).strip())

            res_rowcnt = query.execute_get_row_count(sql)
            if(rowcnt != res_rowcnt):
                print("result row count not match sql[%s] expect_rc:%d real_rc:%d" \
                      % (sql, rowcnt, res_rowcnt))
                print_log.write("result row count not match sql[%s] expect_rc:%d real_rc:%d" \
                             % (sql, rowcnt, res_rowcnt))
                test_result = False
                continue
            else:
                # print("result row count match sql[%s] expect_rc:%d real_rc:%d" \
                #       % (sql, rowcnt, res_rowcnt))
                pass

        continue
"""

"""
def find_valid_script_dic(suite_path):
    files = os.listdir(suite_path)
    files.sort()

    outDirs = []
    f = lambda name : valid_script_folder_pattern.match(name)
    for file in files:
        if os.path.isdir(os.path.join(suite_path, file)) and f(file):
            outDirs.append(os.path.join(suite_path, file))

    return outDirs
"""

if __name__ == "__main__":
    try:
        options, extras = getopt.getopt(sys.argv[1:], "hvf:s:o:t:",
                                       ["help", "version", "file=", "suite=", "output=", "vardir="])
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
            g_test_dir = os.path.join('.', 'suite', value, 't')
            g_result_dir = os.path.join('.', 'suite', value, 'r')
        # 指定单个test文件
        if (name == '-f') or (name == '--file'):
            g_working_mode = 2
            g_test_dir = os.path.join(g_test_dir, value)
        if (name == '-o') or (name == '--output'):
            g_log_file = value
            log_output.g_log_file = value
            log_output.print_log.reset_log_path(value)
        if (name == '-t') or (name == '--vardir'):
            g_log_file = value
            log_output.g_log_file = value
            log_output.print_log.reset_log_path(value)

    print_log.open()

    print "working mode:%d prepared mode:%d, path:%s." % \
          (g_working_mode, g_prepared_mode, g_test_dir)

    try:
        # 根据working mode进行分别处理
        if g_working_mode == 2:
            working_mode2_proc(g_test_dir)
        elif g_working_mode == 3:
            '''
            执行一个目录下的sql文件
            '''
            working_mode3_proc(g_test_dir)
        sys.exit()
    except KeyboardInterrupt:
        pass

    print_log.close()
    sys.exit()