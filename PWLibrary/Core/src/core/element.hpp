#include "core/surface.hpp"
#include "core/collections.hpp"

#include <unordered_map>

using namespace std;
namespace PWL::Core
{
    class IElement
    {
    private:
        unordered_map<string, Surface> status;
        string currect_status;
    public:
        Vec2<uint32_t> position;

        IElement()
        {
            
        }
        /* logical */
        void SetOutputStatus(string currect_status)
        {

        }
        /* render */
        void SetElementStatus(string name)
        {

        }
        Surface GetElement()
        {
            return this->status[this->currect_status];
        }
    };
}
