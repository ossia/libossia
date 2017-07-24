#include "utils.hpp"


namespace ossia
{
namespace max
{

bool find_peers(t_object_base* x)
{
    t_object *patcher, *box, *obj;
    object_obex_lookup(x, gensym("#P"), &patcher);
    for (box = jpatcher_get_firstobject(patcher); box; box =
         jbox_get_nextobject(box)) {
        obj = jbox_get_object(box);
        if (obj
            && object_classname(obj) == object_classname(x)
            && (t_object_base*)obj != x)
            return true;
    }
}

} // namespace max
} // namespace ossia
