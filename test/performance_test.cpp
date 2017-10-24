#include "libwxfreq/libwxfreq.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>

static inline int Log(const char* format, ...) { }
int main(int argc, char* argv[]) {
  if (argc != 3){
    printf("%s conf item_count\n", argv[0]);
    return 0;
  }
  SetRuleConfFile(argv[1]); 
  SetLogFunc(Log);
  int item_count = strtoul(argv[2], NULL, 10);
  
  int ret = RegisterNewShmStat("user", false, rand(),  item_count);
  if (ret != 0) {
    printf("RegisterNewShmStat failed, ret = %d\n", ret);
    return -1;
  }

  if (InitFreq() == false) {
    printf("InitFreq failed\n");
    return -2;
  }

  int oom_num = 0, first_time = 0;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  printf("start time %u %u\n", start.tv_sec, start.tv_usec);
  for (int i = 0; i < item_count; i++) {
    struct BlockResult res;
    res = ReportAndCheck("user", "test", rand(), 1);
    if (res.block_level == kErrorOutOfMemory) {
      oom_num ++;
      if (first_time == 0) first_time = i + 1;
    }
  }
  gettimeofday(&end, NULL);
  printf("end time %u %u\n", end.tv_sec, end.tv_usec);
  printf("average time %u per second\n", (unsigned int)(item_count/(end.tv_sec-start.tv_sec)));
  printf("oom count %u first oom %u memory usage %.3f\n", oom_num, first_time, 100 - 100.0 * oom_num/item_count);
  return 0;
}

