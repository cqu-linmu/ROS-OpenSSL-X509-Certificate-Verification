# ROS-OpenSSL-X509-Certificate-Verification
重庆大学国家卓越工程师学院-智能汽车软件-智能网联汽车信息安全-实验


**✨✨✨实验环境✨✨✨：**<br />Windows 10 Linux Subsystem<br />BRANCH:   vb_release<br />RELEASE:  Ubuntu 20.04.3 LTS<br />KERNEL:   Linux 5.10.16.3-microsoft-standard-WSL2<br />openssl 版本：OpenSSL 1.1.1f  31 Mar 2020
<a name="ToGrH"></a>
# 1. 创建证书链
<a name="h0vGn"></a>
## 1.1. 创建根证书
<a name="xDtsn"></a>
### 1.1.1. 创建根CA目录
在自己定义的地方创建CA目录及根CA目录：
```bash
cd {自定义目录}
sudo mkdir -p ./ca_dir/root/key
cd ./ca_dir/root
sudo mkdir newcerts
sudo touch index.txt
sudo touch index.txt.attr
su root
# 输入密码
echo 01 > serial
su {用户名}
```
<a name="jXYfG"></a>
### 1.1.2. 创建配置文件
```bash
vim openssl.cnf
```
按i进入编辑模式，粘贴以下配置(注意其中的 dir 要修改为自己的目录)
```
[ ca ]
default_ca	= CA_default
 
[ CA_default ]
dir		    = {自定义目录}/ca_dir/root/
certs		= $dir/certs
crl_dir		= $dir/crl
database	= $dir/index.txt
new_certs_dir	= $dir/newcerts
certificate	= $dir/key/{自定义根证书名}.crt
serial		= $dir/serial
crlnumber	= $dir/crlnumber
crl		    = $dir/crl.pem
private_key	= $dir/key/{自定义根私钥名}.pem
RANDFILE	= $dir/key/.rand
unique_subject	= no
 
x509_extensions	= usr_cert
copy_extensions = copy
 
name_opt 	= ca_default
cert_opt 	= ca_default
 
default_days	= 365
default_crl_days= 30
default_md	= sha256
preserve	= no
policy		= policy_ca
 
[ policy_ca ]
countryName		= supplied
stateOrProvinceName	= supplied
organizationName	= supplied
organizationalUnitName	= supplied
commonName		= supplied
emailAddress		= optional
 
[ req ]
default_bits		= 2048
default_keyfile 	= privkey.pem
distinguished_name	= req_distinguished_name
attributes		= req_attributes
x509_extensions	= v3_ca
string_mask = utf8only
utf8 = yes
prompt                  = no
 
[ req_distinguished_name ]
countryName			= CN
stateOrProvinceName		= ChongQing
localityName			= ChongQing
organizationName        = CQU
organizationalUnitName	= Root CA
commonName			= cQU Root CA
 
[ usr_cert ]
basicConstraints = CA:TRUE
 
[ v3_ca ]
basicConstraints = CA:TRUE
 
[ req_attributes ]
```
编辑完成后按esc退出插入模式，按 :wq 完成保存退出。<br />目前得到目录文件如下：<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678847254062-75a90caf-ad33-45a0-8b60-4e309e530538.png#averageHue=%232a323e&clientId=u8c9be993-9bb4-4&from=paste&height=182&id=u57491bab&name=image.png&originHeight=202&originWidth=257&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=53069&status=done&style=none&taskId=u8c8208e3-51d5-4fb5-b9fe-c8ea2905d83&title=&width=231.5315285476711)
<a name="IIhyV"></a>
### 1.1.3. 创建根证书
```bash
# 创建根私钥
sudo openssl genrsa -out ./key/root_private_key.pem 2048
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678847232474-a0f0ed10-bbfb-47b6-98de-9d020531f500.png#averageHue=%23141414&clientId=u8c9be993-9bb4-4&from=paste&height=101&id=u1c7de973&name=image.png&originHeight=112&originWidth=927&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=14363&status=done&style=none&taskId=ua4a53db0-1dd5-4d64-b5b6-cd7a01fc293&title=&width=835.135124372339)
```bash
sudo openssl req -new -key ./key/root_private_key.pem -out ./key/root.csr -config openssl.cnf
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678847287097-ca476bce-1436-436f-87ec-94f97203e348.png#averageHue=%23151515&clientId=u8c9be993-9bb4-4&from=paste&height=53&id=uc81140e1&name=image.png&originHeight=59&originWidth=926&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=13221&status=done&style=none&taskId=u597c1b13-0e9d-44d2-aa63-3cfe1ec3224&title=&width=834.2342234830484)<br />这里没有任何输出log
```bash
sudo openssl ca -selfsign -in ./key/root.csr -out ./key/root.crt -config openssl.cnf
```
这里由于前面的配置文件中指定了根私钥的位置，所以没有通过 -key 来指定私钥。<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678847312074-26c35ccc-da80-4b9a-b36d-d805c73691e4.png#averageHue=%23131313&clientId=u8c9be993-9bb4-4&from=paste&height=471&id=ua373c6eb&name=image.png&originHeight=523&originWidth=938&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=61402&status=done&style=none&taskId=u184ba7a5-8d97-4bdf-87f2-a41c1935725&title=&width=845.045034154535)<br />到这里就完成了根证书的自签名了，我们可以通过以下命令查看证书信息：
```bash
openssl x509 -text -in ./key/root.crt
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678847333274-3a1b15be-37f4-4953-ab0f-7db1a0eabe0f.png#averageHue=%23181818&clientId=u8c9be993-9bb4-4&from=paste&height=725&id=u3e01df3d&name=image.png&originHeight=805&originWidth=837&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=107067&status=done&style=none&taskId=u22ce19ef-6823-48ef-9a19-cf7f97a6c64&title=&width=754.0540443361896)<br />这样我们就得到了根CA证书，其可以用于签发二级代理CA，而不可用于签发服务器CA证书。<br />而当前的CA目录如下：<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678847355498-398474e0-23e7-4a2e-865b-803536d41107.png#averageHue=%232c333e&clientId=u8c9be993-9bb4-4&from=paste&height=369&id=ue8f0dba7&name=image.png&originHeight=410&originWidth=264&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=101476&status=done&style=none&taskId=ub3f2ddb8-9a97-4b43-871f-01e7ccdf718&title=&width=237.83783477270495)<br />ps: 这里作者是通过Visual Studio Code 的远程连接插件连接的WSL，这样可以较为方便地管理文件及编写代码。
<a name="NtjK1"></a>
## 1.2. 创建二级CA代理证书
<a name="Xsu6E"></a>
### 1.2.1. 创建代理CA目录
这里首先如同创建根证书目录时类似：
```bash
cd ..
sudo mkdir agent
cd agent
sudo mkdir key
sudo mkdir newcerts
sudo touch index.txt
sudo touch index.txt.attr
su root
# 输入密码
echo 01 > serial
su {用户名}
```
得到结构如下：<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678849042188-db180ec5-d7c8-4222-b967-99ac1e2f2be6.png#averageHue=%2329313c&clientId=u8c9be993-9bb4-4&from=paste&height=232&id=ue0f6d299&name=image.png&originHeight=257&originWidth=252&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=65346&status=done&style=none&taskId=u4223cff3-94ff-452a-b424-7478265bac8&title=&width=227.02702410121836)<br />注意agent和root目录是平级的。
<a name="ybhq1"></a>
### 1.2.2. 创建代理CA配置文件
在agent目录中创建配置文件openssl.cnf
```bash
vim openssl.cnf
```
```
[ ca ]
default_ca	= CA_default
 
