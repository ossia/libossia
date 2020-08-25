libossia
===

libossia is a modern C++, cross-environment distributed object model for creative coding.

It allows to declare the architecture of your creative coding application's functions as a **tree of OSC nodes and parameters**. These nodes/parameters can have attributes, which allow to declare many of their properties, such as their values, types, units, ranges, etc....

This OSC tree-based architecture (coined "device" in the OSSIA terminology) can then be exposed over the network under several protocols, some of which allow this architecture, and the properties and values of its nodes, to be fully explored and queried. For now, protocols available in the implemenations are: plain OSC, OSCquery, and Minuit - more are part of libossia and will be made available in the future.

libossia offers bindings and implementations for several environments: PureData, Max/MSP, Python, C, C++, openframeworks, Unity3D, QML, Faust, SuperCollider.

You can find some documentation for the library and its various implementations is at https://ossia.github.io

Please feel free to ask questions on the [forum](http://forum.ossia.io/c/libossia), or to come and chat with us ! [![Gitter](https://badges.gitter.im/ossia/libossia.svg)](https://gitter.im/ossia/libossia?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

More about the general project at: http://ossia.io

Documentation :
* [Lib and Implementations](https://ossia.github.io/ossia-docs)
* [Examples](https://github.com/ossia/libossia/tree/master/examples)
* [Doxygen](http://ossia.github.io/libossia/html)
* [Building](https://github.com/ossia/libossia/wiki/Building)
* [Code style](https://github.com/ossia/libossia/wiki/Code-style-guide)

Build Status
============
* Linux, OS X : [![Build Status](https://travis-ci.org/ossia/libossia.svg)](https://travis-ci.org/ossia/libossia)
* Windows : [![Build status](https://ci.appveyor.com/api/projects/status/ut3o8p64lib3ie1w?svg=true)](https://ci.appveyor.com/project/JeanMichalCelerier/libossia)
* Java & Unity3D : [![Azure Pipelines](https://img.shields.io/azure-devops/build/ossia/f914424f-63a4-43f7-b424-67c9dc58ae05/1)](https://dev.azure.com/ossia/libossia/_build?definitionId=1)
* Coveralls : [![Coverage Status](https://coveralls.io/repos/github/ossia/libossia/badge.svg?branch=master)](https://coveralls.io/github/ossia/libossia?branch=master)

License
=======
Available under both LGPLv3 and CeCILL-C

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bhttps%3A%2F%2Fgithub.com%2FOSSIA%2Flibossia.svg?type=shield)](https://app.fossa.io/projects/git%2Bhttps%3A%2F%2Fgithub.com%2FOSSIA%2Flibossia?ref=badge_shield)
