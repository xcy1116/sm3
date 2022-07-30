# sm3
sm3实现与优化，sm3生日攻击，sm3长度扩展攻击

## sm3实现与优化


### 实验概述
通过填充、消息扩展、迭代压缩实现sm3加密过程。

利用循环展开与simd指令，实现对单指令多数据流的数据处理，优化了加密过程。
通过对字符串"abc",即十六进制616263进行加密，计算出10000次加密时间约为0.043s，对比使用gmssl密码库进行加密10000次时间为0.012s,可知算法优化后运行速度与使用密码库位于同一指数级别。
### 代码说明
对初始向量赋值，使用simd指令集实现单指令多数据的性能优化。同时处理8个数据进行赋值。
![1$%NPIN)F G26 EL _Z}N~6](https://user-images.githubusercontent.com/110040961/181903557-428815b7-37a8-4e20-94a5-3b930d4ffd47.png)



消息扩展过程，使用循环展开进行优化。


![_6_E$NSJ6NCT532{S5$_6PX](https://user-images.githubusercontent.com/110040961/181903630-5ef9f85c-f183-4970-b053-5d6c7f29a856.png)



消息扩展过程，将消息分组划分为不同大小的消息字，通过前边的消息字进行异或得到后64个消息字。利用simd指令进行异或操作，同时处理8个数据。

![7DN})192)~1 {(BA3{4Z L](https://user-images.githubusercontent.com/110040961/181903728-291525f1-39a6-4179-9b08-872fc9af3ed6.png)


详细优化内容见cpp文件。

### 运行截图：

![@~ZB51F6{ @_ _~2~@I$V4T](https://user-images.githubusercontent.com/110040961/181868472-8a4f2a30-19e0-4aca-ace3-eee0aeb46329.png)






## 实现sm3生日攻击:

### 实验概述

随机生成字符串str,sm3计算其对应哈希值h。穷举字符串，输出对应hash值前8bit与h相同的字符串与其哈希值。

在所输出的前8bit相同的字符串中，筛选得到前16bit相同的字符串，从而得到碰撞。

8bit碰撞的原像与哈希值如图所示，红框内为发生16bit碰撞所对应的消息与加密后哈希值。

### 代码说明

generateString函数通过对所有字符串元素的随机选择生成所需长度的字符串str。

![((7J()8~7HIW}S9~JWEXYV9](https://user-images.githubusercontent.com/110040961/181909623-06fc3c02-1b73-4fcf-83cc-4184ca6a43c2.png)

通过遍历字符串元素，保证新生成字符串与str及之前生成的字符串不同。

![R~I581~V58PP2%J{ Q9})J7](https://user-images.githubusercontent.com/110040961/181909698-4eef9496-9ada-4e63-bc25-7c82a92f6c9a.png)

若新字符串get_str的哈希值与str的哈希值前8bit相同，则输出get_str与其哈希值，再由此筛选出16bit碰撞的字符串。

### 运行截图：

红框内为发生碰撞的字符串与其对应哈希值。
![生日攻击2](https://user-images.githubusercontent.com/110040961/181728741-8a5cb737-77f1-4a85-99a0-f26aeef0b1b5.png)






## 实现sm3长度扩展攻击：

### 实现思路

随机生成一个消息（secret），用SM3函数算出哈希值（hash1）

生成一个附加消息（append）。首先用hash1推算出这一次加密结束后8个向量的值，再以它们作为初始向量，去加密append，得到另一个hash值(result)

计算secret + padding + append的hash值（result2），如果攻击成功，hash2应该和hash3相等。

### 代码说明

修改初始向量为消息secret加密后输出的向量，并用修改后初始向量对附加消息append进行加密
![4LN5$(I441RQR1H$WX4LYZK](https://user-images.githubusercontent.com/110040961/181914174-25870868-a033-489c-aab1-3dbeac53ffbb.png)

将消息secret填充为64字节，生成secret+padding+append
![F(FHTI(7M%@_%N~Z$P06 75](https://user-images.githubusercontent.com/110040961/181914097-64d773e6-d5f3-4969-a813-a6150a04a8f1.png)


### 运行截图：

![F4(XGC }5V8DRWXCBUDF2TC](https://user-images.githubusercontent.com/110040961/181730708-0c509d58-254b-416b-927f-110e52114db8.png)