[ CA_default ]
dir		    = {自定义目录}/ca_dir/agent/
certs		= $dir/certs
crl_dir		= $dir/crl
database	= $dir/index.txt
new_certs_dir	= $dir/newcerts
certificate	= $dir/key/{自定义代理CA证书文件名}.crt
serial		= $dir/serial
crlnumber	= $dir/crlnumber
crl		    = $dir/crl.pem
private_key	= $dir/key/{自定义代理CA私钥文件名}.pem
RANDFILE	= $dir/key/.rand
unique_subject	= no
 
x509_extensions	= usr_cert
copy_extensions = copy
 
name_opt 	= ca_default
cert_opt 	= ca_default
 
default_days	= 365
default_crl_days= 30
default_md	= sha256
preserve	= no
policy		= policy_ca
 
[ policy_ca ]
countryName		= supplied
stateOrProvinceName	= supplied
organizationName	= supplied
organizationalUnitName	= supplied
commonName		= supplied
emailAddress		= optional
 
[ req ]
default_bits		= 2048
default_keyfile 	= privkey.pem
distinguished_name	= req_distinguished_name
attributes		= req_attributes
x509_extensions	= v3_ca
string_mask = utf8only
utf8 = yes
prompt = no
 
[ req_distinguished_name ]
countryName			= CN
stateOrProvinceName		= ChongQing
localityName			= ChongQing
organizationName        = CQU
organizationalUnitName	= agent CA
commonName			= CQU agent CA
 
[ usr_cert ]
basicConstraints = CA:FALSE
 
[ v3_ca ]
basicConstraints        = CA:TRUE
 
