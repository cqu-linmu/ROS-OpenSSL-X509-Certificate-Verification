#include "ros/ros.h"
#include <signal.h>
#include <sstream>

#include "info_security/data.h"
#include "info_security/CertChecker.h"

// 声明证书检查者
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
    exit(0);
}

int main(int argc, char *argv[])
{
    // 中文显示
    setlocale(LC_ALL,"");
    // 初始化节点
    ros::init(argc,argv,"publisher",ros::init_options::NoSigintHandler);
    // 配置自定义退出 Handler
    signal(SIGINT, MySigintHandler);
    // 创建句柄
    ros::NodeHandle nh;
    // 创建发布者
    ros::Publisher pub = nh.advertise<info_security::data>("fang",10);
    // 设置发布频率(Hz) 这里是0.5 所以是两秒一次
    ros::Rate rate(0.5);

    // 创建要发布的消息
    info_security::data msg;

    // 创建证书检查者
    certChecker = new CertChecker();

    // 从文件中读取代理证书并添加到非信任证书链
    std::string agentPath = CA_DIR_AGENT + "key/agent.crt";
    X509* agent = CertChecker::getCert(agentPath);
    certChecker->addToChain(agent,false);

    // 从文件中读取服务器证书
    std::string servicePath = CA_DIR_SERIVER + "seriver.crt";
    X509* service = CertChecker::getCert(servicePath);

    // 构建服务器验证的证书链
    STACK_OF(X509)* chain = certChecker->buildCertChain(service);
    if (chain != NULL) {
        ROS_INFO("证书验证通过");
    } else {
        ROS_ERROR("证书验证失败");
        return 0;
    }

    // 将证书链解析进去字符串数组 certs
    int length = sk_X509_num(chain);
    std::cout << "证书链长:" << length << std::endl;
    std::string certs[length]; 
    for(int i=0;i<length;i++) {
        X509* tempCert = sk_X509_value(chain,i);
        certs[i] = CertChecker::x509_to_string(tempCert);
        std::cout << certs[i] << std::endl;
    }

    // 正式发送消息都循环
    int count = 0;
    while(ros::ok)
    {
        count++;
        std::stringstream ss;
        ss << "当前是第" << count << "条消息!";

        // 组装消息
        msg.header.seq = count;
        msg.header.stamp = ros::Time::now();
        msg.header.frame_id = "cert";
        msg.data = ss.str();
        msg.certs.clear();
        for(std::string cert:certs) {
            msg.certs.push_back(cert);
        }
        // 发布消息
        pub.publish(msg);
        // 打印 log
        ROS_INFO("%s",ss.str().c_str());
        // 休眠指定时间
        rate.sleep();
        ros::spinOnce();
    }
    return 0;
}
