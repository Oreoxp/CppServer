一、准备
1、去这里，下载这个，https://dev.mysql.com/downloads/repo/yum/

2、安装

```shell
yum install mysql80-community-release-el7-6.noarch.rpm
```

3、禁用默认的mysql模块

```
yum module disable mysql
```

4、安装

```
yum install -y mysql-community-server
```

4.1 报错

```
Curl error (37): Couldn't read a file:// file for file:///etc/pki/rpm-gpg/RPM-GPG-KEY-mysql-2022 [Couldn't open file /etc/pki/rpm-gpg/RPM-GPG-KEY-mysql-2022]
```

4.2 导入验证钥匙

```
rpm --import https://repo.mysql.com/RPM-GPG-KEY-mysql-2022
```

4.3 再来一次

```
 yum install -y mysql-community-server
```


安装完毕

二、启动
1、启动服务

```
systemctl start mysqld.service
```

2、验证

```
 systemctl status mysqld.service
```

3、查看初始密码

```
grep 'temporary password' /var/log/mysqld.log
```

4、进入mysql

```
 mysql -uroot -p
```



### 三、修改密码

进入mysql后

```
1、先修改密码
alter user 'root'@'localhost' identified by 'Root@123456';

2、查看密码策略
show variables like 'validate_password%';

3、修改密码策略
set global validate_password.check_user_name=OFF;
set global validate_password.length=0;
set global validate_password.mixed_case_count=0;
set global validate_password.number_count=0;
set global validate_password.policy=0;
set global validate_password.special_char_count=0;

4、刷新
flush privileges;

5、再查看密码策略
6、再修改密码
alter user 'root'@'localhost' identified by '123456';
flush privileges;

7、退出
exit;

8、再进来
mysql -uroot -p

9、修改远程连接
use mysql;
update user set host='%' where user = 'root';
flush privileges;
exit;
```



### 四、开机启动

1、开机启动

```
systemctl enable mysqld
systemctl daemon-reload
```



2、设置字符集

```
vim /etc/my.cnf
```


内容如下

```
# For advice on how to change settings please see
# http://dev.mysql.com/doc/refman/8.0/en/server-configuration-defaults.html

[mysql]
default-character-set=utf8

[mysqld]
#
# Remove leading # and set to the amount of RAM for the most important data
# cache in MySQL. Start at 70% of total RAM for dedicated server, else 10%.
# innodb_buffer_pool_size = 128M
#
# Remove the leading "# " to disable binary logging
# Binary logging captures changes between backups and is enabled by
# default. It's default setting is log_bin=binlog
# disable_log_bin
#
# Remove leading # to set options mainly useful for reporting servers.
# The server defaults are faster for transactions and fast SELECTs.
# Adjust sizes as needed, experiment to find the optimal values.
# join_buffer_size = 128M
# sort_buffer_size = 2M
# read_rnd_buffer_size = 2M
#
# Remove leading # to revert to previous value for default_authentication_plugin,
# this will increase compatibility with older clients. For background, see:
# https://dev.mysql.com/doc/refman/8.0/en/server-system-variables.html#sysvar_default_authentication_plugin
# default-authentication-plugin=mysql_native_password

datadir=/var/lib/mysql
socket=/var/lib/mysql/mysql.sock

log-error=/var/log/mysqld.log
pid-file=/var/run/mysqld/mysqld.pid

default-storage-engine=INNODB
character_set_server=utf8
 
symbolic-links=0
```



3、重启

```
service mysqld restart
```