[ req_attributes ]
```
<a name="LOinR"></a>
### 1.2.3. 创建二级代理CA证书
首先确定当前目录为 {自定义目录}/ca_dir/agent/
```bash
# 创建私钥
sudo openssl genrsa -out ./key/agent_private_key.pem 2048
# 创建签发请求
sudo openssl req -new -key ./key/agent_private_key.pem -out ./key/agent.csr -config openssl.cnf
# 使用根证书签发代理CA
sudo openssl ca -in ./key/agent.csr -out ./key/agent.crt -config ../root/openssl.cnf
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678849680963-a4e19ae2-dce6-4c5c-94a1-caf01244ba2a.png#averageHue=%23141414&clientId=u8c9be993-9bb4-4&from=paste&height=605&id=ubfbecff8&name=image.png&originHeight=672&originWidth=942&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=85876&status=done&style=none&taskId=u64f5b870-4190-4b87-8876-9c46a98703c&title=&width=848.6486377116971)<br />注意这里使用的是根证书的配置文件，里面配置了根证书及其私钥的文件名等签名配置。<br />通过命令来查看证书内容：
```bash
openssl x509 -text -in ./key/agent.crt
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678849837202-96b61fa9-4e07-45cf-98e7-aa6113b4caf2.png#averageHue=%23171717&clientId=u8c9be993-9bb4-4&from=paste&height=684&id=u12e2c9f0&name=image.png&originHeight=759&originWidth=913&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=101409&status=done&style=none&taskId=uf7ca30db-9aea-4cdf-9d9d-03fe850a792&title=&width=822.5225119222713)<br />这里看到 ：<br />Issuer 内标注了 代理CA证书是由根证书签发的。<br />X509v3 extensions 下有着 CA:TRUE 的标注，表明代理CA证书可以签发下一级证书。<br />但代理CA证书不能用于签发其他代理CA证书（在配置文件中已设置），而实际上的一些代理CA是可以继续签发的其他代理CA的。<br />这里也可以到root/index.txt 中查看到根证书已签发的证书：
```bash
cat root/index.txt
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678850203653-eef98a05-2971-48d4-bedc-fa0691a2c3b8.png#averageHue=%231e1e1e&clientId=u8c9be993-9bb4-4&from=paste&height=58&id=uc79b67c8&name=image.png&originHeight=64&originWidth=921&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=14805&status=done&style=none&taskId=u2f181bdb-9f49-4f5c-9102-1d7aac5f81f&title=&width=829.7297190365956)
<a name="Bhhhh"></a>
## 1.3. 创建三级服务器证书
<a name="tjfgx"></a>
### 1.3.1. 创建目录及配置文件
首先创建目录并创建配置文件：
```bash
cd {自定义目录}/ca_dir
mkdir seriver
vim openssl.cnf
```
```
[ req ]
prompt             = no
distinguished_name = server_distinguished_name
req_extensions     = req_ext
x509_extensions	= v3_req
attributes		= req_attributes
 
[ server_distinguished_name ]
commonName              = CQU_seriver
stateOrProvinceName     = ChongQing
countryName             = CN
organizationName        = CQU
organizationalUnitName  = CQU IT seriver
 
[ v3_req ]
basicConstraints        = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment
 
[ req_attributes ]
 
[ req_ext ]
subjectAltName      = @alternate_names

[ alternate_names ]
DNS.1        = cqu.edu.cn
```
这里服务器配置文件内则主要配置的是服务器的信息。
<a name="mXyg5"></a>
### 1.3.2. 创建服务器证书并签名
```bash
# 创建私钥
sudo openssl genrsa -out ./seriver_private_key.pem 2048
# 创建前面请求
sudo openssl req -new -key ./seriver_private_key.pem -out ./seriver.csr -config openssl.cnf
# 使用代理CA来签发服务器证书
sudo openssl ca -in ./seriver.csr -out ./seriver.crt -config ../agent/openssl.cnf
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1678851249460-3d60b2a1-17ff-417a-b5ac-d953c6e748a1.png#averageHue=%23141414&clientId=u8c9be993-9bb4-4&from=paste&height=618&id=ufbab6a36&name=image.png&originHeight=686&originWidth=928&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=86418&status=done&style=none&taskId=uf4944680-8364-44c2-bac6-998caefba46&title=&width=836.0360252616294)<br />这里可以直接看到log里面有 CA:FALSE，表明本证书不能进行签发。

<a name="UuRaV"></a>
# 2. 代码实现证书链的构建、发送及验证
实验目标：<br />现在目前有节点A和节点B，其都保存并信任根证书。然后节点A需要构建一条证书链(服务器证书到根证书)，并将其发送给节点B。由于节点B信任根证书，所以可以将信任通过证书链传递到了服务器证书，从而验证了节点A的身份。在后续通讯中即可以通过服务器证书中的公钥进行加密，这时只有节点A拥有的私钥才可以解密(本文仅实现证书链的构建、发送即验证，不实现后续通讯)。<br />这里使用 ROS1、C++、OpenSSL 来实现。<br />代码仓库：<br />github：<br />gitee：

> 若读者对于C++基础语法不太熟悉，建议先学习一波。
> 作者在学习使用OpenSSL时查阅到的中文教学资料较少，强烈推荐读者使用 ChatGPT 来查阅本文中使用到OpenSSL相关方法。ps: 本文中的部分代码为ChatGPT编写。

这里附上相关网址：<br />ROS官网： [https://www.ros.org/](https://www.ros.org/)<br />ROS官方文档：[http://wiki.ros.org/cn](http://wiki.ros.org/cn)<br />OpenSSL官网：[https://www.openssl.org/](https://www.openssl.org/)<br />OpenSSL中文手册：[https://www.openssl.net.cn/](https://www.openssl.net.cn/)
<a name="OoSj0"></a>
## 2.1. ROS说明
<a name="KPy2S"></a>
### 2.1.1. 消息订阅
ROS 中的消息订阅模式：<br />在这里发布者(publisher)和订阅者(subscriber)首先分别向核心节点(master)进行注册，然后发布者和订阅者之间就会建立TCP连接。与常规TCP连接不同的是，消息的发送是单向的。这也就是设计模式中的发布订阅模式。<br />![ROS证书链.drawio.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1679446885598-d3dc21da-2595-414b-a02e-1b3d90f6af2a.png#averageHue=%2382b367&clientId=u468a6c02-7160-4&from=drop&id=u4dcb42af&name=ROS%E8%AF%81%E4%B9%A6%E9%93%BE.drawio.png&originHeight=491&originWidth=742&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=28822&status=done&style=none&taskId=u24a74175-e0f3-4163-9883-ceac78fe1c2&title=)<br />具体的ROS的命令以及该如何进行开发，网上有许多可以轻易搜索到的资料，请读者自行学习。
<a name="i6wAE"></a>
### 2.1.2. 自定义消息类型
首先要在包目录下创建msg目录，并创建自定义消息格式文件如下：
```
Header header
string data
string[] certs
```
header 为消息头文件，data 为主要传输数据，certs 为证书链。

<a name="ZmAVa"></a>
### 2.1.3. 配置文件
首先注意要查找到对应依赖。
```cmake
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_generation
)
find_package(OpenSSL REQUIRED)
```
对于自定义消息也要添加消息文件，并生成消息。
```cmake
add_message_files(
  FILES
  data.msg
)
generate_messages(
  DEPENDENCIES
  std_msgs
)
```
然后指定catkin时的 include 目录及依赖。
```cmake
catkin_package(
 INCLUDE_DIRS include
 LIBRARIES info_security
 CATKIN_DEPENDS roscpp std_msgs message_runtime
)
```
指定项目 include 目录。
```cmake
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)
```
添加自定义的工具类并指定动态编译
```cmake
add_library(checker SHARED src/CertChecker.cpp)
```
添加可执行文件及其依赖，注意这里要加上 自定义工具类及OpenSSL，不然会出现找不到 XXX 方法。
```cmake
add_executable(pub src/pub.cpp)
target_link_libraries(pub 
  checker
  OpenSSL::Crypto
  ${catkin_LIBRARIES}
)
add_dependencies(pub ${PROJECT_NAME}_generate_messages_cpp) 

