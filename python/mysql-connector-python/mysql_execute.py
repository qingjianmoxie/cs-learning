# _*_ coding:utf-8 _*_
import sys
import os
import re
import codecs
import difflib
import mysql.connector
import server_info_config
import sql_type_filter
from sql_type_filter import is_need_prepare
from sql_type_filter import check_sql_has_record
import log_output
from log_output import print_log

g_need_record       = False

# 以--echo #开头
note_pattern        = re.compile(r"^(\s*--echo\s*)(#\s*.*)$")
# select "..."
select_note_pattern = re.compile(r"^\s*select\s*\"(.*)\".*")
# --disable_warnings
disable_pattern     = re.compile(r"^\s*--disable_warnings\s*$")
# --enable_warnings
enable_pattern      = re.compile(r"^\s*--enable_warnings\s*$")
# 以# 开头
garbage_pattern     = re.compile(r"^\s*#.*")
# 以--replace 开头
replace_pattern     = re.compile(r"^\s*--replace.*")
# 以--error 开头
error_pattern       = re.compile(r"^\s*--error\s*(\d+)\s*$")
error2_pattern      = re.compile(r"^\s*--ERROR\s*(\d+)\s*$")
# delimiter ; 转 //
delimiter_pattern_1 = re.compile(r"^\s*delimiter\s+(.+?)\s*;\s*$")
# delimiter // 转 ;
delimiter_pattern_2 = re.compile(r"^\s*delimiter\s+;\s*(.+?)\s*$")

# "/*!32312 xxxx*/"语句中的特殊注释
# test文件中没有
pattern4 = re.compile(r"(/\*!\d+)([\w\s]+)(\*/)")
# "/* */"方式的注释
# test文件中没有
pattern5 = re.compile(r"/\*[^!].*?\*/", re.S)

