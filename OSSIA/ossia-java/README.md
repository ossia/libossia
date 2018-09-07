# ossia-java

This is a Java binding of ossia-c library functions.

It relies on [Java Native Access - JNA](https://github.com/java-native-access/) which should be automatically download by CMake at configuration time.

Currently it's a work in progress.

You should configure with `OSSIA_C=ON` and `OSSIA_JAVA=ON` :

    cmake /path/to/libossia -DOSSIA_C=ON -DOSSIA_JAVA=ON

If build succeed, you could try to run the example :

    java -cp OSSIA/ossia-java/ossia-java-example.jar:OSSIA/ossia-java/jna.jar:OSSIA/ossia-java/ossia-java.jar example

The OSCQuery server should start, then it terminates as soon as `/foo/bar` value is no more equals to `0.f`.

## TODO :
- add it to CI