add_executable(sub src/sub.cpp)
target_link_libraries(sub 
  checker
  OpenSSL::Crypto
  ${catkin_LIBRARIES}
)
add_dependencies(sub ${PROJECT_NAME}_generate_messages_cpp) 
```
在 package.xml 中同样要添加依赖标签如下：
```xml
<depend>roscpp</depend>
<depend>std_msgs</depend>
<buildtool_depend>catkin</buildtool_depend>
<build_depend>message_generation</build_depend>
<exec_depend>message_runtime</exec_depend>
```
<a name="QfmB8"></a>
## 2.2. OpenSSL 说明
本文是使用 C++ 来使用 OpenSSL。<br />OpenSSL 下有许多模块，本文主要使用的是 X509 证书模块，及一些辅助功能如 bio、pem等IO模块。

对于本文实验，在完成ROS工作空间及开发包创建以及 [1. 创建证书链](#ToGrH) 中的内容后， 需要完成的内容主要为：

1. 从文件中读取出 X509 证书。
2. 构建证书链
3. 基于ROS发送并接收证书链
4. 验证证书

以下为各内容所需要了解的 OpenSSL 知识。
<a name="iZqZY"></a>
### 2.2.1. 证书读取相关
在 OpenSSL 中证书类为 X509，但在实际使用中基本上都是使用其指针。所以在使用完后，需要清理防止内存泄露。
```cpp
// 一般使用的证书对象
X509* cert;
// 清理证书内存
X509_free(X509* cert);
```
X509 证书的读取有许多种方式，本文中主要通过 BIO 来读取文件，然后通过 PEM 来将 BIO 读取到的内容解析为 X509 证书。这里的 PEM 其实也是 OpenSSL 常用的文件格式，其主要为基于 base64 加密的数据和包裹数据的标题行。<br />主要使用的方法为：
```cpp
// 使用BIO读取文件
// const char* filename : 文件名(文件目录)
// const char* mode : 读取模式 (r 只读, w 只写, rw 读写)
BIO* BIO_new_file(const char* filename, const char* mode);

