# libwxfreq

## 编译
  make

## 安装
  1. 修改makefile文件中的HEADERINSTALLDIR和LIBINSTALLDIR变量
  2. make install

## 单元测试
  1. 以测试文件为目标make. 例如: make demo

## 简介
  libwxfreq是一个高性能频率限制库。通过简洁的几个接口便可以实现通用的可配置的频率
限制功能。配合其他网络框架，便可以实现一个通用的高性能频率控制服务。

## 特点：
  1. 高度可配置。统计时长可配，规则可配。
  2. 支持任意key类型。
 
## 配置文件
  1. 格式
  // appid的元信息，指明apppid的统计时长。默认第一个统计时长是60s，第二个统计时  
  // 时长是3600s， 第三个是86400s。可以手动修改任何一个统计时长为[0, 86400]  
  [appid]
  5 = 30, 60, 90 // appid为5的统计时长分别是30s 60s 90s  
  25 = 10, 60   // appid为25的统计时长分别是10s 60s 86400s  
 
  // 频率规则， 用于说明某个appid的阈值， 拦截等级等  
  [match_rule_name] // 规则名字，用户指明中了哪条规则  
  match_appid = 5   // 匹配的appid  
  item = min_interval >= 4 // 具体的阈值表达式， 支持大于和大于等于两种运算符.  
                           // min_interval 表示第一个统计时长的统计值，  
                           // mid_interval 表示第二个统计时长的统计值，  
                           // max_interval 表示第三个统计时长的统计值，  
  block_level = 1  // 拦截等级， 大于0  
  rule_type = user // 规则对应的key类型  

  2. 例子
  ```
  [appid]
  100 = 30, 60, 90

  [rule1]
  match_appid = 100
  item =  min_interval >= 10
  block_level = 1
  rule_type = user

  [rule2]
  match_appid = 100
  item =  mid_interval >= 15 
  block_level = 1
  rule_type = user

  [rule3]
  match_appid = 100
  item =  max_interval >= 20
  block_level = 1
  rule_type = user

  [rule4]
  match_appid = 100
  item =  min_interval >= 100
  block_level = 1
  rule_type = ip 

  [rule5]
  match_appid = 100
  item =  mid_interval >= 150
  block_level = 1
  rule_type = ip 

  [rule6]
  match_appid = 100
  item =  max_interval >= 200 
  block_level = 1
  rule_type = ip 
  ```

## API接口

  设置规则配置文件
  > void SetRuleConfFile(const char *filename);

  注册一个新的统计维度。type_name 指明类型名称， zero_init标志初始化时是否需要
  初始化为0， key 是共享内存key，item_cnt 表示共享内存大小，最大能统计的item个
  数。
  > int RegisterNewShmStat(const char* type_name, bool zero_init, key_t key,
  >                      unsigned int item_cnt);

  系统初始化
  > bool InitFreq();

  上报一次频率到系统中。type_name 是统计类型，说明key的含义。 key是主体标识，
  appid 是业务标识， cnt 表示本次需要累加的次数。 例如，统计QQ号为3402393864的
  用户在appid为5的服务上的频率，使用ReportAndCheck("user", 3402393864, 5, 1);
  同样统计ip为202.204.105.8访问appid的5的频率，使用ReportAndCheck( "IP", 
  3402393864, 5, 1); 因为202.204.105.8的整形表示也是3402393864。type_name指明了
  key的含义。
  > struct BlockResult ReportAndCheck(const char* type_name, const char *key,
  >                                   const uint32_t appid, const uint32_t cnt);

  只检查当前是否中频率
  > struct BlockResult OnlyCheck(const char* type_name, const char *key,
  >                             const uint32_t appid);

  只上报，不检查
  > int OnlyReport(const char* type_name, const char *key, const uint32_t appid,
  >              const uint32_t cnt);

  获取频率统计值, cnt为0获取当前统计值，不为零表示先累计再获取
  > struct FreqCache GetCache(const char* type_name, const char *key,
  >                          const uint32_t appid, const uint32_t cnt = 0);

  加白操作，linger_time表示加白有效期
  > int AddWhite(const char* type_name, const char *key, const uint32_t appid,
  >            const uint32_t linger_time);

  删除白名单
  > int DeleteWhite(const char* type_name, const char *key, const uint32_t appid);

  手动加拦截
  > int AddBlock(const char* type_name, const char *key, const uint32_t appid,
  >            const uint32_t linger_time, const uint32_t block_level);

  手动删除拦截
  > int DeleteBlock(const char* type_name, const char *key, const uint32_t appid);

  设置写日志方法，默认写本地文件/tmp/libwxfreq.log
  > void SetLogFunc(LogFunction logfunc);

  设置重启时恢复白名单/拦截方法，默认从本地文件/tmp/libwxfreq_db.$type_name读取
  > void SetLoadFunc(LoadFunc func);

  设置白名单/加拦截数据持久化方法。默认写本地文件/tmp/libwxfreq_db.$type_name
  > void SetDumpFunc(DumpFunc func);
