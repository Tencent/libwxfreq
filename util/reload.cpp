#include "reload.h"
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>

#include "freq_manager.h"
#include "appid_manager.h"
#include "log.h"

namespace libwxfreq {

static const char* kDefaultWhiteFileNamePrefix = "/tmp/libwxfreq_db.";

static int LoadFromLocalFile(const char* type_name) {
  char filename[64];
  snprintf(filename, sizeof(filename), "%s%s",
          kDefaultWhiteFileNamePrefix, type_name);
  std::ifstream ifs;
  ifs.open(filename, std::fstream::in);

  if (!ifs.is_open()) {
    gLog("[%s][%d]: can not open %s\n", __FILE__, __LINE__, filename);
    return -1;
  }

  char line[128];
  std::string opname, key;
  uint32_t appid = 0;
  unsigned int expire_time = 0, block_level = 0;
  unsigned int now = time(NULL);
  while (ifs.getline(line, sizeof(line))) {
    std::stringstream ss(line);
    ss >> opname >> key >> appid >> expire_time >> block_level;

    if (opname == "AddWhite") {
      if (expire_time > now) {
        if (FreqManager::AddWhite(type_name, key, appid,
                                  expire_time - now) == false) {
          gLog("[%s][%d]: reload whitelist failed, typename %s key %s"
              " appid %u expiretime %u\n", __FILE__, __LINE__, type_name,
              key.c_str(), appid, expire_time);
          ifs.close();
          return -1;
        }
      }
    } else if (opname == "DeleteWhite") {
      if (FreqManager::DeleteWhite(type_name, key, appid) == false) {
        gLog("[%s][%d]: reload deletewhite failed, typename %s key %s"
            " appid %u\n", __FILE__, __LINE__, type_name, key.c_str(), appid);
        ifs.close();
        return -1;
      }
    } else if (opname == "AddBlock") {
      if (expire_time > now) {
        if (FreqManager::AddBlock(type_name, key, appid,
                                  expire_time - now, block_level) == false) {
          gLog("[%s][%d]: reload block failed, typename %s key %s appid %u"
              " expiretime %u blocklevel %u\n", __FILE__, __LINE__, type_name,
              key.c_str(), appid, expire_time, block_level);
          ifs.close();
          return -1;
        }
      }
    } else if (opname == "DeleteBlock") {
      if (FreqManager::DeleteBlock(type_name, key, appid) == false) {
        gLog("[%s][%d]: reload deleteblock failed, typename %s key %s"
            " appid %u\n", __FILE__, __LINE__, type_name, key.c_str(), appid);
        ifs.close();
        return -1;
      }
    } else {
      gLog("[%s][%d]: reload unknow typename %s\n", type_name);
    }
  }

  ifs.close();
  return 0;
}

static int DumpToLocalFile(const char* opname, const char* type_name,
                          const char *key, const uint32_t appid,
                          const uint32_t linger_time,
                          const uint32_t block_level) {
  char filename[64];
  snprintf(filename, sizeof(filename), "%s%s",
          kDefaultWhiteFileNamePrefix, type_name);
  static std::ofstream ofs(filename, std::fstream::out | std::fstream::app);
  if (!ofs.is_open()) {
    gLog("[%s][%d]: can not open %s\n", __FILE__, __LINE__, filename);
    return -1;
  }
  if (linger_time <= 600) return 0;
  ofs << opname << "\t" << key << "\t" <<  appid << "\t" <<
    time(NULL) + linger_time << "\t" << block_level << std::endl;
  return 0;
}

LoadFunc gLoadFunc = LoadFromLocalFile;
DumpFunc gDumpFunc = DumpToLocalFile;

}  // namespace libwxfreq

