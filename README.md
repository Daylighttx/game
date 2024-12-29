## a simple gameserver

### 游戏业务架构：

#### 1. 程序结构（三层结构）
1. 通道层：收发TCP数据
2. 协议层：原始字节流和游戏消息的转换 **TCP粘包处理**
3. 业务层：处理游戏消息，向其他TCP连接转发数据

#### 2. 玩家上线和下线
1. 上线：向客户端给予随机坐标和昵称，并同步给周围玩家
2. 下线：回收昵称，广播下线消息给所有玩家

#### 3. 移动
1. 向周围玩家广播新位置
2. 视野切换（集合运算）旧邻居相互消失，新邻居相互出现

#### 4. 世界聊天
1. 向所有玩家广播聊天内容

#### 5. 延时自动关闭服务
1. 最后玩家下线开启定时器
2. 定时器超时后退出框架
3. 定时周期内有玩家重新登陆 -> 关闭定时器
4. 定时器：使用框架定时器， timerfd产生超时，时间轮分发超时事件
5. 意义：在对局结束后及时释放服务器资源

#### 6. 玩家信息管理
1. 使用redis数据库来缓存玩家信息
2. 玩家初始化时将随机的昵称push到redis链表中
3. 当退出游戏时将昵称从链表中摘除
4. 调用hiredis跟redis服务器通信

### 登陆服务器架构

#### QT客户端
1. 登录
2. 房间的跟随和创建
3. http + json数据格式
4. 执行游戏客户端进程

#### Nginx
1. 静态页面，实现用户的账户密码注册
2. Fast-Cgi程序，更新密码，登录服务器
3. 调用脚本，system， fork+exec， popen

#### Docker容器
1. 镜像，容器，仓库
2. ##### 创建镜像
- 手动方式：获取原始镜像->安装软件或库->提交为新镜像
- 脚本运行：编写Dockerfile
3. ##### 运行容器
- 守护进程方式运行
- 端口映射（随机映射）
- 利用环境变量传参
4. 部分静态编译，将第三方库静态链接到程序中

#### 分布式
1. 在多台服务器运行容器
2. ##### redis存房间信息
- 主服务器存放数据
- 从服务器产生或操作的数据也是在主服务器上
3. ##### 消息分发
- *redis发布订阅机制*
- fastcgi程序发布创建容器的请求，等待创建结果
- 容器管理进程订阅信息，创建容器并回复端口号
- *RPC*
- Fast-Cgi程序远程调用创建容器的函数
- 容器管理进程实现创建容器的函数
- 使用方法：订阅函数原型，自动化生成代码，填充预留的函数
- 写调用端的代码

