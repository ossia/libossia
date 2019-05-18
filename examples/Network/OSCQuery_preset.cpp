#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>
#include <memory>
#include <functional>

int main(int argc, char** argv)
{
  int remote_port = 9997;

  std::string json_example =
      R"_(
      {
       "DESCRIPTION": "root node",
       "FULL_PATH": "/",
       "ACCESS": 0,
       "CONTENTS": {
         "foo": {
           "DESCRIPTION": "demonstrates a read-only OSC node- single float value ranged 0-100",
           "FULL_PATH": "/foo",
           "ACCESS": 1,
           "TYPE": "f",
           "VALUE": [
             0.5
           ],
           "RANGE": [
             {
               "MIN": 0.0,
               "MAX": 100.0
             }
           ]
         },
         "bar": {
           "DESCRIPTION": "demonstrates a read/write OSC node- two ints with different ranges",
           "FULL_PATH": "/bar",
           "ACCESS": 3,
           "TYPE": "ii",
           "VALUE": [
             4,
             51
           ],
           "RANGE": [
             {
               "MIN": 0,
               "MAX": 50
             },
             {
               "MIN": 51,
               "MAX": 100
             }
           ]
         },
         "baz": {
           "DESCRIPTION": "simple container node, with one method- qux",
           "FULL_PATH": "/baz",
           "ACCESS": 0,
           "CONTENTS": {
             "qux":	{
               "DESCRIPTION": "read/write OSC node- accepts one of several string-type inputs",
               "FULL_PATH": "/baz/qux",
               "ACCESS": 3,
               "TYPE": "s",
               "VALUE": [
                 "half-full"
               ],
               "RANGE": [
                 {
                   "VALS": [ "empty", "half-full", "full" ]
                 }
               ]
             }
           }
         }
       }
      }
      )_";
  if(argc > 1)
  {
    try { remote_port = boost::lexical_cast<int>(argv[1]); } catch(...) {  };
  }

  ossia::net::generic_device device{"OSCQuery Example"};

  if(argc > 2)
  {
    try
    {
      std::ifstream file(argv[2]);
      std::stringstream s;
      s << file.rdbuf();

      json_example = s.str();

    }
    catch(...) { }
  }

  ossia::oscquery::load_oscquery_device(device, json_example);

  std::string w;
  ossia::net::debug_recursively(w, device.get_root_node());
  std::cout << w << std::endl;
}



