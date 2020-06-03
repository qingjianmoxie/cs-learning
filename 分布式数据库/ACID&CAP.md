# ACID

# CAP

consistency 一致性: 
所有节点访问同一份最新的数据副本.
Every read receives the most recent write or an error.

Availability 可用性:
每次请求都能获取到非错的响应, 但是不保证获取的数据为最新数据.
Every request receives a (non-error) response - without the guarantee that it contains the most recent write.

Partition tolerance 分区容错性:
以实际效果而言, 分区相当于对通信的时限要求. 系统如果不能在时限内达成数据一致性, 就意味着发生了分区的情况, 必须就当前操作在C和A之间做出选择.
The system continues to operate despite an arbitrary number of messages being dropped (or delayed) by the network between nodes.


# 高可用与高可靠

+ 可用性(availability)是关于系统可供使用时间的表述，以不可用的时间为衡量指标。不可用时间越短，可用性越高。
+ 可靠性(reliability)是关于系统无故障时间间隔的描述，以发生故障的次数为衡量指标，故障次数越少，可靠性越高。
