
### pub-sub model

producer ----publish--->|---> Server --->|----push---> consumer
producer ----publish--->|        |       |----push---> consumer
producer ----publish--->|        v       |----push---> consumer
producer ----publish--->|    (storage)   |----push---> consumer


### 数据结构
【当前连接1】         ：map[ID, session]
【当前连接2】         ：map[IP:PORT, session]
【当前连接的订阅列表】：map[TopicName, map[IP:PORT, session]]
【订阅列表】          ：map[TopicName, list<Context>]
【历史记录】          ：map[TopicName, vector<Topic>]

注：一个连接可能会在不同的设备上登录（此时IP:PORT）就不是唯一
    Context & IP:PORT 都存储在session中，
    IP:PORT第一次连接就能确定，但Context要登录后才能确定！！！！
    Context包含哪些信息好呢？用户名？用户表数据库id？

### 动作
1. 模块初始化加载：从数据库更新【订阅列表】和【历史记录】

2. 客户端发起连接：根据【订阅列表】的信息，更新【当前连接的订阅列表】
3. 客户端断开连接：将这个客户端连接从【当前连接的订阅列表】中移除
4. 客户端发起订阅：添加这个客户端连接到【当前连接的订阅列表】，同时也添加到【订阅列表】
5. 客户端取消订阅：将这个客户端连接从【当前连接的订阅列表】中移除，同时也从【订阅列表】中移除
6. 服务端分发订阅：从【当前连接的订阅列表】中找到订阅的连接并分发，然后保存分发的内容到【历史记录】
7. 请求历史记录：  从【历史记录】中找出对应的话题，然后进行回应

注：【订阅列表】和【历史记录】是Storage子模块的成员变量

### 消息类型

