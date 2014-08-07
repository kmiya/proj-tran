#ifndef PROJ_MAIN_H_
#define PROJ_MAIN_H_

#include "transform.h"

namespace proj {

// print usage
void usage();
// callback function for mouse event
void onMouse(int event, int x, int y, int flags, void *param);

} // namespace proj

#endif // PROJ_MAIN_H_
