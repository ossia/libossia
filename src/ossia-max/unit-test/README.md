ossia-max unit tests
====================

This folder contains a `logger-server.js` that loads sequencially each patcher which name ends with ".ossia-max-test.maxpat".
Those patchers should contains one or more `ossia.assert` objects and a `ossia.test-logger` object.

Please disable the "Restore Windows on Launch" Max's option to prevent Max from reloading other patchers then the one we want to test.

Each patcher runs under a fresh opened Max application.

Run the tests
=============

In this folder:

    node logger-server.js

Return status : 0 on success, < 0 on errors (can't find patcher, timeout...), > 0 : how many tests failed

You can also run specific test by passing their path to as arguments: 

    node logger-server.js path-to-test-patcher.maxpat path-to-test-patcher2.maxpat ...