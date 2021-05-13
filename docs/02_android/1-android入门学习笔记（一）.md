# Android入门学习笔记（一）

## 资源

### 学习记录

- 16:00 - 17:00 跟着视频把Android常用控件手敲一遍，做个Demo

### 视频课程

- 2021最新版】Android（安卓）开发零基础入门课程【全套】（由小米、阿里、腾讯、爱奇艺一线 https://bilibili.com/video/BV13y4y1E7pF

## 学习笔记

### 常用控件和属性

#### TextView

#### Button

### Mqtt

#### 引入mqtt

GitHub: [paho.mqtt.android](https://github.com/eclipse/paho.mqtt.android)

android引入mqtt，参考：[Paho-MQTT Android接入示例](https://www.alibabacloud.com/help/zh/doc-detail/146630.htm)

1. 在工程build.gradle中，添加Paho仓库地址。本示例使用release仓库。

   ```groovy
   repositories {
       maven {
           url "https://repo.eclipse.org/content/repositories/paho-releases/"
       }
   }
   ```

2. 在应用build.gradle中，添加Paho Android Service

   ```groovy
   dependencies {
       implementation 'org.eclipse.paho:org.eclipse.paho.client.mqttv3:1.1.0'
       implementation 'org.eclipse.paho:org.eclipse.paho.android.service:1.1.1'
   }
   ```

3. 为了使App能够绑定到Paho Android Service，需要在AndroidManifest.xml中添加以下信息：

   - 声明以下服务

   ```groovy
   <!-- Mqtt Service -->
   <service android:name="org.eclipse.paho.android.service.MqttService">
   </service>
   ```

   - 添加Paho MQTT Service所需的权限

    ```groovy
    <uses-permission android:name="android.permission.WAKE_LOCK" />
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.READ_PHONE_STATE" />
    ```

     

#### examples

1. 初始化mqtt

```java
String host = String.format(Locale.CHINA, "tcp://%s:%d", ip, port);
String clientId = "MqttAndroidClient";
client = new MqttAndroidClient(this, host, clientId);
// 设置MQTT监听并且接受消息
client.setCallback(mqttCallback);
```

2. 连接

```java
MqttConnectOptions conOpt = new MqttConnectOptions();
// 清除缓存
conOpt.setCleanSession(true);
// 设置超时时间，单位：秒
conOpt.setConnectionTimeout(10);
// 心跳包发送间隔，单位：秒
conOpt.setKeepAliveInterval(30);
// 用户名
conOpt.setUserName(userName);
// 密码
conOpt.setPassword(passWord.toCharArray());

// 连接
client.connect(conOpt, null, new IMqttActionListener() {
    @Override
    public void onSuccess(IMqttToken asyncActionToken) {
			
    }

    @Override
    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {

    }
});
```

3. 订阅

```java
// IMqttActionListener 的 onSuccess中订阅
@Override
public void onSuccess(IMqttToken asyncActionToken) {
    try {
      // qos，发布消息的服务质量，即：保证消息传递的次数
      // Ø00：最多一次，即：<=1
			// Ø01：至少一次，即：>=1，注意去重
			// Ø10：一次，即：=1
			// Ø11：预留
      _mqttAndroidClient.subscribe("topic1", 1);
    } catch (MqttSecurityException e) {
      e.printStackTrace();
    } catch (MqttException e) {
      e.printStackTrace();
    }
}
```

4. 处理消息

```java
// MqttCallback

@Override
public void connectionLost(Throwable cause) {
  Log.w(TAG, "connectionLost: " + cause.toString());
}

@Override
public void messageArrived(String topic, MqttMessage message) throws Exception {
  Log.i(TAG, MessageFormat.format("messageArrived: topic={0},messageId={1}}", topic, message.getId()));
}

@Override
public void deliveryComplete(IMqttDeliveryToken token) {
  Log.i(TAG, "deliveryComplete: " + token.toString());
}
```



QOS参考：

- [MQTT 入门介绍](https://www.runoob.com/w3cnote/mqtt-intro.html)
- [MQTT系列 | MQTT的QoS介绍](https://zhuanlan.zhihu.com/p/80203905)



### 高德地图

1. 下载高德地图SDK后，导入jar和so。参考：[Android Studio如何快速导入jar和.so文件](http://www.manongjc.com/detail/5-dgfqdqsfxsxtsyy.html)、[Android studio添加第三方库和so](https://blog.csdn.net/anhenzhufeng/article/details/78913341)

2. 高德地图控制台中创建android的key，生成debug和release的sha1码

   1. debug sha1码

      ```bash
      $ keytool -list -v -keystore ~/.android/debug.keystore
      # 输入密码 android，复制sha1的值
      ```

      

   2. release sha1码

      1. 先创建jks文件，见：[Android Studio创建自定义jks签名文件（Mac os系统）](https://blog.csdn.net/sinat_15411661/article/details/78918330)
      2. keytool -list -v -keystore jks的路径，然后输入jks文件的密码，参考：[Android 获取高德地图sha1 （正式 测试）](https://blog.csdn.net/Mr___Xu/article/details/86489004)

3. 此时就可以使用了。



注意地图的生命周期：

```java
	MapView mMapView = null;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState); 
    setContentView(R.layout.activity_main);
    //获取地图控件引用
    mMapView = (MapView) findViewById(R.id.map);
    //在activity执行onCreate时执行mMapView.onCreate(savedInstanceState)，创建地图
    mMapView.onCreate(savedInstanceState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    //在activity执行onDestroy时执行mMapView.onDestroy()，销毁地图
    mMapView.onDestroy();
  }

 @Override
 protected void onResume() {
    super.onResume();
    //在activity执行onResume时执行mMapView.onResume ()，重新绘制加载地图
    mMapView.onResume();
    }

 @Override
 protected void onPause() {
    super.onPause();
    //在activity执行onPause时执行mMapView.onPause ()，暂停地图的绘制
    mMapView.onPause();
    }

 @Override
 protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    //在activity执行onSaveInstanceState时执行mMapView.onSaveInstanceState (outState)，保存地图当前的状态
    mMapView.onSaveInstanceState(outState);
  } 
}
```