class SqlExecOpr:
    '''SQL语句执行操作接口
       1 连接db
       2 断开连接
       3 执行语句
       4 执行sql脚本
    '''
    def __init__(self, prepared_mode=False):
        '''
        :param prepared_mode: 指定mode，当为True时，特定语句使用prepread方式执行
        '''
        self.prepared_mode = prepared_mode

    def connect(self, db_name=""):
        '''
        通过server_info_config中的配置连接DBPROXY
        :return: True - 成功 False - 失败
        '''
        retcode = True
        try:
            self.cnx = mysql.connector.connect(**server_info_config.proxy_info)
            if db_name:
                self.cnx.cmd_init_db(db_name)
        except:
            msg = "Can't Connect to Server %s@%s:%d." %   \
                  (server_info_config.proxy_info["user"], \
                   server_info_config.proxy_info["host"], \
                   server_info_config.proxy_info["port"])
            msg = '{:<}'.format(msg)
            print (msg)
            print_log.write(msg)

            retcode = False

        return retcode


    def disconnect(self):
        try:
            self.cnx.close()
        except:
            pass

    def execute(self, sql):
        '''
        执行单条sql语句
        :param sql: 待执行sql语句
        :return: True - 成功 False - 失败
        '''
        retcode = True

        try:
            if self.prepared_mode and is_need_prepare(sql):
                cursor = self.cnx.cursor(prepared=True)
                if g_need_record:
                    print("mode[prepare] sql[%s]" % (sql,))
                print_log.write("mode[prepare] sql[%s]" % (sql,))
            else:
                cursor = self.cnx.cursor(buffered=True)
                if g_need_record:
                    print("mode[normal] sql[%s]" % (sql,))
                print_log.write("mode[normal] sql[%s]" % (sql,))
            cursor.execute(sql)
            if sql_type_filter.need_commit(sql):
                # Make sure data is committed to the database
                self.cnx.commit()
            if cursor.with_rows:
                # 获取列名
                self.cols  = cursor.description
                # 获取记录
                self.rows = cursor.fetchall()
            cursor.close()
        except mysql.connector.Error as err:
            self.err = err
            retcode = False
            if g_need_record:
                print("{}".format(err))
            print_log.write("{}".format(err))
        except:
            retcode = False
            msg = "sql[%s].\nerrmsg[Unkown error message]." % (sql,)
            msg = '{:<}'.format(msg)
            if g_need_record:
                print (msg)
            print_log.write(msg)
        else:
            pass

        return retcode

    def execute_get_row_count(self, sql):
        '''
        执行带有结果集的请求
        :param sql: sql语句
        :return: 结果集行数 无结果集返回-1
        '''
        count = -1

        try:
            if self.prepared_mode and is_need_prepare(sql):
                cursor = self.cnx.cursor(prepared=True, buffered=True)
                print "mode[prepare] sql[%s]" % (sql,)
                print_log.write("mode[prepare] sql[%s]" % (sql,))
            else:
                cursor = self.cnx.cursor(buffered=True)
                print "mode[normal] sql[%s]" % (sql,)
                print_log.write("mode[normal] sql[%s]" % (sql,))

            cursor.execute(sql)
            count = cursor.rowcount
            if cursor.with_rows:
                cursor.fetchall()

            cursor.close()
        except mysql.connector.Error as err:
            msg = "sql[%s].\nerrmsg[%s]." % (sql, err)
            msg = '{:<}'.format(msg)
            print (msg)
            print_log.write(msg)
        except:
            msg = "sql[%s].\nerrmsg[Unkown error message]." % (sql,)
            msg = '{:<}'.format(msg)
            print (msg)
            print_log.write(msg)

        return count

    def execute_test_file(self, file_path):
        '''
        执行test文件
        :param file_path: test文件路径
        :return: True - 成功 False - 失败
        '''
        # 打开result文件
        result_path = file_path.replace("/t/", "/prepare_r/")
        result_path = result_path.replace(".test", ".result")
        print_result = log_output.LogPrinter(result_path)
        print_result.open()

        retcode = True
        try:
            self.OpenFile(file_path)
            '''
            解析文件中的各行, 并生成result文件
            '''
            sql = ""
            errno = 0
            delimiter = ";"
        except IOError as e:
            msg = e
            msg = '{:<}'.format(msg)
            print (msg)
            print_log.write(msg)
            return False
        except:
            msg = "Unkown error happens, file path[%s]." % file_path
            msg = '{:<}'.format(msg)
            print (msg)
            print_log.write(msg)
            return False
        else:
            for line in self.lines:
                # 如果是空行, 略过
                if "" == line.strip():
                    continue
                # 匹配"--echo #"
                re_echo        = note_pattern.match(line)
                # 匹配select "..."
                re_select_note = select_note_pattern.match(line)
                # 匹配--disable_warnings
                re_disable     = disable_pattern.match(line)
                # 匹配--enable_warnings
                re_enable      = enable_pattern.match(line)
                # 匹配以# 开头
                re_garbage     = garbage_pattern.match(line)
                # 匹配以--replace_regex 开头
                re_replace     = replace_pattern.match(line)
                # 匹配以--error 开头
                re_error       = error_pattern.match(line)
                re_error2      = error2_pattern.match(line)
                # 匹配delimiter ; 转 //
                re_delimiter_1 = delimiter_pattern_1.match(line)
                # 匹配delimiter // 转 ;
                re_delimiter_2 = delimiter_pattern_2.match(line)
                if re_echo:
                    print_result.write(re_echo.group(2).strip('\n'))
                elif re_select_note:
                    print_result.write(line.strip())
                    print_result.write(re_select_note.group(1).strip())
                    print_result.write(re_select_note.group(1).strip())
                elif re_disable or re_enable or re_garbage or re_replace:
                    continue
                elif ";" == delimiter and re_delimiter_1:
                    delimiter = re_delimiter_1.group(1).strip()
                elif ";" != delimiter and re_delimiter_2:
                    delimiter = ";"
                elif re_error or re_error2:
                    errno = int(re_error.group(1).strip())
                # sql语句
                else:
                    retcode = self.handle_sql(delimiter, line, sql, errno, print_result)

        self.CloseFile(file_path)
        print_result.close()
        return retcode

    def handle_sql(self, delimiter, line, sql, errno, print_result):
        retcode = True
        if delimiter in line:
            if ";" == delimiter:
                sql = sql + line.strip()
            else:
                sql = sql + line.strip().replace(delimiter, "", 1)
            if self.execute(sql):
                # select/show 要有记录
                if check_sql_has_record(sql):
                    print_result.write(sql.strip())
                    # 打印列名
                    colname = ""
                    for col in self.cols:
                        colname = colname + str(col[0]) + '\t'
                    colname = colname.strip()
                    print_result.write(colname)
                    record = ""
                    # 结果集为空时, 不判断会报错
                    # if self.rows:
                    for row in self.rows:
                        for column in row:
                            # 为什么不加str()就报错?
                            record = record + str(column) + '\t'
                        record = record.strip() + '\n'
                    record = record.replace("None", "NULL")
                    print_result.write(record.strip())
                else:
                    print_result.write(sql.strip())
            # 执行sql失败
            else:
                if errno != self.err.errno:
                    print("query '%s' failed with wrong errno %d: '%s', instead of %d..." % (sql, self.err.errno, self.err.msg, errno))
                    # 为什么会报Unkown error happens, file path...
                    sys.exit()
                # 重置errno
                errno = 0
                print_result.write(sql.strip())
                errmsg = "ERROR " + self.err.sqlstate + ": " + self.err.msg
                print_result.write(errmsg.strip())
                retcode = False

            # 执行完一条语句, 重置sql
            sql = ""
        else:
            sql = sql + line.lstrip()
        return retcode

    def compare_test_file(self, prepare_file, normal_file):
        '''
        比较prepare模式生成的result文件与正常模式生成的result文件
        :param prepare_file: prepare模式生成的result文件路径
        :param normal_file: 正常模式生成的result文件路径
        :return: True - 一致 False - 不一致
        '''
        try:
            ret = True
            linenum = 0
            self.OpenFile(normal_file)
            with open(prepare_file,'r') as pf:
                # for linenum, normal_line in enumerate(self.file):
                for normal_line in self.lines:
                    linenum += 1
                    prepare_line = pf.readline()
                    if prepare_line:
                        prepare_line = prepare_line.strip() + '\n'
                        # 将unicode编码的字符串转换成utf-8编码
                        normal_line = normal_line.encode('utf-8').strip() + '\n'
                        if prepare_line != normal_line:
                            print("line num : %s" % linenum)
                            diff = difflib.ndiff(normal_line.splitlines(1), prepare_line.splitlines(1))
                            # 这是什么意思?
                            print ''.join(diff),
                            ret = False
                    # prepare缺行
                    else:
                        print("line num : %s" % linenum)
                        diff = difflib.ndiff(normal_line.splitlines(1), prepare_line.splitlines(1))
                        # 这是什么意思?
                        print ''.join(diff),
                        ret = False
                        break
        except IOError as e:
            msg = e
            msg = '{:<}'.format(msg)
            print (msg)
            print_log.write(msg)
            return False
        except:
            msg = "Unkown error happens, prepare file path[%s], normal file path[%s]." % (prepare_file, normal_file)
            msg = '{:<}'.format(msg)
            print (msg)
            print_log.write(msg)
            return False
        self.CloseFile(normal_file)
        if ret:
            os.remove(prepare_file)
            print("{:<}\t{:>}".format(prepare_file, "[ pass ]"))
            return True
        else:
            print("{:<}\t{:>}".format(prepare_file, "[ fail ]"))
            return False


    def OpenFile(self, file_path):
        '''
        以只读方式打开文件
        :param file_path: 文件路径
        :return: None
        '''
        #脚本编码不一致 主要存在 GB2312 UTF-8 GB18030三种编码
        #通过多次尝试决定使用哪种编码打开脚本
        self.file_path = file_path
        try:
            self.file = codecs.open(file_path, "r", encoding="utf-8")
            self.lines = self.file.readlines()
        except:
            self.file.close()
            try:
                self.file = codecs.open(self.file_path, "r", encoding="gb2312")
                self.lines = self.file.readlines()
            except:
                self.file.close()
                try:
                    self.file = codecs.open(self.file_path, "r", encoding="gb18030")
                    self.lines = self.file.readlines()
                except:
                    print ("open file:%s error, maybe open file with illegal encoding"
                            % file_path)

    def CloseFile(self, file_path):
        try:
            self.file.close()
        except:
            pass

if __name__ == "__main__":
    print mysql.connector.HAVE_CEXT

    proxy_info1 = {
        "host": "127.0.0.1",
        "port": 9527,
        "user": "root",
        "password": "123456",
        "charset": "utf8",
        "collation": "utf8_bin",
        "use_pure" : True
    }

    try:
        cnx = mysql.connector.connect(**proxy_info1)
    except:
        print "can not connect"
    else:
        print "can connect"
        print cnx.is_connected()
        cnx.close()
    pass
