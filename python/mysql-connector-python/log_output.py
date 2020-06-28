# _*_ coding:utf-8 _*_
g_log_file       = "result.log"

class LogPrinter:
    '''
    输出结果日志
    '''
    def __init__(self, log_file_name):
        '''
        :param log_file_name: 日志文件路径
        '''
        self.log_path = log_file_name

    def reset_log_path(self, log_file_name):
        '''
        更新日志文件路径
        :param log_file_name: 新日志文件路径
        :return:
        '''
        self.log_path = log_file_name

    def open(self):
        '''
        打开日志文件
        :return: True 成功 False 失败
        '''
        import os
        # 判断路径是否存在
        dirname = os.path.dirname(self.log_path)
        # 如果是当前文件夹, dirname为'', mkdirs会报错
        if dirname:
            isExists=os.path.exists(dirname)
            # 判断结果
            if not isExists:
                # 如果不存在则创建目录
                os.makedirs(dirname)
        try:
            self.f = open(self.log_path, 'w')
            return True
        except:
            msg = "Unable to open result file, please check!"
            print (msg)
            return False

    def close(self):
        '''
        关闭文件句柄
        :return:
        '''
        self.f.close()

    def write(self, content):
        '''
        写文件
        :param content: content 待写入内容
        :return:
        '''
        self.f.write((content+'\n').encode('utf-8'))

print_log    = LogPrinter(g_log_file)
