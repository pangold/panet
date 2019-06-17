### 基础类库

各种数据结构、流和缓冲区、日志、编号、大小端转换、数据校验、线程（池）、顺序执行器、流式执行器等公共类库。

> 注：不对大部分数据结构进行封装，也没有必要，标准库已提供了，但标准库提供的数据结构与算法，第一：时间复杂度均为n，第二：并非线程安全。应对两项冲突的情况，只对小部分用到的数据结构进行重新的封装。

### 基础模块

1. tcp 网络层数据接收
2. protocol 协议解析（解析并分发具体的protobuf消息）

> 目前仅支持TCP，后续会支持UPD。
> 另外使用的协议是自定义的protocol::datagram，后续会支持Http协议

### 公共业务模块

1. rpc
2. pub_sub
3. proxy

> 所有模块的通信协议均是自定义的protocol::datagram协议

### Demo

1. echo 无协议，无数据边界

### Examples

1. echo-server
2. echo-client
3. rpc-server
4. rpc-client
5. pub-server
6. pub-client
7. sub-client
8. proxy-server

### 后续新增特性

1. 客户端自动重连，有了此功能后，各个模块就不需要按固定顺序启动了
2. protocol::codec增加流量/请求数等信息的统计的回调