#include "libwxfreq/libwxfreq.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int main(int argc, char* argv[]) {
  if (argc != 9){
    printf("%s conf appid key keytype shm_key func level time\n", argv[0]);
    return 0;
  }
  SetRuleConfFile(argv[1]); 
  int appid = strtoul(argv[2], NULL, 10);
  const char* key = argv[3];
  const char* keytype = argv[4];
  int shm_key = strtoul(argv[5], NULL, 10);
  const char* func = argv[6];
  int block_level = strtoul(argv[7], NULL, 10);
  int linger_time = strtoul(argv[8], NULL, 10);
  
  int ret = RegisterNewShmStat(keytype, false, shm_key,  1000000);
  if (ret != 0) {
    printf("RegisterNewShmStat failed, ret = %d\n", ret);
    return -1;
  }

  if (InitFreq() == false) {
    printf("InitFreq failed\n");
    return -2;
  }

  if (strcmp(func, "addblock") == 0) {
    ret = AddBlock(keytype, key, appid,  linger_time, block_level);
  } else if (strcmp(func, "deleteblock") == 0) { 
    ret = DeleteBlock(keytype, key, appid);
  } else if (strcmp(func, "addwhite") == 0) { 
    ret = AddWhite(keytype, key, appid, linger_time);
  } else if (strcmp(func, "deletewhite") == 0) { 
    ret = DeleteWhite(keytype, key, appid);
  } else {
    printf("unknown func\n");
    return -3;
  }
  printf("%s return ret %d\n", func, ret);
  return ret;
}
