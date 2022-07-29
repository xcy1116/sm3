# sm3
sm3优化，sm3生日攻击，sm3长度扩展攻击，sm4实现与优化

# sm3实现与优化
通过填充、消息扩展、迭代压缩实现sm3加密过程。

利用循环展开与simd指令，实现对单指令多数据流的数据处理，优化了加密过程。


# 实现sm3生日攻击:

随机选择字符串m,sm3计算其对应哈希值h。穷举字符串，输出对应hash值前8bit与h相同的字符串与其哈希值。在所输出的前8bit相同的字符串中，筛选得到前16bit相同的字符串，从而得到碰撞。

![生日攻击2](https://user-images.githubusercontent.com/110040961/181728741-8a5cb737-77f1-4a85-99a0-f26aeef0b1b5.png)






# 实现sm3长度扩展攻击：

随机生成一个消息（secret），用SM3函数算出哈希值（hash1）
生成一个附加消息（append）。首先用hash1推算出这一次加密结束后8个向量的值，再以它们作为初始向量，去加密append，得到另一个hash值(result)
计算secret + padding + append的hash值（result2），如果攻击成功，hash2应该和hash3相等。


![F4(XGC }5V8DRWXCBUDF2TC](https://user-images.githubusercontent.com/110040961/181730708-0c509d58-254b-416b-927f-110e52114db8.png)
