// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "utils.hpp"
namespace ossia
{
namespace pd
{

std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol)
{
  std::vector<std::string> tags;

  if (tags_symbol)
  {
    char* c = tags_symbol->s_name;
    std::string tag = "";

    while (*c != '\0')
    {
      if (*c == ' ')
      {
        tags.push_back(tag);
        tag = std::string("");
      }
      else
        tag += *c;
      c++;
    }
    tags.push_back(tag);
  }
  return tags;
}

std::string string_from_path(const std::vector<std::string>& vs, fmt::MemoryWriter& fullpath)
{
  auto rit = vs.rbegin();
  for (; rit != vs.rend(); ++rit)
  {
    fullpath << "/" << *rit;
  }
  if (vs.empty())
    fullpath << "/";

  return fullpath.str();
}

bool find_and_display_friend(t_obj_base* x, t_canvas* patcher)
{
  t_gobj* list = patcher->gl_list;

  std::string target_str;
  std::string canvas_str = "canvas";
  std::string xclassname
      = std::string(x->x_obj.o_obj.te_g.g_pd->c_name->s_name);
  if (xclassname == "ossia.remote")
    target_str = "ossia.param";
  else if (xclassname == "ossia.view")
    target_str = "ossia.model";

  while (list && list->g_pd)
  {
    std::string classname = list->g_pd->c_name->s_name;
    if (classname == target_str)
    {
      t_obj_base* p = (t_obj_base*)list;
      if (p->x_node == x->x_node)
      {
        if (x->x_last_opened_canvas)
          glist_noselect(x->x_last_opened_canvas);
        if (x->x_clock)
          clock_unset(x->x_clock);
        glist_noselect(patcher);
        x->x_last_opened_canvas = patcher;
        canvas_vis(glist_getcanvas(patcher), 1);
        glist_select(patcher, &p->x_obj.o_obj.te_g);
        if (x->x_clock)
          clock_delay(x->x_clock, 1000);
        return true;
      }
    }
    else if (classname == canvas_str)
    {
      t_canvas* canvas = (t_canvas*)&list->g_pd;
      if (!canvas_istable(canvas))
      {
        t_gobj* _list = canvas->gl_list;
        if (find_and_display_friend(x, canvas))
          return true;
      }
    }
    list = list->g_next;
  }
  return false;
}

}
}