// 通过PEM解析BIO
// BIO* bp : BIO对象指针
// X509** x : 解析出的X509证书指针保存地址，函数中会创建一个新的证书对象并将其指针写入到传入地址中。
// pem_password_cb* cb : 口令回调函数(本文未使用)
// void* u : 回调函数参数(本文未使用)
X509* PEM_read_bio_X509(BIO* bp, X509** x, pem_password_cb* cb, void* u)
```
<a name="DicWq"></a>
### 2.2.2. 构建证书链相关
在 OpenSSL 中的证书链保存格式为 **X509 证书栈**：
```cpp
// 证书链保存格式
STACK_OF(X509)* chain;
// 证书链添加证书入栈
// STACK_OF(X509)* chain : 证书链指针
// X509* cert : X509 证书指针
int sk_X509_push(STACK_OF(X509)* chain, X509* cert);
// 证书链删除栈顶证书
// STACK_OF(X509)* chain : 证书链指针
X509* sk_X509_pop(STACK_OF(X509)* chain);
```
这里可以看到主要是使用了栈来保存 X509对象，然后通过使用栈对象的指针来操作证书链。<br />如果只是构建一条证书链的话，可以直接使用上述push和pop方法，但其并不能保证证书链有效。<br />如果我们需要构建一条**可信证书链**，首先我们需要确保首先证书链是有效的，其次证书链必须可以链接到一个可信证书。所以本文中使用**证书上下文**来验证并生成证书链。而要使用上下文，则需要了解一些其相关知识。<br />首先上下文中需要大量使用到**可信证书库(Trusted X509 Store)**或**可信证书链(Trusted Chain)**这两个都是帮助上下文判断证书是否可信的根本，两个选择一个使用即可，本文中使用了可信证书库。然后则是**吊销证书列表(Certificate Revocation List, CRL)**，常规的证书验证都是需要 CRL 检查的，以排除证书链中已吊销的证书，但其需要联网并且本次实验为本地自建证书链，所以需要将其禁用。相对于信任证书链对于证书验证而言，还有**非可信证书链(Untrusted Chain)**。我们在构建证书链时，可能有一部分中间证书是非信任的，但其又作为证书链的一部分不可或缺，所以我们可以将其完整添加到非可信证书链中用于辅助构建可信证书链。
```cpp
// 可信证书库
X509_STORE* store;
// 证书上下文及其创建
X509_STORE_CTX* ctx = X509_STORE_CTX_new();
// 证书上下文初始化
// X509_STORE_CTX* ctx : 证书上下文指针
// X509_STORE* store : 可信证书库
// X509* x509 : 待验证证书
// STACK_OF(X509)* untrusted_chain : 非可信证书链
int X509_STORE_CTX_init(X509_STORE_CTX* ctx, X509_STORE* store, X509* x509, STACK_OF(X509)* untrusted_chain);
// 禁用 CRL 检查 (直接使用即可，这里就不解释了)
X509_STORE_set_flags(store, X509_V_FLAG_ALLOW_PROXY_CERTS | X509_V_FLAG_IGNORE_CRITICAL);
// 证书验证
// 返回1表示验证通过，其他表示未通过
int X509_verify_cert(X509_STORE_CTX* ctx);
// 获取构建的可信证书链 
STACK_OF(X509)* X509_STORE_CTX_get1_chain(X509_STORE_CTX *ctx);
// 清理上下文(清空配置)
void X509_STORE_CTX_cleanup(X509_STORE_CTX *ctx)
// 释放上下文(释放内存)
void X509_STORE_CTX_free(X509_STORE_CTX *ctx)
```
<a name="QFuga"></a>
### 2.2.3. 发送及接收上下文相关
对于X509对象而言，我们需要将其转化为一个常用的消息格式，作者这里为了方便选择直接使用字符串 std::string 来保存X509对象。而对于证书链，刚好ROS支持可变长度数组(vector)，所以这里就直接使用字符串数组来发送了。<br />然后我们的工作就变成了 X509 的序列化和反序列化，作者这里直接使用了 ChatGPT 来编写下列代码，所以就不多解释啦。
```cpp
std::string x509_to_string(X509 *cert) {
    std::string cert_str;
    BIO *bio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_X509(bio, cert)) {
        char *cert_data;
        long cert_size = BIO_get_mem_data(bio, &cert_data);
        cert_str.assign(cert_data, cert_size);
    }
    BIO_free(bio);
    return cert_str;
}

