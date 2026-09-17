#include "StdInc.h"
#include "Base.h"


namespace notsa {
// This must remain in a cpp file, because __FILE__ is only stable here, 
// in a header it depends on the file that includes it
fs::path GetSourceCodeBasePath() {
    return fs::path(__FILE__).parent_path();
}
}; // namespace notsa
