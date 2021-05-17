[[toc]]

# Redis

## 基础

### Redis支持的数据类型

#### String
```bash
set key value
```

#### Hash

```bash
hmset name key1 value1 key2 value2
```

#### List

```bash
lpush name value # 头部
rpush name value # 尾部添加
lrem name index  # 删除
llen name        # 长度
```

#### Set

```bash
sadd name value
```

#### ZSet
```bash
zadd name score value
```

## 参考

1. [几率大的Redis面试题（含答案）](https://blog.csdn.net/Butterfly_resting/article/details/89668661)
2. [Redis常见面试题](https://www.cnblogs.com/jasontec/p/9699242.html)
3. [吐血整理60个Redis面试题,全网最全了](https://zhuanlan.zhihu.com/p/93515595)