X509* string_to_x509(const std::string& cert_str) {
    BIO* cert_bio = BIO_new_mem_buf(cert_str.data(), cert_str.size());
    if (cert_bio == nullptr) {
        return nullptr;
    }
    X509* cert = PEM_read_bio_X509(cert_bio, nullptr, nullptr, nullptr);
    BIO_free(cert_bio);
    return cert;
}
```
<a name="U0N5a"></a>
### 2.2.4. 验证证书相关
在 [2.2.2. 构建证书链相关](#DicWq) 中以及提到了关键的证书验证方法 X509_verify_cert ，但这个方法的默认验证函数是由 OpenSSL 封装的，我们并不清楚内部实现的验证逻辑。<br />不过大致上可以描述为：<br />在签发证书时 CA 会根据证书内的信息 通过某个 hash 算法得到其 hash 值，然后用自己的私钥对其进行加密，从而得到了数字签名。<br />在验证证书时，我们通过证书内的签发者信息，去查找到签发者的证书，进而获取到其公钥之后使用公钥对数字签名进行解密得到 hash值 h1，同样对待验证证书内的信息通过同样的 hash 算法计算得到 hash 值 h2 ，然后对比 h1 和 h2 即可知道证书有无被串改。<br />OpenSSL 还是提供了自定义验证逻辑的回调函数配置方法：
```cpp
// 配置自定义的验证逻辑回调函数
// X509_STORE_CTX *ctx : 证书上下文
// X509_STORE_CTX_verify_cb verify : 自定义的证书验证函数
void X509_STORE_CTX_set_verify_cb(X509_STORE_CTX* ctx, X509_STORE_CTX_verify_cb verify);

// 自定义证书验证函数
int verifyCallback(int ok, X509_STORE_CTX* ctx) {
    // 自定义验证逻辑
    // 返回值为1表示验证成功，0表示验证失败
}

// 获取当前验证证书
X509* X509_STORE_CTX_get_current_cert(X509_STORE_CTX* ctx);
// 获取证书信息
X509_NAME* X509_get_subject_name(const X509* a)
// 获取错误层数
int X509_STORE_CTX_get_error_depth(X509_STORE_CTX* ctx)
// 获取错误
int X509_STORE_CTX_get_error(X509_STORE_CTX *ctx)
// 将错误编号转为字符串
const char* X509_verify_cert_error_string(long n)
```
<a name="sss93"></a>
## 2.3. 代码讲解
首先先附上本次实验的文件目录结构：<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/26727041/1679457509152-9d06b39a-22bb-46c1-81f0-03625ae0417a.png#averageHue=%23292b33&clientId=u468a6c02-7160-4&from=paste&height=615&id=u46b6a04b&name=image.png&originHeight=683&originWidth=298&originalType=binary&ratio=1.1100000143051147&rotation=0&showTitle=false&size=99292&status=done&style=none&taskId=u3f488ad7-34d4-4b9c-a37b-a9b1149d72e&title=&width=268.4684650085836)<br />大致讲解一下 info_securty 目录下的主要文件或目录的作用：

- ca_dir ：证书目录
- CertChecker.h CertChecker.cpp ：为方便使用 OpenSSL 建立的工具类的头文件及源文件
- Const.h ：定义了证书目录及路径常量 (读者要复现的话需要修改路径)
- data.msg ：ROS 的自定义消息格式文件
- pub.cpp sub.cpp ：发布者和订阅者，里面创建了相应节点并使用了 CertChecker ，这里包含主要的实验流程。
- CMakeLists.txt ：CMake 的配置文件，这里包含编译的配置。
- package.xml ：包的说明配置文件，这里主要为 ROS 包的配置，需要与 CMakeLists.txt 保持一致。

其他文件或目录为 ROS 工作空间配置文件及编译后产生的相关文件，不需要在意。
<a name="KgZrO"></a>
### 2.3.1. CertChecker
CertChecker 是用于方便管理和调用 OpenSSL 的自定义工具类(以下简称**检查者**)，里面包括三个静态方法及四个成员方法。之所以要创建单独工具类有以下原因：

1. 封装部分方法，提升代码可读性、可维护性。
2. 便于统一进行资源的创建、初始化、回收。
3. 降低代码耦合。

检查者头文件及源文件如下：
> 这里需要注意的是，在初始化中已默认读取了根证书并添加到可信证书库。
> 其余代码已有足够多注释，相信读者结合上文可以自行理解。

```cpp
#pragma once

