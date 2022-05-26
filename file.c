//
// File descriptors
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "file.h"

struct devsw devsw[NDEV];
struct {
  struct file file[NFILE];
} ftable;

