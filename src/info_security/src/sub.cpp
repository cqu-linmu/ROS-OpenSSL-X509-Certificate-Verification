#include "ros/ros.h"
#include <signal.h>

#include "info_security/data.h"
#include "info_security/CertChecker.h"

// 创建证书检查者
CertChecker* certChecker = nullptr;

// 自定义的 Ctrl + C 退出 handler
void MySigintHandler(int sig)
{
	//这里主要进行退出前的数据保存、内存清理、告知其他节点等工作
    if(certChecker != nullptr) {
        delete certChecker;
    }
	ROS_INFO("shutting down!");
	ros::shutdown();
}

// 消息处理回调函数
void doMsg(const info_security::dataConstPtr &msg)
{
    // 打印 log
    ROS_INFO("订阅：%s",msg->data.c_str());
    // 使用 cert 保存服务器证书
    X509* cert = nullptr;
    // 获取证书链长度
    int cert_count = msg->certs.size();
    // 遍历证书字符串
    for(std::string certString:msg->certs) {
        // 将字符串重新解析为 X509 证书
        X509* temp = CertChecker::string_to_x509(certString);
        if(temp == nullptr) {
            ROS_ERROR("解析失败");
            break;
        }
        // 使用 cert 接收服务器证书
        if(cert == nullptr) {
            cert = temp;
        }
        // 将 证书添加到 非信任证书链
        certChecker->addToChain(temp,false);
        // 打印 解析文件消息
        char buf[256];
        X509_NAME_oneline(X509_get_subject_name(temp), buf, 256);
        std::cout << "已解析：" << buf << std::endl;
    }
    // 验证证书
    int res = certChecker->verifyCert(cert);
    if (res == 1) {
        ROS_INFO("证书验证通过");
    } else {
        ROS_ERROR("证书验证失败");
    }
}
int main(int argc, char *argv[])
{
    // 中文显示
    setlocale(LC_ALL,"");
    // 初始化节点
    ros::init(argc,argv,"subscriber",ros::init_options::NoSigintHandler);
    // 配置自定义退出 Handler
    signal(SIGINT, MySigintHandler);

    // 创建句柄
    ros::NodeHandle nh;
    // 创建订阅者
    ros::Subscriber sub = nh.subscribe("fang",10,doMsg);
    // 创建证书检查者
    certChecker = new CertChecker();
    // 进入循环接收消息
    ros::spin();
    return 0;
}
