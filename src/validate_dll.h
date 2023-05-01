#include <optional>
#include <string>

struct ExportContext {
  int fHasOrdinal1;
  unsigned long nExports;
};

std::optional<std::string> isDllValid(const std::string& path);