#include "info_security/Const.h"

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <openssl/err.h>

class CertChecker {
    private:
        // 可信证书库
        X509_STORE* store;
        // 可信证书链
        STACK_OF(X509)* trusted_chain;
        // 非可信证书链
        STACK_OF(X509)* untrusted_chain;
        // 构建的证书链
        STACK_OF(X509)* build_chain;
        
    public:
        // 无参构造
        CertChecker();
        // 析构函数
        ~CertChecker();
        // 从文件中读取证书
        static X509* getCert(std::string certPath);
        // 将 X509 证书转化为字符串
        static std::string x509_to_string(X509 *cert);
        // 从字符串中解析出 X509 证书
        static X509* string_to_x509(const std::string& cert_str);
        // 添加进入可信证书库
        bool addToStore(X509* cert);
        // 添加进入证书链 trust: 是否可信
        bool addToChain(X509* cert,bool trust);
        // 校验证书
        int verifyCert(X509* cert);
        // 校验证书并构建证书链
        STACK_OF(X509)* buildCertChain(X509* cert);
};
```
```cpp
#include "info_security/CertChecker.h"
#include <iostream>

CertChecker::CertChecker(/* args */)
{
    store = X509_STORE_new();
    // 设置默认证书目录 
    X509_STORE_set_default_paths(store);
    trusted_chain = sk_X509_new_null();
    untrusted_chain = sk_X509_new_null();
    build_chain = sk_X509_new_null();

    // 加载受信任的证书(根证书)
    std::string rootPath = CA_DIR_ROOT + "key/root.crt";
    X509* root = CertChecker::getCert(rootPath);
    this->addToStore(root);
    this->addToChain(root,true);
}

CertChecker::~CertChecker()
{
    // 释放资源
    X509_STORE_free(store); 
    sk_X509_free(trusted_chain);
    sk_X509_free(untrusted_chain);
    sk_X509_free(build_chain);
}
X509* CertChecker::getCert(std::string certPath) {
    // 创建 BIO 对象，用于读取文件
    BIO* bio = BIO_new_file(certPath.c_str(), "r");
    if(bio == NULL) {
        std::cout << "读取文件出错" << std::endl;
    }
    X509* cert = nullptr; 
    PEM_read_bio_X509(bio, &cert, nullptr, nullptr);
    if(!cert) {
        std::cout << "解析文件出错" << std::endl;
    }
    // 关闭 BIO 对象
    BIO_free(bio);
    return cert;
}

bool CertChecker::addToChain(X509* cert,bool trust) {
    if(cert == nullptr) {
        return false;
    }
    // 入栈
    if(trust) {
        sk_X509_push(trusted_chain, cert);
    }
    else {
        sk_X509_push(untrusted_chain, cert);
    }
    return true;
}

bool CertChecker::addToStore(X509* cert) {
    // 将证书添加到 X509_STORE 中
    if (X509_STORE_add_cert(store, cert) != 1) {
        // 错误处理
        return false;
    }
    return true;
}

int verifyCallback(int ok, X509_STORE_CTX *ctx)
{
    // 自定义验证逻辑
    // 返回值为1表示验证成功，0表示验证失败
    X509* cert = X509_STORE_CTX_get_current_cert(ctx);
    char buf[256];
    X509_NAME_oneline(X509_get_subject_name(cert), buf, 256);
    std::cout << "正在验证：" << buf << std::endl;
    
    if (!ok) {
        // 验证失败，打印错误信息
        int depth = X509_STORE_CTX_get_error_depth(ctx);
        int err = X509_STORE_CTX_get_error(ctx);
        printf("Error at depth %d: %s\n", depth, buf);
        printf("  Error code %d (%s)\n", err, X509_verify_cert_error_string(err));
        return 0;
    }
    std::cout << "验证通过" << std::endl;
    return ok;
}

int CertChecker::verifyCert(X509* cert)
{
    // 初始化上下文
    X509_STORE_CTX *ctx = X509_STORE_CTX_new();
    if(ctx == nullptr) {
        std::cout << "初始化失败" << std::endl;
    }

    // 参数说明 (ctx, store, 待验证证书, 非信任的待验证证书链)
    if(!X509_STORE_CTX_init(ctx, store, cert, untrusted_chain)) {
        std::cout << "初始化上下文失败" << std::endl;
    }
    // 设置自定义验证回调函数
    // X509_STORE_CTX_set_verify_cb(ctx, verifyCallback);
    // 禁用 CRL 检查
    X509_STORE_set_flags(store, X509_V_FLAG_ALLOW_PROXY_CERTS | X509_V_FLAG_IGNORE_CRITICAL);
    // 设置可信证书链
    // X509_STORE_CTX_trusted_stack(ctx, trusted_chain);
    // 验证证书链的可信性
    int result = X509_verify_cert(ctx);
    // int result = X509_verify(server_cert, store);
    if(result != 1) {
        int err = X509_STORE_CTX_get_error(ctx);
        std::cout << X509_verify_cert_error_string(err) << std::endl;
    }
    X509_STORE_CTX_free(ctx);
    return result;
}

