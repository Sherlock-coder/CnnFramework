### 背景介绍

这是我们的程序设计综合实践课的最终成品,主题是实现一个可配置的神经网络算法,不能使用python,我们尝试用C++实现了一个简单的cnn框架，并成功应用在手写数字和新冠肺炎识别(简化版)任务上,此外成品还包括一个C++实现的类numpy库.能力有限,尚有不足之处,欢迎交流.

:email:2577273670@qq.com

### 文件信息

cnn:cnn类库

cnn.h:只需要include 这个头文件就可以开始使用cnn类库

ImagePreprocess:读取图片像素值类库

ImagePreprocess.hpp:只需要include这个头文件即可使用读取图片像素值类库

digit.cpp:手写数字

mnist:手写数字相关数据，训练集，测试集等，其中win.txt为训练好的模型的参数

digitTest:测试文件夹，将不多于100张图片放入该文件夹，运行digit.exe进行预测

covid.cpp,covid.exe:新冠肺炎预测

ctData:肺炎图像

Data:肺炎预测相关数据，其中win.txt为训练好的模型的参数

covidTest:测试文件夹，将不多于40张图片放入该文件夹，运行covid.exe进行预测

api.pptx:类库接口

fc1:继承自fc的一个类，在实现covid.cpp中有应用

NumRos:一个类numpy数学库，独立于上述cnn类库

Network:基于NumRos实现的神经网络

















 