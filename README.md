# 介绍

![DSC01798.jpg](https://github.com/DaqoLee/K_Clock/blob/main/img/DSC01791.jpg?raw=true) 

这是一个基于ESP32-PICO-D4设计的单体翻页时钟，其主要特点是采用电机内置结构，体积小巧、并且结
构件大部分都采用PCB，整体没有用到一条线，十分简洁。目前有两个版本：
* **二十四时辰**：esp系统时间 + sntp校时
* **天气预报**：数据来源[心知天气](https://www.seniverse.com/)

## 开源文件
* [ PCB工程(嘉立创EDA专业版)](https://oshwhub.com/daqolee/dan-tfan-ye-shi-zhong)
* 3D打印文件（stl）
* [程序源码(esp-idf-v4.4)](https://github.com/DaqoLee/K_Clock.git)
* 零件BOM表

## 介绍视频
[单体翻页时钟_bilibili](https://www.bilibili.com/video/BV1Uo4y1A714/)

# 基本结构
## 3D模型

![微信截图_20230615184917.png](https://github.com/DaqoLee/K_Clock/blob/main/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230615184917.png?raw=true)

本结构的特点就是用PCB做框架，电机内置，把叶片滚轮设计成滑环就可以实现电机360度旋转。这种设计  
优点就是空间利用率高，使结构更加紧凑。缺点就是成本较高，不易装配和维护。
![微信截图_20230615184628.png](https://github.com/DaqoLee/K_Clock/blob/main/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230615184628.png?raw=true)

## 结构零件
整个结构所用的PCB。 

![IMG_20230615_172628.jpg](https://github.com/DaqoLee/K_Clock/blob/main/img/IMG_20230615_172628.jpg?raw=true)
### 叶片
24片版本PCB厚度`0.8mm`（有点大材小用），当然也可以使用面板打印，如果是60片版的厚度需要`≤0.5mm`。
![微信截图_20230616080442.png](https://github.com/DaqoLee/K_Clock/blob/main/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230616080442.png?raw=true) 

![微信截图_20230616080403.png](https://github.com/DaqoLee/K_Clock/blob/main/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230616080403.png?raw=true)
### 二十四时辰对照表
![9a2fce66-aa30-411a-b7b3-c9c588af768b.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/9a2fce66-aa30-411a-b7b3-c9c588af768b.png)  
上面两个版本主要是为了验证可行性，所以图案相当的直男。0.8mm不支持彩色丝印，彩色图案最好选择  
双面面板打印或者其他工艺。下面是我在网上找到的一些图案，大家可以参考一下。

### 二十四时辰
![微信截图_20230615195644.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230615195644.png)  
![微信截图_20230615195941.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230615195941.png)

### 天气图标
![微信截图_20230615200713.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230615200713.png)  
![微信截图_20230615200555.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230615200555.png)

## 主控&底板&侧板

![微信截图_20230616082647.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230616082647.png)  
**主要器件：**
* 主控芯片：`ESP32-PICO-D4`
* 电机驱动：`TMC2209`
* ~~无线充电：`BQ51013B`（这个版本翻车了，暂时不能用）~~
* 锂电池充电：`TP4065`
* 5V稳压芯片：`TPS63002`

**目前底板无线充电还不能用，开关机电路也还需要改进，其他功能正常。因为程序和硬件都没有考虑低功
耗问题，所以电池坚持不了多久。测试待机电流10mA左右，电机运行电流200mA左右，建议插电使用。**

主控和底板已经在嘉立创SMT贴好  
![IMG_20230615_173302.jpg](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/IMG_20230615_173302.jpg)  
只需要补焊接线柱和排母就可以了  
![IMG_20230615_173358.jpg](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/IMG_20230615_173358.jpg)  
主控和侧板使用`彩色丝印`，PCB厚度只能`1.6mm`。  
![IMG_20230615_172925.jpg](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/IMG_20230615_172925.jpg)  
这里需要注意的是`弹簧顶针是从PCB背部穿过来的，并不是贴在表面的`。这样做的目的是可以选择更大行程
提高容错率。
![IMG_20230616_111006.jpg](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/IMG_20230616_111006.jpg)
## 叶片滚轮
叶片滚轮（滑环）有24/60两个版本，厚度`1.6mm`

![微信截图_20230616103233.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230616103233.png)  
每块滑环PCB连接一相（A/B相两条线），然后通过四根铜柱连接到触点。**两块滑环PCB要旋转180度连接**。  
`电机与滑环是通过弹簧排针和触点连接`，电机与转接PCB（厚度`0.8mm`）焊好后一定要用热熔胶固定好，否  
则容易损坏电机。除了基本的元件外还有一些接线柱、弹簧顶针（POGOPIN）、焊接铜套等后面会给BOM表。  
![IMG_20230616_105608.jpg](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/IMG_20230616_105608.jpg)  
电机选择的是`GM15BY永磁步进电机`，推荐买`减速比100以上，不焊线，轴长4.4/5mm版本`
![IMG_20230615_173543.jpg](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/IMG_20230615_173543.jpg)  
![下载.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E4%B8%8B%E8%BD%BD.png)  
因为叶片PCB比较硬，很难形变，所以需要打印一个辅助装配叶片的零件，然后按照顺序装配就可以了
![微信截图_20230616113354.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230616113354.png)  
![微信截图_20230616113526.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230616113526.png)  
![Snipaste_2023-06-16_11-41-46.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/Snipaste_2023-06-16_11-41-46.png)  
![微信截图_20230616114244.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230616114244.png)    
![Snipaste_2023-06-16_11-45-49.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/Snipaste_2023-06-16_11-45-49.png)  
![Snipaste_2023-06-16_11-46-40.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/Snipaste_2023-06-16_11-46-40.png))  
![DSC01791.jpg](https://github.com/DaqoLee/K_Clock/blob/main/img/DSC01791.jpg?raw=true))

**介绍视频已经上传到B站了:[单体翻页时钟_bilibili](https://www.bilibili.com/video/BV1Uo4y1A714/)，欢迎大家一键三连支持。**


# 程序源码

**程序比较简单，网上已经有很多`网络时钟`的开源代码，把显示部分改成控制步进电机就可以了。**
![微信截图_20230616132606.png](//image.lceda.cn/pullimage/R2QZrHMtthvPci5a0kXTTfFTkOkQJRJysDrQqg8q.png)
**代码我稍微封装了一下，更换不同叶片只需要修改几个参数就可以了**
```
void ClockTask(void *arg)
{
    EventBits_t uxBits;
    Clock_config_t clockConfig = {
        .wifiConfig = {
            .sta = {
                "ssid", //ssid
                "password", // password
                .bssid_set = false,
            }
        },
        .ntpUrl = "ntp.aliyun.com",
        .userKey = "userKey", //心知天气userKey
        .location = "guangzhou",
        .powerPin = GPIO_NUM_4,
        .pages = 24, //叶片数
        .ntpInterval = 30,//网络校准时间间隔/分钟
        .speed = 2000, // 电机速度
        .compensation = 1000, //步进数误差补偿

        .stepperConfig = {
            .stepPin = 27, 
            .dirPin = 26, 
            .enPin = 25,
            .ms1Pin = 14, // 细分控制
            .ms2Pin = 12, //
            .timer_group = TIMER_GROUP_0,
            .timer_idx = TIMER_1,
            .miStep = MICROSTEP_16,  //电机驱动细分
            .stepAngle = 0.086//0.086 0.12 步进电机减速后的步进角
        }

    };
    Clock1.config(&clockConfig);
    Clock1.init();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    Clock1.runPages(Clock1.getTimeHour());
    //Clock1.runPages(Clock1.getWeatherCode());
    while (1)
    {
        uxBits = xEventGroupWaitBits(
                       clock_event_group,
                       SNTP_BIT | BTNJUMP_BIT | BTNLONG_BIT,
                       pdTRUE,
                       pdFALSE,
                       60000/portTICK_PERIOD_MS );//60000/portTICK_PERIOD_MS portMAX_DELAY

        if(uxBits & SNTP_BIT)//时间校准事件
        {
            //Clock1.runPages(Clock1.getWeatherCode());
            //Clock1.runPages(Clock1.getTimeHour());
        }
        else if (uxBits & BTNJUMP_BIT)//按键短按跳变/松开
        {
            Clock1.runInf(200); //短按微调时间
        }
        else if (uxBits & BTNLONG_BIT)//按键长按
        {    
            Clock1.powerOFF();//长按关机
        }
        else //最大等待时间，这里是一分钟刷新一次。
        {     
            Clock1.runPages(Clock1.getTimeHour());
        }
    }
    
}
```

**源码链接：**[https://github.com/DaqoLee/K_Clock.git](https://github.com/DaqoLee/K_Clock.git)

# 挖坑
**单体版主要是对结构进行验证，现在双体版本已经在路上了，敬请期待。**  
![IMG_20230615_175053.jpg](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/IMG_20230615_175053.jpg)  
![IMG_20230615_175303.jpg](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/IMG_20230615_175303.jpg)  
![微信截图_20230616150707.png](https://raw.githubusercontent.com/DaqoLee/K_Clock/a2d646b528f1f3724960ddbcaa5da76e46630d56/img/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230616150707.png)  

# 器件BOM


| 名称 | 参数 | 数量 | 链接 |
| --- | --- | --- | --- |
|GM15BY减速步进电机  | 轴长4.4mm、减速比1：150 |1  |  [步进电机](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.7c902e8dM8tzXP&id=543899450393&_u=d1r0amg7ebef) |
| POGOPIN弹簧顶针 | M102 （2 * 4.6）| 10 | [弹簧顶针](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.2a3d2e8dWj0rYe&id=625464084733&_u=g1r0amg7f117) |
| M2铜柱 | 24mm | 4 | [铜柱](https://detail.tmall.com/item.htm?_u=g1r0amg7569f&id=594046864224&spm=a1z09.2.0.0.2a3d2e8dWj0rYe) |
| PCB方形铜接线端子 | 5 * 5 * 5平底 | 8 | [方形接线端子](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.2a3d2e8dWj0rYe&id=614073950829&_u=g1r0amg7fd07) |
| SMT表贴片铜柱 | ￠3 * ￠5 * 3 + ￠4 * 1.6 | 1 | [贴片铜柱](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.2a3d2e8dWj0rYe&id=680807959486&_u=g1r0amg743e1) |
| 挡边轴承 | 5 * 8 * 2.5 | 1 | [轴承](https://detail.tmall.com/item.htm?_u=g1r0amg729bd&id=611878216362&spm=a1z09.2.0.0.2a3d2e8dWj0rYe) |
| pogopin连接器 | 4P侧焊公座 |1  | [连接器](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.2a3d2e8dWj0rYe&id=42066325628&_u=g1r0amg7065c) |
| 螺丝 | M1.6 * 4、M2 * 4、M2.5 * 4 | 50 | [螺丝](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.2a3d2e8dWj0rYe&id=40328525933&_u=g1r0amg7cb57) |