// 构建证书链
STACK_OF(X509)* CertChecker::buildCertChain(X509* cert) {
    build_chain = sk_X509_new_null();

    // 创建一个验证器并设置一些参数
    X509_STORE_CTX* ctx = X509_STORE_CTX_new();
    if (!ctx) {
        // 处理错误
        return NULL;
    }
    // 禁用 CRL 检查
    X509_STORE_set_flags(store, X509_V_FLAG_ALLOW_PROXY_CERTS | X509_V_FLAG_IGNORE_CRITICAL);
    // 初始化上下文
    if (!X509_STORE_CTX_init(ctx, store, cert, untrusted_chain)) {
        // 处理错误
        return NULL;
    }
    // 设置自定义验证回调函数
    X509_STORE_CTX_set_verify_cb(ctx, verifyCallback);
    // X509_STORE_CTX_trusted_stack(ctx, trusted_chain);
    int ret = X509_verify_cert(ctx);
    if (ret != 1) {
        // 验证失败，处理错误
        int err = X509_STORE_CTX_get_error(ctx);
        // 处理错误码
        return NULL;
    }
    // 生成证书链并获取
    build_chain = X509_STORE_CTX_get1_chain(ctx);
    X509_STORE_CTX_cleanup(ctx);
    X509_STORE_CTX_free(ctx);
    return build_chain;
}

std::string CertChecker::x509_to_string(X509 *cert) {
    std::string cert_str;
    BIO *bio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_X509(bio, cert)) {
        char *cert_data;
        long cert_size = BIO_get_mem_data(bio, &cert_data);
        cert_str.assign(cert_data, cert_size);
    }
    BIO_free(bio);
    return cert_str;
}

X509* CertChecker::string_to_x509(const std::string& cert_str) {
    BIO* cert_bio = BIO_new_mem_buf(cert_str.data(), cert_str.size());
    if (cert_bio == nullptr) {
        return nullptr;
    }
    X509* cert = PEM_read_bio_X509(cert_bio, nullptr, nullptr, nullptr);
    BIO_free(cert_bio);
    return cert;
}
```
<a name="wBqlE"></a>
### 2.3.2. 发布者
发布者内首先可以看到声明了检查者指针并赋值为空指针，然后则是一个自定义的退出处理函数，在此函数内进行内存回收、打印日志及退出。<br />在主函数内，可以看到首先创建并配置了发布者节点，然后创建证书检查者对象，之后就开始拼接证书路径并开始调用 getCert 方法从文件中读取代理CA证书及服务器证书。我们这里通过 addToChain(agent, false) 将代理CA证书添加到非信任证书链中。然后调用 buildCertChain 方法构建证书链。然后将证书链转为字符串数组并最终添加到 自定义消息 msg.certs 内。最后在循环内，将消息发送出去。
```cpp
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
```
<a name="s5uyi"></a>
### 2.3.3. 订阅者
在订阅者中同样声明了一个检查者、定义了退出处理函数。在主函数中也是主要定义节点，不同的是这里有一个消息处理回调函数，即订阅者在接收到消息后就会调用这个函数进行处理。在消息处理回调函数中，这里首先打印一些日志，然后将消息中的字符串重新解析为证书并添加到非信任证书链，这里我们保存下第一个证书即服务器证书的指针。然后调用 verifyCert 对证书进行校验。
```cpp
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
```
<a name="XDAIx"></a>
### 2.3.4. 额外说明

1. 非信任证书链可以包含待验证证书及一些可信证书
2. 如果代码中通过使用 setlocale(LC_ALL,"") 来使得输出显示中文，这样的前提条件是当前环境中已经有中文语言包。如果读者在复现时出现中文显示异常( ??? 或 其他乱码)，可以参照 [Ubuntu 安装中文支持](http://t.csdn.cn/TvdQ3)。
3. 发布者和订阅者的自定义退出处理函数略有不同，在发布者中多调用了 exit(0) ，这是因为两者的循环方式不同。
4. 本文中没有真正实现自定义证书验证，仅提供了模板及日志便于查看学习。
<a name="b2KGB"></a>
# 3. 写在最后
本人经验尚浅，如本文有任何错误或改进之处欢迎各位留言！😉<br />如果觉得不错的话，可以**✨**一下吗？
